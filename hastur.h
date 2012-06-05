#ifndef __HASTUR_CLIENT__
#define __HASTUR_CLIENT__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file hastur.h
 * @brief Hastur client public API
 *
 * This file contains the full public Hastur client API as exposed by
 * this library.  You can find more general documentation including
 * hints and tips for message structure in the README.md file included
 * with this library.
 */

#include <time.h>

/**
 * These constants are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Strings are turned into JSON strings.
 */
#define HASTUR_STRING   1

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * Strings are turned into JSON strings.
 */
#define HASTUR_STR      1

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * Ints are turned into JSON integers.
 */
#define HASTUR_INT      2

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * Longs are turned into JSON integers.
 */
#define HASTUR_LONG     3

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * "Bare" values are inserted directly into the JSON output.
 */
#define HASTUR_BARE     4

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * Doubles are turned into JSON floating-point numbers.  Bare
 * floating-point constants in C are already double, so you don't have
 * to typecast.
 */
#define HASTUR_DOUBLE   5

/**
 * These constants are passed to functions ending in _v as the types
 * in a label/type/value triple.
 *
 * Comma-separated arrays are turned into JSON arrays of strings.
 * These aren't generally needed for labels, but are used internally
 * for events and could be useful for other kinds of rendering in some
 * cases.
 */
#define HASTUR_COMMA_SEPARATED_ARRAY  6

/**
 * Declare a label of type string.
 */
#define HASTUR_STR_LABEL(label, value) (label), HASTUR_STRING, (const char *)(value)

/**
 * Declare a label of type string.
 */
#define HASTUR_STRING_LABEL(label, value) (label), HASTUR_STRING, (const char *)(value)

/**
 * Declare a label of type integer.
 */
#define HASTUR_INT_LABEL(label, value) (label), HASTUR_INT, (int)(value)

/**
 * Declare a label of type long.
 */
#define HASTUR_LONG_LABEL(label, value) (label), HASTUR_LONG, (long)(value)

/**
 * Declare a label with a bare JSON string as its contents
 */
#define HASTUR_BARE_LABEL(label, value) (label), HASTUR_BARE, (const char *)(value)

/**
 * Declare a label of type double.  Also usable for floats.
 */
#define HASTUR_DOUBLE_LABEL(label, value) (label), HASTUR_DOUBLE, (double)(value)

/**
 * Declare a string label to be split on strings.
 */
#define HASTUR_COMMA_SEPARATED_LABEL(label, value) \
  (label), HASTUR_COMMA_SEPARATED_ARRAY, (const char *)(value)

/**
 * If HASTUR_NOW is passed as a timestamp, Hastur will instead query the current time.
 */
#define HASTUR_NOW 0

/**
 * This error means that Hastur got an otherwise-unclassified error
 * sending via UDP.  More information might have been printed to
 * stderr.
 */
#define HASTUR_UDP_ERROR    -1

/**
 * This error means that Hastur got an otherwise-unclassified error
 * creating serialized JSON.  More information might have been printed
 * to stderr.
 */
#define HASTUR_JSON_ERROR    -2

/* Process exit codes */

/**
 * This error occurs when the user misuses threads in a way that
 * Hastur isn't okay with, usually by calling hastur_start() from a
 * background thread or by calling hastur_no_background_thread() when
 * one already exists.
 */
#define HASTUR_EXIT_THREAD_CONFIG     1

/**
 * This error means a thread operation has failed, often thread
 * creation.
 */
#define HASTUR_EXIT_THREAD_ERROR      2


/**
 * If this function has been called, do not start a background thread
 * when calling hastur_start().  Exit and die if a background thread has
 * already been started.
 */
void hastur_no_background_thread(void);

/**
 * Start the Hastur client.  Send process registration.  Start a
 * background thread unless specifically instructed not to via
 * hastur_no_background_thread().
 */
int hastur_start(void);

/**
 * Return a current timestamp in microseconds since the Unix epoch.
 */
time_t hastur_timestamp(void);

/**
 * Send a counter with the given name and value.
 */
int hastur_counter(const char *name, int value);

/**
 * Send a counter with the given name, value and timestamp, and the
 * specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.
 */
