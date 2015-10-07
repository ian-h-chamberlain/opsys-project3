#include <list>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "process.h"

// use -D DEBUG_MODE when compiling to generate verbose output for debugging purposes

/**
 *  printQueue(std::list<Process>) - Prints the queue given, in the format [Q 1 2 ... ]
 */
void printQueue(const std::list<Process> &queueToPrint);

/**
 * simulate(std::list<Process>) - Simulate processes based on the queue passed in
 */
int simulateFCFS(const std::list<Process> &processes, std::ofstream &outfile, int t_cs) {
    std::list<Process> execQueue(processes);    // the execution queue
    std::list<Process> ioQueue; // a container for the processes in I/O
    Process curProc;

    int contextSwitches = 0;
    double burstTime = 0.0;
    double waitTime = 0.0;
    double turnaroundTime = 0.0;
    int numBursts = 0;

    outfile << "Algorithm FCFS" << std::endl;
    outfile << "-- average CPU burst time: ";
    std::list<Process>::iterator itr = execQueue.begin();
    while (itr != execQueue.end()) {
        burstTime += itr->getBurstTime() * itr->getNumBursts();
        numBursts += itr->getNumBursts();
        itr++;
    }

    // n = the number of processes can simply be found using processes.size()
    // so creating another variable for it was extraneous

    // initial cpu time is zero
    int t = 0;
    int last_t = 0;     // this stores the previous time so we can go back to it

    std::cout << "time " << t << "ms: Simulator started for FCFS ";
    printQueue(execQueue);

    // run process until we run out of processes in either queue
    while (execQueue.size() > 0 || ioQueue.size() > 0) {

        if (execQueue.size() > 0) {
            // accept the new process
            curProc = execQueue.front();
        }

        // check the I/O queue for any events that have occurred in the meantime
        std::list<Process>::iterator itr = ioQueue.begin();
        while (itr != ioQueue.end()) {
            if ((itr->getDoneTime() > 0 && itr->getDoneTime() < t)
                    || execQueue.size() <= 0) {

                // temporarily hold the last process
                Process tmp;
                if (curProc.getNum() == execQueue.front().getNum()) {
                    tmp = execQueue.front();
                    execQueue.pop_front();
                }
                // make an empty process if needed
                else {
                    tmp = Process(-1, 0, 0, 0, 0);
                }

                std::cout << "time " << itr->getDoneTime() << "ms: P" << itr->getNum()
                    << " completed I/O ";

                // if the queue is empty, use the time from this ioQueue process
                if (execQueue.size() <= 0) {
                    last_t = t;

                    t = itr->getDoneTime();
                    

#ifdef DEBUG_MODE
                    std::cerr << "t reset to " << t << std::endl;
#endif
                }
                
                // do IO work
                itr->runIO();

                // and re-add the process to the execution queue
                execQueue.push_back(*itr);
                itr = ioQueue.erase(itr);

                printQueue(execQueue);

                // put the temporary process back if it existed originally
                if (tmp.getNum() != -1) {
                    execQueue.push_front(tmp);
                }

                // set the current process again
                curProc = execQueue.front();

#ifdef DEBUG_MODE
                std::cerr << "IO: ";
                printQueue(ioQueue);
                std::cerr << "P" << execQueue.back().getNum() << ": " << execQueue.back().getDoneTime();
                std::cerr << std::endl;
#endif
            }
            // keep moving through the IO queue
            else {
                itr++;
            }
        }

        // if the current process is in line for CPU and has not fired any bursts yet
        if (curProc.getDoneTime() < 0) {
            // add time for context switch
            t += t_cs;
            contextSwitches++;

            // remove it from the queue
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
                    // check for ties and break them using getNum()
                    else if (itr->getDoneTime() == curProc.getDoneTime()) {
                        if (itr->getNum() > curProc.getNum()) {
                            ioQueue.insert(itr, curProc);
                            break;
                        }
                    }
                    itr++;
                }
                if (itr == ioQueue.end())
                    ioQueue.push_back(curProc);
            }

            // update last_t
            last_t = t;

#ifdef DEBUG_MODE
            std::cerr << "curProc = " << curProc.getNum() << " ";
            printQueue(execQueue);
            std::cerr << "IO: ";
            printQueue(ioQueue);
#endif
        }
        else {
            // reset t to the last t if necessary
            if (t != last_t) {
                t = last_t;
            }

            // the process must be ending, so remove it from the queue
            execQueue.pop_front();

#ifdef DEBUG_MODE
            std::cerr << "P" << curProc.getNum() << ": ";
            std::cerr << curProc.getBurstTime() << std::endl;
#endif
            std::list<Process>::iterator proc_itr = execQueue.begin();
            while (proc_itr != execQueue.end()) {
                proc_itr->wait(curProc.getBurstTime());
                proc_itr++;
            }


            // print the message for the process completing a burst and entering I/O
            if (!curProc.isComplete()) {

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " completed its CPU burst ";
                printQueue(execQueue);

                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " performing I/O ";
                printQueue(execQueue);
            }
            // or terminate the process if it's on its last burst
            else {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " terminated ";
                printQueue(execQueue);

                waitTime += curProc.getWaitTime();
            }
        }
    }

    turnaroundTime = burstTime + waitTime;

    burstTime /= numBursts;
    waitTime /= numBursts;
    turnaroundTime /= numBursts;
    outfile << std::setprecision(5) << burstTime << std::endl;
    outfile << "-- average wait time: " << std::setprecision(5) << waitTime << std::endl;
    outfile << "-- average turnaround time: " << std::setprecision(5) << turnaroundTime << std::endl;
    outfile << "-- total number of context switches: " << contextSwitches << std::endl;

    // return the final time in ms
    return t;
}

/**
 *  printQueue(std::list<Process>) a helper function to print the process queue
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
