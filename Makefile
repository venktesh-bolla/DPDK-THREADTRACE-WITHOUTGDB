#   BSD LICENSE
#
#   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = test_bktrace

# all source are stored in SRCS-y
SRCS-y := main.c

CFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind 
#CFLAGS += -ggdb3 -L/usr/lib/x86_64-linux-gnu/ -lunwind 
LDFLAGS += -L/usr/lib/x86_64-linux-gnu/ -lunwind 
#CFLAGS += -ggdb3 -fno-stack-protector -z execstack -lunwind -l unwind-coredump -l unwind-generic -l unwind-ptrace -l unwind-x86_64
#CFLAGS += -ggdb3  -Wstack-usage=100
#CFLAGS += -ggdb -O2 -fstack-usage -fdump-rtl-expand -Wstack-usage=10
#CFLAGS += -ggdb -O2 -fstack-protector-all
#CFLAGS += -ggdb -O2 -fstack-check
#CFLAGS += -O3 -fsanitize=address
#CFLAGS += -O3

CFLAGS += $(WERROR_FLAGS)
#LDFLAGS += -Wl,-Map


include $(RTE_SDK)/mk/rte.extapp.mk
