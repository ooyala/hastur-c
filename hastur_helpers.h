#ifndef __HASTUR_CLIENT_HELPERS__
#define __HASTUR_CLIENT_HELPERS__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

int __hastur_send(const char *message);
const char *__hastur_format_json(const char *message_type, ...);
const char *__hastur_default_labels(void);
const char *__hastur_generate_labels(va_list argp);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_CLIENT_HELPERS__ */
