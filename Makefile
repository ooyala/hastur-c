# Let's just do this ghetto and rebuild every time.

HASTUR_SOURCE=hastur.c hastur_helpers.c hastur_stringbuilder.c

all: library tester

tester: tester.c $(HASTUR_SOURCE)
	gcc -o tester -Wall -O2 -ggdb tester.c $(HASTUR_SOURCE)

library: $(HASTUR_SOURCE:.c=.o)
	libtool -static $(HASTUR_SOURCE:.c=.o) -o libhastur.a
