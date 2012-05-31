#ifndef __HASTUR_CLIENT__
#define __HASTUR_CLIENT__

#ifndef __cplusplus
extern "C" {
#endif

#include <time.h>

time_t hastur_timestamp(void);

int hastur_counter(const char *name, int value);
int hastur_counter_full(const char *name, int value, time_t timestamp, const char *labels);
int hastur_counter_v(const char *name, int value, time_t timestamp, ...);

typedef int (*deliver_with_type)(const char *message, void *user_data);
void hastur_deliver_with(deliver_with_type callback, void *user_data);
deliver_with_type hastur_get_deliver_with(void);
void *hastur_get_deliver_with_user_data(void);

int hastur_get_agent_port(void);
void hastur_set_agent_port(int port);

const char* hastur_get_app_name(void);
void hastur_set_app_name(const char *app_name);

#ifndef __cplusplus
}
#endif

#endif /* __HASTUR_CLIENT__ */
