﻿#include "Shaders/common.h"
#include "Shaders/app_common_impl.h"
#include "contract.h"
#include "contract_sid.i"

#include <algorithm>

void On_error(const char* msg)
{
    Env::DocGroup root("");
    {
        Env::DocAddText("error", msg);
    }
}

// MANAGER
void On_action_create_contract(const ContractID& cid)
{
    Env::GenerateKernel(nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0, "Create Serialize contract", 0);
}

void On_action_destroy_contract(const ContractID& cid)
{
    Env::GenerateKernel(&cid, 1, nullptr, 0, nullptr, 0, nullptr, 0, "Destroy Serialize contract", 0);
}

void On_action_view_contracts(const ContractID& cid)
{
    EnumAndDumpContracts(s_SID);
}

// PLAYER
void On_action_add_collection(const ContractID& cid)
{
    char collectionName[0x20];
    uint32_t collectionNameSize = 0;
    collectionNameSize = Env::DocGetText("collection_name", collectionName, sizeof(collectionName));
    if (collectionNameSize < 2)
    {
        return On_error("collection_name should be non-empty");
    }

    std::string collectionNameStr(collectionName, collectionNameSize);
    Dogs::Collection collection;
    collection.name = collectionNameStr;

    // creating key
    Env::Key_T<Dogs::Key1> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.key) = 0;

    uint32_t valueLen = 0, keyLen = sizeof(Dogs::Key1);

    // loading serialized buf of collections
    Dogs::Collections collections;

    Env::VarReader reader(key, key);
    if (reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        std::vector<char> v(valueLen, '\0');
        auto buf = reinterpret_cast<Serialization::Buffer*>(v.data());
        if (!reader.MoveNext(&key, keyLen, buf, valueLen, 1))
        {
            return On_error("error of move next");
        }

        // deserialization of collections
        collections = Serialization::deserialize<Dogs::Collections>(v.data() + sizeof(Serialization::Buffer),
            v.size() - sizeof(Serialization::Buffer));

        if (std::find(collections.begin(), collections.end(), collection) != collections.end())
        {
            return On_error("collection_name exists");
        }
    }

    // adding new collection
    collections.push_back(collection);

    // serialization of collections
    auto serializedBuffer = Serialization::serialize(collections);

    // transaction for saving new collections
    Env::GenerateKernel(&cid, Dogs::Actions::AddCollection::s_iMethod, serializedBuffer,
        sizeof(Serialization::Buffer) + serializedBuffer->size, nullptr, 0, nullptr, 0,
        "Added new collection", 0);
}

void On_action_view_collections(const ContractID& cid)
{
    // creating key
    Env::Key_T<Dogs::Key1> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.key) = 0;

    // loading serialized buf of collections
    uint32_t valueLen = 0, keyLen = sizeof(Dogs::Key1);

    Env::VarReader reader(key, key);
    if (!reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        return On_error("error of move next");
    }

    std::vector<char> v(valueLen, '\0');
    auto buf = reinterpret_cast<Serialization::Buffer*>(v.data());
    if (!reader.MoveNext(&key, keyLen, buf, valueLen, 1))
    {
        return On_error("error of move next");
    }

    // deserialization of collections
    Dogs::Collections collections = Serialization::deserialize<Dogs::Collections>(v.data() + sizeof(Serialization::Buffer),
        v.size() - sizeof(Serialization::Buffer));

    // printing all collecitons' name
    Env::DocAddGroup("collections");
    for (auto coll : collections)
    {
        Env::DocAddText("", coll.name.c_str());
    }
}

