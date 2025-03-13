/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "comm_layer.h"
#include "acc.h"

#ifndef __ZEPHYR__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#else

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#endif

#define BIND_PORT 4242

#define MSG_SIZE 32
/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

/* UART configuration */
#define UART_DEVICE_NODE DT_NODELABEL(uart7)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

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
    if (!buf) {
        printf("Error: NULL buffer passed to print_uart\n");
        return;
    }

    int msg_len = strlen(buf);
    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(uart_dev, buf[i]);
    }
}

// kill program if status variable is negative
void checkKillCode(int code) {
    if (code < 0) { exit(1); }
}

int setup_UART() {
    if (!device_is_ready(uart_dev)) {
        printf("UART device not found or not ready!\n");
        return 1;
    }
    
    // /* configure interrupt and callback to receive data */
	int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 1;
	}
	uart_irq_rx_enable(uart_dev);
    return 0;
}

// Setup code for creating a TCP server on a specified port
int setupTcpServer(int port) {

    int ret, opt, server;

    // create an ipv6 socket bound to any address and specified port
    struct sockaddr_in6 bind_addr = {
        .sin6_family = AF_INET6,
        .sin6_addr = IN6ADDR_ANY_INIT,
        .sin6_port = htons(port),
    };

    server = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (server < 0) {
        printf("error: socket: %d\n", errno);
        return -1; // error code
    }

    // configure socket with proper options - IPv6 only?
    socklen_t optlen = sizeof(int);
    ret = getsockopt(server, IPPROTO_IPV6, IPV6_V6ONLY, &opt, &optlen);

    if (ret == 0) {
        if (opt) {
            printf("IPV6_V6ONLY option is on, turning it off.\n");

            opt = 0;
            ret = setsockopt(server, IPPROTO_IPV6, IPV6_V6ONLY, &opt, optlen);
            if (ret < 0) {
                printf("Cannot turn off IPV6_V6ONLY option\n");
            } else {
                printf("Sharing same socket between IPv6 and IPv4\n");
            }

        }
    }

    // bind server to address and port
    if (bind(server, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        printf("error: bind: %d\n", errno);
        return -2; // error code
    }

    // start listening for connections
    if (listen(server, 5) < 0) {
        printf("error: listen: %d\n", errno);
        return -3; // error code
    }
    
    printf("Single-threaded TCP echo server waits for a connection on port %d...\n", port);

    // return server value
    return server;

}

int acceptTcpConnection(int server) {

    struct sockaddr_in6 client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char addr_str[32];
    int client = accept(server, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client < 0) {
        printf("error: accept: %d\n", errno);
        return -1;
    }

    inet_ntop(client_addr.sin6_family, &client_addr.sin6_addr, addr_str, sizeof(addr_str));
    printf("Connection from %s\n", addr_str);

    return client;

}

int rxEnvironmentInfo(int client, struct EnvironmentInfoPayload* payload) {

    char buf[ENVIRONMENT_INFO_PAYLOAD_LENGTH];
    int len = recv(client, buf, sizeof(buf)-1, 0);

    if (len < 0) {
        printf("error: recv: %d\n", errno);
    }
    else if (len == 0) {
        printf("Connection Closed.\n");
    }
    else {
        buf[len] = '\0';
        unpackEnvironmentInfoPayload(buf, payload);
    }
    
    return len;

}

int txVehicleSpeed(int client, const struct VehicleSpeedPayload* payload) {

    // pack vehicle speed into bytes
    char buf[VEHICLE_SPEED_PAYLOAD_LENGTH];
    packVehicleSpeedPayload(payload, buf);

    int outLen = send(client, buf, VEHICLE_SPEED_PAYLOAD_LENGTH, 0);
    if (outLen < 0) {
        printf("error: send: %d\n", errno);
        return -1;
    }

    return outLen;

}

int main(void) {

    int status; // temp status var
    char tx_buf[MSG_SIZE];

    // setup tcp server
    int server = setupTcpServer(BIND_PORT);
    checkKillCode(server);
    setup_UART();

    // wait for an incoming connection
    while (1) {

        // accept incoming client connection (sim pc)
        int client = acceptTcpConnection(server);

        // sim pc will transmit environmental info, so create a struct to contain info
        struct EnvironmentInfoPayload simEnvInfo = {0};

        // controller will transmit vehicle speed back to sim pc, so create struct
        struct VehicleSpeedPayload vehSpeedInfo = {0};

        // initialize acc controller
        initAcc();

        // main program loop
        while (1) {

            /* wait for input from the user */
            char message[32];
            if (k_msgq_get(&uart_msgq, &tx_buf, K_SECONDS(1)) == 0) {
        
                sprintf(message, "%s", tx_buf);

                vehSpeedInfo.vehicleSpeed_mps = atof(message);
                
                status = txVehicleSpeed(client, &vehSpeedInfo);
            }
            else {
                /* Optional response back to the TCP client */
                const char *response = "No speed info avaliable.";        
                send(client, response, strlen(response), 0);
            }

            // rx environment info from simulator
            status = rxEnvironmentInfo(client, &simEnvInfo);

            uint8_t pwmCmd;
            // controller logic
            pwmCmd = stepAcc(
                vehSpeedInfo.vehicleSpeed_mps,
                simEnvInfo.leadSpeed_mps, 
                simEnvInfo.setSpeed_mps, 
                simEnvInfo.leadDistance_m
            );

            /* Send received TCP data over UART */
            char buf[4];
            sprintf(buf, "%u", pwmCmd);
            print_uart(buf);
            print_uart("\r\n");  // Add newline for readability on UART output
            
        }

    }

    return 0;

}
