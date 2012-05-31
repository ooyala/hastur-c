#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hastur.h"
#include "hastur_helpers.h"

/* Return an error code meaning that JSON wasn't rendered correctly */
#define JSON_ERROR (-1)

time_t hastur_timestamp(void) {
  struct timeval tv;

  gettimeofday(&tv, NULL);  /* TODO: test that this is UTC */

  return tv.tv_sec * 1_000_000 + tv.tv_usec;
}

int hastur_counter(const char *name, int value) {
  const char *json = format_json("counter",
				 "name", VALUE_STR, name,
				 "value", VALUE_INT, value,
				 "timestamp", VALUE_LONG, hastur_timestamp(),
				 "labels", VALUE_STR, "{}",
				 NULL);

  return json ? hastur_send(json) : JSON_ERROR;
}

static int hastur_agent_port = 8150;

int hastur_get_agent_port(void) {
  return hastur_agent_port;
}

void hastur_set_agent_port(int port) {
  hastur_agent_port = port;
}

static deliver_with_type hastur_deliver_with = NULL;
static void *hastur_deliver_with_user_data = NULL;

deliver_with_type hastur_get_deliver_with(void) {
  return hastur_deliver_with;
}

deliver_with_type hastur_get_deliver_with_user_data(void) {
  return hastur_deliver_with_user_data;
}

void hastur_deliver_with(deliver_with_type callback, void *user_data) {
  hastur_deliver_with = callback;
  hastur_deliver_with_user_data = user_data;
}
