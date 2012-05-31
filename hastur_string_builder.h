#ifndef __HASTUR_STRING_BUILDER__
#define __HASTUR_STRING_BUILDER__

#ifdef __cplusplus
extern "C" {
#endif

/** @file hastur_string_builder.h
 *  A string builder utility to avoid O(N^2) message assembly.
 */

/**
 * The string builder as data.
 */
typedef struct string_builder_struct {
  char *buf_start;  /**< The buffer being managed */
  char *buf_tail;   /**< Tail-pointer to the managed buffer */
  int   buf_len;    /**< Allocated length of the managed buffer */
} string_builder_t;

/**
 * Create a new string-builder to manage the given empty buffer.
 * Do not use on a buffer with contents you care about!
 * The length is the full length of the buffer, and writes
 * after that length is reached will be ignored.
 */
string_builder_t *string_builder_new(char *buf, int len);

/**
 * Get the current assembled buffer.
 */
char *string_builder_start(string_builder_t *builder);

/**
 * Append to the buffer.  Time to append is linear in the
 * specified string, not in the current buffer length.
 */
void string_builder_append(string_builder_t *builder, const char *str);

/**
 * Append a single character to the buffer in constant time.
 */
void string_builder_append_char(string_builder_t *builder, char c);

#ifdef __cplusplus
}
#endif

#endif /* __HASTUR_STRING_BUILDER__ */
