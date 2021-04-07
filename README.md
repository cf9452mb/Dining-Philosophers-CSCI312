# Project5

compile firstNode.c: cc -std=c99 firstNode.c -o firstNode

compile node.c     : cc node.c -o node

compile coordinator.c:  cc coordinator.c -o coordinator


Start program by opening up a window and executing ./firstNode

Open up 5 more windows and execute ./node

**Note: If you run ./firstNode or ./node and the binding fails, Control+C and re-run the program.
        This may happen when running the programs multiple times since the ports may be the same 
        as the previous program that was ran**

Connect each program by entering the neighbors port number which will display when each program is ran

**Note: Make sure to have 6 total processes, otherwise error message will print and program(s) exit**
