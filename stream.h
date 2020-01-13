#pragma once

#include "./deps/hiredis/hiredis.h"

typedef struct {
    redisContext *conn; // Redis connection.
    const char *key;    // Stream key.
    char *consumer;     // Stream consumer.
} Stream;

// Creates a new stream.
Stream* Stream_New
(
    redisContext *conn, // Redis connection.
    const char *key     // Stream key.
);

// Creates a new stream consumer.
Stream* Stream_NewConsumer
(
    redisContext *conn, // Redis connection.
    const char *key     // Stream key.
);

// Write buffer to stream.
// Returns 0 incase of an error.
int Stream_Write
(
    const Stream *stream,   // Stream.
    unsigned char *buff,    // Buffer to send.
    size_t buff_len         // Buffer size.
);

// Reads a single item from stream.
void Stream_Read
(
    const Stream *stream,   // Stream
    unsigned char *buff,    // Buffer to store response into.
    size_t *buff_len        // Response length.
);

// Free stream.
void Stream_Free
(
    Stream *stream
);