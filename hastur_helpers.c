#include "hastur.h"
#include "hastur_helpers.h"
#include "hastur_string_builder.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

/* 64k is more than we can send in a single UDP datagram */
#define BUFLEN (64 * 1024)

char buf[BUFLEN];
char sub_buf[BUFLEN];  /* For formatting individual args */


static void format_json_from_va_list(string_builder_t *builder, va_list argp) {
  int first = 1;

  const char *label;
  int value_type;
  const char *value_str;
  int value_int;
  long value_long;
  double value_double;

  while(1) {
    label = va_arg(argp, const char *);
    if(!label) break;  /* When we hit a NULL, stop. */

    if(first) {
      first = 0; /* not first any more */
    } else {
      string_builder_append_char(builder, ',');
    }

    string_builder_append_char(builder, '\"');
    string_builder_append(builder, label);
    string_builder_append(builder, "\":");

    value_type = va_arg(argp, int);
    switch(value_type) {
    case HASTUR_STRING:
      value_str = va_arg(argp, const char *);
      string_builder_append(builder, "\"");
      string_builder_append(builder, value_str);  /* TODO: escape string */
      string_builder_append(builder, "\"");
      break;

    case HASTUR_BARE:
      value_str = va_arg(argp, const char *);
      string_builder_append(builder, value_str);
      break;

    case HASTUR_INT:
      value_int = va_arg(argp, int);
      sprintf(sub_buf, "%d", value_int);
      string_builder_append(builder, sub_buf);
      break;

    case HASTUR_LONG:
      value_long = va_arg(argp, long);
      sprintf(sub_buf, "%ld", value_long);
      string_builder_append(builder, sub_buf);
      break;

    case HASTUR_DOUBLE:
      value_double = va_arg(argp, double);
      sprintf(sub_buf, "%g", value_double);
      string_builder_append(builder, sub_buf);
      break;

    case HASTUR_COMMA_SEPARATED_ARRAY: {
      const char *index;
      const char *end_ptr;
      int first = 1;

      value_str = va_arg(argp, const char *);
      index = value_str;
      end_ptr = value_str + strlen(value_str);

      string_builder_append_char(builder, '[');

      /* Convert from CSV to array */
      while(index < end_ptr) {
	const char *word = index;

	if(first) {
	  first = 0;
	} else {
	  string_builder_append_char(builder, ',');
	}

	index = strchr(index, ',');
	if(!index) {
	  /* Append final word */
	  string_builder_append_char(builder, '\"');
	  string_builder_append(builder, word);
	  string_builder_append_char(builder, '\"');
	  break;
	}
	string_builder_append_char(builder, '\"');
	string_builder_append_chars(builder, word, (index - word));
	string_builder_append_char(builder, '\"');

	index++;  /* Advance past the comma */
      }

      string_builder_append_char(builder, ']');

      break;
    }

    default:
      fprintf(stderr, "Unrecognized value type %d!\n", value_type);
      /* And continue... */
    }
  }
}

const char *__hastur_format_json(const char *message_type, ...) {
  /* varargs list for label/type/value triples */
  va_list argp;
  string_builder_t *builder;

  builder = string_builder_new(buf, BUFLEN);

  va_start(argp, message_type);

  string_builder_append(builder, "{\"type\":\"");
  string_builder_append(builder, message_type);
  string_builder_append(builder, "\",");

  format_json_from_va_list(builder, argp);

  string_builder_append(builder, "}");

  va_end(argp);

  return buf;
}

static int send_error(const char *s)
{
  perror(s);
  return HASTUR_UDP_ERROR;
}

int __hastur_send(const char *message) {
  struct sockaddr_in si_other;
  int s;
  deliver_with_type deliver_with;
  void *user_data;

  /* Use deliver_with, if specified */
  deliver_with = hastur_get_deliver_with();
  user_data = hastur_get_deliver_with_user_data();
  if(deliver_with) {
    return (*deliver_with)(message, user_data);
  }

  /* Otherwise, send via local UDP */

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    return send_error("socket");
  }

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(hastur_get_agent_port());
  if (inet_aton("0.0.0.0", &si_other.sin_addr)==0) {
    return send_error("inet_aton() failed");
  }

  if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, sizeof(si_other)) < 0) {
    return send_error("sendto()");
  }

  close(s);

  return 0;
}

