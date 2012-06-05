#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test_helper.h"
#include "hastur.h"

time_t time_plus_n(void *user_data) {
  int n = (long int)user_data;

  return time(0) + n;
}

int main(int argc, char **argv) {
  hastur_deliver_with(copy_to_buf, NULL);
  hastur_timestamp_with(use_fake_timestamp, NULL);
  hastur_set_app_name("thread_tester");

  /* hastur_bg_time_with(time_plus_n, (void*)0); */

  /* Process registration will be captured by copy_to_buf */
  hastur_start();
  assert_message_equal("{\"type\":\"reg_process\","
		       "\"data\":{},"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  hastur_counter("my.counter", 7);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"my.counter\","
		       "\"value\":7,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  print_assertion_messages();

  return 0;
}