void On_action_add_attribute_to_collection(const ContractID& cid)
{
    // creating key for collections
    Env::Key_T<Dogs::Key1> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.key) = 0;

    // loading serialized buf of collections
    uint32_t valueLen = 0, keyLen = sizeof(Dogs::Key);

    Env::VarReader reader(key, key);
    if (!reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        return On_error("error of move next");
    }

    std::vector<char> iv(valueLen, '\0');
    auto buf = reinterpret_cast<Serialization::Buffer*>(iv.data());
    if (!reader.MoveNext(&key, keyLen, buf, valueLen, 1))
    {
        return On_error("error of move next");
    }

    // deserialization collections
    Dogs::Collections collections = Serialization::deserialize<Dogs::Collections>(iv.data() + sizeof(Serialization::Buffer),
        iv.size() - sizeof(Serialization::Buffer));

    // loading name of collection, that will get new attribute, from function parameters
    char collectionName[0x20];
    uint32_t collectionNameSize = 0;
    collectionNameSize = Env::DocGetText("collection_name", collectionName, sizeof(collectionName));
    if (collectionNameSize < 2)
    {
        return On_error("collection_name should be non-empty");
    }

    std::string collectionNameStr(collectionName, collectionNameSize);
    Dogs::Collection collection;
    collection.name = collectionNameStr;

    // checking if such collection already exists
    if (std::find(collections.begin(), collections.end(), collection) == collections.end())
    {
        return On_error("collection_name doesn't exist");
    }

    // loading name of attribute for collection  from function parameters
    char attributeName[0x20];
    uint32_t attributeNameSize = 0;
    attributeNameSize = Env::DocGetText("attribute_name", attributeName, sizeof(attributeName));
    if (attributeNameSize < 2)
    {
        return On_error("attribute_name should be non-empty");
    }

    std::string attributeNameStr(attributeName, attributeNameSize);
    Dogs::Attribute attribute;
    attribute.name = attributeNameStr;

    // creating key for attributes
    Utility::Hash256 name_hash = Utility::get_hash(collectionNameStr.c_str(), collectionNameStr.size());
    Dogs::Key key_(name_hash);
    Env::Key_T<Dogs::Key> keyAtt = { .m_KeyInContract = key_ };
    keyAtt.m_Prefix.m_Cid = cid;

    valueLen = 0;
    keyLen = sizeof(Dogs::Key);

    Dogs::Attributes attributes;

    // loading vector of attributes for collection
    Env::VarReader readerAtt(keyAtt, keyAtt);
    // если по ключу не достается ничего, то в пустой созданный attributes
    if (readerAtt.MoveNext(&keyAtt, keyLen, nullptr, valueLen, 0))
    {
        std::vector<char> iv(valueLen, '\0');
        auto buf = reinterpret_cast<Serialization::Buffer*>(iv.data());
        if (!readerAtt.MoveNext(&keyAtt, keyLen, buf, valueLen, 1))
        {
            return On_error("error of move next");
        }

        // deserialization attributes of collection
        attributes = Serialization::deserialize<Dogs::Attributes>(iv.data() + sizeof(Serialization::Buffer),
            iv.size() - sizeof(Serialization::Buffer));

        // checking if such attribute already exists
        if (std::find(attributes.attributes.begin(), attributes.attributes.end(), attribute) != attributes.attributes.end())
        {
            return On_error("attributes_name exists");
        }
    }

    attributes.attributes.push_back(attribute);

    // serialization of attributes
    auto serializedBuffer = Serialization::serialize(attributes);

    // transaction for saving new attributes
    Env::GenerateKernel(&cid, Dogs::Actions::AddAttribute::s_iMethod, serializedBuffer,
        sizeof(Serialization::Buffer) + serializedBuffer->size, nullptr, 0, nullptr, 0,
        "Added new attribute", 0);
}

