# DPDK-THREADTRACE-WITHOUTGDB
dump all threads stack and register information. useful in target where no gdb or pdump is not available.

* [Depdency]
> libunwind, pthread_self (on each thread whose back trace and register extract is required.)

* [BUILD]
> CFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind , LDFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind 

## Use Cases
* Binary are stripped.
* Binary and Application have no debug symbols.
* Rare cases & combinations when faults occurs.
* Errors or faults difficult to reproduce.
* There are no access to GDB or remote GDB, ptrace or pstack-dump.
* Inspect stack for each thread.
* Inspect & dump global and debug variables.
* DPDK when secondary causes primary to segfault. Running GDB for primary causes Secondary to segfault.

### Q & A
* Does this work for all shared library? Yes
* Does this work mixed libraries static and shared? Yes
* Does this work for all stripped libraries? Yes
* Can we register SIGUSER1 to dump intermediate? Yes
