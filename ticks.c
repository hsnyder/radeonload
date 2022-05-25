/*
    Copyright (C) 2012 Lauri Kasanen

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
#include <pthread.h>

struct bits_t *results = NULL;

struct collector_args_t {
	unsigned int ticks;
	unsigned int dumpinterval;
};

static void *collector(void *arg) {
	struct collector_args_t *args = (struct collector_args_t *) arg;

	const unsigned int ticks = args->ticks;
	const unsigned int dumpinterval = args->dumpinterval;

	struct bits_t res[2];

	// Save one second's worth of history
	struct bits_t *history = calloc(ticks * dumpinterval, sizeof(struct bits_t));
	unsigned int cur = 0, curres = 0;

	const useconds_t sleeptime = 1e6 / ticks;

	while (1) {
		unsigned int stat;
		getgrbm(&stat);

		memset(&history[cur], 0, sizeof(struct bits_t));

		if (stat & bits.gui) history[cur].gui = 1;
		getsclk(&history[cur].sclk);
		getmclk(&history[cur].mclk);
		gettemp(&history[cur].temp);

		usleep(sleeptime);
		cur++;
		cur %= ticks * dumpinterval;

		// One second has passed, we have one sec's worth of data
		if (cur == 0) {
			unsigned int i;

			memset(&res[curres], 0, sizeof(struct bits_t));

			for (i = 0; i < ticks * dumpinterval; i++) {
				res[curres].gui += history[i].gui;
				res[curres].mclk += history[i].mclk;
				res[curres].sclk += history[i].sclk;
				res[curres].temp += history[i].temp;
			}

			getvram(&res[curres].vram);

			// Atomically write it to the pointer
			__sync_bool_compare_and_swap(&results, results, &res[curres]);

			curres++;
			curres %= 2;
		}
	}

	return NULL;
}

void collect(unsigned int ticks, unsigned int dumpinterval) {

	// Start a thread collecting data
	pthread_t tid;
	pthread_attr_t attr;

	// We don't care to join this thread
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	struct collector_args_t *args = malloc(sizeof(*args));
	args->ticks = ticks;
	args->dumpinterval = dumpinterval;

	pthread_create(&tid, &attr, collector, args);
}
