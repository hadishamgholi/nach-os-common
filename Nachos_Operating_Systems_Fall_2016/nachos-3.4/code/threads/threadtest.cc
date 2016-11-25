/*// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "synchlist.h"

#include "time.h"

// testnum is set in main.cc
int testnum = 1;

int counter = 0;
int buffer[15] = {};
Lock * bufferLock = new Lock("ourLock");
List * threadQueue = new List();

Thread *p1, *p2, *c1, *c2;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void producer(int which){
	printf("\n+++ producer %d is starting...\n\n" , which);
	if ((unsigned long)time(NULL)%5 < 2) currentThread->Yield();
	while(true)
	{

		bufferLock->Acquire();
		if ((unsigned long)time(NULL)%5 < 2) currentThread->Yield();

		if (counter < 10){
			printf("+++ producer %d is writing...\n" , which);
			buffer[counter] = 1;
			++counter;
			bufferLock->Release();
			printf(">>> counter is : %d \n" , counter);
		} else {
			bufferLock->Release();
//			if (p1 != currentThread) {
//				printf("producer1 %d is sleeping...\n" ,which );
//				p1->Sleep();
//				threadQueue->Append(p1);
//			} else {
//				printf("producer2 is sleeping...\n" ,which );
//				p2->Sleep();
//				threadQueue->Append(p2);
//			}
			currentThread->Yield();
//			currentThread->Sleep();
		}

		if ((unsigned long)time(NULL)%5 < 2) currentThread->Yield();

//		if (counter == 1){
//			while (!threadQueue->IsEmpty()) {
//				Thread *thread;
//				thread = (Thread *) threadQueue->Remove();
//				printf("WWW %s is waking up...\n" ,thread->getName() );
//				scheduler->ReadyToRun(thread);
//			}
//		}

		nanosleep((const struct timespec[]){{0, 400000000L}}, NULL);
		if ((unsigned long)time(NULL)%5 < 2) currentThread ->Yield();
	}
}


void consumer(int which){
	printf("\n--- consumer %d is starting...\n\n" , which);
	if ((unsigned long)time(NULL)%2 == 1) currentThread->Yield();
	while(true){

			bufferLock->Acquire();
			if ((unsigned long)time(NULL)%2 == 1) currentThread->Yield();
			if (counter > 0){
				printf("--- consumer %d is reading...\n" , which);
				buffer[counter] = 0;
				--counter;
				bufferLock->Release();
				printf(">>> counter is : %d \n" , counter);
			} else {
				bufferLock->Release();
//				if (c1 != currentThread) {
//					printf("consumer1 is sleeping...\n" ,which );
//					c1->Sleep();
//					threadQueue->Append(c1);
//				} else {
//					printf("consumer2 is sleeping...\n" ,which );
//					c2->Sleep();
//					threadQueue->Append(c2);
//				}
//				printf("consumer %d is sleeping...\n" ,which );
				currentThread->Yield();
//				currentThread->Sleep();
			}
			if ((unsigned long)time(NULL)%2 == 1) currentThread->Yield();

//			if (counter == 9){
//				while (!threadQueue->IsEmpty()) {
//					Thread *thread;
//					thread = (Thread *) threadQueue->Remove();
//					printf("WWW %s is waking up...\n" ,thread->getName() );
//					scheduler->ReadyToRun(thread);
//				}
//			}

		nanosleep((const struct timespec[]){{0, 300000000L}}, NULL);
		if ((unsigned long)time(NULL)%2 == 1) currentThread->Yield();
	}
}


void
SimpleThread(int which)
{
    int num;
    
    for (num = 1; num <= 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void
starter(int which){

//	int num;
//
//	for (num = 1; num <= 5; num++) {
//		printf("*** thread %d looped %d times\n", which, num);
//		currentThread->Yield();
//	}
}

void
myTHreadTwo(int which){
	int num;
	printf("*** thread %d looped %d times\n", which, num);
	currentThread->Yield();
	printf("*** thread %d looped %d times\n", which, num);
	currentThread->Yield();
	printf("*** thread %d looped %d times\n", which, num);
	currentThread->Yield();
	printf("*** thread %d looped %d times\n", which, num);
	currentThread->Yield();
	printf("*** thread %d looped %d times\n", which, num);
	currentThread->Yield();
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    p1 = new Thread("producer1");
    p2 = new Thread("producer2");
    c1 = new Thread("consumer1");
    c2 = new Thread("consumer2");

    //starter(10);

    printf("%d\n", (unsigned long)time(NULL));

    p1->Fork(producer, 1);
    p2->Fork(producer, 2);

    c1->Fork(consumer, 1);
    c2->Fork(consumer, 2);

//    myTHread(20);
//    SimpleThread(0);
//    SimpleThread(2);
//    SimpleThread(3);
    //myTHread(1);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <unistd.h>

#define MAX_THREAD_TIME 100
#define INIT_THREAD_TIME 200


// testnum is set in main.cc
int testnum = 1;


void os_function(int n)
{
	char * name = currentThread->getName();
	printf(">>> thread %s, running for %d seconds\n", name ,n);
	usleep(n * 1000 * 1000);

	return;
}


void
ThreadTest1()
{
    Thread *t1 = new Thread("1");
    Thread *t2 = new Thread("2");
    Thread *t3 = new Thread("3");
    Thread *t4 = new Thread("4");
    Thread *t5 = new Thread("5");
    Thread *t6 = new Thread("6");
    Thread *t7 = new Thread("7");
    Thread *t8 = new Thread("8");
    Thread *t9 = new Thread("9");
    Thread *t10 = new Thread("10");


    Thread *t11 = new Thread("1");
    Thread *t12 = new Thread("2");
    Thread *t13 = new Thread("3");
    Thread *t14 = new Thread("4");
    Thread *t15 = new Thread("5");
    Thread *t16 = new Thread("6");
    Thread *t17 = new Thread("7");
    Thread *t18 = new Thread("8");
    Thread *t19 = new Thread("9");
    Thread *t20 = new Thread("10");


	t1->Fork(os_function, 11); // 11
	t2->Fork(os_function, 4);
	t3->Fork(os_function, 3);
	t4->Fork(os_function, 6);
	t5->Fork(os_function, 7); // 7
	t6->Fork(os_function, 10); // 10
	t7->Fork(os_function, 8); // 8
	t8->Fork(os_function, 2);
	t9->Fork(os_function, 1);
	t10->Fork(os_function, 5);

	t11->Fork(os_function, 11);
	t12->Fork(os_function, 4);
	t13->Fork(os_function, 3);
	t14->Fork(os_function, 6);
	t15->Fork(os_function, 7);
	t16->Fork(os_function, 10);
	t17->Fork(os_function, 8);
	t18->Fork(os_function, 2);
	t19->Fork(os_function, 1);
	t20->Fork(os_function, 5);

	// second time results should be something like this if your scheduler works correctly
	// J9 J8 J3 J2 J10 J4 J5 J7 J6 J1


}


//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
