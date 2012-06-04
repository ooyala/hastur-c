#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#include "hastur.h"
#include "hastur_helpers.h"

/* Return an error code meaning that JSON wasn't rendered correctly */
#define JSON_ERROR (-1)

/* Fake out the macro system so I can pass multiple underlying
   arguments through a single macro argument */
#define WRAP1(a) a
#define WRAP2(a, b) a, b
#define WRAP3(a, b, c) a, b, c
#define WRAP4(a, b, c, d) a, b, c, d
#define WRAP5(a, b, c, d, e) a, b, c, d, e

/* Define all the various methods to send Hastur messages.  They're
   very repetitive, so we set up macros to generate them and then call
   those macros. */

/* You might ask, why not make multiple smaller macros and call them
   from a single ALL_MESSAGE_FUNCS top-level macro?  It's a reasonable
   approach.  But the horrid WRAP() thing I'm using to pass multiple
   arguments through a single macro argument makes that very
   difficult... */

#define ALL_MESSAGE_FUNCS(msg_name, params, sent_params)                            \
  int hastur_ ## msg_name ( params ) {                                              \
    time_t timestamp = hastur_timestamp();                                          \
    const char *json;                                                               \
    json = __hastur_format_json( #msg_name ,                                        \
                                 sent_params,                                       \
                                 "timestamp", HASTUR_LONG, timestamp,               \
                                 "labels", HASTUR_BARE, __hastur_default_labels(),  \
                                 NULL);                                             \
    return json ? __hastur_send(json) : JSON_ERROR;                                 \
}                                                                                   \
                                                                                    \
int hastur_ ## msg_name ## _v(params, time_t timestamp, ...) {                      \
  const char *json;                                                                 \
  va_list argp;                                                                     \
  const char *labels;                                                               \
  va_start(argp, timestamp);                                                        \
  labels = __hastur_generate_labels(argp);                                          \
  va_end(argp);                                                                     \
  if(timestamp == 0) { timestamp = hastur_timestamp(); }                            \
  json = __hastur_format_json( #msg_name ,                                          \
                               sent_params,                                         \
                               "timestamp", HASTUR_LONG, timestamp,                 \
                               "labels", HASTUR_BARE, labels,                       \
                               NULL);                                               \
  return json ? __hastur_send(json) : JSON_ERROR;                                   \
}                                                                                   \
                                                                                    \
int hastur_ ## msg_name ## _labelstr(params, time_t timestamp,                      \
                                     const char *labels) {                          \
  const char *json;                                                                 \
  if(timestamp == 0) { timestamp = hastur_timestamp(); }                            \
  json = __hastur_format_json( #msg_name ,                                          \
                               sent_params,                                         \
                               "timestamp", HASTUR_LONG, timestamp,                 \
                               "labels", HASTUR_BARE, labels,                       \
                               NULL);                                               \
  return json ? __hastur_send(json) : JSON_ERROR;                                   \
}

/* TODO: Event is separate */

/* Now, actually declare Hastur message functions */

ALL_MESSAGE_FUNCS(counter, WRAP2(const char *name, int value),
		  WRAP2(HASTUR_STRING_LABEL("name", name),
			HASTUR_INT_LABEL("value", value)));

ALL_MESSAGE_FUNCS(gauge, WRAP2(const char *name, double value),
		  WRAP2(HASTUR_STRING_LABEL("name", name),
			HASTUR_DOUBLE_LABEL("value", value)));

ALL_MESSAGE_FUNCS(mark, WRAP2(const char *name, const char *value),
		  WRAP2(HASTUR_STRING_LABEL("name", name),
			HASTUR_STRING_LABEL("value", value)));

ALL_MESSAGE_FUNCS(log, WRAP2(const char *subject, const char *json_data),
		  WRAP2(HASTUR_STRING_LABEL("subject", subject),
			HASTUR_STRING_LABEL("data", json_data)));

ALL_MESSAGE_FUNCS(reg_process, WRAP2(const char *name, const char *json_data),
		  WRAP2(HASTUR_STRING_LABEL("name", name),
			HASTUR_STRING_LABEL("data", json_data)));

ALL_MESSAGE_FUNCS(info_process, WRAP2(const char *tag, const char *json_data),
		  WRAP2(HASTUR_STRING_LABEL("tag", tag),
			HASTUR_STRING_LABEL("data", json_data)));

ALL_MESSAGE_FUNCS(info_agent, WRAP2(const char *tag, const char *json_data),
		  WRAP2(HASTUR_STRING_LABEL("tag", tag),
			HASTUR_STRING_LABEL("data", json_data)));

ALL_MESSAGE_FUNCS(reg_pluginv1, WRAP4(const char *name, const char *plugin_path,
                                         const char *plugin_args, double plugin_interval),
		  WRAP4(HASTUR_STRING_LABEL("name", name),
			HASTUR_STRING_LABEL("plugin_path", plugin_path),
			HASTUR_STRING_LABEL("plugin_args", plugin_args),
			HASTUR_DOUBLE_LABEL("interval", plugin_interval)));

ALL_MESSAGE_FUNCS(hb_process, WRAP3(const char *name, double value, double timeout),
		  WRAP3(HASTUR_STRING_LABEL("name", name),
			HASTUR_DOUBLE_LABEL("value", value),
			HASTUR_DOUBLE_LABEL("timeout", timeout)));

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

static timestamp_with_type hastur_timestamp_with_callback = NULL;
static void *hastur_timestamp_with_user_data = NULL;

timestamp_with_type hastur_get_timestamp_with(void) {
  return hastur_timestamp_with_callback;
}

void *hastur_get_timestamp_with_user_data(void) {
  return hastur_timestamp_with_user_data;
}

void hastur_timestamp_with(timestamp_with_type callback, void *user_data) {
  hastur_timestamp_with_callback = callback;
  hastur_timestamp_with_user_data = user_data;
}

time_t hastur_timestamp(void) {
  struct timeval tv;

  if(hastur_timestamp_with_callback) {
    time_t ret;

    hastur_timestamp_with_callback(&ret, hastur_timestamp_with_user_data);
    return ret;
  }

  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000000 + tv.tv_usec;
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