static char hex_digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			       'A', 'B', 'C', 'D', 'E', 'F' };

char tid_buf[64];
static const char *buf_to_hex(const char *buf, int len) {
  char *tid_index = tid_buf;
  const char *buf_index = buf;

  *tid_index = '\0';
  while(len) {
    *tid_index = hex_digits[(*buf_index & 0xF0) >> 4];
    tid_index++;
    *tid_index = hex_digits[*buf_index & 0x0F];
    tid_index++;

    len--;
    buf_index++;
  }

  return tid_buf;
}

extern pthread_t __hastur_start_thread;

static const char *get_tid(void) {
  pthread_t cur = pthread_self();

  if(pthread_equal(cur, __hastur_start_thread)) {
    return "main";
  }

  return buf_to_hex((const char*)&cur, sizeof(pthread_t));
}

#ifdef WIN32
#define getpid _getpid
#endif

/* This is amply big enough for a decimalized long int */
char pid_buf[32];

/* By analogy with get_tid.  Also, note we convert to string. */
static const char *get_pid(void) {
  sprintf(pid_buf, "%ld", (long int)getpid());
  return pid_buf;
}

char labels_buf[BUFLEN];

const char *__hastur_default_labels(void) {
  int pid = getpid();

  snprintf(labels_buf, BUFLEN, "{\"app\":\"%s\",\"pid\":%d,\"tid\":\"%s\"}",
	   hastur_get_app_name(), pid, get_tid());

  return labels_buf;
}

char label_buf[BUFLEN];

const char *__hastur_generate_labels(va_list argp) {
  string_builder_t *builder;
  va_list arg_copy;

  int app_seen = 0;
  int pid_seen = 0;
  int tid_seen = 0;

  int count = 0;

  /* Copy the arg list so we can see if app, pid or tid is supplied */
  va_copy(arg_copy, argp);

  builder = string_builder_new(label_buf, BUFLEN);

  string_builder_append_char(builder, '{');
  format_json_from_va_list(builder, argp);

  /* Now iterate through arg_copy and look for labels */
  while(1) {
    const char *label;
    int value_type;

    label = va_arg(arg_copy, const char *);
    if(!label) break;

    count++; /* Update total count of labels */

    if(!strncmp("app", label, 4)) {
      app_seen = 1;
    } else if(!strncmp("pid", label, 4)) {
      pid_seen = 1;
    } else if(!strncmp("tid", label, 4)) {
      tid_seen = 1;
    }

    value_type = va_arg(arg_copy, int);
    switch(value_type) {
    case HASTUR_INT:
      va_arg(arg_copy, int);
      break;
    case HASTUR_STR:
    case HASTUR_COMMA_SEPARATED_ARRAY:
    case HASTUR_BARE:
      va_arg(arg_copy, const char *);
      break;
    case HASTUR_LONG:
      va_arg(arg_copy, long);
      break;
    case HASTUR_DOUBLE:
      va_arg(arg_copy, double);
      break;
    default:
      fprintf(stderr, "Unrecognized Hastur type %d!\n", value_type);
    }
  }

  va_end(arg_copy);

  /* Now append any label(s) we haven't already seen */

  if(!app_seen) {
    if(count != 0) {
      string_builder_append_char(builder, ',');
    }

    string_builder_append(builder, "\"app\":\"");
    string_builder_append(builder, hastur_get_app_name());
    string_builder_append_char(builder, '\"');
  }
  if(!pid_seen) {
    string_builder_append(builder, ",\"pid\":");
    string_builder_append(builder, get_pid());
  }
  if(!tid_seen) {
    string_builder_append(builder, ",\"tid\":\"");
    string_builder_append(builder, get_tid());
    string_builder_append_char(builder, '\"');
  }

  string_builder_append_char(builder, '}');

  return label_buf;
}
