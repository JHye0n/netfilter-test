LDLIBS=-lnetfilter_queue

all: netfilter-test

netfilter-test: nfqnl_test.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

nfqnl_test.o:
	g++ -c -o nfqnl_test.o nfqnl_test.c

clean:
	rm -rf netfilter-test *.o
