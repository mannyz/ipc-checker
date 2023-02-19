# ipc-checker

ipc-checker is a simple hometool for measuring IPC.
The measurement is based on the producer-consumer interaction scenario.

To run a test with a measurement cycle, you can use the tool with `run_test` target:
```
ipc-checker run_test --chunks-size=100 --chunks-total-count=1000000 --connection=(netsock|pipe|shmem|uxsock) --connection-buffer-size=4096
```

Or with thread affinity for producer and consumer:
```
ipc-checker run_test --chunks-size=100 --chunks-total-count=1000000 --connection=(netsock|pipe|shmem|uxsock) --connection-buffer-size=4096 --environment-consumer-cpu=<CPU1> --environment-producer-cpu=<CPU2>
```

As a result, the program will display information about the total time spent on data transfer and delay quantiles:
```
Total time: 656ms
0.999 = 5'409ns
0.99 = 1'289ns
0.95 = 817ns
0.5 = 275ns
```

To get help for test settings, you can use the command:
```
ipc-checker run_test --help
```

For more correct measurements, the producer and consumer should be assigned individual CPUs
that will not be affected by the OS. To do this, use
[`isolcpus`](https://www.kernel.org/doc/html/v4.14/admin-guide/kernel-parameters.html#cpu-lists) - special kernel option configuration.
You can follow the next steps for Debian/Ubuntu:
- add isolcpus in the file `/etc/default/grub` to the option GRUB_CMDLINE_LINUX:
```
GRUB_CMDLINE_LINUX="isolcpus=<CPU1>,<CPU2>"
```
- run `sudo update-grub`;
- reboot the system;
- after that the kernel will not to use any of your cores from isolcpus.


Build was carried out on `Debian GNU/Linux 12 (bookworm)` using next dependencies (the result was obtained using `apt-show-versions`):
```
cmake:amd64/bookworm 3.25.1-1
clang:amd64/bookworm 1:14.0-55.7~deb12u1
gcc:amd64/bookworm 4:12.2.0-3
libgmock-dev:amd64/bookworm 1.12.1-0.2
libgtest-dev:amd64/bookworm 1.12.1-0.2
libsctp-dev:amd64/bookworm 1.0.19+dfsg-2
```

Unfortunately, I don't have more time to describe the tool's program structure in detail. But you can always look at the source code or get global help for the tool:
```
ipc-checker --help
```