#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test_helper.h"
#include "hastur.h"

#define SECOND 1
#define MINUTE 60
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)

time_t time_plus_n(void *user_data) {
  int n = (long int)user_data;

  return time(0) + n;
}

int main(int argc, char **argv) {
  hastur_deliver_with(check_message_expectation, NULL);
  hastur_timestamp_with(use_fake_timestamp, NULL);
  hastur_set_app_name("thread_tester");

  expect_message("{\"type\":\"reg_process\","
                 "\"data\":{\"language\":\"c\",\"version\":\"0.0.1\"},"
                 "\"timestamp\":" FAKE_NOW_TIME_STRING ","
                 "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  /* Delay 2 seconds before background thread starts */
  hastur_delay_bg_thread_start(2);
  hastur_start();

  /* BG thread is delaying, but exists so we can use "-BG-TID-" successfully. */
  expect_message("{\"type\":\"hb_process\","
		 "\"name\":\"process_heartbeat\","
		 "\"value\":0,"
		 "\"timeout\":60,"
		 "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		 "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"-BG-TID-\"}}", NULL);

  sleep(4);  /* Let them both happen */
  satisfied_expectations();

  /* Now stop using expectations and check directly */
  hastur_deliver_with(copy_to_message_buf, NULL);

  /* Test a simple foreground counter */
  hastur_counter("my.counter", 7);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"my.counter\","
		       "\"value\":7,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  /* Skip time forward to the following minute */
  hastur_bg_time_with(time_plus_n, (void*)(1 * MINUTE + SECOND));
  sleep(2);

  /* Make sure next heartbeat arrives on schedule */
  assert_message_equal("{\"type\":\"hb_process\","
		       "\"name\":\"process_heartbeat\","
		       "\"value\":0,"
		       "\"timeout\":60,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"thread_tester\",\"pid\":-PID-,\"tid\":\"-BG-TID-\"}}", NULL);

  print_assertion_messages();

  return assertion_success();
}
