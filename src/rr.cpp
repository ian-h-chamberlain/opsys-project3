/**
 * Operating Systems Project 3
 * Ian Chamberlain & Brian Kovacik
 *
 * Reads an input file of processes and simulates them
 */
#include <list>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <fstream>
#include <iomanip>

#include "process.h"
#include "memory.h"

// use -D DEBUG_MODE when compiling to generate verbose output for debugging purposes

typedef std::list<Process> procQueue;

void printQueue1(const procQueue &queueToPrint);
void printQueue1(const std::list<Process> &queueToPrint);

int defragment(std::map<int, MemoryPartition>& partitions, int *t);
void deallocate(std::map<int, MemoryPartition>& partitions, char id, int t);
void printMemory(const std::map<int, MemoryPartition>& partitions, int t);
int allocateMemoryFirstFit(std::map<int, MemoryPartition>& partitions, char proc, int size, int t, int offset);
int allocateMemoryBestFit(std::map<int, MemoryPartition>& partitions, char proc, int size, int t);

int simulateRR(const std::list<Process> &processes, std::ofstream &outfile, int t_cs, int t_sl, int mem_type) {

    procQueue execQueue;    // the execution queue

    std::list<Process> ioQueue; // a container for the processes in I/O
    std::map<int, MemoryPartition> memoryBank;

    MemoryPartition tmp('.', 256);
    memoryBank[0] = tmp;

    int t = 0;
    int curProcTime = 0;

    double burstTime = 0.0;
    double waitTime = 0.0;
    double turnaroundTime = 0.0;
    double defragTime = 0.0;
    int numBursts = 0;
    int contextSwitches = 0;

    std::cout << "time 0ms: Simulator started for RR and ";
    if (mem_type == 0) {
        std::cout << "First-Fit" << std::endl;
        outfile << "RR and First-Fit" << std::endl;
    }
    else if (mem_type == 1) {
        std::cout << "Next-Fit" << std::endl;
        outfile << "RR and Next-Fit" << std::endl;
    }
    else {
        std::cout << "Best-Fit" << std::endl;
        outfile << "RR and Best-Fit" << std::endl;
    }

    int last_alloc = 0;

    procQueue::iterator proc_itr;
    Process curProc;

    // now run the processes
    while (t == 0 || execQueue.size() > 0 || ioQueue.size() > 0 || curProcTime > 0) {

        std::list<Process>::const_iterator proc_itr1 = processes.begin();
        while (proc_itr1 != processes.end()) {
            if (proc_itr1->getArriveTime() == t) {

                int tmp;
                if (mem_type == 0) {
                    tmp = allocateMemoryFirstFit(memoryBank, proc_itr1->getNum(), proc_itr1->getPriority(), t, 0);
                }
                else if (mem_type == 1) {
                    tmp = allocateMemoryFirstFit(memoryBank, proc_itr1->getNum(), proc_itr1->getPriority(), t, last_alloc);
                }
                else if (mem_type == 2) {
                    tmp = allocateMemoryBestFit(memoryBank, proc_itr1->getNum(), proc_itr1->getPriority(), t);
                }
                else {
                    // error
                    std::cerr << "ERROR: invalid memory type" << std::endl;
                    return -1;
                }

                if (tmp < 0) {
                    std::cout << "time " << t << "ms: Process '" << proc_itr1->getNum()
                        << "' unabled to be added; lack of memory" << std::endl;
                    int old_t = t;
                    last_alloc = defragment(memoryBank, &t);

                    if (t - old_t <= 0) {
                        // no room for process, skip it
                        std::cout << "time " << t << "ms: Process '" << proc_itr1->getNum()
                            << "' unabled to be added; lack of memory" << std::endl;
                        proc_itr1++;
                    }

                    // account for defrag time in statistics
                    waitTime += (t - old_t);
                    defragTime += (t - old_t);
                    continue;
                }
                else {
                    if (mem_type == 1)
                        last_alloc = tmp;

                    std::cout << "time " << t << "ms: Process '" << proc_itr1->getNum()
                        << "' added to system ";

                    execQueue.push_back(*proc_itr1);
                    printQueue1(execQueue);

                    numBursts += proc_itr1->getNumBursts();

                    printMemory(memoryBank, t);
                }
            }
            proc_itr1++;
        } 

        if (t == 0) {
            proc_itr = execQueue.begin();
            curProc = *proc_itr;
            execQueue.erase(execQueue.begin());
        }

        std::list<Process>::iterator itr = ioQueue.begin();
        // check for completed IO processes
        while (itr != ioQueue.end()) {
            if (t == itr->getDoneTime()) {

                // temporarily add the process for the sake of output
                procQueue::iterator tmp = execQueue.end();
		execQueue.push_back(*itr);
		tmp--;

                std::cout << "time " << t << "ms: Process '" << itr->getNum()
                    << "' completed I/O ";

                // actually change the process to reflect that it completed IO
                itr->runIO();
#ifdef DEBUG_MODE
                std::cout << std::endl << "  remain " << itr->getRemainingTime() << std::endl;
                std::cout << "curproc " << curProc.getNum() << std::endl;
                std::cout << "  remain " << curProc.getRemainingTime() << std::endl;
#endif

                // preempt the process if the remaining time is too long
                if (curProc.getRemainingTime() > itr->getBurstTime()) {
                    procQueue::iterator pos = execQueue.erase(tmp);   // we don't need the process in the queue
                    printQueue1(execQueue);
                    execQueue.insert(pos, curProc);
                    std::cout << "time " << t << "ms: Process '" << curProc.getNum() 
                        << "' preempted by Process '" << itr->getNum() << "' ";
                    curProc = *itr;
                    curProcTime = t + t_cs;
                    // calculate wait time
                    proc_itr = execQueue.begin();
                    while (proc_itr != execQueue.end()) {
                        Process tmp(*proc_itr);
                        procQueue::iterator tmp_itr(proc_itr);
                        tmp_itr++;
                        procQueue::iterator pos = execQueue.erase(proc_itr);
                        tmp.wait(t_cs);
                        execQueue.insert(pos, tmp);
                        proc_itr = tmp_itr;
                    }

                    contextSwitches++;
                    printQueue1(execQueue);
                }
                // or, if curProc is done (which only happens when the exec queue is empty)
                // replace it with something from IO
                else if (curProc.getRemainingTime() <= 0) {
                    printQueue1(execQueue);
                    execQueue.erase(tmp);   // not necessary to keep this in the queue
                    curProc = *itr;
                }
                // otherwise, just add the IO process to the exec queue
                else {
                    printQueue1(execQueue);
                    execQueue.insert(execQueue.erase(tmp), *itr); //same as above
                }

                itr = ioQueue.erase(itr);

            }
            else {
                itr++;
            }
        }

        // prepare for the next process to start after a context switch
        if (curProcTime <= 0) {
            curProcTime = t + t_cs;
            // calculate wait time
            proc_itr = execQueue.begin();
            while (proc_itr != execQueue.end()) {
                Process tmp(*proc_itr);
                procQueue::iterator tmp_itr(proc_itr);
                tmp_itr++;
                procQueue::iterator pos = execQueue.erase(proc_itr);
                tmp.wait(t_cs);
                execQueue.insert(pos, tmp);
                proc_itr = tmp_itr;
            }
            contextSwitches++;
        }
        // or start a process if necessary
        else if (t == curProcTime || t == curProc.getArriveTime()) {
            std::cout << "time " << t << "ms: Process '" << curProc.getNum()
                << "' started using the CPU ";
            printQueue1(execQueue);
        }
        // and finish a process if it's done
        if (curProc.getRemainingTime() <= 0) {

            if (!curProc.isComplete()) {
                curProc.runBurst(t);
                burstTime += curProc.getBurstTime();

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
                std::cout << "time " << t << "ms: Process '" << curProc.getNum()
                    << "' terminated ";
                printQueue1(execQueue);

                deallocate(memoryBank, curProc.getNum(), t);
                waitTime += curProc.getWaitTime();
            }
            // or start I/O if not terminated
            else {
                std::cout << "time " << t << "ms: Process '" << curProc.getNum()
                    << "' completed its CPU burst ";
                printQueue1(execQueue);

                std::cout << "time " << t << "ms: Process '" << curProc.getNum()
                    << "' performing I/O ";
                printQueue1(execQueue);

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
            std::cout << "P" << curProc.getNum() << std::endl;
            std::cout << "  remain: " << curProc.getRemainingTime() << std::endl;
            std::cout << "  t = " << t << std::endl;
#endif
            continue;
        }
        t++;
        if (t > curProcTime && !curProc.isComplete()) {
            curProc.runForMs(1);
            procQueue::iterator proc_itr = execQueue.begin();
            while (proc_itr != execQueue.end()) {
                Process tmp(*proc_itr);
                procQueue::iterator tmp_itr(proc_itr);
                tmp_itr++;
                procQueue::iterator pos = execQueue.erase(proc_itr);
                tmp.wait(1);
                execQueue.insert(pos, tmp);
                proc_itr = tmp_itr;
            }
        }
/*** New stuff, don't grade
std::cerr << curProc.getNum() << std::endl;
	if (t-curProcTime == t_sl && t && execQueue.size()) {
		std::cout << "time " << t << "ms: Process '" << curProc.getNum() << "' preempted due to time slice expiration ";
		procQueue::iterator pos = execQueue.erase(execQueue.begin());
		execQueue.push_back(*pos);
		printQueue1(execQueue);	
	}
*/

    }

    turnaroundTime = burstTime + waitTime;

    burstTime /= numBursts;
    waitTime /= numBursts;
    turnaroundTime /= numBursts;

    outfile << "-- average CPU burst time: ";
    outfile << std::setprecision(5) << burstTime << " ms" << std::endl;;
    outfile << "-- average wait time: ";
    outfile << std::fixed << std::setprecision(2) << waitTime << " ms" << std::endl;
    outfile << "-- average turnaround time: ";
    outfile << std::setprecision(2) << turnaroundTime << " ms" << std::endl;
    outfile << "-- total number of context switches: " << contextSwitches << std::endl;
    outfile << "-- total defragmentation time: ";
    outfile << std::setprecision(0) << defragTime << " ms" << std::endl;
    outfile << "-- percentage defragmentation time: ";
    outfile << std::setprecision(0) << (100 * defragTime) / t << std::endl;

    return t;

}

/**
 * printQueue() - print a priority queue
 */

void printQueue1(const procQueue &queueToPrint) {
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