int hastur_counter_v(const char *name, int value, time_t timestamp, ...);

/**
 * Send a counter with the given name, value and timestamp, and the
 * specified labels.  Labels are specified as a JSON string.
 */
int hastur_counter_labelstr(const char *name, int value, time_t timestamp, const char *labels);

/**
 * Send a gauge with the given name and value.
 */
int hastur_gauge(const char *name, double value);

/**
 * Send a gauge with the given name, value and timestamp, and the
 * specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.
 */
int hastur_gauge_v(const char *name, double value, time_t timestamp, ...);

/**
 * Send a gauge with the given name, value and timestamp, and the
 * specified labels.  Labels are specified as a JSON string.
 */
int hastur_gauge_labelstr(const char *name, double value, time_t timestamp, const char *labels);

/**
 * Send a mark with the given name and value.
 */
int hastur_mark(const char *name, const char *value);

/**
 * Send a mark with the given name, value and timestamp, and the
 * specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.
 */
int hastur_mark_v(const char *name, const char *value, time_t timestamp, ...);

/**
 * Send a mark with the given name, value and timestamp, and the
 * specified labels.  Labels are specified as a JSON string.
 */
int hastur_mark_labelstr(const char *name, const char *value, time_t timestamp, const char *labels);

/**
 * Send an event with the given subject and data.  The event name is
 * similar to other message names.  The subject is specific to the
 * individual event sent.  The body is further data (stack trace,
 * contributing statistics, error messages for human readers).  Attn
 * lists to whose attention the event should be brought -- email
 * addresses, team names, phone numbers or other identifying
 * information.  The attn list is a comma-separated string.
 */
int hastur_event(const char *name, const char *subject, const char *body, const char *attn);

/**
 * Send an event with the given subject, data, timestamp and labels.
 * The event name is similar to other message names.  The subject is
 * specific to the individual event sent.  The body is further data
 * (stack trace, contributing statistics, error messages for human
 * readers).  Attn lists to whose attention the event should be
 * brought -- email addresses, team names, phone numbers or other
 * identifying information.  The attn list is a comma-separated
 * string.  Labels are specified with the HASTUR_[TYPE]_LABEL macros
 * followed by a trailing NULL.
 */
int hastur_event_v(const char *name, const char *subject, const char *body, const char *attn,
		   time_t timestamp, ...);

/**
 * Send an event with the given subject, data, timestamp and labels.
 * The event name is similar to other message names.  The subject is
 * specific to the individual event sent.  The body is further data
 * (stack trace, contributing statistics, error messages for human
 * readers).  Attn lists to whose attention the event should be
 * brought -- email addresses, team names, phone numbers or other
 * identifying information.  The attn list is a comma-separated
 * string.  Labels are specified as a JSON string.
 */
int hastur_event_labelstr(const char *name, const char *subject, const char *body, const char *attn,
			  time_t timestamp, const char *labels);

/**
 * Send a log with the given subject and data.  The JSON data is a
 * JSON object, given as a string.
 */
int hastur_log(const char *subject, const char *json_data);

/**
 * Send a log with the given subject, data and timestamp, and the
 * specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_log_v(const char *subject, const char *json_data, time_t timestamp, ...);

/**
 * Send a log with the given subject, data and timestamp, and the
 * specified labels.  Labels are specified as a JSON string.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_log_labelstr(const char *subject, const char *json_data, time_t timestamp, const char *labels);

/**
 * Send a process registration with the given tag and data.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_reg_process(const char *tag, const char *json_data);

/**
 * Send a process registration with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_reg_process_v(const char *tag, const char *json_data, time_t timestamp, ...);

/**
 * Send a process registration with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified as a JSON string.
 * The JSON data is a JSON object, given as a string.
 */
int hastur_reg_process_labelstr(const char *tag, const char *json_data, time_t timestamp, const char *labels);

/**
 * Send process information with the given tag and data.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_info_process(const char *tag, const char *json_data);

/**
 * Send process information with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_info_process_v(const char *tag, const char *json_data, time_t timestamp, ...);

/**
 * Send process information with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified as a JSON string.
 * The JSON data is a JSON object, given as a string.
 */
