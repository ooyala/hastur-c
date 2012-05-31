#include <stdio.h>

#include "hastur.h"

int print_message(const char *message, void *user_data) {
  printf("Hastur message:\n%s\n", message);

  return 0;
}

int main(int argc, char **argv) {
  hastur_deliver_with(print_message, NULL);

  hastur_counter("my.counter", 7);

  return 0;
}
