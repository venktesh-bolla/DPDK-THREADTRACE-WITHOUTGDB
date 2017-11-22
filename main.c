/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_eal.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_malloc.h>
#include <rte_memzone.h>
#include <rte_cycles.h>
#include <rte_version.h>

#define UNW_LOCAL_ONLY 
#include <libunwind.h>
#include <dirent.h>
#include <sys/ptrace.h>
#include <sys/user.h>

uint8_t g_threads = 0;
pthread_t tid[64] = {0};

int unwind_thread_callstack (void);
void sig_pthreadhandler(__rte_unused int signo, __rte_unused siginfo_t *info, __rte_unused void *dummy);
void sig_handler(int signo);

__attribute__((noinline))
void sig_pthreadhandler(__rte_unused int signo, __rte_unused siginfo_t *info, __rte_unused void *dummy)
{
    //printf(" Inside sig_pthreadhandler %lu for signal %u\n", pthread_self(), signo);
    unwind_thread_callstack ();
    fflush(stdout);

#if 0
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, signo);
    sigsuspend(&mask);
#endif

    return;
}

__attribute__((noinline))
void sig_handler(int signo)
{
    char cmd[30] = {0};

    switch (signo) {
        case SIGSEGV:
           

            printf(" In sig_handler for signal %d\n", SIGSEGV);
            rte_delay_ms(10);
            printf(" DPDK Version 0x%x\n", RTE_VERSION);

            struct rte_config *config = rte_eal_get_configuration();
            printf(" Config: msater %u lcore count %u process %d\n", config->master_lcore, config->lcore_count, config->process_type);

            printf(" rte_sys_gettid %d \n",  rte_sys_gettid());

            printf(" ---------------- Stack Unwind BEGIN ----------------------\n");
            //signal(signo, SIG_DFL);
            //printf(" Inside sig_pthreadhandler %lu\n", pthread_self());
            //unwind_thread_callstack();
            //printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

            
            for (int i = 0; i < g_threads; i++) {
                if (tid[i]) {
                    //if (pthread_self() != tid[i]) {
                    if (1) {
                        printf(" ++++++++++++++++++++++++ Thread index %d ID %lu ++++++++++++++++++++++++\n", i, tid[i]);
                        if (pthread_kill(tid[i], SIGUSR2) != 0) 
                            continue;

                        rte_delay_ms(10);
                    }
                }
            }
            printf(" ---------------- Stack Unwind DONE ----------------------\n");

            printf("----------------- MMAP BEGIN -----------------\n");
            sprintf(cmd, "pmap -x %d", rte_sys_gettid());
            system(cmd);
            printf("----------------- MMAP DONE -----------------\n");
            
            fflush(stdout);
            exit(2);
        
        default:
            printf(" unknown signal %d", signo);
        break;
    }

    fflush(stdout);

    return;
}

__attribute__((noinline))
int unwind_thread_callstack (void)
{
    unw_cursor_t cursor; unw_context_t uc;
    unw_word_t ip, sp, bp, offp;
    unw_proc_info_t pi;

    char name[256];
    int depth = 0;

    unw_getcontext (&uc);
    unw_init_local (&cursor, &uc);

    while (unw_step(&cursor) > 0)
    {
        name[0] = '\0';
        unw_get_proc_name (&cursor, name, 256, &offp);
        unw_get_reg (&cursor, UNW_X86_64_RIP, &ip);
        unw_get_reg (&cursor, UNW_X86_64_RSP, &sp);
        unw_get_reg (&cursor, UNW_X86_64_RBP, &bp);

        unw_get_proc_info (&cursor, &pi);
        depth += 1;

        printf ("%2d) (0x%016lx in %20s) - ip {0x%016lx} sp {0x%016lx} bp {0x%016lx} proc {0x%016lx-%016lx}\n",
                    depth, offp, name, 
                    (long) ip, (long) sp, (long) bp, 
                    (long) pi.start_ip, (long) pi.end_ip);


        if (depth > 20) {
            printf (" --- too many unwind!! \n'''");
            break;
        }
            
    }

    return 0;
}



static int
lcore_hello(__attribute__((unused)) void *arg)
{
	unsigned lcore_id;
    uint64_t test[100000] = {0xefefefef};

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sig_pthreadhandler;
    sigaction(SIGUSR2, &sa, NULL); 


	lcore_id = rte_lcore_id();
    test[lcore_id] =0xBADABADA;

    tid[g_threads] = pthread_self();
    g_threads += 1;

	//printf(" hello from core %u %p 0x%"PRIx64"\n", lcore_id, test, test[2 * 100000]);
    while (1);


	return 0;
}

int
main(int argc, char **argv)
{
	int ret;
	unsigned lcore_id;

    //signal(SIGINT , sig_handler);
    //signal(SIGHUP , sig_handler);
    //signal(SIGINT , sig_handler);
    //signal(SIGQUIT, sig_handler);
    //signal(SIGILL , sig_handler);
    //signal(SIGABRT, sig_handler);
    //signal(SIGKILL, sig_handler);
    signal(SIGSEGV, sig_handler);
    //signal(SIGTERM, sig_handler);
    //signal(SIGSTOP, sig_handler);
    //signal(SIGTSTP, sig_handler);
    //signal(SIGBUS , sig_handler); 

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	/* call lcore_hello() on every slave lcore */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_hello, NULL, lcore_id);
	}

	/* call it on master lcore too */
	lcore_hello(NULL);

	rte_eal_mp_wait_lcore();

	return 0;
}
