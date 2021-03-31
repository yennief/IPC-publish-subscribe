# IPC-publish-subscribe

Program compilation:

In order to compile the server program use the command

gcc -Wall inf145307_s.c -o server

which will generate an executable file named server in the current directory.


To compile the client program use the command 

gcc -Wall inf145307_k.c -o client

which will generate an executable file named client in the current directory.


Starting the program:

In order to run the program, you must first use the command

./server

which will start the server program.

Each new client should be opened in a new terminal window.

To run the client program use the command

./customer

After starting the client program, the MENU of available options will be displayed in the terminal, which will guide the user.


A short description of the inf145307_k.c file content:

There is an option menu in the client program, in which we can choose from logging in, recipient registration, message type registration, message type broadcast, message receipt and logout.

A short description of the inf145307_s.c file content:

In the server program, the server responds to the client by sending appropriate messages, adequate to the given option.
