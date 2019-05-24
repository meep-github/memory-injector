# memory-injector
A simple and effective memory/shellcode injector for unix based operating systems in C


Setup:

Run setup.sh:

    $ chmod +x setup.sh
    $ ./setup.sh
Or compile the source code yourself:

    $ gcc -o ./inject src/inject.c
    $ gcc -o ./hello src/hello.c
    

**Usage**


Firstly, run the `hello` program:
    
    $ ./hello
 Now, run `inject` with one of the two arguments to help find a process ID to inject.
 
    $ ./inject [-p PROCESS ID | -n PROCESS NAME]
Example 1:

    $ ./hello
    
    # Then, on another terminal, run:
    
    $ ./inject -n hello
    
    [!] Found Process ID 2468 From Process Name hello
    
    [+] Process Attachment Status: Success
    [+] Registers Read Status: Success
    [+] Injecting Shellcode Into Current RIP Address [98ec1ab5]
    [+] Injection Status: Success
    
    Detaching ... Done.
Example 2:

    $ ./hello
    
    # Then, on another terminal, run the following:
    
    $ ps -u [enter your username here] | grep hello | awk {'print $1'} | head -n 1
    2468    # Yours will be different to mine!
    
    $ ./inject -p 2468
    
    [+] Process Attachment Status: Success
    [+] Registers Read Status: Success
    [+] Injecting Shellcode Into Current RIP Address [98ec1ab5]
    [+] Injection Status: Success
    
    Detaching ... Done.


 This has changed the next functioning instruction (the RIP address) to the shellcode in the file `inject.c` which is `/bin/sh`.
 When we go back over to `hello` and press ENTER, it should execute `/bin/sh` instead of exiting which shows a successful injection.
 
    Testing. Hello!
    Press ENTER to exit :)
  
    $ ls
    hello   inject   README.md   setup.sh   src
  
