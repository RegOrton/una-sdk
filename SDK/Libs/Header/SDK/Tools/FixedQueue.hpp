
#pragma once

#include <cstddef>
#include <array>
#include <optional>

namespace SDK::Tools
{

template<typename T, size_t Capacity>
class FixedQueue {
public:
    bool push(const T& value) {
        if (full()) return false;
        data_[mTail] = value;
        mTail = (mTail + 1) % Capacity;
        ++mSize;
        return true;
    }

    std::optional<T> pop() {
        if (empty()) return std::nullopt;
        T value = data_[mHead];
        mHead = (mHead + 1) % Capacity;
        --mSize;
        return value;
    }

    bool empty() const { return mSize == 0; }
    bool full()  const { return mSize == Capacity; }
    size_t size() const { return mSize; }

private:
    std::array<T, Capacity> data_{};
    size_t mHead = 0;
    size_t mTail = 0;
    size_t mSize = 0;
};

} // namespace SDK::Tools
