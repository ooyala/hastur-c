#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#include "hastur.h"
#include "hastur_helpers.h"

/* Return an error code meaning that JSON wasn't rendered correctly */
#define JSON_ERROR (-1)

time_t hastur_timestamp(void) {
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int hastur_counter(const char *name, int value) {
  const char *json = __format_json("counter",
				   "name", HVALUE_STR, name,
				   "value", HVALUE_INT, value,
				   "timestamp", HVALUE_LONG, hastur_timestamp(),
				   "labels", HVALUE_BARE, __default_labels(),
				   NULL);

  return json ? __hastur_send(json) : JSON_ERROR;
}

int hastur_counter_v(const char *name, int value, time_t timestamp, ...) {
  return 0;
}

static int hastur_agent_port = 8150;

int hastur_get_agent_port(void) {
  return hastur_agent_port;
}

void hastur_set_agent_port(int port) {
  hastur_agent_port = port;
}

static deliver_with_type hastur_deliver_with_callback = NULL;
static void *hastur_deliver_with_user_data = NULL;

deliver_with_type hastur_get_deliver_with(void) {
  return hastur_deliver_with_callback;
}

void *hastur_get_deliver_with_user_data(void) {
  return hastur_deliver_with_user_data;
}

void hastur_deliver_with(deliver_with_type callback, void *user_data) {
  hastur_deliver_with_callback = callback;
  hastur_deliver_with_user_data = user_data;
}

static char *hastur_app_name = NULL;

const char* hastur_get_app_name(void) {
  return hastur_app_name;
}

void hastur_set_app_name(const char *app_name) {
  if(hastur_app_name) {
    free(hastur_app_name);
  }
  hastur_app_name = strdup(app_name);
}
