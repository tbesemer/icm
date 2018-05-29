# Simple Demo Program

This shows an example that runs under Linux, using `pthreads()` below the surface.  Notes:

1. It creates two Tasks.
2. Each Task subsribes to a shared Event Notice, while the second task only subscribes
to one specific Event Notice.
3. The tasks loop and wait for Event Notices.
4. The parent (main) process loops and sends Event Notices.

## Building
Source the following and make:

```
. ../do_env_posix_linuxrc
make

```
Then, simply run it:

```
./two_tasks
```

