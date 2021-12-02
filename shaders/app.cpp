#include "Shaders/common.h"
#include "Shaders/app_common_impl.h"
#include "contract.h"

#include "contract_sid.i"

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

void On_action_serialize(const ContractID& cid)
{
    Serialize::Test::Att obj;
    obj.type = 90;
    obj.name = "Name";

    yas::count_ostream cs;
    yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
    sizeCalc& obj;

    auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
    std::vector<char> v(paramSize, '\0');
    Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(v.data());
    buf->size = cs.total_size;

    yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
    yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(ms);
    ar& obj;
    Env::DocAddNum32("first", paramSize);

    Env::GenerateKernel(&cid, Serialize::Test::s_iMethod, buf, paramSize, nullptr, 0, nullptr, 0, "Serialized data sent to contract", 0);
}

void On_action_deserialize(const ContractID& cid)
{
    Serialize::Test::Att test;
    if (!Env::VarReader::Read_T("0", test))
         return On_error("Failed to read contract's current params");

    Env::DocAddNum32("type", test.type);
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
        {"deserialize", On_action_deserialize}
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
