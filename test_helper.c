#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "test_helper.h"
#include "hastur.h"
#include "hastur_helpers.h"

static int failed_assertions = 0;
static int total_assertions = 0;

int use_fake_timestamp(time_t *timestamp, void *user_data) {
  *timestamp = FAKE_NOW_TIMESTAMP;
  return 0;
}

#define BUFLEN (64 * 1024)
static char message_buf[BUFLEN];

int copy_to_message_buf(const char *message, void *user_data) {
  strncpy(message_buf, message, BUFLEN);

  return 0;
}

/* From http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c */
// You must free the result if result is non-NULL.
char *str_replace(const char *orig, const char *rep, const char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep || !(len_rep = strlen(rep)))
        return NULL;
    if (!(ins = strstr(orig, rep)))
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);

    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

static char *substitute_message(const char *templated_message) {
  int pid = getpid();
  char pid_buf[20];  /* Big enough for any pid */
  pthread_t bg_tid;
  const char *tid_buf;
  char *tmp;
  char *to_free = NULL;  /* malloc'd memory expecting later free */
  char *substituted;

  snprintf(pid_buf, 20, "%d", pid);
  tmp = str_replace(templated_message, "-PID-", pid_buf);
  if(tmp) {
    substituted = tmp;
    to_free = tmp;
  } else {
    substituted = (char*)templated_message;
  }

  bg_tid = hastur_get_bg_thread_id();
  tid_buf = __hastur_buf_to_hex((const char *)&bg_tid, sizeof(pthread_t));
  tmp = str_replace(substituted, "-BG-TID-", tid_buf);
  if(tmp) {
    if(to_free) free(to_free);
    to_free = substituted;
    substituted = tmp;
  } else {
    /* Keep "substituted" the same, along with "to_free" if any. */
  }

  /* Nothing substituted?  Return NULL.  Otherwise return the new
     version of the message. */
  return (substituted == templated_message) ? NULL : substituted;
}

int assert_message_equal(const char *message_template, const char *assertion_message) {
  char *expected_value = substitute_message(message_template);

  assert_equal(expected_value ? expected_value : message_template, message_buf, assertion_message);

  if(expected_value)
    free(expected_value);

  return 0;
}

int assert_equal(const char *expected, const char *actual, const char *message) {
  total_assertions++;

  if(strcmp(expected, actual)) {
    failed_assertions++;

    fprintf(stderr, "String \'%s\' and string \'%s\' are not equal!\n",
	    expected, actual);
    if(message)
      fprintf(stderr, "%s", message);
  }

  return 0;
}

void print_assertion_messages(void) {
  fprintf(stderr, "Total assertions: %d\n", total_assertions);
  fprintf(stderr, "Correct assertions: %d\n", total_assertions - failed_assertions);
  fprintf(stderr, "Failed assertions: %d\n", failed_assertions);
}
