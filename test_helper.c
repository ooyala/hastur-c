#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_helper.h"

static int failed_assertions = 0;
static int total_assertions = 0;

int use_fake_timestamp(time_t *timestamp, void *user_data) {
  *timestamp = FAKE_NOW_TIMESTAMP;
  return 0;
}

#define BUFLEN (64 * 1024)
static char buf[BUFLEN];

int copy_to_buf(const char *message, void *user_data) {
  strncpy(buf, message, BUFLEN);

  return 0;
}

int assert_message_equal(const char *expected_value, const char *message) {
  /* Need to check against the actual PID of this process. */
  char expected_with_pid[BUFLEN];
  int pid = getpid();
  const char *old_pid_location;
  char *new_pid_location;
  const char *value_suffix;

  old_pid_location = strstr(expected_value, "-PID-");
  if(old_pid_location) {
    /* Suffix starts after "-PID-" */
    value_suffix = old_pid_location + strlen("-PID-");

    /* Copy everything before "-PID-" into place */
    memmove(expected_with_pid, expected_value, old_pid_location - expected_value);

    /* Copy the actual PID into place along with the suffix */
    new_pid_location = expected_with_pid + (old_pid_location - expected_value);
    sprintf(new_pid_location, "%d%s", pid, value_suffix);
  } else {
    /* No "-PID-" in string */
    strcpy(expected_with_pid, expected_value);
  }

  assert_equal(expected_with_pid, buf, message);

  return 0;
}

int assert_equal(const char *expected, const char *actual, const char *message) {
  total_assertions++;

  if(strcmp(expected, actual)) {
    failed_assertions++;

    fprintf(stderr, "String \'%s\' and string \'%s\' are not equal!\n",
	    expected, actual);
    if(message)
      fprintf(stderr, "%s", message);
  }

  return 0;
}

void print_assertion_messages(void) {
  fprintf(stderr, "Total assertions: %d\n", total_assertions);
  fprintf(stderr, "Correct assertions: %d\n", total_assertions - failed_assertions);
  fprintf(stderr, "Failed assertions: %d\n", failed_assertions);
}
