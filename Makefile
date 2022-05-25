# Requires GNU Make
# This is Linux-specific software, so we can depend on GNU make.

bin = radeonload
src = $(filter-out amdgpu.c auth_xcb.c,$(wildcard *.c))

CFLAGS_SECTIONED = -ffunction-sections -fdata-sections
LDFLAGS_SECTIONED = -Wl,-gc-sections

CFLAGS ?= -g
CFLAGS += -Wall -Wextra -pthread
CFLAGS += -Iinclude
CFLAGS += $(CFLAGS_SECTIONED)
CFLAGS += $(shell pkg-config --cflags pciaccess)
CFLAGS += $(shell pkg-config --cflags libdrm)

# autodetect libdrm features
ifeq ($(shell pkg-config --atleast-version=2.4.66 libdrm && echo ok), ok)
	CFLAGS += -DHAS_DRMGETDEVICE=1
endif

ifeq ($(shell pkg-config --atleast-version=2 libdrm_amdgpu && echo ok), ok)
	amdgpu ?= 1
else
	amdgpu ?= 0
endif

ifeq ($(amdgpu), 1)
	src += amdgpu.c
	CFLAGS += -DENABLE_AMDGPU=1
	LIBS += $(shell pkg-config --libs libdrm_amdgpu)

	ifeq ($(shell pkg-config --atleast-version=2.4.79 libdrm_amdgpu && echo ok), ok)
		CFLAGS += -DHAS_AMDGPU_QUERY_SENSOR_INFO=1
	endif
endif

obj = $(src:.c=.o)
LDFLAGS ?= -Wl,-O1
LDFLAGS += $(LDFLAGS_SECTIONED)
LIBS += $(shell pkg-config --libs pciaccess)
LIBS += $(shell pkg-config --libs libdrm)
LIBS += -lm

# On some distros, you might have to change this to ncursesw
LIBS += $(shell pkg-config --libs ncursesw 2>/dev/null || \
		shell pkg-config --libs ncurses 2>/dev/null || \
		echo "-lncursesw")

.PHONY: all clean 

all: $(bin)

$(obj): $(wildcard include/*.h) 

$(bin): $(obj)
	$(CC) -o $(bin) $(obj) $(CFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(bin) 

