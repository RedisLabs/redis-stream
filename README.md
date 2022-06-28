# redis-stream
Exposes [Redis stream](https://redis.io/topics/streams-intro) through the command line

### Requirements

You need to have [CMake]() installed on your machine.


## make it executable
Redis stream utilizes [hiredis]() as main dependency.

```bash
cmake -S . -B build/
cd build
make
```

## Example
Suppose we want to list all files containing both `'a'` and `'z'` characters
e.g. azores.txt, arizona.md.

Using multiple `grep` processes.

We'll start by recursively scanning our file-system, filtering files containing `'a'` character using grep, these will be written into a redis stream `fs` for later parallel consumption:

`ls -R | grep a | xargs -L1 | redis-stream -s fs`

To apply the second filter (files containing both `'a'` and `'z'` characters) we'll be pulling from the `fs` stream using multiple consumers, feeding our data once again into `grep`.

```
redis-stream -s fs | grep z
redis-stream -s fs | grep z
redis-stream -s fs | grep z
```