//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include <atomic>

namespace egret
{
    class IHashObject
    {
    public:
        virtual ~IHashObject() = default;
        virtual size_t getHashCode() const = 0;
    };

    class HashObject : public IHashObject
    {
    private:
        static std::atomic<size_t> s_hashCount;
        size_t m_hashCode;

    public:
        HashObject();
        virtual ~HashObject() = default;

        size_t getHashCode() const override
        {
            return m_hashCode;
        }
    };
}
