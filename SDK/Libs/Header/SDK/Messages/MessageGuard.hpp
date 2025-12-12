#pragma once

#include "SDK/Interfaces/IAppComm.hpp"
#include "SDK/Messages/MessageBase.hpp"

namespace SDK
{

template<typename T>
class MessageGuard
{
public:
    MessageGuard(SDK::Interface::IAppComm& comm, T* ptr) noexcept
        : mComm(comm)
        , mPtr(ptr)
    {}

    MessageGuard(const MessageGuard&)            = delete;
    MessageGuard& operator=(const MessageGuard&) = delete;

    MessageGuard(MessageGuard&&)            = delete;
    MessageGuard& operator=(MessageGuard&&) = delete;

    ~MessageGuard() noexcept
    {
        cleanup();
    }

    T*       get()           const noexcept { return mPtr;  }
    T*       operator->()    const noexcept { return mPtr;  }
    T&       operator*()     const noexcept { return *mPtr; }
    explicit operator bool() const noexcept { return mPtr != nullptr; }

    T* release() noexcept
    {
        T* tmp = mPtr;
        mPtr   = nullptr;
        return tmp;
    }

    bool send(uint32_t timeout = 0)
    {
        if (!mPtr) {
            return false;
        }

        return mComm.sendMessage(mPtr, timeout);
    }

    bool ok() const noexcept
    {
        if (!mPtr) {
            return false;
        }

        return mPtr->getResult() == SDK::MessageResult::SUCCESS;
    }

private:
    void cleanup() noexcept
    {
        if (mPtr) {
            mComm.releaseMessage(mPtr);
        }
        mPtr = nullptr;
    }

    SDK::Interface::IAppComm& mComm;
    T*                        mPtr;
};

template<typename T>
MessageGuard<T> make_msg(SDK::Interface::IAppComm& comm)
{
    T* raw = comm.template allocateMessage<T>();
    return MessageGuard<T>(comm, raw);
}

} // namespace SDK
