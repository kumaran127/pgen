/**
 * This file is part of pgen, a packet generator tool.
 * Copyright (C) 2013  Balakumaran Kannan <kumaran.4353@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PGEN_H
#define PGEN_H 1

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <errno.h>
#include <linux/limits.h>

#define DEF_PGEN_CONF "/etc/pgen.conf"
#define ETH_ALEN 6
#define IPV6_ADDR_MAX_LEN 40
#define CHAR_MAC_LEN 18
#define MAX_OPTION_LEN 200
#define MAX_VALUE_LEN 200
#define MAX_LINE_LENGTH 1024

#define PGEN_INFO(MSG) do {                                                    \
	fprintf(stdout, "File:%s, Line:%d, %s\n", __FILE__,	__LINE__, MSG);        \
	} while(0)
#define PGEN_ERROR(MSG, errno) do {                                            \
	fprintf(stderr, "File:%s, Line:%d, %s, errno:%d\n", __FILE__, __LINE__,    \
			MSG, errno);                                                       \
	} while(0)
#define PGEN_PRINT_DATA(...) do {                                              \
	fprintf(stdout, __VA_ARGS__);                                              \
	} while (0)

char* pgen_ethr_hdr_writer(FILE *, char *);
char* pgen_arp_writer(FILE *, char *);
char* pgen_ipv6_writer(FILE *, char *);
char* pgen_icmp6_writer(FILE *, char *);

/* Helpers */
void usage();
int pgen_strcmp(const char *, const char *);
int pgen_parse_option(FILE *, char *, char *);
int send_packet(const char *, const char *, const char *, const int);
int mac_writer(char *, const char *);
int ip4_writer(char *, const char *);
int pgen_store_num(int *, const char *);
int validate_mac(const char *);
int validate_num(const char *);
int encode_name(char *, const char *);
#endif /* PGEN_H */
