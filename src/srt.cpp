#include <list>
#include <iostream>
#include <algorithm>

#include "process.h"

// use -D DEBUG_MODE when compiling to generate verbose output for debugging purposes

void printQueue(const std::list<Process> &queueToPrint);

int simulateSRT(const std::list<Process> &processes, int t_cs) {

    auto comparator = [](Process p1, Process p2) { return p1.getBurstTime() > p2.getBurstTime(); };

    std::priority_queue<Process, std::vector<Process>, decltype(comparator) >
        execQueue(processes);    // the execution queue
    std::list<Process> ioQueue; // a container for the processes in I/O

    // get the first process
    Process curProc = execQueue.front();
    execQueue.pop_front();

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
                curProc = execQueue.front();
                execQueue.pop_front();
            }
            t++;
            continue;
        }
    }

    return t;

}
