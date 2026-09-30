#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
using namespace std;

struct Process {
    int pid, arrival, burst, remaining;
    int start = -1, finish = -1, wait = 0, tat = 0;
    bool complete = false;
    int priority = 0;
    int age = 0;
};

struct MemoryPartition {
    int id, size;
    bool occupied = false;
    int pid = -1;
};

vector<Process> processes;
vector<MemoryPartition> memory;
map<int, vector<int>> messageQueue;
int quantum = 3;

// ---------------- Messaging ----------------
void sendMessage(int from, int to, int msg) {
    messageQueue[to].push_back(msg);
    cout << "Message from P" << from << " to P" << to << ": " << msg << endl;
}

void receiveMessages(int pid) {
    cout << "Process " << pid << " received messages: ";
    for (int m : messageQueue[pid])
        cout << m << " ";
    cout << endl;
    messageQueue[pid].clear();
}

// ---------------- Aging Mechanism ----------------
void applyAging() {
    for (auto& p : processes) {
        if (!p.complete) p.age++;
        if (p.age > 5) p.priority = min(2, p.priority + 1);
    }
}

// ---------------- Banker's Algorithm ----------------
bool bankersAlgorithm(vector<int> available, vector<vector<int>> maxNeed, vector<vector<int>> allocation) {
    int n = allocation.size(), m = available.size();
    vector<vector<int>> need(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = maxNeed[i][j] - allocation[i][j];

    vector<bool> finish(n, false);
    vector<int> work = available;
    while (true) {
        bool progress = false;
        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool canRun = true;
                for (int j = 0; j < m; j++)
                    if (need[i][j] > work[j]) canRun = false;
                if (canRun) {
                    for (int j = 0; j < m; j++) work[j] += allocation[i][j];
                    finish[i] = true;
                    progress = true;
                }
            }
        }
        if (!progress) break;
    }
    return all_of(finish.begin(), finish.end(), [](bool f){ return f; });
}

// ---------------- Memory Allocation ----------------
void allocateMemoryBestFit(vector<Process>& processes) {
    cout << "\n--- Memory Allocation (Best Fit) ---\n";
    for (auto& p : processes) {
        auto bestFit = memory.end();
        for (auto it = memory.begin(); it != memory.end(); ++it)
            if (!it->occupied && it->size >= p.burst && (bestFit == memory.end() || it->size < bestFit->size))
                bestFit = it;

        if (bestFit != memory.end()) {
            bestFit->occupied = true;
            bestFit->pid = p.pid;
            cout << "Process " << p.pid << " allocated to Partition " << bestFit->id << " (" << bestFit->size << " KB)\n";
        } else {
            cout << "??  Process " << p.pid << " could not be allocated due to insufficient space.\n";
        }
    }
}

// ---------------- Performance Analysis ----------------
void analyzePerformance() {
    if (processes.empty()) {
        cout << "No processes were provided; performance metrics are unavailable.\n";
        return;
    }
    int totalWT = 0, totalTAT = 0;
    for (const auto& p : processes) {
        totalWT += p.wait;
        totalTAT += p.tat;
    }
    cout << "\n--- System Performance Metrics ---\n";
    cout << "Average Wait Time: " << totalWT / (double)processes.size() << " ms\n";
    cout << "Average Turnaround Time: " << totalTAT / (double)processes.size() << " ms\n";
}

// ---------------- Hybrid Scheduler ----------------
bool allDone() {
    for (auto& p : processes)
        if (!p.complete) return false;
    return true;
}

void hybridScheduler() {
    int time = 0;
    queue<int> rrQ;
    vector<bool> inQueue(processes.size(), false);

    while (!allDone()) {
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].arrival <= time && !processes[i].complete && !inQueue[i]) {
                rrQ.push(i);
                inQueue[i] = true;
            }
        }

        float avgWait = 0;
        int count = 0;
        for (auto& p : processes) {
            if (p.complete) {
                avgWait += p.wait;
                count++;
            }
        }
        if (count > 0) avgWait /= count;

        bool useSRTF = avgWait > 4;

        applyAging();

        if (useSRTF) {
            int shortest = -1;
            for (size_t i = 0; i < processes.size(); i++) {
                if (!processes[i].complete && processes[i].arrival <= time) {
                    if (shortest == -1 || processes[i].remaining < processes[static_cast<size_t>(shortest)].remaining)
                        shortest = static_cast<int>(i);
                }
            }
            if (shortest != -1) {
                if (processes[shortest].start == -1)
                    processes[shortest].start = time;
                processes[shortest].remaining--;
                time++;
                if (processes[shortest].remaining == 0) {
                    processes[shortest].complete = true;
                    processes[shortest].finish = time;
                    processes[shortest].tat = time - processes[shortest].arrival;
                    processes[shortest].wait = processes[shortest].tat - processes[shortest].burst;
                    receiveMessages(processes[shortest].pid);
                }
            } else time++;
        } else {
            if (!rrQ.empty()) {
                int i = rrQ.front(); rrQ.pop();
                inQueue[i] = false;

                if (processes[i].start == -1)
                    processes[i].start = time;

                int exec = min(quantum, processes[i].remaining);
                processes[i].remaining -= exec;
                time += exec;

                if (processes[i].remaining == 0) {
                    processes[i].complete = true;
                    processes[i].finish = time;
                    processes[i].tat = time - processes[i].arrival;
                    processes[i].wait = processes[i].tat - processes[i].burst;
                    receiveMessages(processes[i].pid);
                } else {
                    rrQ.push(i);
                    inQueue[i] = true;
                }
            } else time++;
        }
    }
}

// ---------------- Main ----------------
int main() {
    int n;
    cout << "Enter number of processes: ";
    if (!(cin >> n) || n <= 0) {
        cerr << "Please enter a positive number of processes.\n";
        return 1;
    }

    for (int i = 0; i < n; i++) {
        int at, bt;
        cout << "Enter Arrival Time and Burst Time for Process " << i + 1 << ": ";
        if (!(cin >> at >> bt) || at < 0 || bt <= 0) {
            cerr << "Arrival time must be non-negative and burst time must be positive.\n";
            return 1;
        }
        processes.push_back({i + 1, at, bt, bt});
    }

    memory = {{1, 100}, {2, 200}, {3, 300}};
    allocateMemoryBestFit(processes);

    vector<vector<int>> alloc = {{0, 1}, {1, 0}, {1, 0}};
    vector<vector<int>> req = {{1, 0}, {0, 1}, {0, 1}};
    vector<int> avail = {1, 1};

    if (!bankersAlgorithm(avail, req, alloc)) {
        cout << "\n??  Unsafe State Detected: Deadlock Risk Present!\n";
    } else {
        cout << "\n? Safe State: No Deadlock Detected.\n";
    }

    if (n >= 3) {
        sendMessage(1, 2, 42);
        sendMessage(2, 3, 99);
    }

    cout << "\nRunning Hybrid Scheduler...\n";
    hybridScheduler();

    cout << "\nPID\tAT\tBT\tWT\tTAT\n";
    for (auto& p : processes)
        cout << p.pid << "\t" << p.arrival << "\t" << p.burst << "\t" << p.wait << "\t" << p.tat << endl;

    analyzePerformance();
    return 0;
}

