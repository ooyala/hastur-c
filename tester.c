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

  return 0;
}
