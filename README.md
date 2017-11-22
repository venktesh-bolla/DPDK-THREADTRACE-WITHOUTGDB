# DPDK-THREADTRACE-WITHOUTGDB
dump all threads stack and register information. useful in target where no gdb or pdump is not available.

* [Depdency]
> libunwind, pthread_self (on each thread whose back trace and register extract is required.)

* [BUILD]
> CFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind , LDFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind 
