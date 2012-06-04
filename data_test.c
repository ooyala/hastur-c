#include <stdio.h>

#include "hastur.h"

int main(int argc, char **argv) {
  printf("Sending Hastur test messages!  Look for hastur.c.test.* message names to retrieve them.\n");

  hastur_counter("hastur.c.test.counter", 1);
  hastur_event("hastur.c.test.event", "Testing!", "no body", "");

  printf("Done.\n");

  return 0;
}
