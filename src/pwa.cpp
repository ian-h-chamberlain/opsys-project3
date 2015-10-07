#include <list>
#include <queue>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <set>

#include "process.h"

// use -D DEBUG_MODE when compiling to generate verbose output for debugging purposes

typedef std::set<Process, ComparePriority> procQueue;

void printQueue(const procQueue &queueToPrint);
void printQueue(const std::list<Process> &queueToPrint);

int simulatePWA(const std::list<Process> &processes, std::ofstream &outfile, int t_cs) {

    procQueue execQueue(processes.begin(), processes.end(), ComparePriority());    // the execution queue
    std::list<Process> ioQueue; // a container for the processes in I/O

    int t = 0;
    int curProcTime = 0;

    double burstTime = 0.0;
    double waitTime = 0.0;
    double turnaroundTime = 0.0;
    int numBursts = 0;
    int contextSwitches = 0;

    outfile << "Algorithm PWA" << std::endl;
    procQueue::iterator proc_itr = execQueue.begin();
    while (proc_itr != execQueue.end()) {
        burstTime += proc_itr->getBurstTime() * proc_itr->getNumBursts();
        numBursts += proc_itr->getNumBursts();
        proc_itr++;
    }

    // do our output
    std::cout << "time " << t << "ms: Simulator started for PWA ";
    printQueue(execQueue);

    // get the first process
    Process curProc = *(execQueue.begin());
    execQueue.erase(execQueue.begin());

    // now run the processes
    while (execQueue.size() > 0 || ioQueue.size() > 0 || curProcTime > 0) {

        std::list<Process>::iterator itr = ioQueue.begin();
        // check for completed IO processes
        while (itr != ioQueue.end()) {
            if (t == itr->getDoneTime()) {

                // temporarily add the process for the sake of output
                procQueue::iterator tmp = execQueue.insert(*itr).first;

                std::cout << "time " << t << "ms: P" << itr->getNum()
                    << " completed I/O ";

                // actually change the process to reflect that it completed IO
                itr->runIO();
#ifdef DEBUG_MODE
                std::cout << std::endl << "  remain " << itr->getRemainingTime() << std::endl;
                std::cout << "  priority " << itr->getPriority() << std::endl;
                std::cout << "curproc " << curProc.getNum() << std::endl;
                std::cout << "  remain " << curProc.getRemainingTime() << std::endl;
                std::cout << "  priority " << curProc.getPriority() << std::endl;
#endif

                // preempt the process if the remaining time is too long
                if (curProc.getPriority() > itr->getPriority()) {
                    printQueue(execQueue);
                    execQueue.insert(curProc);
                    execQueue.erase(tmp);   // we don't need the process in the queue
                    std::cout << "time " << t << "ms: P" << curProc.getNum() 
                        << " preempted by P" << itr->getNum() << " ";
                    curProc = *itr;
                    curProcTime = t + t_cs;
                    // calculate wait time
                    proc_itr = execQueue.begin();
                    while (proc_itr != execQueue.end()) {
                        Process tmp(*proc_itr);
                        procQueue::iterator tmp_itr(proc_itr);
                        tmp_itr++;
                        execQueue.erase(proc_itr);
                        tmp.wait(t_cs);
                        execQueue.insert(tmp);
                        proc_itr = tmp_itr;
                    }

                    contextSwitches++;
                    printQueue(execQueue);
                }
                // or, if curProc is done (which only happens when the exec queue is empty)
                // replace it with something from IO
                else if (curProc.getRemainingTime() <= 0) {
                    printQueue(execQueue);
                    execQueue.erase(tmp);   // not necessary to keep this in the queue
                    curProc = *itr;
                }
                // otherwise, just add the IO process to the exec queue
                else {
                    printQueue(execQueue);
                    execQueue.erase(tmp);   // same as above
                    execQueue.insert(*itr);
                }

                itr = ioQueue.erase(itr);

            }
            else {
                itr++;
            }
        }

        if (execQueue.size() >= 0) {
            // prepare for the next process to start after a context switch
            if (curProcTime <= 0) {
                curProcTime = t + t_cs;
                // calculate wait time
                proc_itr = execQueue.begin();
                while (proc_itr != execQueue.end()) {
                    Process tmp(*proc_itr);
                    procQueue::iterator tmp_itr(proc_itr);
                    tmp_itr++;
                    execQueue.erase(proc_itr);
                    tmp.wait(t_cs);
                    execQueue.insert(tmp);
                    proc_itr = tmp_itr;
                }
                contextSwitches++;
            }
            // or start a process if necessary
            else if (t == curProcTime) {
                std::cout << "time " << t << "ms: P" << curProc.getNum()
                    << " started using the CPU ";
                printQueue(execQueue);
            }
            // and finish a process if it's done
            if (curProc.getRemainingTime() <= 0) {

                if (!curProc.isComplete()) {
                    curProc.runBurst(t);

#ifdef DEBUG_MODE
                    std::cout << "burst run:" << std::endl;
                    std::cout << "  remain: " << curProc.getRemainingTime() << std::endl;
                    std::cout << "  curProcTime: " << curProcTime << std::endl;
                    std::cout << "  t=" << t << std::endl;
                    std::cout << "  numBursts " << curProc.getNumBursts() << std::endl;
                    std::cout << "  curBursts " << curProc.getCurBurst() << std::endl;
#endif
                }

                // check for termination
                if (curProc.isComplete()) {
                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " terminated ";
                    printQueue(execQueue);
                }
                // or start I/O if not terminated
                else {
                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " completed its CPU burst ";
                    printQueue(execQueue);

                    std::cout << "time " << t << "ms: P" << curProc.getNum()
                        << " performing I/O ";
                    printQueue(execQueue);

                    waitTime += curProc.getWaitTime();
                    curProc.wait(-curProc.getWaitTime());   // reset the wait time
                    // add the process to I/O
                    ioQueue.push_back(curProc);
                }

                curProcTime = 0;
                // get the first process in the queue if there is one
                if (execQueue.size() > 0) {
                    curProc = *(execQueue.begin());
                    execQueue.erase(execQueue.begin());
                }
                else {
                    // skip ahead to the next I/O completion
                    std::list<Process>::iterator itr;
                    // find the minimum done time
                    int min_time = -1;
                    for (itr = ioQueue.begin(); itr != ioQueue.end(); itr++) {
                        if (min_time < 0 || itr->getDoneTime() < min_time)
                            min_time = itr->getDoneTime();
                    }
                    if (min_time > 0)
                        t = min_time;
                    else {
                        // game over, end simulation
                        break;
                    }
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
            procQueue::iterator proc_itr = execQueue.begin();
            while (proc_itr != execQueue.end()) {
                Process tmp(*proc_itr);
                procQueue::iterator tmp_itr(proc_itr);
                tmp_itr++;
                execQueue.erase(proc_itr);
                tmp.wait(1);
                if (tmp.getWaitTime() >= 3 * tmp.getBurstTime()) {
                    if (tmp.getPriority() > 0)
                        tmp.setPriority(tmp.getPriority() - 1);
                }
                execQueue.insert(tmp);
                proc_itr = tmp_itr;
            }
        }
    }

    turnaroundTime = burstTime + waitTime;

    burstTime /= numBursts;
    waitTime /= numBursts;
    turnaroundTime /= numBursts;
    outfile << "-- average CPU burst time: " << std::setprecision(5) << burstTime << std::endl;;
    outfile << "-- average wait time: " << std::fixed << std::setprecision(2) << waitTime << std::endl;
    outfile << "-- average turnaround time: " << std::setprecision(2) << turnaroundTime << std::endl;
    outfile << "-- total number of context switches: " << contextSwitches << std::endl;

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
