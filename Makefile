HASTUR_SOURCE=hastur.c hastur_helpers.c hastur_string_builder.c

OPTS = -Wall -ggdb

all: library tester data_test threaded_tester

clean:
	rm -rf tester data_test threaded_tester libhastur.a *.o *.dSYM *~

tester: tester.c test_helper.c $(HASTUR_SOURCE)
	gcc -o tester $(OPTS) $^ -lpthread

threaded_tester: threaded_tester.c test_helper.c $(HASTUR_SOURCE)
	gcc -o threaded_tester $(OPTS) $^ -lpthread

data_test: data_test.c $(HASTUR_SOURCE)
	gcc -o data_test $(OPTS) data_test.c $(HASTUR_SOURCE) -lpthread

library: $(HASTUR_SOURCE:.c=.o)
	libtool -static $(HASTUR_SOURCE:.c=.o) -o libhastur.a
