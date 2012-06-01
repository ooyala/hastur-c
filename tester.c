#include <stdio.h>
#include <string.h>

#include "hastur.h"

/* Deliver_with handlers: */

int print_message(const char *message, void *user_data) {
  printf("Hastur message:\n%s\n", message);

  return 0;
}

int copy_message(const char *message, void *copy_to) {
  strcpy(copy_to, message);

  return 0;
}

/* Main */

int main(int argc, char **argv) {
  hastur_deliver_with(print_message, NULL);

  hastur_set_app_name(argv[0]);

  hastur_counter("my.counter", 7);

  hastur_counter_v("labeled.counter", 1, HASTUR_NOW, "mylabel1", HASTUR_INT, 7, "mylabel2", HASTUR_STRING, "bobo", NULL);

  hastur_counter_v("labeled.counter.2", 1, HASTUR_NOW,
		   HASTUR_INT_LABEL("mylabel1", 7),
		   HASTUR_STRING_LABEL("mylabel2", "bobo"),
		   NULL);

  return 0;
}
