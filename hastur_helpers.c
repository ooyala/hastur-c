#include "hastur.h"
#include "hastur_helpers.h"
#include "hastur_stringbuilder.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

/* 64k is all that we can send (or more) for UDP datagrams */
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
    case HVALUE_STRING:
      value_str = va_arg(argp, const char *);
      string_builder_append(builder, "\"");
      string_builder_append(builder, value_str);  /* TODO: escape quotes */
      string_builder_append(builder, "\"");
      break;

    case HVALUE_BARE:
      value_str = va_arg(argp, const char *);
      string_builder_append(builder, value_str);
      break;

    case HVALUE_INT:
      value_int = va_arg(argp, int);
      sprintf(sub_buf, "%d", value_int);
      string_builder_append(builder, sub_buf);
      break;

    case HVALUE_LONG:
      value_long = va_arg(argp, long);
      sprintf(sub_buf, "%ld", value_long);
      string_builder_append(builder, sub_buf);
      break;

    default:
      fprintf(stderr, "Unrecognized value type %d!", value_type);
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
  string_builder_append(builder, "\"");

  format_json_from_va_list(builder, argp);

  string_builder_append(builder, "}");

  va_end(argp);

  return buf;
}

static int send_error(const char *s)
{
  perror(s);
  return -1;
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

static const char *get_tid(void) {
  /* TODO: useful get_tid() */
  return "main";
}

#ifdef WIN32
#define getpid _getpid
#endif

char labels_buf[BUFLEN];

const char *__hastur_default_labels(void) {
  int pid = getpid();

  snprintf(labels_buf, BUFLEN, "{\"app\":\"%s\",\"pid\":%d,\"tid\":\"%s\"}",
	   hastur_get_app_name(), pid, get_tid());

  return labels_buf;
}

const char *__hastur_generate_labels(va_list argp) {
  return "";
}
