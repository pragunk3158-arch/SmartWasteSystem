#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include <string>

// ============================================================================
// DATA STRUCTURE 1: GENERIC SINGLY LINKED LIST
// ============================================================================
// Demonstrates:
// - Dynamic node memory allocation
// - Pointer manipulation (head, next)
// - Insertion, Traversal, Search, and Deletion
// ============================================================================
template <typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~LinkedList() {
        clear();
    }

    // Copy constructor
    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node* curr = other.head;
        while (curr) {
            insertTail(curr->data);
            curr = curr->next;
        }
    }

    // Copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* curr = other.head;
            while (curr) {
                insertTail(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    void insertHead(const T& val) {
        Node* newNode = new Node(val);
        newNode->next = head;
        head = newNode;
        if (!tail) tail = head;
        count++;
    }

    void insertTail(const T& val) {
        Node* newNode = new Node(val);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    bool remove(std::function<bool(const T&)> predicate) {
        if (!head) return false;

        if (predicate(head->data)) {
            Node* temp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete temp;
            count--;
            return true;
        }

        Node* curr = head;
        while (curr->next && !predicate(curr->next->data)) {
            curr = curr->next;
        }

        if (curr->next) {
            Node* temp = curr->next;
            curr->next = curr->next->next;
            if (temp == tail) tail = curr;
            delete temp;
            count--;
            return true;
        }
        return false;
    }

    T* find(std::function<bool(const T&)> predicate) {
        Node* curr = head;
        while (curr) {
            if (predicate(curr->data)) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const T* find(std::function<bool(const T&)> predicate) const {
        Node* curr = head;
        while (curr) {
            if (predicate(curr->data)) {
                return &(curr->data);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    void forEach(std::function<void(T&)> action) {
        Node* curr = head;
        while (curr) {
            action(curr->data);
            curr = curr->next;
        }
    }

    void forEach(std::function<void(const T&)> action) const {
        Node* curr = head;
        while (curr) {
            action(curr->data);
            curr = curr->next;
        }
    }

    std::vector<T> toVector() const {
        std::vector<T> vec;
        vec.reserve(count);
        Node* curr = head;
        while (curr) {
            vec.push_back(curr->data);
            curr = curr->next;
        }
        return vec;
    }

    void clear() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
    Node* getHead() const { return head; }
};

// ============================================================================
// DATA STRUCTURE 2: MAX-HEAP PRIORITY QUEUE
// ============================================================================
// Demonstrates:
// - Complete Binary Tree array representation
// - Heap invariants: Parent >= Children
// - Heapify Up (O(log N)) on push
// - Heapify Down (O(log N)) on pop
// - Used to prioritize CRITICAL and HIGH bins before routine bins
// ============================================================================
template <typename T, typename Compare = std::less<T>>
class MaxHeapPriorityQueue {
private:
    std::vector<T> heap;
    Compare comp;

    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

    void heapifyUp(int i) {
        while (i > 0 && comp(heap[parent(i)], heap[i])) {
            std::swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void heapifyDown(int i) {
        int maxIndex = i;
        int l = leftChild(i);
        int r = rightChild(i);
        int n = static_cast<int>(heap.size());

        if (l < n && comp(heap[maxIndex], heap[l])) {
            maxIndex = l;
        }
        if (r < n && comp(heap[maxIndex], heap[r])) {
            maxIndex = r;
        }

        if (i != maxIndex) {
            std::swap(heap[i], heap[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

public:
    MaxHeapPriorityQueue() {}

    void push(const T& val) {
        heap.push_back(val);
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    T pop() {
        if (heap.empty()) {
            throw std::runtime_error("Priority Queue underflow");
        }
        T topVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return topVal;
    }

    const T& top() const {
        if (heap.empty()) {
            throw std::runtime_error("Priority Queue is empty");
        }
        return heap[0];
    }

    bool isEmpty() const { return heap.empty(); }
    int size() const { return static_cast<int>(heap.size()); }
    void clear() { heap.clear(); }
};

// ============================================================================
// DATA STRUCTURE 3: FIFO QUEUE
// ============================================================================
// Demonstrates:
// - First-In-First-Out (FIFO) queue for regular dispatch orders
// - Linked-node based queue
// ============================================================================
template <typename T>
class Queue {
private:
    struct QNode {
        T data;
        QNode* next;
        QNode(const T& val) : data(val), next(nullptr) {}
    };

    QNode* frontNode;
    QNode* rearNode;
    int count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~Queue() {
        clear();
    }

    void enqueue(const T& val) {
        QNode* newNode = new QNode(val);
        if (!rearNode) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("Queue underflow");
        }
        QNode* temp = frontNode;
        T val = temp->data;
        frontNode = frontNode->next;
        if (!frontNode) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
        return val;
    }

    const T& front() const {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return frontNode->data;
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }

    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }
};

// ============================================================================
// ALGORITHMS: SEARCHING & SORTING
// ============================================================================

namespace Algorithms {

    // QuickSort implementation on std::vector
    template <typename T, typename Compare>
    int partition(std::vector<T>& arr, int low, int high, Compare comp) {
        T pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (comp(arr[j], pivot)) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    template <typename T, typename Compare>
    void quickSortHelper(std::vector<T>& arr, int low, int high, Compare comp) {
        if (low < high) {
            int pi = partition(arr, low, high, comp);
            quickSortHelper(arr, low, pi - 1, comp);
            quickSortHelper(arr, pi + 1, high, comp);
        }
    }

    template <typename T, typename Compare>
    void quickSort(std::vector<T>& arr, Compare comp) {
        if (!arr.empty()) {
            quickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1, comp);
        }
    }

    // Linear search
    template <typename T, typename Predicate>
    int linearSearch(const std::vector<T>& arr, Predicate pred) {
        for (size_t i = 0; i < arr.size(); ++i) {
            if (pred(arr[i])) return static_cast<int>(i);
        }
        return -1;
    }

    // Binary search (array must be sorted according to key extractor)
    template <typename T, typename KeyType, typename KeyExtractor>
    int binarySearch(const std::vector<T>& arr, const KeyType& targetKey, KeyExtractor getKey) {
        int left = 0;
        int right = static_cast<int>(arr.size()) - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            KeyType midKey = getKey(arr[mid]);

            if (midKey == targetKey) return mid;
            if (midKey < targetKey) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
}

#endif // DATA_STRUCTURES_H
