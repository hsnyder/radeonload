/*
    Copyright (C) 2012 Lauri Kasanen
    Copyright (C) 2018 Genesis Cloud Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "radeontop.h"
#include <getopt.h>

void die(const char * const why) {
	puts(why);
	exit(1);
}


static void help(const char * const me, const unsigned int ticks, const unsigned int dumpinterval) {
	printf("\tUsage: %s [-hmv] [-b bus] [-d file] [-i seconds] [-l limit] [-p device] [-t ticks]\n\n"
		"-b --bus 3		Pick card from this PCI bus (hexadecimal)\n"
		"-d --dump file		Dump data to this file, - for stdout\n"
		"-i --dump-interval 1	Number of seconds between dumps (default %u)\n"
		"-l --limit 3		Quit after dumping N lines, default forever\n"
		"-m --mem		Force the /dev/mem path, for the proprietary driver\n"
		"-p --path device	Open DRM device node by path\n"
		"-t --ticks 50		Samples per second (default %u)\n"
		"-v --visual  		Print bars to give a visual indication of load\n"
		"\n"
		"-h --help		Show this help\n",
		me, dumpinterval, ticks);
	die("");
}

int main(int argc, char **argv) {
	// Temporarily drop privileges to do option parsing, etc.
	seteuid(getuid());

	const unsigned int default_ticks = 120;
	const unsigned int default_dumpinterval = 1;

	unsigned int ticks = default_ticks;
	short bus = -1;
	unsigned char forcemem = 0;
	unsigned int device_id = 0;
	unsigned int limit = 0;
	char *dump = "-";
	unsigned int dumpinterval = default_dumpinterval;
	const char *path = NULL;

	int visual = 0;

	// opts
	const struct option opts[] = {
		{"bus", 1, 0, 'b'},
		{"dump", 1, 0, 'd'},
		{"dump-interval", 1, 0, 'i'},
		{"help", 0, 0, 'h'},
		{"limit", 1, 0, 'l'},
		{"mem", 0, 0, 'm'},
		{"path", 1, 0, 'p'},
		{"ticks", 1, 0, 't'},
		{"visual", 0, 0, 'v'},
		{0, 0, 0, 0}
	};

	while (1) {
		int c = getopt_long(argc, argv, "b:d:hi:l:mp:t:v", opts, NULL);
		if (c == -1) break;

		switch(c) {
		case 'h':
		case '?':
			help(argv[0], default_ticks, default_dumpinterval);
			break;

		case 'v':
			visual = 1;
			break;
		case 't':
			ticks = atoi(optarg);
			break;
		case 'm':
			forcemem = 1;
			break;
		case 'b':
			bus = strtoul(optarg, NULL, 16);
			break;
		case 'l':
			limit = atoi(optarg);
			break;
		case 'd':
			dump = optarg;
			break;
		case 'i':
			dumpinterval = atoi(optarg);
			if (dumpinterval < 1)
				dumpinterval = 1;
			break;
		case 'p':
			path = optarg;
		}
	}

	// init (regain privileges for bus initialization and ultimately drop them afterwards)
	seteuid(0);
	init_pci(path, &bus, &device_id, forcemem);
	// after init_pci we can assume that bus/device_id exists (otherwise it would die())

	setuid(getuid());

	const int family = getfamily(device_id);
	if (!family)
		puts(_("Unknown Radeon card. <= R500 won't work, new cards might."));

	const char * const cardname = family_str[family];

	initbits(family);

	// runtime
	collect(ticks, dumpinterval);

	dumpdata(ticks, dump, limit, bus, dumpinterval, cardname, visual);

	cleanup();
	return 0;
}
