#include "consumer.h"
#include "stream.h"
#include "hiredis.h"

#include <unistd.h>

void consumer(Config *config)
{
    // Create stream.
    redisContext *conn = redisConnect(config->host, config->port);
    Stream *stream = Stream_NewConsumer(conn, config->stream);

    unsigned char buff[1024];
    ssize_t bytes_read = 0;

    while ((bytes_read = read(STDIN_FILENO, buff, sizeof(buff))))
    {
        Stream_Write(stream, buff, bytes_read);
    }

    // Send EOF indicator.
    Stream_Write(stream, NULL, 0);

    Stream_Free(stream);
}
