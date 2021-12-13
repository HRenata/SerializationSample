#include "Shaders/common.h"
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
    Env::GenerateKernel(nullptr, 5, nullptr, 0, nullptr, 0, nullptr, 0, "Create Serialize contract", 0);

    char collectionName[0x20];
    uint32_t nNameSize = 0;
    nNameSize = Env::DocGetText("collection_name", collectionName, sizeof(collectionName));
    if (nNameSize < 2)
    {
        return On_error("collection_name should be non-empty");
    }

    //Env::Key_T<const char*> key;
    //_POD_(key.m_Prefix.m_Cid) = cid;
    //key.m_KeyInContract = "collections";

    //uint32_t valueLen = 0, keyLen = sizeof(Serialize::Key);

    //// deserialize
    //Env::VarReader reader(key, key);
    //reader.MoveNext(&key, keyLen, nullptr, valueLen, 0);

    //std::vector<char> iv(valueLen, '\0');
    //auto buf = reinterpret_cast<Serialize::Buffer*>(iv.data());
    //reader.MoveNext(&key, keyLen, buf, valueLen, 1);

    //yas::mem_istream ms(iv.data() + sizeof(Serialize::Buffer), iv.size() - sizeof(Serialize::Buffer));
    //yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

    //Serialize::Collections collections;
    //iar& collections;

    //Env::DocAddNum32("size", collections.size());
    //

    //std::string collectionNameStr(collectionName, nNameSize);
    //Serialize::Collection collection;
    //collection.name = collectionNameStr;

    //if (std::find(collections.begin(), collections.end(), collection) == collections.end())
    //{
    //    return On_error("collection_name should be non-empty");
    //}
    //
    //collections.push_back(collection);


    //// serialize
    //{
    //    yas::count_ostream cs;
    //    yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
    //    sizeCalc& collections;

    //    auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
    //    std::vector<char> ov(paramSize, '\0');
    //    Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(ov.data());
    //    buf->size = cs.total_size;

    //    yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
    //    yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(ms);
    //    ar& collections;

    //    Env::GenerateKernel(&cid, Serialize::Actions::AddCollection::s_iMethod, buf, paramSize, nullptr, 0, nullptr, 0,
    //        "Added new collection", 0);
    //}
}

void On_action_serialize(const ContractID& cid)
{
    Env::GenerateKernel(&cid, Serialize::Test::s_iMethod, nullptr, 0, nullptr, 0, nullptr, 0, "Serialized data sent to contract", 0);
}

void On_action_deserialize(const ContractID& cid)
{
    Env::Key_T<Serialize::Key> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.key) = 0;

    uint32_t valueLen = 0, keyLen = sizeof(Serialize::Key);

    Env::VarReader reader(key, key);
    if (!reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        return On_error("Tralalala");
    }

    std::vector<char> v(valueLen, '\0');
    auto buf = reinterpret_cast<Serialize::Buffer*>(v.data());
    if(!reader.MoveNext(&key, keyLen, buf, valueLen, 1))
    {
        return On_error("Tralalala2");
    }

    yas::mem_istream ms(v.data() + sizeof(Serialize::Buffer), v.size() - sizeof(Serialize::Buffer));
    yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

    Serialize::Test::Att obj; 
    iar& obj;

    Env::DocAddNum32("type", obj.type);
    Env::DocAddText("name", obj.name.c_str());
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
                    Env::DocGroup grMethod("serialize");
                    Env::DocAddText("cid", "ContractID");
                }
                {
                    Env::DocGroup grMethod("deserialize");
                    Env::DocAddText("cid", "ContractID");
                } 
                {
                    Env::DocGroup grMethod("add");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("name", "string");
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
        {"serialize", On_action_serialize},
        {"deserialize", On_action_deserialize},
        {"add", On_action_add_collection}
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
