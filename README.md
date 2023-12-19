Chat-Client-and-Server<br />
An application that is dedicated to practice and improve my socket programming skills <br />
I mainly learnt from [Beej's guide to networking](https://beej.us/guide/bgnet/) and [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/) <br />


This application mainly uilitzed UDP, C and Thread synchronization to achieve a simple "chat"-like facility that enables
someone at one terminal (or Xterm) to communicate with someone at another terminal. <br />

Process structure of the application: <br />

Required threads (in each of the processes):
• One of the threads does nothing other than await input from the keyboard.
• Another thread does nothing other than await a UDP datagram.
• There will also be a thread which prints characters to the screen.
• Finally a thread which sends data to the remote UNIX process over the network using
UDP.


All four threads will share access to a customized `list ADT`:
• The keyboard input thread, on receipt of input, adds the input to the list of messages
that need to be sent to the remote s-talk client.
• The UDP output thread will take each message off this list and send it over the network
to the remote client.
• The UDP input thread, on receipt of input from the remote s-talk client, will put the
message onto the list of messages that need to be printed to the local screen.
• The screen output thread will take each message off this list and output it to the screen. 


Example:
![image](https://github.com/suke404/Chat-Client-and-Server/assets/85700523/3745ed8b-9d5b-44f7-8d90-fdc877366669)
