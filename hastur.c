#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>  /* For EINVAL */

#include <pthread.h>

#include "hastur.h"
#include "hastur_helpers.h"

static pthread_mutex_t hastur_mutex = PTHREAD_MUTEX_INITIALIZER;

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
int hastur_ ## msg_name ( params ) {                                                \
  time_t timestamp = hastur_timestamp();                                            \
  const char *json;                                                                 \
  int ret_code = 0;                                                                 \
  pthread_mutex_lock(&hastur_mutex);                                                \
  json = __hastur_format_json( #msg_name ,                                          \
                               sent_params,                                         \
                               "timestamp", HASTUR_LONG, timestamp,                 \
                               "labels", HASTUR_BARE, __hastur_default_labels(),    \
                               NULL);                                               \
  ret_code = json ? __hastur_send(json) : HASTUR_JSON_ERROR;                        \
  pthread_mutex_unlock(&hastur_mutex);                                              \
  return ret_code;                                                                  \
}                                                                                   \
                                                                                    \
int hastur_ ## msg_name ## _v(params, time_t timestamp, ...) {                      \
  const char *json;                                                                 \
  va_list argp;                                                                     \
  const char *labels;                                                               \
  int ret_code = 0;                                                                 \
  pthread_mutex_lock(&hastur_mutex);                                                \
  va_start(argp, timestamp);                                                        \
  labels = __hastur_generate_labels(argp);                                          \
  va_end(argp);                                                                     \
  if(timestamp == 0) { timestamp = hastur_timestamp(); }                            \
  json = __hastur_format_json( #msg_name ,                                          \
                               sent_params,                                         \
                               "timestamp", HASTUR_LONG, timestamp,                 \
                               "labels", HASTUR_BARE, labels,                       \
                               NULL);                                               \
  ret_code = json ? __hastur_send(json) : HASTUR_JSON_ERROR;                        \
  pthread_mutex_unlock(&hastur_mutex);                                              \
  return ret_code;                                                                  \
}                                                                                   \
                                                                                    \
