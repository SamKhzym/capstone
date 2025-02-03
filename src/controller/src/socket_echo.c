#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/socket.h>
#include <string.h>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              S[9` MMiii88888**
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* UART configuration */
#define UART_DEVICE_NODE DT_NODELABEL(uart7)
#define LED_DEVICE_NODE DT_ALIAS(led0)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static const struct device *const led_dev = DEVICE_DT_GET(LED_DEVICE_NODE);

void print_uart(const char *buf) {
    int msg_len = strlen(buf);
    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(uart_dev, buf[i]);
    }
}

/* TCP server configuration */
#define BIND_PORT 4242

void main(void) {
    /* Check if UART device is ready */
    if (!device_is_ready(uart_dev)) {
        printk("UART device not found or not ready!\n");
        return;
    }

    if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

    k_msleep(1000);

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

    /* Socket setup for TCP server */
    int serv;
    struct sockaddr_in bind_addr = {
        .sin_family = AF_INET,
        .sin_addr = INADDR_ANY_INIT,
        .sin_port = htons(BIND_PORT),
    };

    serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv < 0) {
        printk("Socket creation failed: %d\n", errno);
        return;
    }

    if (bind(serv, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        printk("Bind failed: %d\n", errno);
        close(serv);
        return;
    }

    if (listen(serv, 5) < 0) {
        printk("Listen failed: %d\n", errno);
        close(serv);
        return;
    }

    printk("Server listening on port %d...\n", BIND_PORT);

    k_msleep(1000);

    /* Periodic UART message and TCP server loop */
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char addr_str[32];
        
        /* Accept client connections */
        int client = accept(serv, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client < 0) {
            printk("Accept failed: %d\n", errno);
            continue;
        }

        inet_ntop(client_addr.sin_family, &client_addr.sin_addr, addr_str, sizeof(addr_str));
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
            // print_uart(buf);
            // print_uart("\r\n");  // Add newline for readability on UART output

            /* Optional response back to the TCP client */
            const char *response = "Received your message";
            send(client, response, strlen(response), 0);
        }

        close(client);
        printk("Connection from %s closed\n", addr_str);
    }

    close(serv); 
}
