#ifndef __HASTUR_STRINGBUILDER__
#define __HASTUR_STRINGBUILDER__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct string_builder_struct {
  char *buf_start;
  char *buf_tail;
  int   buf_len;
} string_builder_t;

string_builder_t *string_builder_new(char *buf, int len);
char *string_builder_start(string_builder_t *builder);
void string_builder_append(string_builder_t *builder, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_STRINGBUILDER__ */
