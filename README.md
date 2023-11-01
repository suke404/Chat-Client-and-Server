# cmpt300A2

## Memory Issue

During testing, we discovered a memory issue in the program that can cause memory leaks and lost memory blocks. The issue appears to be related to the dynamic linker (`ld.so`) when loading shared libraries.

Due to it being an issue with the dynamic linker, we could not ascertain the exact cause of the issue. We found that the problem was only present when running our program on CSIL machines.
On our home computers, the memory leak was not present.

\n We also had issues properly terminating the threads using pthread_join. Threads were hanging during the join process and thus the program would run indefinitely. We resolved this by
cancelling the threads, and then joining them, which would prevent any infinite looping.