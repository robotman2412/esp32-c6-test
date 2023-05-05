/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "runtime.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <memory>

namespace runtime {

// Set up user context and run user code.
int runUserCode(bool &success, kernel::ctx_t &kctx, loader::Linkage &prog, abi::Context &actx, int argc, const char **argv, const char **envp) {
	memset(&kctx.u_regs, 0, sizeof(kctx.u_regs));
	
	// Allocate user stack.
	kctx.u_regs.sp = actx.map(setting::STACK_ENTRIES * sizeof(long));
	kctx.u_regs.sp += setting::STACK_ENTRIES * sizeof(long);
	if (!kctx.u_regs.sp) {
		success = false;
		return -1;
	}
	
	// Measure envp.
	// int envp_len;
	// for (envp_len = 0; envp[envp_len]; envp_len++);
	
	// Map memory for argc and envp.
	// size_t mem = actx.map(envp_len * sizeof(char*) + argc * sizeof(char*));
	// if (!mem) {
	// 	success = false;
	// 	return -1;
	// }
	
	// Copy argv and envp to user memory.
	// char **new_argv = (char **) mem;
	// char **new_envp = (char **) (mem + argc * sizeof(char*));
	// memcpy(new_argv, argv, argc * sizeof(char*));
	// memcpy(new_envp, envp, envp_len * sizeof(char*));
	
	// Set up usermode registers.
	kctx.u_pc      = (long) prog.getEntryFunc();
	// kctx.u_regs.a0 = argc;
	// kctx.u_regs.a1 = (long) new_argv;
	// kctx.u_regs.a2 = (long) new_envp;
	asm volatile ("mv %0, gp" : "=r" (kctx.u_regs.gp));
	asm volatile ("mv %0, tp" : "=r" (kctx.u_regs.tp));
	
	// Run user program.
	// kernel::setCtx(&kctx);
	asm volatile (
		"  fence\n"		// Fence for user data
		"  fence.i\n"	// Fence for user code
		"  li a0, %0\n"	// Set system call number
		"  ecall\n"		// Perform system call
		:: "i" (kernel::SYS_USERJUMP)
		: "a0", "memory"
	);
	printf("?????\n");
	
	// Return from this whole ordeal.
	success = true;
	return -1;
}

// FreeRTOS task-code.
static void taskCode(void *context) {
	// Get program context.
	auto  pair = (std::pair<loader::Linkage, abi::Context &> *) context;
	auto &prog = pair->first;
	auto &actx = pair->second;
	prog.garbageCollect();
	
	// Try to set up the kernel.
	if (prog.isProgReady()) {
		// Allocate a context.
		kernel::ctx_t kctx;
		kctx.pid = actx.getPID();
		kernel::setCtx(&kctx);
		
		// Collect user parameters.
		int         argc   = 1;
		const char *argv[] = { "a.out" };
		const char *envp[] = { NULL };
		
		// Run user code.
		bool success;
		int ec = runUserCode(success, kctx, prog, actx, argc, argv, envp);
		if (success) {
			printf("Process %d exited with code %d\n", kctx.pid, ec);
		} else {
			printf("Unable to run process %d\n", kctx.pid);
		}
		
		printf("!!!\n");
		kernel::setDefaultCtx();
	}
	
	printf("lolol\n");
	
	// Free resources and exit.
	delete pair;
	vTaskDelete(NULL);
}

// Take a pre-loaded linkage and start it under a new thread.
bool startPreloaded(loader::Linkage &&linkage, abi::Context &ctx) {
	// This pointer will be managed by the task from now on.
	auto ptr = new std::pair<loader::Linkage, abi::Context &>(std::move(linkage), ctx);
	TaskHandle_t handle;
	auto res = xTaskCreate(taskCode, "", 2048, ptr, 0, &handle);
	if (res == pdPASS) {
		return true;
	} else {
		delete ptr;
		return false;
	}
}

}
