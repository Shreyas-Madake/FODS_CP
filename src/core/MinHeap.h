#pragma once

/**
 * @file MinHeap.h
 * @brief Custom Binary Min-Heap template implementation for Dijkstra's Algorithm and priority queue needs.
 *
 * @section design Design Decisions & Why Hand-Implemented
 * For a Data Structures course capstone (FODS), wrapping std::priority_queue is unacceptable.
 * This custom binary min-heap is implemented completely from scratch using std::vector for internal
 * dynamic storage.
 *
 * Why Binary Heap over Fibonacci Heap?
 * While Fibonacci heaps offer O(1) amortized decreaseKey, they have high constant factor overhead,
 * complex pointer-heavy node structures, and poor cache locality. For real-world graph sizes (up to
 * thousands of nodes), a binary min-heap backed by a contiguous std::vector provides exceptional
 * cache locality, simple implementation, and O(log n) performance for all operations, making it the
 * standard choice in production graph routers.
 *
 * @section complexity Time & Space Complexity:
 * - push(val)       : O(log n) time
 * - pop()           : O(log n) time
 * - top()           : O(1) time
 * - size(), empty() : O(1) time
 * - decreaseKey()   : O(log n) time
 * - Space Complexity: O(n) storage in underlying vector.
 */

#include <vector>
#include <functional>
#include <stdexcept>

/**
 * @brief Helper struct for Dijkstra priority queue entries.
 */
struct HeapNode {
    int nodeId;
    long long dist;

    bool operator>(const HeapNode& other) const {
        return dist > other.dist;
    }

    bool operator<(const HeapNode& other) const {
        return dist < other.dist;
    }
};

template <typename T, typename Compare = std::less<T>>
class MinHeap {
public:
    MinHeap() = default;
    explicit MinHeap(const Compare& comp) : m_comp(comp) {}

    /**
     * @brief Check if the heap is empty.
     * @complexity O(1)
     */
    bool empty() const {
        return m_data.empty();
    }

    /**
     * @brief Get the number of elements in the heap.
     * @complexity O(1)
     */
    size_t size() const {
        return m_data.size();
    }

    /**
     * @brief Access the minimum element in the heap.
     * @complexity O(1)
     */
    const T& top() const {
        if (m_data.empty()) {
            throw std::out_of_range("MinHeap::top() called on empty heap.");
        }
        return m_data[0];
    }

    /**
     * @brief Insert a new element into the heap and maintain min-heap property.
     * @complexity O(log n)
     */
    void push(const T& item) {
        m_data.push_back(item);
        siftUp(static_cast<int>(m_data.size()) - 1);
    }

    /**
     * @brief Remove the minimum element from the heap.
     * @complexity O(log n)
     */
    void pop() {
        if (m_data.empty()) {
            throw std::out_of_range("MinHeap::pop() called on empty heap.");
        }
        m_data[0] = m_data.back();
        m_data.pop_back();
        if (!m_data.empty()) {
            siftDown(0);
        }
    }

    /**
     * @brief Decrease the value of an element at a given index and restore heap property.
     * @complexity O(log n)
     */
    void decreaseKey(int index, const T& newVal) {
        if (index < 0 || index >= static_cast<int>(m_data.size())) {
            return;
        }
        m_data[index] = newVal;
        siftUp(index);
    }

    /**
     * @brief Clear all elements from the heap.
     * @complexity O(n)
     */
    void clear() {
        m_data.clear();
    }

    /**
     * @brief Get direct access to underlying storage (for advanced lookups).
     */
    const std::vector<T>& getData() const {
        return m_data;
    }

private:
    std::vector<T> m_data;
    Compare m_comp;

    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return (2 * i) + 1; }
    int rightChild(int i) const { return (2 * i) + 2; }

    void siftUp(int i) {
        while (i > 0 && m_comp(m_data[i], m_data[parent(i)])) {
            std::swap(m_data[i], m_data[parent(i)]);
            i = parent(i);
        }
    }

    void siftDown(int i) {
        int minIndex = i;
        int l = leftChild(i);
        int size = static_cast<int>(m_data.size());

        if (l < size && m_comp(m_data[l], m_data[minIndex])) {
            minIndex = l;
        }

        int r = rightChild(i);
        if (r < size && m_comp(m_data[r], m_data[minIndex])) {
            minIndex = r;
        }

        if (minIndex != i) {
            std::swap(m_data[i], m_data[minIndex]);
            siftDown(minIndex);
        }
    }
};
