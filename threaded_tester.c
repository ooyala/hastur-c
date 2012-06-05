#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test_helper.h"
#include "hastur.h"

int main(int argc, char **argv) {
  hastur_deliver_with(copy_to_buf, NULL);
  hastur_timestamp_with(use_fake_timestamp, NULL);
  hastur_set_app_name("thread_tester");

  /* No process registration will be sent over UDP because we set deliver_with */
  hastur_start();

  hastur_counter("my.counter", 7);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"my.counter\","
		       "\"value\":7,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  print_assertion_messages();

  return 0;
}
