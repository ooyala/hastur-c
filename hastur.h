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
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Strings are turned into JSON strings.
 */
#define HASTUR_STRING   1

/**
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Strings are turned into JSON strings.
 */
#define HASTUR_STR      1

/**
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Ints are turned into JSON integers.
 */
#define HASTUR_INT      2

/**
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Longs are turned into JSON integers.
 */
#define HASTUR_LONG     3

/**
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * "Bare" values are inserted directly into the JSON output.
 */
#define HASTUR_BARE     4

/**
 * These labels are passed to functions ending in _v as the types in
 * a label/type/value triple.
 *
 * Doubles are turned into JSON floating-point numbers.  Bare
 * floating-point constants in C are already double, so you don't have
 * to typecast.
 */
#define HASTUR_DOUBLE     5

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
 * If HASTUR_NOW is passed as a timestamp, Hastur will instead query the current time.
 */
#define HASTUR_NOW 0

/**
 * Return a current timestamp in microseconds since the Unix epoch.
 */
time_t hastur_timestamp(void);

/**
 * Send a counter with the given name.
 */
int hastur_counter(const char *name, int value);

/**
 * Send a counter with the given name, value and timestamp, and the
 * specified labels.  Labels are specified with the
 * HASTUR_[TYPE]_LABEL macros, followed by a trailing NULL.
 */
int hastur_counter_v(const char *name, int value, time_t timestamp, ...);

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
