#include "producer.h"
#include "stream.h"
#include "./deps/hiredis/hiredis.h"

#include <unistd.h>

void producer(Config *config) {
    // Create stream.    
    redisContext *conn = redisConnect(config->host, config->port);
    Stream *stream = Stream_NewConsumer(conn, config->stream);

    unsigned char buff[2048];
    size_t bytes_read = 0;

    while(1) {        
        Stream_Read(stream, buff, &bytes_read);
        if(bytes_read == 0) {
            // Reintroduce empty message for other consumers.
            Stream_Write(stream, buff, bytes_read);
            break;
        }
        
        // Write buff to stdout.
        size_t bytes_written = write(STDOUT_FILENO, buff, bytes_read);
    }

    Stream_Free(stream);
}
