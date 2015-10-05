#include <list>
#include <iostream>
#include <algorithm>
#include <set>

#include "process.h"

// use -D DEBUG_MODE when compiling to generate verbose output for debugging purposes

typedef std::set<Process, CompareProcess> procQueue;

void printQueue(const procQueue &queueToPrint);
void printQueue(const std::list<Process> &queueToPrint);

int simulateSRT(const std::list<Process> &processes, int t_cs) {

    procQueue execQueue(processes.begin(), processes.end(), CompareProcess());    // the execution queue
    std::list<Process> ioQueue; // a container for the processes in I/O

    int t = 0;
    int curProcTime = 0;

    // do our output
    std::cout << "time " << t << "ms: Simulator started for SRT ";
    printQueue(execQueue);

    // get the first process
    Process curProc = *(execQueue.begin());
    execQueue.erase(execQueue.begin());

    // now run the processes
    while (execQueue.size() > 0 || ioQueue.size() > 0) {

        std::list<Process>::iterator itr = ioQueue.begin();
        // check for completed IO processes
        while (itr != ioQueue.end()) {
            if (t == itr->getDoneTime()) {
                std::cout << "time " << t << "ms: P" << itr->getNum()
                    << " completed I/O ";

                itr->runIO();

                // preempt the process if the remaining time is too long
                if (curProc.getRemainingTime() > itr->getBurstTime()) {
                    execQueue.insert(curProc);
                    printQueue(execQueue);
                    std::cout << "time " << t << "ms: P" << curProc.getNum() 
                        << " preempted by P" << itr->getNum() << " ";
                    curProc = *itr;
                    curProcTime = t + t_cs;
                    printQueue(execQueue);
                }
                else {
                    printQueue(execQueue);
                    execQueue.insert(*itr);
                }

                itr = ioQueue.erase(itr);

                // TODO: fix preemption and account for partially completed bursts

            }
            else {
                itr++;
            }
        }

        if (execQueue.size() >= 0) {
            // prepare for the next process to start after a context switch
            if (curProcTime <= 0) {
                curProcTime = t + t_cs;
            }
            // or start a process if necessary
            else if (t == curProcTime) {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " started using the CPU ";
                printQueue(execQueue);
            }
            // and finish a process if it's done
            else if (curProc.getRemainingTime() <= 0) {

                curProc.runBurst(t);

                if (curProc.isComplete()) {
                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " terminated ";
                    printQueue(execQueue);
                }
                else {
                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " completed its CPU burst ";
                    printQueue(execQueue);

                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " performing I/O ";
                    printQueue(execQueue);

                    ioQueue.push_back(curProc);
                }

                curProcTime = 0;
                if (execQueue.size() > 0) {
                    curProc = *(execQueue.begin());
                    execQueue.erase(execQueue.begin());
                }
                else {
                    // skip ahead to the next I/O completion
                    std::list<Process>::iterator itr;
                    int min_time = -1;
                    for (itr = ioQueue.begin(); itr != ioQueue.end(); itr++) {
                        if (min_time < 0 || itr->getDoneTime() < min_time)
                            min_time = itr->getDoneTime();
                    }
                    t = max_time;
                }

#ifdef DEBUG_MODE
                printQueue(ioQueue);
                std::cout << "P" << curProc.getNum() << std::endl;
                std::cout << "  remain: " << curProc.getRemainingTime() << std::endl;
                std::cout << "  t = " << t << std::endl;
#endif
                continue;
            }
        }
        t++;
        if (t > curProcTime && !curProc.isComplete()) {
            curProc.runForMs(1);
        }
    }

    return t;

}

/**
 * printQueue() - print a priority queue
 */

void printQueue(const procQueue &queueToPrint) {
    // if we have size zero, then just print "[Q]"
    if (queueToPrint.size() == 0) {
        std::cout << "[Q]" << std::endl;
        return;
    }
    // print "[Q 1 2 ... ]"
    std::cout << "[Q";
    procQueue::const_iterator itr;
    for (itr = queueToPrint.begin(); itr != queueToPrint.end(); itr++) {
            std::cout << " " << itr->getNum();
    }
    std::cout << "]" << std::endl;
}
