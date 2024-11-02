## Task Scheduler Boilerplate

Task scheduler Boilerplate with test and example 

## Summary

1. the main function launches the Task Scheduler. 
2. Scheduler starts many threads with stop token 
3. all threads use scheduler to send thread-shared data to other threads
4. scheduler can stop all threads 
5. scheduler can wake up and kill all threads
6. scheduler can kill all threads
7. scheduler can restart task processes in all threads
