HASTUR_SOURCE=hastur.c hastur_helpers.c hastur_string_builder.c

all: library tester data_test threaded_tester

tester: tester.c test_helper.c $(HASTUR_SOURCE)
	gcc -o tester -Wall -O2 -ggdb $^ -lpthread

threaded_tester: threaded_tester.c test_helper.c $(HASTUR_SOURCE)
	gcc -o threaded_tester -Wall -O2 -ggdb $^ -lpthread

data_test: data_test.c $(HASTUR_SOURCE)
	gcc -o data_test -Wall -O2 -ggdb data_test.c $(HASTUR_SOURCE) -lpthread

library: $(HASTUR_SOURCE:.c=.o)
	libtool -static $(HASTUR_SOURCE:.c=.o) -o libhastur.a
