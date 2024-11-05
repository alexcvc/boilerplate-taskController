## Task Manager Boilerplate

Task Manager (TM) is a class that can start, stop, shutdown and control other threads, pass data to each other, wake up threads and many other things.
This task is useful in case of multi-threaded systems in embedded telecontrol applications.

## Summary of capabilities

1. Main function is daemon (optional)
1. The main function will start the application context with the TM.
2. The TM starts a large number of threads with a stop token.
3. Threads are able to send any kind of shared data to other threads that are using the TM.
4. The TM is able to stop all threads.
5. The TM has the ability to restart or reconfigure threads.
6. The TM is able to kill all threads.
7. The TM is able to restart task processes in all threads.

## What is in the plan

Enjoy and good ideas are welcome!
