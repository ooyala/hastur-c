#include <time.h>
#include <sys/time.h>

#define FAKE_NOW_TIMESTAMP 1338829820286099
#define FAKE_NOW_TIME_STRING "1338829820286099"

int use_fake_timestamp(time_t *timestamp, void *user_data);
int copy_to_buf(const char *message, void *user_data);
int assert_message_equal(const char *expected_value, const char *message);
int assert_equal(const char *expected, const char *actual, const char *message);
void print_assertion_messages(void);
