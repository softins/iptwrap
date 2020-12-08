/*-----------------------------------------------------------------------------
 * Copyright (C) 2012, Software Insight Ltd. All Rights Reserved.
 *-----------------------------------------------------------------------------
 *
 * File:	iptwrap.c
 *
 * Description:	Main program for IPtables wrapper.
 *		This program invokes iptables as root, called from apache.
 *
 * Author:	Tony Mountifield
 * Date:	12-Nov-2012
 *
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "iptwrap.h"
#include "cmd.h"

static void usage(char *prog)
{
	fprintf(stderr, "usage: %s [options]\n"
			"options:\n"
			"   -c chain       Name of iptables chain\n"
			"   -a IPaddr      IP address to add to chain\n"
			"   -d IPaddr      IP address to delete from chain\n"
			"   -l IPaddr      IP address to list from chain\n"
			"   -?             Help (this message)\n"
			, prog);
	exit(1);
}

int main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	int c;
	int chainopt = 0, ipadd = 0, ipdel = 0, iplist = 0;
	int status;
	int found = 0;
	FILE *fp;
	char *p;
	char chain[32],ipaddr[16];
	char buf[256];

	while ((c = getopt(argc, argv, "c:d:a:l:?")) != EOF)
		switch (c) {
			case 'c':	/* name of chain */
				if (strlen(optarg) >= sizeof(chain)) {
					fprintf(stderr, "%s: chain name too long for -%c\n", argv[0], c);
					usage(argv[0]);
				}
				strcpy(chain, optarg);
				chainopt = 1;
				break;
			case 'a':	/* IP to add */
				if (strlen(optarg) >= sizeof(ipaddr)) {
					fprintf(stderr, "%s: IP address too long for -%c\n", argv[0], c);
					usage(argv[0]);
				}
				strcpy(ipaddr, optarg);
				ipadd = 1;
				break;
			case 'd':	/* IP to delete */
				if (strlen(optarg) >= sizeof(ipaddr)) {
					fprintf(stderr, "%s: IP address too long for -%c\n", argv[0], c);
					usage(argv[0]);
				}
				strcpy(ipaddr, optarg);
				ipdel = 1;
				break;
			case 'l':	/* IP to list */
				if (strlen(optarg) >= sizeof(ipaddr)) {
					fprintf(stderr, "%s: IP address name too long for -%c\n", argv[0], c);
					usage(argv[0]);
				}
				strcpy(ipaddr, optarg);
				iplist = 1;
				break;
			case '?':	/* help */
				usage(argv[0]);
				break;
			default:
				usage(argv[0]);
		}

	if (optind < argc)
		usage(argv[0]);

	/* check that we have all the info we need */

	if (!chainopt) {
		fprintf(stderr, "%s: chain name must be supplied\n", argv[0]);
		usage(argv[0]);
	}

	if ((ipadd+ipdel+iplist) > 1) {
		fprintf(stderr, "%s: cannot specify more than one of -a, -d and -l\n", argv[0]);
		usage(argv[0]);
	}

	if (ipadd || ipdel) {
		argv[0] = "/sbin/iptables";
		argv[1] = ipadd ? "-A" : "-D";
		argv[2] = chain;
		argv[3] = "-s";
		argv[4] = ipaddr;
		argv[5] = "-j";
		argv[6] = "ACCEPT";
		argv[7] = NULL;

		status = cmd(argv);
	} else if (iplist) {
		argv[0] = "/sbin/iptables";
		argv[1] = "-nL";
		argv[2] = chain;
		argv[3] = NULL;

		fp = cmdpopen(argv);
		if (fp) {
			while (fgets(buf, sizeof(buf), fp)) {
				p = strstr(buf, ipaddr);
				if (!p) continue;
				if (p > buf && p[-1] != ' ') continue;
				if (p[strlen(ipaddr)] != ' ') continue;
				found = 1;
			}
			cmdpclose(fp);
		}

		return !found;
	} else {
		fprintf(stderr, "%s: must specify either -a, -d or -l\n", argv[0]);
		usage(argv[0]);
	}

	return 0;
}