int hastur_ ## msg_name ## _labelstr(params, time_t timestamp,                      \
                                     const char *labels) {                          \
  const char *json;                                                                 \
  int ret_code = 0;                                                                 \
  if(timestamp == 0) { timestamp = hastur_timestamp(); }                            \
  pthread_mutex_lock(&hastur_mutex);                                                \
  json = __hastur_format_json( #msg_name ,                                          \
                               sent_params,                                         \
                               "timestamp", HASTUR_LONG, timestamp,                 \
                               "labels", HASTUR_BARE, labels,                       \
                               NULL);                                               \
  ret_code = json ? __hastur_send(json) : HASTUR_JSON_ERROR;                        \
  pthread_mutex_unlock(&hastur_mutex);                                              \
  return ret_code;                                                                  \
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

ALL_MESSAGE_FUNCS(event, WRAP4(const char *name, const char *subject, const char *body,
			       const char *attn),
		  WRAP4(HASTUR_STRING_LABEL("name", name),
			HASTUR_STRING_LABEL("subject", subject),
			HASTUR_STRING_LABEL("body", body),
			HASTUR_COMMA_SEPARATED_LABEL("attn", attn)));

ALL_MESSAGE_FUNCS(log, WRAP2(const char *subject, const char *json_data),
		  WRAP2(HASTUR_STRING_LABEL("subject", subject),
			HASTUR_STRING_LABEL("data", json_data)));

ALL_MESSAGE_FUNCS(reg_process, WRAP1(const char *json_data),
		  WRAP1(HASTUR_BARE_LABEL("data", json_data)));

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

static pthread_t hastur_background_thread;
static int hastur_background_thread_initialized = 0;
static int hastur_started = 0;
pthread_t __hastur_start_thread;

pthread_t hastur_get_bg_thread_id(void) {
  if(hastur_background_thread_initialized)
    return hastur_background_thread;
  return 0;
}

static int hastur_no_background_thread_set = 0;

void hastur_no_background_thread(void) {
  if(hastur_background_thread_initialized) {
    fprintf(stderr, "Background thread is already running and you called hastur_no_background_thread!\n");
    exit(HASTUR_EXIT_THREAD_CONFIG);
  }

  hastur_no_background_thread_set = 1;
}

typedef struct scheduler_entry_struct {
  int period;
  periodic_call_type callback;
  void *user_data;
  struct scheduler_entry_struct *next;
} scheduler_entry_t;

scheduler_entry_t *hastur_scheduler_entries = NULL;

/* Not really.  There's four periods.  But this is plenty big for expansion. */
#define PERIODS 20

static bg_time_with_type hastur_bg_time_with_callback;
static void *hastur_bg_time_with_user_data;

void hastur_bg_time_with(bg_time_with_type callback, void *user_data) {
  hastur_bg_time_with_callback = callback;
  hastur_bg_time_with_user_data = user_data;
}

static time_t hastur_bg_thread_time(void) {
  if(!hastur_bg_time_with_callback)
    return time(0);

  return hastur_bg_time_with_callback(hastur_bg_time_with_user_data);
}

static void every_minute_heartbeat(void *user_data) {
  hastur_hb_process("process_heartbeat", 0.0, 60.0);
}

/**
 * The background thread's top-level function.
 */
static void *hastur_run_background_thread(void* user_data) {
  scheduler_entry_t *copied_table;
  scheduler_entry_t *index;
  time_t last_run[PERIODS];

  memset(last_run, 0, sizeof(time_t) * PERIODS);

  hastur_every(HASTUR_MINUTE, every_minute_heartbeat, NULL);

  while(1) {
    int will_run[PERIODS];
    time_t now;
    int num_entries = 0;
    int i;

    now = hastur_bg_thread_time();

    memset(will_run, 0, sizeof(int) * PERIODS);

    pthread_mutex_lock(&hastur_mutex);
    will_run[HASTUR_FIVE_SECONDS] = ((now - last_run[HASTUR_FIVE_SECONDS]) >= 5);
    will_run[HASTUR_MINUTE] = ((now - last_run[HASTUR_MINUTE]) >= 60);
    will_run[HASTUR_HOUR] = ((now - last_run[HASTUR_HOUR]) >= 60 * 60);
    will_run[HASTUR_DAY] = ((now - last_run[HASTUR_DAY]) >= 60 * 60 * 24);

    /* Find the number of entries */
    index = hastur_scheduler_entries;
    while(index) { index = index->next; num_entries++; }

    /* Allocate a table of that many entries */
    copied_table = num_entries > 0 ? malloc(sizeof(scheduler_entry_t) * num_entries) : NULL;

    /* Copy the entries from the list into the table */
    index = hastur_scheduler_entries;
    i = 0;
    while(index) { copied_table[i] = *index; index = index->next; i++; }

    index = copied_table;
    pthread_mutex_unlock(&hastur_mutex);

    /* Now, *not* holding the mutex, run the due entries in the table */
    for(i = 0; i < num_entries; i++) {
      if(will_run[index->period]) {
	/* Run this item */
	index->callback(index->user_data);
      }

      index++;
    }

    if(copied_table)
      free(copied_table);

    pthread_mutex_lock(&hastur_mutex);
    if(will_run[HASTUR_FIVE_SECONDS])
      last_run[HASTUR_FIVE_SECONDS] = now;
    if(will_run[HASTUR_MINUTE])
      last_run[HASTUR_MINUTE] = now;
    if(will_run[HASTUR_HOUR])
      last_run[HASTUR_HOUR] = now;
    if(will_run[HASTUR_DAY])
      last_run[HASTUR_DAY] = now;
    pthread_mutex_unlock(&hastur_mutex);

    sleep(1);
  }

  return NULL;
}

int hastur_start(void) {
  const char *app_name = hastur_get_app_name();

  if(!app_name)
    app_name = "unregistered";

  if(hastur_started) {
    if(!pthread_equal(__hastur_start_thread, pthread_self())) {
      fprintf(stderr, "hastur_start called from two different threads!"
	      "  Hastur_start must always be called from the main thread!");
      exit(HASTUR_EXIT_THREAD_CONFIG);
    }

  } else {
    __hastur_start_thread = pthread_self();
    hastur_reg_process("{}");

    hastur_started = 1;
  }

  if(!hastur_no_background_thread_set && !hastur_background_thread_initialized) {
    int status = pthread_create(&hastur_background_thread, NULL, hastur_run_background_thread, NULL);

    if(status < 0) {
      fprintf(stderr, "Error (%d) creating hastur background thread!\n", status);
      exit(HASTUR_EXIT_THREAD_ERROR);
    }

    hastur_background_thread_initialized = 1;
  }

  return 0;
}

int hastur_every(int period, periodic_call_type callback, void *user_data) {
  scheduler_entry_t *entry;

  if(period < HASTUR_FIVE_SECONDS || period > HASTUR_DAY) {
    return EINVAL;  /* Argument error */
  }

  entry = malloc(sizeof(scheduler_entry_t));
  entry->period = period;
  entry->callback = callback;
  entry->user_data = user_data;
  entry->next = NULL;

  pthread_mutex_lock(&hastur_mutex);
  entry->next = hastur_scheduler_entries;
  hastur_scheduler_entries = entry;
  pthread_mutex_unlock(&hastur_mutex);

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
