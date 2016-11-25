// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "scheduler.h"
#include "system.h"
#include <time.h>

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads to empty.
//----------------------------------------------------------------------

#define MAX_THREAD_TIME 100
#define INIT_THREAD_TIME 200

int counter = 1;
int threadId = 0;
//char* threadIdChar;
int startTime;
int endTime;
int lastRecordedTime;
//clock_t startTime;
//clock_t endTime;
//clock_t lastRecordedTime;
bool firstThreadHasStarted = false;

Scheduler::Scheduler()
{
    int i;
    for (i = 0; i < 50; ++i)
    {
        timesOfThreads[i] = INIT_THREAD_TIME * 1.0;        // INIT_THREAD_TIME means no time estimation is available yet.
    }
    readyList = new List; 
    waiting = 0;
    runningThreadId = 0;
} 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{
    delete readyList; 
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRun (Thread *thread)
{
    DEBUG('t', "Putting thread %s on ready list.\n", thread->getName());

    thread->setStatus(READY);
    readyList->Append((void *)thread);
    waiting++;
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun ()
{
    if (firstThreadHasStarted){
        //endTime = clock();
        endTime = time(NULL);
        //printf("endTime is %f\n", (double) endTime);
        //double elapsed = (double)(endTime - lastRecordedTime)/ CLOCKS_PER_SEC;    //elapsed has the time of the thread has been running
        double elapsed = (double)(endTime - lastRecordedTime);
        //printf("***** %f %d %d\n",timesOfThreads[runningThreadId], INIT_THREAD_TIME, runningThreadId);
        if (timesOfThreads[runningThreadId] > 100.0){

            timesOfThreads[runningThreadId] = elapsed;
            //printf("elapsed is %f and timesOfThreads[threadId] is %f \n",elapsed , timesOfThreads[runningThreadId] );
        } else {
            timesOfThreads[runningThreadId] = (elapsed + timesOfThreads[runningThreadId])/2.0;
            //printf(">> elapsed is %f and timesOfThreads[threadId] is %f \n",elapsed , timesOfThreads[runningThreadId] );
        }
    }
    printf("-tt %f %f %f %f %f %f %f %f %f %f \n", timesOfThreads[1], timesOfThreads[2], timesOfThreads[3], timesOfThreads[4], timesOfThreads[5], timesOfThreads[6], timesOfThreads[7], timesOfThreads[8], timesOfThreads[9], timesOfThreads[10]);

    double minTime = MAX_THREAD_TIME;       // supposing that no thread lasts more than MAX_THREAD_TIME.
    Thread* shortestThread = NULL;
    Thread* tempThread = NULL;
    bool newThreadExists = false;
    //Thread* resultThread = NULL;
    int i ;
    for(i = 0; i < waiting; i++){

        tempThread = (Thread *) readyList->Remove();
        int threadId = atoi(tempThread->getName());
        //printf("time of thread %f , thread id %s \n", timesOfThreads[threadId] , tempThread->getName());

        if (timesOfThreads[threadId] > MAX_THREAD_TIME){
            newThreadExists = true;
        } else if(timesOfThreads[threadId] < minTime) {
            minTime = timesOfThreads[threadId];
            shortestThread = tempThread;
        }
        readyList->Append(tempThread);
    }

    printf("-ww %d threads are waiting!\n", waiting);

    
    // return (Thread *)readyList->Remove();
    if (newThreadExists){

        shortestThread = (Thread *) readyList->Remove();

    } else {

        printf("-ss shortest thread found!\n");

        for(i = 0; i < waiting; i++){

            tempThread = (Thread *) readyList->Remove();
            int threadId = atoi(tempThread->getName());

            if (timesOfThreads[threadId] != minTime){
                readyList->Append(tempThread);
            }else{
                shortestThread = tempThread;
            }
        }
    }
    waiting--;

    runningThreadId = atoi(shortestThread->getName());
    lastRecordedTime = time(NULL);

    printf("--> granting cpu to thread %d.\n", runningThreadId);

    return shortestThread;
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread){
    firstThreadHasStarted = true;

    //startTime = clock();
    // printf("startTime is %f\n", (double) startTime);


    Thread *oldThread = currentThread;
    
#ifdef USER_PROGRAM			// ignore until running user programs 
    if (currentThread->space != NULL) {	// if this thread is a user program,
        currentThread->SaveUserState(); // save the user's CPU registers
	currentThread->space->SaveState();
    }
#endif

    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    currentThread = nextThread;		    // switch to the next thread
    currentThread->setStatus(RUNNING);      // nextThread is now running
    
   //  DEBUG('t', "Switching from thread \"%s\" to thread \"%s\"\n",
	  // oldThread->getName(), nextThread->getName());
    // printf("  Switching from thread \"%s\" to thread \"%s\"\n",oldThread->getName(), nextThread->getName());
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);
    
    DEBUG('t', "Now in thread \"%s\"\n", currentThread->getName());
    // printf("!!! Now in thread \"%s\"\n", currentThread->getName());
    // If the old thread gave up the processor because it was finishing,
    // we need to delete its carcass.  Note we cannot delete the thread
    // before now (for example, in Thread::Finish()), because up to this
    // point, we were still running on the old thread's stack!
    if (threadToBeDestroyed != NULL) {
        delete threadToBeDestroyed;
	threadToBeDestroyed = NULL;
    }
    
#ifdef USER_PROGRAM
    if (currentThread->space != NULL) {		// if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
	currentThread->space->RestoreState();
    }
#endif

}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    printf("Ready list contents:\n");
    readyList->Mapcar((VoidFunctionPtr) ThreadPrint);
}
