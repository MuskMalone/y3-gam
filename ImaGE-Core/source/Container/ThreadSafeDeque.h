#pragma once
#include <deque>
#include <mutex>
#include <algorithm>

template<typename T>
class ThreadSafeDeque {
public:
    // Write 'count' elements from the input pointer 'data' into the deque.
    void write(const T* data, size_t count) {
        std::lock_guard<std::mutex> lock(mMutex);
        for (size_t i = 0; i < count; ++i) {
            mBuffer.push_back(data[i]);
        }
    }

    // Read up to 'count' elements from the deque into the output pointer 'out'.
    // Returns the number of elements actually read.
    size_t read(T* out, size_t count) {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t available = mBuffer.size();
        size_t toRead = std::min(count, available);
        for (size_t i = 0; i < toRead; ++i) {
            out[i] = mBuffer.front();
            mBuffer.pop_front();
        }
        return toRead;
    }

private:
    std::deque<T> mBuffer;
    std::mutex mMutex;
};
