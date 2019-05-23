# memory-injector
A simple and effective memory/shellcode injector for unix based operating systems in C


Setup:

Run setup.sh:

    $ chmod +x setup.sh
    $ ./setup.sh

Or compile the source code yourself:

    $ gcc -o ./inject src/inject.c
    $ gcc -o ./hello src/hello.c
    

Usage:

  Firstly, run the `hello` program:
    
    $ ./hello
    
    Testing. Hello!
    Press ENTER to exit :)

Then, find its process ID using `top` or `ps -u [user] | grep hello | awk {'print $1'}`. Yours will be different to mine:
  
    $ ps -u root | grep hello | awk {'print $1'}
    
    2468
 
 Now, run `inject` with the process ID as the first argument.
 
    $ ./inject 2468
    
    [+] Process Attachment Status: Success
    [+] Registers Read Status: Success
    [+] Injecting Shellcode Into Current RIP Address [98ec1ab5]
    [+] Injection Status: Success
    
    Detaching ... Done.
 
 This has changed the next functioning instruction to the shellcode in the file `inject.c` which is `/bin/sh`.
 When we go back over to `hello` and press ENTER, it should execute `/bin/sh` instead of exiting which shows a successful injection.
 
    Testing. Hello!
    Press ENTER to exit :)
  
    $ ls
    hello   inject   setup.sh   src
  
