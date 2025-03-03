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

#endif

#define BIND_PORT 4242

// kill program if status variable is negative
void checkKillCode(int code) {
    if (code < 0) { exit(1); }
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
    int len = recv(client, buf, sizeof(buf), 0);

    if (len < 0) {
        printf("error: recv: %d\n", errno);
    }

    if (len > 0) {
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

    // setup tcp server
    int server = setupTcpServer(BIND_PORT);
    checkKillCode(server);

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

            // rx environment info from simulator, 
            status = rxEnvironmentInfo(client, &simEnvInfo);

            // controller logic
            uint8_t pwmCmd = stepAcc(
                0,
                simEnvInfo.leadSpeed_mps, 
                simEnvInfo.setSpeed_mps, 
                simEnvInfo.leadDistance_m
            );
            
            status = txVehicleSpeed(client, &vehSpeedInfo);

            sleep(1);
            
        }

    }

    return 0;

}
