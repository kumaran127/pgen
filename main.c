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

#include "pgen.h"

/**
 * It starts exactly here
 */
int32_t main(int32_t argc, char **argv) {
	FILE *fp;
	int32_t buff_size;
	char if_name[IFNAMSIZ];
	char dst_mac[CHAR_MAC_LEN];
	char *cp_buff = NULL;
	char *cp_cur = NULL;
	char option[MAX_OPTION_LEN];
	char value[MAX_VALUE_LEN];
	char conf_file[PATH_MAX];
	int32_t len;

	/* Choose the conf file */
	if (argc < 2)
		strcpy(conf_file, DEF_PGEN_CONF);
	else if (argc == 2)
		strcpy(conf_file, argv[1]);
	else {
		usage();
		goto err;
	}

	/* Open Conf file */
	fp = fopen(conf_file, "r");
	if (!fp) {
		PGEN_ERROR("fopen error", errno);
		return -1;
	}

	/* Get buffer size */
	if (pgen_parse_option(fp, option, value))
		goto err;
	if (strcmp(option, "BUFF_SIZE"))
		goto err;
	if (pgen_store_num(&buff_size, value))
		goto err;

	/* Get interface name */
	if (pgen_parse_option(fp, option, value))
		goto err;
	if (strcmp(option, "IF_NAME"))
		goto err;
	if (!strcpy(if_name, value))
		goto err;
	if (validate_if(if_name))
		goto err;

	/* Get recipient mac address */
	if (pgen_parse_option(fp, option, value))
		goto err;
	if (strcmp(option, "PK_DST_MAC"))
		goto err;
	if (!strcpy(dst_mac, value))
		goto err;

	/* Allocating the packet itself ;) */
	cp_buff = malloc(buff_size);
	if (!cp_buff) {
		PGEN_ERROR("malloc failed", errno);
		goto err;
	}
	memset(cp_buff, 0, buff_size);
	cp_cur = cp_buff;

	/* write data in the packet buffer */
	while (pgen_parse_option(fp, option, value) != EOF) {
		if (!strcmp(option, "ETHER_HEADER")) {
			cp_cur = pgen_ethr_hdr_writer(fp, cp_cur);
		}
		/* L2 */
		else if (!strcmp(option, "ARP")) {
			cp_cur = pgen_arp_writer(fp, cp_cur);
		}
		/* L3 */
		else if (!strcmp(option, "IPV6")) {
			cp_cur = pgen_ipv6_writer(fp, cp_cur);
		}
		/* L4 */
		else if (!strcmp(option, "ICMP6")) {
			cp_cur = pgen_icmp6_writer(fp, cp_cur);
		}
		else if (!strcmp(option, "UDP")) {
			cp_cur = pgen_udp_writer(fp, cp_cur);
		}
		else if (!strcmp(option, "RAW")) {
			len = raw_data_writer(fp, cp_cur);
			if (len < 0)
				goto err;
			cp_cur += len;
		}
		else {
			PGEN_INFO("Packet type not yet supported");
			PGEN_PRINT_DATA("%s\t%s\n", option, value);
			goto err;
		}
		if (!cp_cur)
			goto err;
	}

	/* Cut the extra bytes */
	buff_size = cp_cur - cp_buff;
	/* Heap might grow in either direction */
	if (buff_size < 0)
		buff_size *= -1;
	cp_buff = realloc((void *)cp_buff, buff_size);

	/* Send the packet in wire */
	if (send_packet(if_name, dst_mac, cp_buff, buff_size)) {
		PGEN_INFO("send_packet failed");
		goto err;
	}

	PGEN_INFO("SUCCESS");
	fclose(fp);
	/* free will accept NULL also */
	free(cp_buff);
	return 0;

err:
	PGEN_INFO("ERROR CASE");
	fclose(fp);
	/* free will accept NULL also */
	free(cp_buff);

	return -1;
}
