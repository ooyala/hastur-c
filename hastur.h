#ifndef __HASTUR_CLIENT__
#define __HASTUR_CLIENT__

#ifdef __cplusplus
extern "C" {
#endif

/**  @file hastur.h
 *   @brief Hastur client public API
 */

#include <time.h>

/* Values passed to format_json */
#define HASTUR_STRING   1
#define HASTUR_STR      1
#define HASTUR_INT      2
#define HASTUR_LONG     3
#define HASTUR_BARE     4

/*
 * Return a current timestamp in microseconds since the Unix epoch.
 */
time_t hastur_timestamp(void);

/*
 * Send a counter with the given name.
 */
int hastur_counter(const char *name, int value);

/*
 * Send a counter with the given name, value and timestamp, and the specified
 * labels.  Labels are specified by a series of 
 */
int hastur_counter_v(const char *name, int value, time_t timestamp, ...);

typedef int (*deliver_with_type)(const char *message, void *user_data);
void hastur_deliver_with(deliver_with_type callback, void *user_data);
deliver_with_type hastur_get_deliver_with(void);
void *hastur_get_deliver_with_user_data(void);

int hastur_get_agent_port(void);
void hastur_set_agent_port(int port);

const char* hastur_get_app_name(void);
void hastur_set_app_name(const char *app_name);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_CLIENT__ */
