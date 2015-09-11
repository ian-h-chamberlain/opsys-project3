#include <deque>
#include <iostream>

#include "process.h"

void printQueue(const std::deque<Process> &queueToPrint);

/**
 * simulate: Simulate processes based on the queue passed in
 */
int simulate(const std::deque<Process> &processes) {
    std::deque<Process> execQueue(processes);
    std::deque<Process> ioQueue;

    // number of processes

    // context-switch time delay (ms)
    int t_cs = 13;

    // initial cpu time is zero
    int t = 0;
    std::cout << "time " << t << "ms: Simulator started ";
    printQueue(execQueue);

    // run process until we run out of processes in either queue
    while (execQueue.size() != 0) { // || ioQueue.size() != 0) {
        // add time for context switch
        t += t_cs;

        if (execQueue.size() != 0) {
            // accept the new process
            Process curProc = execQueue.front();
            execQueue.pop_front();

            // print that it is beginning 
            std::cout << "time " << t << "ms: P" << curProc.getNum()
                << " started using the CPU ";
            printQueue(execQueue);

            // and change the clock time and number of bursts it needs
            t += curProc.getBurstTime();
            curProc.runBurst();

            // check whether or not the process should be terminated
            if (!curProc.isComplete()) {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " completed its CPU burst ";
                printQueue(execQueue);

                // add to the ioQueue once process completes its burst
                execQueue.push_back(curProc);
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
void printQueue(const std::deque<Process> &queueToPrint) {
    // if we have size zero, then just print "[Q]"
    if (queueToPrint.size() == 0) {
        std::cout << "[Q]" << std::endl;
        return;
    }
    // print "[Q 1 2 ... ]"
    std::cout << "[Q ";
    for (unsigned int i=0; i<queueToPrint.size() - 1; i++) {
        std::cout << queueToPrint[i].getNum() << " ";
    }
    std::cout << queueToPrint[queueToPrint.size() - 1].getNum() << "]" << std::endl;
}
