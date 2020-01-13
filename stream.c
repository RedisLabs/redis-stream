#include "stream.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void _random_string
(
    char *s,
    const int len
)
{
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

static void _ack_item
(
    const Stream *stream,
    const char *item_id
)
{
    void *reply = redisCommand(stream->conn, "XACK %s mygroup %s", stream->key, item_id);
    if(reply) {
        freeReplyObject(reply);
    } else {
        // TODO: handel error.
        fprintf(stderr, "error: %d\n", stream->conn->err);
    }
}

static void _parse_element
(
    redisReply *reply,
    const char **item_id,
    unsigned char *buff,
    size_t *buff_len
)
{
    *buff_len = 0;
    *item_id = NULL;

    if(!reply) return;

    /* 1) 1) "my_stream"
     *    2) 1) 1) "1578836898641-0"
     *          2) 1) "data"
     *             2) "Makefile\ndeps\nmain.c\nmain.o\npipe\n"*/

    assert(reply->type == REDIS_REPLY_ARRAY && reply->elements == 1);
    reply = reply->element[0];

    assert(reply->type == REDIS_REPLY_ARRAY && reply->elements == 2);
    const char *stream_name = reply->element[0]->str;
    redisReply *stream_item = reply->element[1];

    assert(stream_item->type == REDIS_REPLY_ARRAY && stream_item->elements == 1);
    stream_item = stream_item->element[0];
    
    assert(stream_item->type == REDIS_REPLY_ARRAY && stream_item->elements == 2);
    *item_id = stream_item->element[0]->str;
    redisReply *item = stream_item->element[1];
    
    assert(item->type == REDIS_REPLY_ARRAY && item->elements == 2);
    const char *key = item->element[0]->str;
    const char *value = item->element[1]->str;

    *buff_len = item->element[1]->len;
    memcpy(buff, value, *buff_len);
    buff[*buff_len] = '\0';
}

static int _creates_consumer_group
(
    Stream *stream
)
{
    void *reply = redisCommand(stream->conn, "XGROUP CREATE %s my_group 0 MKSTREAM", stream->key);
    if(reply == NULL) {
        // TODO: handel error.
        fprintf(stderr, "Failed to create consumer group\n");
        fprintf(stderr, "error: %d\n", stream->conn->err);
        return 0;
    }

    freeReplyObject(reply);
    return 1;
}

// Creates a new stream.
Stream* Stream_New
(
    redisContext *conn, // Redis connection.
    const char *key     // Stream key.
)
{
    assert(conn && key);
    Stream *stream = malloc(sizeof(Stream));
    stream->conn = conn;
    stream->key = key;
    stream->consumer = NULL;

    return stream;
}

// Creates a new stream consumer.
Stream* Stream_NewConsumer
(
    redisContext *conn, // Redis connection.
    const char *key   // Stream key.
)
{
    assert(conn && key);
    Stream *stream = malloc(sizeof(Stream));
    stream->conn = conn;
    stream->key = key;
    stream->consumer = malloc(sizeof(char) * 8);

    _random_string(stream->consumer, 8);
    assert(_creates_consumer_group(stream));
    
    return stream;
}

// Write buffer to stream.
// Returns 0 incase of an error.
int Stream_Write
(
    const Stream *stream,   // Stream.
    unsigned char *buff,    // Buffer to send.
    size_t buff_len         // Buffer size.
)
{
    // XADD stream * data buff.
    void *reply = redisCommand(stream->conn, "XADD %s * data %b", stream->key, buff, buff_len);
    if(reply) {
        freeReplyObject(reply);
        return 1;
    } else {
        // TODO: handel error.
        fprintf(stderr, "error: %d\n", stream->conn->err);
        return 0;
    }
}

// Reads a single item from stream.
void Stream_Read
(
    const Stream *stream,   // Stream
    unsigned char *buff,    // Buffer to store response into.
    size_t *buff_len        // Response length.
)
{
    void *reply = redisCommand(stream->conn, "XREADGROUP COUNT 1 BLOCK 0 GROUP my_group %s STREAMS %s >", stream->consumer, stream->key);
    if(reply) {
        const char *item_id;
        _parse_element(reply, &item_id, buff, buff_len);
        _ack_item(stream, item_id);
        freeReplyObject(reply);
    }
}

// Free stream.
void Stream_Free
(
    Stream *stream
)
{
    assert(stream);
    if(stream->consumer) free(stream->consumer);

    redisFree(stream->conn);
    free(stream);
}
