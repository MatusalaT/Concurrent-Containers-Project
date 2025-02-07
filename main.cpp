#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <random>
#include <cstring>

// Simplified random number generator
std::vector<int> generateRandomNumbers(int count) {
    std::vector<int> numbers;
    static thread_local std::mt19937 generator{std::random_device{}()};
    std::uniform_int_distribution<int> distribution(1, 100);

    for (int i = 0; i < count; ++i) {
        numbers.push_back(distribution(generator));
    }
    return numbers;
}

// SGL Queue
template <typename T>
class SGLQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;

public:
    void enqueue(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
    }

    bool dequeue(T& result) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        result = queue.front();
        queue.pop();
        return true;
    }
};

// MSQueue
template <typename T>
class MSQueue {
private:
    struct Node {
        T value;
        std::atomic<Node*> next;
        Node(T val) : value(val), next(nullptr) {}
    };

    std::atomic<Node*> head, tail;

public:
    MSQueue() {
        Node* dummy = new Node(T());
        head.store(dummy);
        tail.store(dummy);
    }

    void enqueue(T value) {
        Node* newNode = new Node(value);
        Node* curTail;
        Node* nullNode = nullptr;

        while (true) {
            curTail = tail.load(std::memory_order_relaxed);
            Node* nextNode = curTail->next.load(std::memory_order_relaxed);

            if (curTail == tail.load(std::memory_order_relaxed)) {
                if (nextNode == nullptr) {
                    if (curTail->next.compare_exchange_weak(nullNode, newNode,
                                                            std::memory_order_release, std::memory_order_relaxed)) {
                        tail.compare_exchange_weak(curTail, newNode,
                                                    std::memory_order_release, std::memory_order_relaxed);
                        break;
                    }
                } else {
                    tail.compare_exchange_weak(curTail, nextNode,
                                                std::memory_order_release, std::memory_order_relaxed);
                }
            }
        }
    }

    bool dequeue(T& result) {
        Node* oldHead;
        Node* newHead;

        do {
            oldHead = head.load(std::memory_order_relaxed);
            if (oldHead == tail.load(std::memory_order_relaxed))
                return false;

            newHead = oldHead->next.load(std::memory_order_relaxed);
        } while (!head.compare_exchange_weak(oldHead, newHead,
                                             std::memory_order_release, std::memory_order_relaxed));

        result = newHead->value;
        delete oldHead;
        return true;
    }

    ~MSQueue() {
        while (Node* oldHead = head.load()) {
            head.store(oldHead->next);
            delete oldHead;
        }
    }
};

// FlatCombining Queue
template <typename T>
class FlatCombiningQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;

public:
    void enqueue(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
    }

    bool dequeue(T& result) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        result = queue.front();
        queue.pop();
        return true;
    }
};

// SGL Stack with elimination
template <typename T>
class SGLEliminationStack {
private:
    std::stack<T> stack;
    std::mutex mtx;

public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        stack.push(value);
    }

    bool pop(T& result) {
        std::lock_guard<std::mutex> lock(mtx);
        if (stack.empty()) return false;
        result = stack.top();
        stack.pop();
        return true;
    }
};

// Treiber Stack with elimination
template <typename T>
class TreiberEliminationStack {
private:
    struct Node {
        T value;
        Node* next;
        Node(T val) : value(val), next(nullptr) {}
    };

    std::atomic<Node*> head{nullptr};

public:
    void push(T value) {
        Node* newNode = new Node(value);
        do {
            newNode->next = head.load(std::memory_order_relaxed);
        } while (!head.compare_exchange_weak(newNode->next, newNode,
                                             std::memory_order_release, std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node* oldHead = head.load(std::memory_order_relaxed);
        do {
            if (oldHead == nullptr) return false;
        } while (!head.compare_exchange_weak(oldHead, oldHead->next,
                                             std::memory_order_release, std::memory_order_relaxed));

        result = oldHead->value;
        delete oldHead;
        return true;
    }
};

// FlatCombining Stack with elimination
template <typename T>
class FlatCombiningEliminationStack {
private:
    std::stack<T> stack;
    std::mutex mtx;

public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        stack.push(value);
    }

    bool pop(T& result) {
        std::lock_guard<std::mutex> lock(mtx);
        if (stack.empty()) return false;
        result = stack.top();
        stack.pop();
        return true;
    }
};

// General test function for stacks
template <typename StackType>
void testStack(int numThreads, int numOperations, StackType& stack, const std::string& testName) {
    std::vector<int> inputs = generateRandomNumbers(numThreads * numOperations);
    auto start = std::chrono::high_resolution_clock::now();

    // Parallel push
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < numOperations; ++j) {
                stack.push(inputs[i * numOperations + j]);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Test: " << testName << "\n"
              << "Threads: " << numThreads << ", Operations: " << numOperations
              << ", Time: " << duration << "ms\n";
}

// General test function for queues
template <typename QueueType>
void testQueue(int numThreads, int numOperations, QueueType& queue, const std::string& testName) {
    std::vector<int> inputs = generateRandomNumbers(numThreads * numOperations);
    auto start = std::chrono::high_resolution_clock::now();

    // Parallel enqueue
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < numOperations; ++j) {
                queue.enqueue(inputs[i * numOperations + j]);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Test: " << testName << "\n"
              << "Threads: " << numThreads << ", Operations: " << numOperations
              << ", Time: " << duration << "ms\n";
}

int main(int argc, char* argv[]) {
    // if (argc != 3) {
    //     std::cerr << "Usage: " << argv[0] << " <num_threads> <num_operations>\n";
    //     return 1;
    // }

    if (argc == 2 && std::strcmp(argv[1], "-h") == 0) {
        std::cout << "Usage: ./program <num_threads> <num_operations>\n";
        std::cout << "Description:\n";
        std::cout << "- num_threads: Number of threads to use for the test\n";
        std::cout << "- num_operations: Number of operations per thread\n";
        return 0;
    }

    if (argc != 3) {
        std::cerr << "Usage: ./program <num_threads> <num_operations> or ./program -h for help\n";
        return 1;
    }

    int numThreads = std::stoi(argv[1]);
    int numOperations = std::stoi(argv[2]);

    SGLEliminationStack<int> sglEliminationStack;
    TreiberEliminationStack<int> treiberEliminationStack;
    FlatCombiningEliminationStack<int> flatCombiningEliminationStack;

    SGLQueue<int> sglQueue;
    MSQueue<int> msQueue;
    FlatCombiningQueue<int> flatCombiningQueue;

    // Test stacks
    testStack(numThreads, numOperations, sglEliminationStack, "SGLEliminationStack");
    testStack(numThreads, numOperations, treiberEliminationStack, "TreiberEliminationStack");
    testStack(numThreads, numOperations, flatCombiningEliminationStack, "FlatCombiningEliminationStack");

    // Test queues
    testQueue(numThreads, numOperations, sglQueue, "SGLQueue");
    testQueue(numThreads, numOperations, msQueue, "MSQueue");
    testQueue(numThreads, numOperations, flatCombiningQueue, "FlatCombiningQueue");

    return 0;
}