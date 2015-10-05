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

        std::list<Process>::iterator itr;
        // check for completed IO processes
        for (itr = ioQueue.begin(); itr != ioQueue.end(); itr++) {
            if (t == itr->getDoneTime()) {
                std::cout << "time " << itr->getDoneTime() << "ms: P" << itr->getNum()
                    << " completed I/O ";

                execQueue.insert(*itr);
                itr = ioQueue.erase(itr);
                printQueue(execQueue);

                // preempt the process if the remaining time is too long
                if (curProcTime != 0) {
                    if (curProc.getBurstTime() - (t - curProcTime) < itr->getBurstTime()) {
                        /*
                        execQueue.insert(curProc);
                        curProc = *itr;
                        curProcTime = t + t_cs;
                        itr = ioQueue.erase(itr);
                        std::cout << "time " << itr->getDoneTime() << "ms: P" << itr->getNum()
                            << " completed I/O ";
                        printQueue(execQueue);
                        */
                    }
                }

                // TODO: fix preemption and account for partially completed bursts

            }
        }

        if (execQueue.size() > 0) {
            if (curProcTime == 0) {
                curProcTime = t + t_cs;
            }
            else if (t == curProcTime) {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " started using the CPU ";
                curProc.runBurst(t + curProc.getBurstTime());
                printQueue(execQueue);
            }
            else if (t >= curProcTime + curProc.getBurstTime()) {
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
                curProc = *(execQueue.begin());
                execQueue.erase(execQueue.begin());
                continue;
            }
        }
        t++;
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
