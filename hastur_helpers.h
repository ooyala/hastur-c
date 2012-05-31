/* Values passed to format_json */
#define HVALUE_STRING   1
#define HVALUE_STR      1
#define HVALUE_INT      2
#define HVALUE_LONG     3
#define HVALUE_BARE     4

int __hastur_send(const char *message);
const char *__format_json(const char *message_type, ...);
const char *__default_labels(void);
