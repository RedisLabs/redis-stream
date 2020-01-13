#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h> 
#include <stdlib.h>

#include "config.h"
#include "consumer.h"
#include "producer.h"

static int _piped_stdin(void) {
    return !isatty(fileno(stdin));
}

static int _piped_stdout(void) {
    return !isatty(fileno(stdout));
}

static void usage(void) {
    fprintf(stderr, "redis-stream exposes redis stream data structure through the command line.\n");
    fprintf(stderr, "A stream can write data coming in from stdin into redis and stream data from redis into stdout.\n\n");
    fprintf(stderr, "Example:\nRead data from `ls` and stream it to redis: ls | redis-stream\n");
    fprintf(stderr, "Read data from a redis stream and write it to `grep`: redis-stream | grep\n");
    fprintf(stderr, "Combined: ls | redis-stream | grep\n\n");
    fprintf(stderr, "Generally speaking: produce | redis-stream, redis-stream | consumer\n\n");
    fprintf(stderr, "Additional arguments:\n");
    fprintf(stderr, "-h <host>\n-p <port>\n-a <password>\n-s <stream>\n");
}

static Config parse_args(int argc, char *argv[]) {
    int i = 1;
    Config config;
    config.port = 6379;
    config.password = NULL;
    config.host = "127.0.0.1";
    config.stream = "my_stream";

    while(i < argc) {
        if(!strcmp(argv[i], "-h")) {
            config.host = argv[++i];
        }
        else if(!strcmp(argv[i], "-p")) {
            config.port = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-a")) {
            config.password = argv[++i];
        }
        else if(!strcmp(argv[i], "-s")) {
            config.stream = argv[++i];
        }
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
        }
        i++;
    }

    return config;
}

int main(int argc, char *argv[]) {
    // initialize random seed.
    srand (time(NULL));

    Config config = parse_args(argc, argv);

    int piped_stdin = _piped_stdin();
    int piped_stdout = _piped_stdout();

    if(!piped_stdin && !piped_stdout) {
        usage();
        return 0;
    }

    if(piped_stdin) consumer(&config);
    if(piped_stdout) producer(&config);
    
    return 0;
}
