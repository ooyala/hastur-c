#include "hastur_string_builder.h"
#include <stdlib.h>
#include <string.h>

/* This shit is why I'm tempted by C++ and its built-in std::string.
   Then I use the STL to build a hash table and the feeling passes,
   along with all feeling below my neck. */

/* If the re-building of infrastructure gets bad enough I'll convert
   to a framework or something.  I hope I don't need to. */

string_builder_t *string_builder_new(char *buf, int len) {
  string_builder_t *ret;

  ret = malloc(sizeof(string_builder_t));
  ret->buf_start = buf;
  ret->buf_tail = buf;
  ret->buf_len = len;

  return ret;
}

char *string_builder_start(string_builder_t *builder) {
  return builder->buf_start;
}

void string_builder_append(string_builder_t *builder, const char *str) {
  strncpy(builder->buf_tail, str, (builder->buf_len - (builder->buf_tail - builder->buf_start)));
  builder->buf_tail += strlen(str);

  if(builder->buf_tail > builder->buf_start + builder->buf_len) {
    builder->buf_tail = builder->buf_start + builder->buf_len;
  }
}

void string_builder_append_char(string_builder_t *builder, char c) {
  if(builder->buf_tail >= builder->buf_start + builder->buf_len) return;

  *builder->buf_tail = c;
  builder->buf_tail++;
}
