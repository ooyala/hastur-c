What Is It?
-----------

Hastur is a monitoring system written by Ooyala.  It uses Cassandra
for time series storage, resulting in remarkable power, flexibility
and scalability.

Hastur works hard to make it easy to add your data and easy to get it
back at full resolution.  For instance, it makes it easy to query in
big batches from a REST server, build a dashboard of metrics, show
errors in production or email you when an error rate gets too high.

This library helps you get your data into Hastur.  See the
"hastur-server" ruby gem for the back end, and for how to get your
data back out.

How Do I Use It?
----------------

Build this library and link your application against it.

Add Hastur calls to your application, such as:

  hastur_counter("my.thing.to.count", 1);          # Add 1 to my.thing.to.count
  hastur_gauge("other.thing.foo_latency", 371.1);  # Record a latency of 371.1

You can find extensive per-method documentation in the header file, or
see "Is It Documented?" below for friendly Doxygen documentation.

This is enough to instrument your application code, but you'll need to
install a local daemon and have a back-end collector for it to talk
to.  See the hastur-server ruby gem for specifics.

Hastur optionally allows you to send at regular intervals using
hastur_every(), which will call a function from a background thread:

  static int total = 0;

  void send_back_stats(void *user_data) {
    hastur_gauge("total.counting.so.far", total);
  }

  hastur_every("minute", send_back_stats);

  while(1) { sleep(1); total++; }

The Doxygen documentation (see below) has far more specifics.

Is It Documented?
-----------------

We use Doxygen for our C API.  See www.doxygen.org for details about
how to install and run it.  Most package systems, including Linux
distributions, have an existing package for Doxygen.

Mechanism
---------

Your messages are automatically timestamped in microseconds, labeled
and converted to a JSON structure for transport and storage.

Hastur sends the JSON over a local UDP socket to a local "Hastur
Agent", a daemon that forwards your data to the shared Hastur servers.
That means that your application will never slow down for Hastur --
failed sends become no-ops.  Note that local UDP won't randomly drop
packets like internet UDP, though you can lose them if there's no
Hastur Agent running.

The Hastur Agent forwards the messages to Hastur Routers over ZeroMQ
(see "http://0mq.org").  The routers send it to the sinks, which
preprocess your data, index it and write it to Cassandra.  They also
forward to the syndicators for the streaming interface (e.g. to email
you if there's a problem).

Cassandra is a highly scalable clustered key-value store inspired
somewhat by Amazon Dynamo.  It's a lot of the "secret sauce" that
makes Hastur interesting.

Hints and Tips
--------------

1. You can retrieve messages with the same name prefix all together from
the REST API (for instance: "my.thing.*").  It's usually a good idea
to give metrics the same prefix if you will retrieve them at the same
time.  This prefix syntax is very efficient for Cassandra.  That's why
we made it easy to use.

2. Every call allows you to pass labels - a one-level string-to-string
hash of tags about what that call means and what data goes with it.
For instance, you might call:

  hastur_gauge_v("my.thing.total_latency", 317.4, NULL, "units", "usec", NULL);

Eventually you'll be able to query messages by label through the REST
interface, but for now that's inconvenient.  However, it's easy to
subscribe to labels in the streaming interface.  So labels are a
powerful way to mark data as being interesting to alert you about.

For example:

  hastur_gauge_v("my.thing.total_latency", 317.4, NULL, "severity", "omg", NULL);

It's easy to subscribe to any latency with a severity label in the
streaming interface, which would let you calculate how bad the overall
latency pretty well.  See the hastur-server gem for details of the
trigger interface.

3. You can group multiple messages together by giving them the same
timestamp.  For instance:

  ts = hastur_timestamp(NULL);
  hastur_gauge_v("my.thing.latency1", val1, ts, NULL);
  hastur_gauge_v("my.thing.latency2", val2, ts, NULL);
  hastur_counter_v("my.thing.counter371", 1, ts, NULL);

This makes it easy to query all events with exactly that timestamp
and the same prefix ("my.thing.*"), and otherwise to make sure they're
exactly the same.

Do *not* give multiple messages the same name *and* the same
timestamp.  Hastur will only store a single event with the same name
and timestamp from the same node.  If you give several of them the
same name and timestamp, you'll lose all but one.

Keep in mind that timestamps are in microseconds -- you're not limited
to one event with the same name per second.

Why Is It in C?
---------------

C is portable, linkable from every other language, and there are no
worries about ABI or compatibility between two different compilers on
the same system.  Any other language can easily call C through
dyload/JNA/FFI or wrap it via SWIG.  I don't need to worry about
trying to support STL hash tables for labels.  I don't need to worry
about accepting various STL/Boost structures as arguments.  I don't
need to worry about whether using exceptions will mean that people
attempting to write sane, reliable C++ simply can't use my library or
whether somebody has linked to a library (libjpeg?) that takes
callbacks but wasn't recompiled with C++ exception support despite
being written in plain C.

C is an excellent least common denominator.  C++ is not.

When I want a modern language rather than a least common denominator,
I use the Ruby bindings.  They have a better interface with a far
shorter line count.  Or if you prefer Perl or Python, you should be be
able to produce roughly the same functionality with about 300 lines of
code.
