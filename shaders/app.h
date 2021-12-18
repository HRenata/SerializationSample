#pragma once

#include "Shaders/common.h"
#include "Shaders/app_common_impl.h"

#include "contract.h"
#include <string>

template<typename ObjT, typename KeyT>
bool loadAndDeserializeObject(ObjT& obj, Env::Key_T<KeyT> key)
{
    uint32_t valueLen = 0, keyLen = sizeof(KeyT);

    // loading serialized buf of object
    Env::VarReader reader(key, key);
    if (reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        auto buf = std::make_unique<uint8_t[]>(valueLen + 1);
        if (!reader.MoveNext(&key, keyLen, buf.get(), valueLen, 1))
        {
            return false;
        }

        // deserialization of object
        obj = Serialization::deserialize<ObjT>(buf.get() + sizeof(Serialization::Buffer),
            valueLen - sizeof(Serialization::Buffer));
    }
    return true;
}

std::string getTextNameToString(const char* key)
{
    char name[0x20];
    auto size = Env::DocGetText(key, name, sizeof(name));
    return std::string(name, size);
}
