#include <time.h>
#include <sys/time.h>

#define FAKE_NOW_TIMESTAMP 1338829820286099
#define FAKE_NOW_TIME_STRING "1338829820286099"

/* For simple debugging, use copy_to_message_buf and
   assert_message_equal.  It works, but it only allows for a single
   message to be "in flight" and compared against at once. */

int use_fake_timestamp(time_t *timestamp, void *user_data);
int copy_to_message_buf(const char *message, void *user_data);
int assert_message_equal(const char *expected_value, const char *message);
int assert_equal(const char *expected, const char *actual, const char *message);

/* Print out assertion messages */
void print_assertion_messages(void);

/* Return -1 for assertion(s) failed, 0 for all succeeded */
int assertion_success(void);

/* First, call expect_message() for one or more message templates.
   Then set the delivery mechanism to check_message expectation.
   Then, call satisfied_expectations.  Now you should get Mocha-like
   functionality where it complains about unexpected messages and then
   complains if it doesn't see the messages it expects. */

int expect_message(const char *message_template, const char *assertion_message);
int check_message_expectation(const char *message, void *user_data);
int satisfied_expectations(void);
