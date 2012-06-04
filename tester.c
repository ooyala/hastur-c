#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hastur.h"

static int failed_assertions = 0;
static int total_assertions = 0;

static int assert_equal(const char *expected, const char *actual, const char *message) {
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

/* Deliver_with handlers: */

int print_message(const char *message, void *user_data) {
  printf("Hastur message:\n%s\n", message);

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

/* Timestamping code */

#define FAKE_NOW_TIMESTAMP 1338829820286099
#define FAKE_NOW_TIME_STRING "1338829820286099"

int use_fake_timestamp(time_t *timestamp, void *user_data) {
  *timestamp = FAKE_NOW_TIMESTAMP;
  return 0;
}

/* Main */

int main(int argc, char **argv) {
  hastur_deliver_with(copy_to_buf, NULL);
  hastur_timestamp_with(use_fake_timestamp, NULL);
  hastur_set_app_name("tester");

  hastur_counter("my.counter", 7);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"my.counter\","
		       "\"value\":7,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  hastur_counter_v("labeled.counter", 1, HASTUR_NOW, "mylabel1", HASTUR_INT, 7, "mylabel2", HASTUR_STRING, "bobo", NULL);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"labeled.counter\","
		       "\"value\":1,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"mylabel1\":7,\"mylabel2\":\"bobo\","
		       "\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  hastur_counter_v("labeled.counter.fake_pid", 1, HASTUR_NOW,
		   HASTUR_INT_LABEL("mylabel1", 7),
		   HASTUR_STRING_LABEL("mylabel2", "bobo"),
		   HASTUR_INT_LABEL("pid", 319),
		   NULL);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"labeled.counter.fake_pid\","
		       "\"value\":1,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"mylabel1\":7,\"mylabel2\":\"bobo\",\"pid\":319,"
		       "\"app\":\"tester\",\"tid\":\"main\"}}", NULL);

  hastur_event("my.event", "OMG!  3175 foozles warbled!",
	       "no body to love...", "bob,jim,555-1212");

  assert_message_equal("{\"type\":\"event\","
		       "\"name\":\"my.event\","
		       "\"subject\":\"OMG!  3175 foozles warbled!\","
		       "\"body\":\"no body to love...\","
		       "\"attn\":[\"bob\",\"jim\",\"555-1212\"],"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}"
		       "}", NULL);

  hastur_event_v("single.attn.event", "OMG!  3175 foozles warbled!",
		 "no body to love...", "555-1212", HASTUR_NOW,
		 HASTUR_STRING_LABEL("label", "some_value"),
		 HASTUR_STRING_LABEL("app", "also tester!"), NULL);

  assert_message_equal("{\"type\":\"event\","
		       "\"name\":\"single.attn.event\","
		       "\"subject\":\"OMG!  3175 foozles warbled!\","
		       "\"body\":\"no body to love...\","
		       "\"attn\":[\"555-1212\"],"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{"
		         "\"label\":\"some_value\",\"app\":\"also tester!\",\"pid\":-PID-,\"tid\":\"main\"}"
		       "}", NULL);

  hastur_event("my.event", "OMG!  3175 foozles warbled!",
	       "no body to love...", "");

  assert_message_equal("{\"type\":\"event\","
		       "\"name\":\"my.event\","
		       "\"subject\":\"OMG!  3175 foozles warbled!\","
		       "\"body\":\"no body to love...\","
		       "\"attn\":[],"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}"
		       "}", NULL);

  fprintf(stderr, "\n\n");
  fprintf(stderr, "Total assertions: %d\n", total_assertions);
  fprintf(stderr, "Correct assertions: %d\n", total_assertions - failed_assertions);
  fprintf(stderr, "Failed assertions: %d\n", failed_assertions);

  return 0;
}
