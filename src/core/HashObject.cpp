//
// Created by wuxianggujun on 25-9-4.
//

#include "HashObject.hpp"

namespace egret
{
    std::atomic<size_t> HashObject::s_hashCount{1};
    
    HashObject::HashObject()
    {
        m_hashCode = s_hashCount.fetch_add(1);
    }
}
