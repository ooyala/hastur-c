#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test_helper.h"
#include "hastur.h"

/* Main */

int main(int argc, char **argv) {
  hastur_deliver_with(copy_to_message_buf, NULL);
  hastur_timestamp_with(use_fake_timestamp, NULL);
  hastur_set_app_name("tester");

  /* No process registration will be sent over UDP because we set deliver_with */
  hastur_no_background_thread();
  hastur_start();

  assert_message_equal("{\"type\":\"reg_process\","
		       "\"data\":{\"language\":\"c\",\"version\":\"0.0.1\"},"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  hastur_counter("my.counter", 7);
  assert_message_equal("{\"type\":\"counter\","
		       "\"name\":\"my.counter\","
		       "\"value\":7,"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}}", NULL);

  hastur_counter_v("labeled.counter", 1, HASTUR_NOW,
		   "mylabel1", HASTUR_INT, 7, "mylabel2", HASTUR_STRING, "bobo", NULL);
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

  hastur_set_message_name_prefix("ots.team.");
  hastur_event("my.event", "OMG!  3175 foozles warbled!",
	       "no body to love...", "");
  hastur_set_message_name_prefix(NULL);

  assert_message_equal("{\"type\":\"event\","
		       "\"name\":\"ots.team.my.event\","
		       "\"subject\":\"OMG!  3175 foozles warbled!\","
		       "\"body\":\"no body to love...\","
		       "\"attn\":[],"
		       "\"timestamp\":" FAKE_NOW_TIME_STRING ","
		       "\"labels\":{\"app\":\"tester\",\"pid\":-PID-,\"tid\":\"main\"}"
		       "}", NULL);

  print_assertion_messages();

  return assertion_success();
}
