#ifndef __HASTUR_CLIENT__
#define __HASTUR_CLIENT__

#ifdef __cplusplus
extern "C" {
#endif

/**  @file hastur.h
 *   @brief Hastur client public API
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
 * "Bare" values are inserted directly into JSON.
 */
#define HASTUR_BARE     4

/**
 * Return a current timestamp in microseconds since the Unix epoch.
 */
time_t hastur_timestamp(void);

/**
 * Send a counter with the given name.
 */
int hastur_counter(const char *name, int value);

/**
 * Send a counter with the given name, value and timestamp, and the specified
 * labels.  Labels are specified by a series of 
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
