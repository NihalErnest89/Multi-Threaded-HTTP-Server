#Assignment 3 directory

This directory contains source code and other files for Assignment 3.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.

##Design

Used an array of void pointers, declared in the queue struct.

Used mutex locks from pthread. In push and pop, mutex lock at the beginning, before while loop, and unlock at the end of the function.

Used condition variables from pthread. Used in waits during while loops of push and pop.
also used for pthread cond signal.

Overall structure of push and pop were inspired by pseudo outline shown in lectures.
This includes the in and out integer variables, used to make the indexes wrap around the array size, using modulus

##Testing/Bugs

Found that doing the lock after the while loop and wait leads to endless busy waiting.
This led me to place the locks before the while loop containing the waits.

Found that we don't have to prevent popping from empty array or pushing to a full array.
Instead just wait till the opposing function is called elsewhere to make the array suitable.
