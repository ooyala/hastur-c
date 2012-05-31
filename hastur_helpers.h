/* Values passed to format_json */
#define VALUE_STRING   1
#define VALUE_INT      2
#define VALUE_LONG     3

int __hastur_send(const char *message);
int __format_json(const char *message_type, ...);
