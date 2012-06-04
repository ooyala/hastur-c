HASTUR_SOURCE=hastur.c hastur_helpers.c hastur_string_builder.c

all: library tester data_test

tester: tester.c $(HASTUR_SOURCE)
	gcc -o tester -Wall -O2 -ggdb tester.c $(HASTUR_SOURCE)

data_test: data_test.c $(HASTUR_SOURCE)
	gcc -o data_test -Wall -O2 -ggdb data_test.c $(HASTUR_SOURCE)

library: $(HASTUR_SOURCE:.c=.o)
	libtool -static $(HASTUR_SOURCE:.c=.o) -o libhastur.a
