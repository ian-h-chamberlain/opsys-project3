#include <list>
#include <iostream>
#include <algorithm>

#include "process.h"

// #define DEBUG_MODE

bool compareProcesses (const Process &p1, const Process &p2);

void printQueue(const std::list<Process> &queueToPrint);

/**
 * simulate: Simulate processes based on the queue passed in
 */
int simulate(const std::list<Process> &processes) {
    std::list<Process> execQueue(processes);
    std::list<Process> ioQueue;
    Process curProc;

    // context-switch time delay (ms)
    int t_cs = 13;

    // initial cpu time is zero
    int t = 0;
    std::cout << "time " << t << "ms: Simulator started ";
    printQueue(execQueue);

    // run process until we run out of processes in either queue
    while (execQueue.size() > 0 || ioQueue.size() > 0) {

        if (execQueue.size() > 0) {
            // accept the new process
            curProc = execQueue.front();
        }
        else {
            t = ioQueue.front().getDoneTime() + ioQueue.front().getBurstTime();
            curProc.runIO();
        }

        // check the I/O queue for any events that have occurred in the meantime
        std::list<Process>::iterator itr = ioQueue.begin();
        while (itr != ioQueue.end()) {
            if (itr->getDoneTime() < t) {

                // temporarily hold the last process
                Process tmp;
                if (curProc.getNum() == execQueue.front().getNum()) {
                    tmp = execQueue.front();
                    execQueue.pop_front();
                }
                else {
                    tmp = Process(-1, 0, 0, 0);
                }

                std::cout << "time " << itr->getDoneTime() << "ms: P" << itr->getNum()
                    << " completed I/O ";

                itr->runIO();

                execQueue.push_back(*itr);
                itr = ioQueue.erase(itr);

                printQueue(execQueue);

                // put the temporary process back
                if (tmp.getNum() != -1) {
                    execQueue.push_front(tmp);
                }

#ifdef DEBUG_MODE
                std::cerr << "IO: ";
                printQueue(ioQueue);
#endif
                continue;
            }
            else
                itr++;
        }

        // if the process is in line for CPU and has not fired any bursts yet
        if (curProc.getDoneTime() < 0) {
            // add time for context switch
            t += t_cs;

            execQueue.pop_front();
            // print that it is beginning 
            std::cout << "time " << t << "ms: P" << curProc.getNum()
                << " started using the CPU ";
            printQueue(execQueue);

            // then add time for the process
            t += curProc.getBurstTime();

            // set the done time for the IO after the burst
            curProc.runBurst(t);

            execQueue.push_front(curProc);

            // add the process to the IO queue
            if (!curProc.isComplete()) {
                std::list<Process>::iterator itr = ioQueue.begin();
                while (itr != ioQueue.end()) {
                    if (itr->getDoneTime() > curProc.getDoneTime()) {
                        ioQueue.insert(itr, curProc);
                        break;
                    }
                    itr++;
                }
                if (itr == ioQueue.end())
                    ioQueue.push_back(curProc);
            }

#ifdef DEBUG_MODE
            std::cerr << "curProc = " << curProc.getNum() << " ";
            printQueue(execQueue);
            std::cerr << "IO: ";
            printQueue(ioQueue);
#endif
        }
        else {
            // the process must be ending
            execQueue.pop_front();
            // re-add the process
            if (!curProc.isComplete()) {

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " completed its CPU burst ";
                printQueue(execQueue);

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " performing I/O ";
                printQueue(execQueue);

                // set the done time for IO

#ifdef DEBUG_MODE
                std::cerr << "inserting " << curProc.getNum()
                    << " with time " << curProc.getDoneTime() << " ";
                printQueue(execQueue);
                std::cerr << "IO: ";
                printQueue(ioQueue);
#endif
            }
            else {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " terminated ";
                printQueue(execQueue);
            }
        }
    }

    // return the final time in ms
    return t;
}

/**
 *  printQueue: a helper function to print the process queue
 */
void printQueue(const std::list<Process> &queueToPrint) {
    // if we have size zero, then just print "[Q]"
    if (queueToPrint.size() == 0) {
        std::cout << "[Q]" << std::endl;
        return;
    }
    // print "[Q 1 2 ... ]"
    std::cout << "[Q";
    std::list<Process>::const_iterator itr;
    for (itr = queueToPrint.begin(); itr != queueToPrint.end(); itr++) {
            std::cout << " " << itr->getNum();
    }
    std::cout << "]" << std::endl;
}
