CFLAGS := -D_REENTRANT -Wall -pedantic -Isrc
CFLAGS += -fPIC -g
LDFLAGS = -lpthread -rdynamic

LIBNAME = libthreadpool
SHARED_SUFFIX = .so
STATIC_SUFFIX = .a

ifdef DEBUG
CFLAGS += -g
endif

ifeq ($(strip $(PROFILE)),1)
CFLAGS += -DPROFILE
endif

SHARED = $(LIBNAME)$(SHARED_SUFFIX)
STATIC = $(LIBNAME)$(STATIC_SUFFIX)

TARGETS = $(SHARED) $(STATIC)

all: $(TARGETS)

OBJS := \
	src/threadpool.o

SORT_OBJS := \
	tests/list.o

SORT_DATA := \
	input.txt \
	output.txt \
	check.txt

deps := $(OBJS:%.o=%.o.d)
src/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF $@.d -c $<

tests/%: tests/%.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/list.o: tests/list.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF $@.d -c $<

tests/sort: tests/sort.c $(OBJS) $(SORT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

TESTS =
#TESTS += tests/shutdown
#TESTS += tests/thrdtest
#TESTS += tests/heavy
TESTS += tests/mapreduce
TESTS += tests/sort

$(LIBNAME)$(SHARED_SUFFIX): $(OBJS)
	$(CC) -shared -o $@ $< ${LDLIBS}

$(LIBNAME)$(STATIC_SUFFIX): $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm -f $(TARGETS) *~ */*~ $(OBJS) $(SORT_OBJS) $(TESTS) $(deps) core $(SORT_DATA)

test: $(TESTS)
check: $(TESTS)
	@for test in $^ ; \
	do \
		echo "Execute $$test..." ; $$test && echo "OK!\n" ; \
	done

sort: tests/sort
	./tests/sort 50000
	sort -n input.txt > check.txt
	diff output.txt check.txt

-include $(deps)
