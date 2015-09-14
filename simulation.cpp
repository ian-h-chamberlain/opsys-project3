#include <list>
#include <iostream>
#include <algorithm>

#include "process.h"

bool compareProcesses (const Process &p1, const Process &p2);

void printQueue(const std::list<Process> &queueToPrint);

/**
 * simulate: Simulate processes based on the queue passed in
 */
int simulate(const std::list<Process> &processes) {
    std::list<Process> execQueue(processes);
    Process curProc;

    // context-switch time delay (ms)
    int t_cs = 13;

    // initial cpu time is zero
    int t = 0;
    std::cout << "time " << t << "ms: Simulator started ";
    printQueue(execQueue);

    // run process until we run out of processes in either queue
    while (execQueue.size() > 0) {

        // accept the new process
        curProc = execQueue.front();
        execQueue.pop_front();

        // if the process is in line for CPU and has not fired any bursts yet
        if (!curProc.isIO() && curProc.getDoneTime() < 0) {

            // add time for context switch
            t += t_cs;

            // print that it is beginning 
            std::cout << "time " << t << "ms: P" << curProc.getNum()
                << " started using the CPU ";
            printQueue(execQueue);

            // set the done time for the burst
            curProc.runBurst(t);

            std::list<Process>::iterator itr = execQueue.begin();
            // place the process back into the queue according to its done_time
            while (itr->getDoneTime() > 0 && itr->getDoneTime() < curProc.getDoneTime())
                itr++;
            execQueue.insert(itr, curProc);
            std::cerr << "inserting " << curProc.getNum();
            if (curProc.isIO())
                std::cerr << "IO";
            std::cerr << " with time " << curProc.getDoneTime() << " ";
            printQueue(execQueue);
        }
        else if (!curProc.isIO()) {
            t = curProc.getDoneTime();
            // re-add the process
            if (!curProc.isComplete()) {

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " completed its CPU burst ";
                printQueue(execQueue);

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " performing I/O ";
                printQueue(execQueue);

                // set the done time for IO
                curProc.toggleIO();
                curProc.startIO(t);

                std::list<Process>::iterator itr = execQueue.begin();
                // place the process back into the queue according to its done_time
                while (itr->getDoneTime() < 0 || itr->getDoneTime() < curProc.getDoneTime())
                    itr++;
                execQueue.insert(itr, curProc);
                std::cerr << "inserting " << curProc.getNum();
                if (curProc.isIO())
                    std::cerr << "IO";
                std::cerr << " with time " << curProc.getDoneTime() << " ";
                printQueue(execQueue);
            }
            else {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " terminated ";
                printQueue(execQueue);
            }
        }

        else if (curProc.isIO()) {
            t = curProc.getDoneTime();

            curProc.toggleIO();
            execQueue.push_back(curProc);

            std::cout << "time " << t << "ms: P" << curProc.getNum()
                << " completed I/O ";
            printQueue(execQueue);
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
        if (!(itr->isIO()))
            std::cout << " " << itr->getNum();
        else
            std::cout << " " << itr->getNum() << "IO";
    }
    std::cout << "]" << std::endl;
}