void On_action_view_attributes_by_collection(const ContractID& cid)
{
    // loading name of collection, from wich will be loaded attributes, from function parameters
    char collectionName[0x20];
    uint32_t collectionNameSize = 0;
    collectionNameSize = Env::DocGetText("collection_name", collectionName, sizeof(collectionName));
    if (collectionNameSize < 2)
    {
        return On_error("collection_name should be non-empty");
    }

    // creating key for loading attributes
    std::string collectionNameStr(collectionName, collectionNameSize);
    Utility::Hash256 name_hash = Utility::get_hash(collectionNameStr.c_str(), collectionNameStr.size());
    Dogs::Key key_(name_hash);
    Env::Key_T<Dogs::Key> keyAtt = { .m_KeyInContract = key_ };
    keyAtt.m_Prefix.m_Cid = cid;

    // loading vector of attributes for collection
    uint32_t valueLen = 0, keyLen = sizeof(Dogs::Key);

    Env::VarReader readerAtt(keyAtt, keyAtt);
    if (!readerAtt.MoveNext(&keyAtt, keyLen, nullptr, valueLen, 0))
    {
        return On_error("error of move next");
    }

    std::vector<char> v(valueLen, '\0');
    auto buf = reinterpret_cast<Serialization::Buffer*>(v.data());
    if (!readerAtt.MoveNext(&keyAtt, keyLen, buf, valueLen, 1))
    {
        return On_error("error of move next");
    }

    // deserialization attributes
    Dogs::Attributes attributes = Serialization::deserialize< Dogs::Attributes>(v.data() + sizeof(Serialization::Buffer),
        v.size() - sizeof(Serialization::Buffer));

    // printing all attributes for collection 
    Env::DocAddGroup("attributes");
    for (auto att : attributes.attributes)
    {
        Env::DocAddText("", att.name.c_str());
    }
}


BEAM_EXPORT void Method_0()
{
    Env::DocGroup root("");
    {
        Env::DocGroup gr("roles");
        {
            {
                Env::DocGroup grRole("manager");

                {
                    Env::DocGroup grMethod("create_contract");
                }
                {
                    Env::DocGroup grMethod("destroy_contract");
                    Env::DocAddText("cid", "ContractID");
                }
                {
                    Env::DocGroup grMethod("view_contracts");
                }
            }

            {
                Env::DocGroup grRole("player");

                {
                    Env::DocGroup grMethod("add_collection");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("collection_name", "string");
                }
                {
                    Env::DocGroup grMethod("view_collections");
                    Env::DocAddText("cid", "ContractID"); 
                }
                {
                    Env::DocGroup grMethod("add_attribute");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("collection_name", "string");
                    Env::DocAddText("attribute_name", "string");
                }
                {
                    Env::DocGroup grMethod("view_attributes");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("collection_name", "string");
                }
            }
        }
    }
}

constexpr size_t ACTION_BUF_SIZE = 32;
constexpr size_t ROLE_BUF_SIZE = 16;

using Action_func_t = void (*)(const ContractID&);

template <typename T>
auto find_if_contains(const char* str, const std::vector<std::pair<const char*, T>>& v) 
{
    return std::find_if(v.begin(), v.end(), [&str](const auto& p) 
        {
            return !strcmp(str, p.first);
        }
    );
}

BEAM_EXPORT void Method_1()
{
    const std::vector<std::pair<const char*, Action_func_t>> VALID_PLAYER_ACTIONS = 
    {
        {"add_collection", On_action_add_collection},
        {"view_collections", On_action_view_collections},
        {"add_attribute", On_action_add_attribute_to_collection},
        {"view_attributes", On_action_view_attributes_by_collection}
    };

    const std::vector<std::pair<const char*, Action_func_t>> VALID_MANAGER_ACTIONS = 
    {
        {"create_contract", On_action_create_contract},
        {"destroy_contract", On_action_destroy_contract},
        {"view_contracts", On_action_view_contracts}
    };

    const std::vector<std::pair<const char*, const std::vector<std::pair<const char*, Action_func_t>>&>> VALID_ROLES = 
    {
        {"player", VALID_PLAYER_ACTIONS},
        {"manager", VALID_MANAGER_ACTIONS}
    };

    char action[ACTION_BUF_SIZE], role[ROLE_BUF_SIZE];

    if (!Env::DocGetText("role", role, sizeof(role))) 
    {
        return On_error("Role not specified");
    }

    auto it_role = find_if_contains(role, VALID_ROLES);

    if (it_role == VALID_ROLES.end()) 
    {
        return On_error("Invalid role");
    }

    if (!Env::DocGetText("action", action, sizeof(action))) 
    {
        return On_error("Action not specified");
    }

    auto it_action = find_if_contains(action, it_role->second);

    if (it_action != it_role->second.end()) 
    {
        ContractID cid;
        Env::DocGet("cid", cid);
        it_action->second(cid);
    }
    else 
    {
        On_error("Invalid action");
    }
}
