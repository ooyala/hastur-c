#include "hastur_helpers.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/* 64k is all that we can send (or more) for UDP datagrams */
#define BUFLEN (64 * 1024)

const char * format_json(const char *message_type, ...) {
  char buf[BUFLEN];
  char sub_buf[BUFLEN];  /* For formatting individual args */

  /* varargs list for label/type/value triples */
  va_list argp;
  const char *label;
  int value_type;
  const char *value_str;
  int value_int;
  long value_long;

  buf[0] = '\0';  /* NUL-terminate the buffer */
  va_start(argp, message_type);

  /* TODO(noah): Add a string-builder for efficiency? */
  strncat(buf, BUFLEN, "{\n\"type\":\"");
  strncat(buf, BUFLEN, message_type);
  strncat(buf, BUFLEN, "\"");

  while(1) {
    label = va_arg(argp, const char *);
    if(!label) break;  /* When we hit a NULL, stop. */

    strncat(buf, BUFLEN, "\"");
    strncat(buf, BUFLEN, label);
    strncat(buf, BUFLEN, "\":");

    value_type = va_arg(argp, int);
    switch(value_type) {
    case VALUE_STRING:
      value_str = va_arg(argp, const char *);
      strncat(buf, BUFLEN, "\"");
      strncat(buf, BUFLEN, value_str);  /* TODO: escape quotes */
      strncat(buf, BUFLEN, "\"");
      break;

    case VALUE_INT:
      value_int = va_arg(argp, int);
      itoa(value_int, sub_buf, 10);
      strncat(buf, BUFLEN, subbuf);
      break;

    case VALUE_LONG:
      value_long = va_arg(argp, long);
      ltoa(value_long, sub_buf, 10);
      strncat(buf, BUFLEN, subbuf);
      break;

    default:
      fprintf(stderr, "Unrecognized value type %d!", value_type);
      /* And continue... */
    }

    strncat(buf, BUFLEN, ",")
  }

  strncat(buf, BUFLEN, "\"c\": 1}");

  va_end(argp);

  return buf;
}

static int send_error(const char *s)
{
  perror(s);
  return -1;
}

int hastur_send(const char *message) {
  struct sockaddr_in si_other;
  int s, i;
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

  if (sendto(s, message, strlen(message), 0, &si_other, sizeof(si_other)) < 0) {
    return send_error("sendto()");
  }

  close(s);

  return 0;
}