int hastur_info_process_labelstr(const char *tag, const char *json_data, time_t timestamp, const char *labels);

/**
 * Send agent information with the given tag and data.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_info_agent(const char *tag, const char *json_data);

/**
 * Send agent information with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros followed by a trailing NULL.  The JSON
 * data is a JSON object, given as a string.
 */
int hastur_info_agent_v(const char *tag, const char *json_data, time_t timestamp, ...);

/**
 * Send agent information with the given tag, data and timestamp,
 * and the specified labels.  Labels are specified as a JSON string.
 * The JSON data is a JSON object, given as a string.
 */
int hastur_info_agent_labelstr(const char *tag, const char *json_data, time_t timestamp, const char *labels);

/**
 * Alias hb_process, the heartbeat message, as "heartbeat" for convenience.
 */
#define hastur_heartbeat hastur_hb_process

/**
 * Alias hb_process, the heartbeat message, as "heartbeat" for convenience.
 */
#define hastur_heartbeat_v hastur_hb_process_v

/**
 * Alias hb_process, the heartbeat message, as "heartbeat" for convenience.
 */
#define hastur_heartbeat_labelstr hastur_hb_process_labelstr

/** Period for hastur_every */
#define HASTUR_FIVE_SECONDS  1

/** Period for hastur_every */
#define HASTUR_MINUTE        2

/** Period for hastur_every */
#define HASTUR_HOUR          3

/** Period for hastur_every */
#define HASTUR_DAY           4

/**
 * A function type for periodic calls.  User_data can be ignored
 * if desired.
 */
typedef void (*periodic_call_type)(void *user_data);

/**
 * The given callback is scheduled to be called approximately each
 * given period from a pthreads background thread.  It will be called
 * with the given user_data.  Hastur_every returns EINVAL if given
 * an invalid period.
 */
int hastur_every(int period, periodic_call_type callback, void *user_data);

/**
 * A function type for delivering messages.  User_data can be ignored
 * if desired.  Message is the message to be delivered.  The function
 * should return 0 for success and less than 0 for failure.
 */
typedef int (*deliver_with_type)(const char *message, void *user_data);

/**
 * Instruct Hastur to use the specified callback to deliver its
 * messages rather than sending them over UDP.  The user_data
 * parameter will be passed to the specified callback.
 */
void hastur_deliver_with(deliver_with_type callback, void *user_data);

/**
 * Get the callback passed to hastur_deliver_with, or NULL if none was
 * specified.
 */
deliver_with_type hastur_get_deliver_with(void);

/**
 * Get the user_data passed to hastur_deliver_with, or NULL if none
 * was specified.
 */
void *hastur_get_deliver_with_user_data(void);

/**
 * A function type for getting timestamps.  User_data can be ignored
 * if desired.  New_time is a pointer to the new timestamp as time_t.
 * The function should return 0 for success and less than 0 for
 * failure.
 */
typedef int (*timestamp_with_type)(time_t *new_time, void *user_data);

/**
 * Instruct Hastur to use the specified callback to get new timestamps
 * rather than using the current time.  The user_data parameter will
 * be passed to the specified callback.
 */
void hastur_timestamp_with(timestamp_with_type callback, void *user_data);

/**
 * Get the callback passed to hastur_timestamp_with, or NULL if none
 * was specified.
 */
timestamp_with_type hastur_get_timestamp_with(void);

/**
 * Get the user_data passed to hastur_timestamp_with, or NULL if none
 * was specified.
 */
void *hastur_get_timestamp_with_user_data(void);

/**
 * Set the local UDP port for the Hastur agent.  Default is 8150.
 */
int hastur_get_agent_port(void);

/**
 * Get the local UDP port for the Hastur agent.  Default is 8150 if
 * none is specified.
 */
void hastur_set_agent_port(int port);

/**
 * Get the application name, if any, that Hastur tags messages with.
 * Frequently argv[0] is a good value for this if you don't otherwise
 * know.  No default.
 */
const char* hastur_get_app_name(void);

/**
 * Set the application name that Hastur tags messages with.
 * Frequently argv[0] is a good value for this if you don't otherwise
 * know.  There is no default.
 */
void hastur_set_app_name(const char *app_name);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_CLIENT__ */
