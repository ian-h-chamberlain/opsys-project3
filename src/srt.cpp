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

    // get the first process
    Process curProc = *(execQueue.begin());
    execQueue.erase(execQueue.begin());

    int t = 0;

    std::cout << "time " << t << "ms: Simulator started for SRT ";
    printQueue(execQueue);

    while (execQueue.size() > 0 || ioQueue.size() > 0) {
        if (execQueue.size() > 0) {
            if (curProc.getDoneTime() - curProc.getBurstTime() == t) {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " started using the CPU ";
                printQueue(execQueue);
            }
            else if (curProc.getDoneTime() < 0) {
                curProc.runBurst(t + t_cs);
            }
            else if (curProc.getDoneTime() >= t) {
                ioQueue.push_back(curProc);
                curProc = *(execQueue.begin());
                execQueue.erase(execQueue.begin());
            }
            t++;
            continue;
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
