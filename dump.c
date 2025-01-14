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
#include <math.h>

static unsigned char quit = 0;

static void sighandler(int sig) {

	switch (sig) {
		case SIGTERM:
		case SIGINT:
			quit = 1;
		break;
	}
}

void dumpdata(const unsigned int ticks, const char file[], const unsigned int limit,
		const unsigned char bus, const unsigned int dumpinterval, const char * card, int visual) {

#ifdef ENABLE_NLS
	// This is a data format, so disable decimal point localization
	setlocale(LC_NUMERIC, "C");
#endif

	// Set up signals to exit gracefully when terminated
	struct sigaction sig;

	sig.sa_handler = sighandler;

	sigaction(SIGTERM, &sig, NULL);
	sigaction(SIGINT, &sig, NULL);

	if(strcmp("-",file))
		printf(_("Dumping to %s, "), file);

	if (limit)
		printf(_("line limit %u.\n"), limit);


	printf("GPU: %s on bus %02x \n\n", card, bus);

	// Check the file can be output to
	FILE *f = NULL;
	if (file[0] == '-')
		f = stdout;
	else
		f = fopen(file, "a");

	if (!f)
		die(_("Can't open file for writing."));

	// This does not need to be atomic. A delay here is acceptable.
	while(!results)
		usleep(16000);

	// Action
	unsigned int count;

	if (visual) {
		fprintf(f, "%16s    %16s    %6s    %8s\n", 
				"GPU", "VRAM", "Temp C", "VRAM GB" );
		fprintf(f, "%16s    %16s    %6s    %8s\n", 
				"----------------", "----------------", "------", "--------" );
	} else {
		const char * dash8 = "--------";
		fprintf(f, "%8s %8s %6s   %8s   %8s %8s %8s %8s\n", 
				"GPU", "VRAM", "Temp C", "VRAM GB", "mclk%", "mclk ghz", "sclk%", "sclk ghz");
		fprintf(f, "%8s %8s %6s   %8s   %8s %8s %8s %8s\n", 
				dash8, dash8, "------", dash8, dash8, dash8, dash8, dash8);
	}
			

	for (count = limit; !limit || count; count--) {

		/*
		struct timeval t;
		gettimeofday(&t, NULL);
		fprintf(f, "%llu.%llu: ", (unsigned long long) t.tv_sec, (unsigned long long) t.tv_usec);
		*/

		// Again, no need to protect these. Worst that happens is a slightly
		// wrong number.
		float k = 1.0f / ticks / dumpinterval;
		float gui = 100 * results->gui * k;
		float vram = 100.0f * results->vram / vramsize;
		float vramgb = results->vram / 1024.0f / 1024.0f / 1024.0f;
		float mclk = 100.0f * (results->mclk * k) / (mclk_max / 1e3f);
		float sclk = 100.0f * (results->sclk * k) / (sclk_max / 1e3f);
		float mclk_ghz = results->mclk * k / 1000.0f;
		float sclk_ghz = results->sclk * k / 1000.0f;

		if (!bits.vram) { 
			vram = -1; 
			vramgb = -1; 
		}

		float temp_c = results->temp / 1000.0f;

		if (!(sclk_max != 0 && sclk > 0)) {
			mclk = -1;
			mclk_ghz = -1;
			sclk = -1;
			sclk_ghz = -1;
		}

		if (visual) {
			char bar_gpu[11] = "          ";
			char bar_mem[11] = "          ";
			for (int i = 0; i < 10; i++) {
				if (i < roundf(gui/10))  bar_gpu[i] = '=';
				if (i < roundf(vram/10)) bar_mem[i] = '=';
			}
			fprintf(f, "[%10s]%3.0f%%    [%10s]%3.0f%%    %6.1f    %8.3f\n", 
				bar_gpu, gui, bar_mem, vram, temp_c, vramgb);
		} else {
			fprintf(f, "%7.2f%% %7.2f%% %6.1f   %8.3f   %7.2f%% %8.3f %7.2f%% %8.3f\n", 
				gui, vram, temp_c, vramgb, mclk, mclk_ghz, sclk, sclk_ghz);
		}

		fflush(f);

		// Did we get a termination signal?
		if (quit)
			break;

		// No sleeping on the last line.
		if (!limit || count > 1)
			sleep(dumpinterval);
	}

	fflush(f);

	if (f != stdout) {
		fsync(fileno(f));
		fclose(f);
	}
}
