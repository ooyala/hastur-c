#ifndef __HASTUR_CLIENT_HELPERS__
#define __HASTUR_CLIENT_HELPERS__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

/* Values passed to format_json */
#define HVALUE_STRING   1
#define HVALUE_STR      1
#define HVALUE_INT      2
#define HVALUE_LONG     3
#define HVALUE_BARE     4

int __hastur_send(const char *message);
const char *__hastur_format_json(const char *message_type, ...);
const char *__hastur_default_labels(void);
const char *__hastur_generate_labels(va_list argp);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_CLIENT_HELPERS__ */
