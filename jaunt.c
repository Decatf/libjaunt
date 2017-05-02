/*
 * libjaunt
 * Copyright (C) 2017 <decatf@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define LOG_TAG "libjaunt"

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <pthread.h>
#include <cutils/log.h>
#include <sys/prctl.h>

#include "jaunt.h"

void* libtcg_arm_handle = NULL;
static int initialized = 0;

int (*init_tcg_arm)(void);
void (*exec)(
	uint32_t *regs, uint64_t* fpregs,
    uint32_t *cpsr, uint32_t *fpscr,
    uint32_t *fpexc,
    int dump_reg);

void init_arm_tcg_lib(void)
{
	if (likely(libtcg_arm_handle != NULL))
		return;

	void *handle = dlopen("libtcg_arm.so", RTLD_NOW | RTLD_LOCAL);
	if (handle) {
		init_tcg_arm = dlsym(handle, "init_tcg_arm");
		exec = dlsym(handle, "exec");

		if (init_tcg_arm == NULL || exec == NULL) {
			initialized = 0;
			return;
		}

		init_tcg_arm();

		libtcg_arm_handle = handle;
		initialized = 1;
	}
}

inline int check_vfp_magic(struct vfp_sigframe *vfp) {
	return vfp->magic == VFP_MAGIC;
}

void sig_handler(int signal, siginfo_t *info, void *context) {
	(void) info;
	struct sigcontext *uc_mcontext = &((ucontext_t*)context)->uc_mcontext;
	struct aux_sigframe *aux = (struct aux_sigframe*)&((ucontext_t*)context)->uc_regspace;
	struct vfp_sigframe *vfp = (struct vfp_sigframe*)aux;

	if (signal != SIGILL && info->si_code != ILL_ILLOPC) return;

	/* One time init*/
	init_arm_tcg_lib();

	if (unlikely(!initialized)) {
		printf("ERROR: ARM emulation unit not initialized.\n");
		abort();
	}

	if (unlikely(!check_vfp_magic(vfp))) {
		printf("ERROR: VFP Magic check failed. magic = 0x%lX\n", vfp->magic);
		abort();
	}

	exec(
		(uint32_t*)&uc_mcontext->arm_r0,
		(uint64_t*)vfp->ufp.fpregs,
		(uint32_t*)&uc_mcontext->arm_cpsr,
		(uint32_t*)&vfp->ufp.fpscr,
		(uint32_t*)&vfp->ufp_exc.fpexc,
		0);
}

struct sigaction old_sa, new_sa = {
	.sa_flags     = SA_SIGINFO,
	.sa_sigaction = &sig_handler };

static void prepare() { }
static void parent() { }
static void child() { }

void __attribute__ ((constructor)) init(void) {
	pthread_atfork(&prepare, &parent, &child);
	sigaction(SIGILL, &new_sa, &old_sa);
}

void __attribute__ ((destructor)) finish(void) {
	sigaction(SIGILL, &old_sa, &new_sa);
}
