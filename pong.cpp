/*
 * This file is part of Pong.
 *
 * MIT License
 *
 * Copyright (c) 2018 Ted Percival
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstdio>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

static void usage(FILE *dest, const char *argv0) {
    fprintf(dest, "Usage: %s <remote-address>\n", argv0);
}

static int pong(int sock) {
    // payload (octets):
    // 0: type: echo reply = 129 = 0x81
    // 1: code: 0
    // 2: checksum(1)
    // 3: checksum(0)
    // 4: identifier(1): may be zero
    // 5: identifier(0): may be zero
    // 6: sequence number(1): may be zero
    // 7: sequence number(0): may be zero
    // 8...: arbitrary data
    // TODO: Use identifier & sequence number fields
    static const uint8_t msg[] = {
        0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        'P', 'I', 'N', 'G', '!' };
    static const size_t msglen = sizeof(msg);
    if (send(sock, msg, msglen, 0) != msglen) {
        perror("send");
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(stderr, argv[0]);
        return 1;
    }

    struct addrinfo *addrs, hints = {
        .ai_flags = AI_ADDRCONFIG,
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_RAW,
        .ai_protocol = IPPROTO_ICMPV6,
    };

    int gaierr = getaddrinfo(argv[1], NULL, &hints, &addrs);
    if (gaierr) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gaierr));
        return 1;
    }

    int sock = -1;

    struct addrinfo *chosen_addr = NULL;

    for (
            struct addrinfo *addr = addrs;
            sock == -1 && addr != NULL;
            addr = addr->ai_next)
    {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock == -1) {
            perror("socket");
            // continue anyway
        } else {
            chosen_addr = addr;
        }
    }

    if (sock == -1) {
        fprintf(stderr, "No viable network addresses\n");
        return 1;
    }

    if (connect(sock, chosen_addr->ai_addr, chosen_addr->ai_addrlen) == -1) {
        perror("connect");
        return 1;
    }

    freeaddrinfo(addrs);
    addrs = NULL;
    chosen_addr = NULL;

    return pong(sock);
}
