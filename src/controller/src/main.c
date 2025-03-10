#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/net/socket.h>
#include <string.h>                                                                                    
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* UART configuration */
#define UART_DEVICE_NODE DT_NODELABEL(uart7)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

#define MSG_SIZE 32

/* TCP server configuration */
#define BIND_PORT 4242

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

void print_uart(const char *buf) {
    int msg_len = strlen(buf);
    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(uart_dev, buf[i]);
    }
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int main(void) {
    char tx_buf[MSG_SIZE];

    /* Check if UART device is ready */
    if (!device_is_ready(uart_dev)) {
        printk("UART device not found or not ready!\n");
        return 1;
    }

  	/* configure interrupt and callback to receive data */
	int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
	}
	uart_irq_rx_enable(uart_dev);


    /* Socket setup for TCP server */
    int serv;
    struct sockaddr_in6 bind_addr = {
        .sin6_family = AF_INET6,
        .sin6_addr = IN6ADDR_ANY_INIT,
        .sin6_port = htons(BIND_PORT),
    };

    serv = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (serv < 0) {
        printk("Socket creation failed: %d\n", errno);
        return 1;
    }

    if (bind(serv, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        printk("Bind failed: %d\n", errno);
        close(serv);
        return 1;
    }

    if (listen(serv, 5) < 0) {
        printk("Listen failed: %d\n", errno);
        close(serv);
        return 1;
    }

    printk("Server listening on port %d...\n", BIND_PORT);

    /* Periodic UART message and TCP server loop */
    while (1) {
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char addr_str[32];
        
        /* Accept client connections */
        int client = accept(serv, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client < 0) {
            printk("Accept failed: %d\n", errno);
            continue;
        }

        inet_ntop(client_addr.sin6_family, &client_addr.sin6_addr, addr_str, sizeof(addr_str));
        printk("Connection from %s\n", addr_str);

        /* Create a separate loop for handling the client connection */
        while (1) {
            /* Periodically send "180" over UART every 1 second */
            //print_uart("180\r\n");
            //k_sleep(K_MSEC(1000));  // 1-second delay

            /* Receive data from TCP client */
            char buf[128];
            int len = recv(client, buf, sizeof(buf) - 1, 0);

            if (len <= 0) {
                if (len < 0) {
                    printk("Recv failed: %d\n", errno);
                }
                break;  // Exit inner loop if client disconnects
            }

            buf[len] = '\0';  // Null-terminate the received data
            printk("Received string: %s\n", buf);

            /* Send received TCP data over UART */
            print_uart(buf);
            print_uart("\r\n");  // Add newline for readability on UART output

            /* wait for input from the user */
            char message[] = "Sending speed info...";
            if (k_msgq_get(&uart_msgq, &tx_buf, K_SECONDS(1)) == 0) {
        
                int msg_len = min(strlen(tx_buf), strlen(message));
                for (int i = 0; i < msg_len; i++) {
                    message[i] = tx_buf[i];
                }
                
                send(client, message, strlen(message), 0);
            }
            else {
                /* Optional response back to the TCP client */
                const char *response = "No speed info avaliable.";        
                send(client, response, strlen(response), 0);
            }
            
            
        }

        close(client);
        printk("Connection from %s closed\n", addr_str);
    }

    close(serv); 
    return 0;
}
