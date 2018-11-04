PROGS = tfcrypt skeinhash tftest tfrand tfblktst tferand tfecrypt tfetest genrconst
SUPPORT = mhexdump.c xstrlcpy.c
PROGSRCS = $(PROGS:=.c)
PROGOBJS = $(PROGSRCS:.c=.o)
SRCS = $(filter-out $(PROGSRCS) $(SUPPORT), $(wildcard *.c))
HDRS = $(wildcard *.h)
SUPPOBJS = $(SUPPORT:.c=.o)
OBJS = $(SRCS:.c=.o)

ifneq (,$(DEBUG))
override CFLAGS+=-Wall -O0 -g
else
override CFLAGS+=-O3
endif

default: $(OBJS) libtf.a libsupp.a tfcrypt skeinhash
all: $(OBJS) libtf.a libsupp.a $(PROGS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

libtf.a: $(OBJS)
	$(AR) cru $@ $^

libsupp.a: $(SUPPOBJS)
	$(AR) cru $@ $^

$(PROGS): %: %.o libtf.a libsupp.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f libtf.a libsupp.a $(OBJS) $(PROGOBJS) $(SUPPOBJS) $(PROGS)
