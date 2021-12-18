#include "app.h"
#include "contract_sid.i"
#include "pictures.h"

#include <algorithm>
#include <memory>

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
    std::string collectionName = getTextNameToString("collection_name");
    if (collectionName.empty())
    {
        return On_error("collection_name should be non-empty");
    }

    Dogs::Collection collection;
    collection.name = collectionName;

    // creating key
    Env::Key_T<Dogs::Collections::Key> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.pubKey) = 0;

    // loading serialized buf of collections
    Dogs::Collections collections;

    if(!loadAndDeserializeObject(collections, key))
    {
        return On_error("Cann't read collections");
    }

    // checking if such collection name doesn't exist
    if (std::find(collections.collections.begin(), collections.collections.end(), collection) != collections.collections.end())
    {
        return On_error("collection_name exists");
    }

    // adding new collection
    collections.collections.push_back(collection);

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
    Env::Key_T<Dogs::Collections::Key> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.pubKey) = 0;

    // loading collections
    Dogs::Collections collections;

    if(!loadAndDeserializeObject(collections, key))
    {
        return On_error("Cann't read collections");
    }

    // printing all collecitons' name
    Env::DocAddGroup("collections");
    for (auto coll : collections.collections)
    {
        Env::DocAddText("", coll.name.c_str());
    }
}

void On_action_add_attribute_to_collection(const ContractID& cid)
{    
    // loading name of collection, that will get new attribute, from function parameters
    std::string collectionName = getTextNameToString("collection_name");
    if (collectionName.empty())
    {
        return On_error("collection_name should be non-empty");
    }
    
    // loading name of attribute for collection  from function parameters
    std::string attributeName = getTextNameToString("attribute_name");
    if (collectionName.empty())
    {
        return On_error("attribute_name should be non-empty");
    }

    // creation collection
    Dogs::Collection collection;
    collection.name = collectionName;

    // creating key for collections
    Env::Key_T<Dogs::Collections::Key> keyColl;
    _POD_(keyColl.m_Prefix.m_Cid) = cid;
    _POD_(keyColl.m_KeyInContract.pubKey) = 0;

    // loading collections
    Dogs::Collections collections;

    if (!loadAndDeserializeObject(collections, keyColl))
    {
        return On_error("Cann't read collections");
    }

    // checking if such collection name doesn't exist
    if (std::find(collections.collections.begin(), collections.collections.end(), collection) == collections.collections.end())
    {
        return On_error("collection_name doesn't exist");
    }


    // creating attribute
    Dogs::Attribute attribute;
    attribute.name = attributeName;

    // creating key for attributes
    Utility::Hash256 attributeNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    Dogs::Attributes::Key key(0, attributeNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyAtt = { .m_KeyInContract = key };
    keyAtt.m_Prefix.m_Cid = cid;

    // loading attributes for collection
    Dogs::Attributes attributes;

    if (!loadAndDeserializeObject(attributes, keyAtt))
    {
        return On_error("Cann't read collections");
    }

    // checking if such attribute already exists
    if (std::find(attributes.attributes.begin(), attributes.attributes.end(), attribute) != attributes.attributes.end())
    {
        return On_error("attributes_name exists");
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
    std::string collectionName = getTextNameToString("collection_name");
    if (collectionName.empty())
    {
        return On_error("collection_name should be non-empty");
    }

    // creating key for loading attributes
    Utility::Hash256 collectionNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    
    Dogs::Attributes::Key key(0, collectionNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyAtt = { .m_KeyInContract = key };
    keyAtt.m_Prefix.m_Cid = cid;

    // loading attributes for collection
    Dogs::Attributes attributes;

    if (!loadAndDeserializeObject(attributes, keyAtt))
    {
        return On_error("Cann't read collections");
    }

    // printing all attributes for collection 
    Env::DocAddGroup("attributes");
    for (auto att : attributes.attributes)
    {
        Env::DocAddText("", att.name.c_str());
    }
}

void On_action_add_image_to_attribute(const ContractID& cid)
{
    // loading name of collection, that will get new attribute, from function parameters
    std::string collectionName = getTextNameToString("collection_name");
    if (collectionName.empty())
    {
        return On_error("collection_name should be non-empty");
    }

    // loading name of attribute for collection  from function parameters
    std::string attributeName = getTextNameToString("attribute_name");
    if (collectionName.empty())
    {
        return On_error("attribute_name should be non-empty");
    }

    // creation collection
    Dogs::Collection collection;
    collection.name = collectionName;

    // creating key for collections
    Env::Key_T<Dogs::Collections::Key> keyColl;
    _POD_(keyColl.m_Prefix.m_Cid) = cid;
    _POD_(keyColl.m_KeyInContract.pubKey) = 0;

    // loading collections
    Dogs::Collections collections;

    if (!loadAndDeserializeObject(collections, keyColl))
    {
        return On_error("Cann't read collections");
    }

    // checking if such collection name doesn't exist
    if (std::find(collections.collections.begin(), collections.collections.end(), collection) == collections.collections.end())
    {
        return On_error("collection_name doesn't exist");
    }


    // creating attribute
    Dogs::Attribute attribute;
    attribute.name = attributeName;

    // creating key for attributes
    Utility::Hash256 collectionNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    Dogs::Attributes::Key key(0, collectionNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyAtt = { .m_KeyInContract = key };
    keyAtt.m_Prefix.m_Cid = cid;

    // loading attributes for collection
    Dogs::Attributes attributes;

    if (!loadAndDeserializeObject(attributes, keyAtt))
    {
        return On_error("Cann't read collections");
    }

    // checking if such attribute already exists
    if (std::find(attributes.attributes.begin(), attributes.attributes.end(), attribute) == attributes.attributes.end())
    {
        return On_error("attributes_name doesn't exist");
    }


    // load image to data
    // load size
    auto imageSize = Env::DocGetBlob("data", nullptr, 0);
    if (!imageSize)
    {
        return On_error("data not specified");
    }

    uint32_t imageObjSize = sizeof(Gallery::Image) + imageSize;
    auto* image = (Gallery::Image*)Env::Heap_Alloc(imageObjSize);
    image->m_Size = imageSize;

    //load image to Image
    if (Env::DocGetBlob("data", image + 1, imageSize) != imageSize)
    {
        On_error("data can't be parsed");
        return;
    }

    // creating key for attributes
    Utility::Hash256 attributeNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    Dogs::ImagesInfos::Key key_(0, collectionNameHash, attributeNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyIm = { .m_KeyInContract = key_ };
    keyIm.m_Prefix.m_Cid = cid;

    // loading images for 
    Dogs::ImagesInfos images;

    if (!loadAndDeserializeObject(images, keyIm))
    {
        return On_error("Cann't read imageInfos");
    }

    // getting image hash
    // picture to uint8_t*
    auto pData = reinterpret_cast<const uint8_t*>(image + 1);
    uint32_t nData = image->m_Size;
    auto imageHash = Utility::get_hash(pData, nData);

    // creating image info
    Dogs::ImageInfo imageInfo;
    imageInfo.hash = std::string(reinterpret_cast<char const*>(imageHash.m_p), sizeof imageHash.m_p);

    // checking if such image already exists
    if (std::find(images.images.begin(), images.images.end(), imageInfo) != images.images.end())
    {
        return On_error("image exists");
    }

    images.images.push_back(imageInfo);

    // serialization of attributes
    auto serializedBuffer = Serialization::serialize(images);

    // transaction for saving image info
    Env::GenerateKernel(&cid, Dogs::Actions::AddImageInfo::s_iMethod, serializedBuffer,
        sizeof(Serialization::Buffer) + serializedBuffer->size, nullptr, 0, nullptr, 0,
        "Added new attribute", 0);

    // transaction for saving image
    uint32_t charge = 90000;
    charge += imageSize * 110;

    Env::GenerateKernel(&cid, Dogs::Actions::AddImage::s_iMethod, image, imageObjSize, nullptr, 0,
        nullptr, 0, "Saved new image", charge);
}

void On_action_get_all_images_ids(const ContractID& cid)
{
    // loading name of collection, that will get new attribute, from function parameters
    std::string collectionName = getTextNameToString("collection_name");
    if (collectionName.empty())
    {
        return On_error("collection_name should be non-empty");
    }

    // loading name of attribute for collection  from function parameters
    std::string attributeName = getTextNameToString("attribute_name");
    if (collectionName.empty())
    {
        return On_error("attribute_name should be non-empty");
    }

    // creation collection
    Dogs::Collection collection;
    collection.name = collectionName;

    // creating key for collections
    Env::Key_T<Dogs::Collections::Key> keyColl;
    _POD_(keyColl.m_Prefix.m_Cid) = cid;
    _POD_(keyColl.m_KeyInContract.pubKey) = 0;

    // loading collections
    Dogs::Collections collections;

    if (!loadAndDeserializeObject(collections, keyColl))
    {
        return On_error("Cann't read collections");
    }

    // checking if such collection name doesn't exist
    if (std::find(collections.collections.begin(), collections.collections.end(), collection) == collections.collections.end())
    {
        return On_error("collection_name doesn't exist");
    }


    // creating attribute
    Dogs::Attribute attribute;
    attribute.name = attributeName;

    // creating key for attributes
    Utility::Hash256 collectionNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    Dogs::Attributes::Key key(0, collectionNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyAtt = { .m_KeyInContract = key };
    keyAtt.m_Prefix.m_Cid = cid;

    // loading attributes for collection
    Dogs::Attributes attributes;

    if (!loadAndDeserializeObject(attributes, keyAtt))
    {
        return On_error("Cann't read collections");
    }

    // checking if such attribute already exists
    if (std::find(attributes.attributes.begin(), attributes.attributes.end(), attribute) == attributes.attributes.end())
    {
        return On_error("attributes_name doesn't exist");
    }

    // creating key for attributes
    Utility::Hash256 attributeNameHash = Utility::get_hash(collectionName.c_str(), collectionName.size());
    Dogs::ImagesInfos::Key key_(0, collectionNameHash, attributeNameHash);
    Env::Key_T<Dogs::Attributes::Key> keyIm = { .m_KeyInContract = key_ };
    keyIm.m_Prefix.m_Cid = cid;

    // loading images for 
    Dogs::ImagesInfos images;

    if (!loadAndDeserializeObject(images, keyIm))
    {
        return On_error("Cann't read imageInfos");
    }

    Env::DocGroup root("");
    Env::DocArray gr0("items");

    for (auto im : images.images)
    {
        Env::DocGroup gr1("");
        Env::DocAddText("id", im.hash.c_str());
    }

}

void On_action_view_image_by_id(const ContractID& cid)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// add image
void On_action_save_attribute_picture(const ContractID& cid)
{
    // load image to data
    // 1. load size
    auto imageSize = Env::DocGetBlob("data", nullptr, 0);
    if (!imageSize)
    {
        return On_error("data not specified");
    }

    uint32_t imageObjSize = sizeof(Gallery::Image) + imageSize;
    auto* image = (Gallery::Image*)Env::Heap_Alloc(imageObjSize);

    PubKey pkArtist;
    Env::DerivePk(pkArtist, &cid, sizeof(cid));

    _POD_(image->m_pkArtist) = pkArtist;
    image->m_Size = imageSize;

    // 2. load image to Image
    if (Env::DocGetBlob("data", image + 1, imageSize) != imageSize)
    {
        On_error("data can't be parsed");
        return;
    }

    uint32_t charge = 90000;
    charge += imageSize * 110;

    Env::GenerateKernel(&cid, Gallery::Method::AddExhibit::s_iMethod, image, imageObjSize, nullptr, 0, nullptr, 0,
        "CryptoDogs upload picture", charge);
}

void On_action_load_attribute_picture(const ContractID& cid)
{
    char id[0x20];
    uint32_t collectionNameSize = 0;
    collectionNameSize = Env::DocGetText("id", id, sizeof(id));
    if (collectionNameSize < 2)
    {
        return On_error("collection_name should be non-empty");
    }
    Env::DocAddText("", id);
    std::string idStr(id, collectionNameSize);


    Env::Key_T<Gallery::Events::Add::Key> k0, k1;
    _POD_(k0.m_Prefix.m_Cid) = cid;
    _POD_(k1.m_Prefix.m_Cid) = cid;
    k0.m_KeyInContract.m_ID = idStr;
    k1.m_KeyInContract.m_ID = idStr;
    _POD_(k0.m_KeyInContract.m_pkArtist).SetZero();
    _POD_(k1.m_KeyInContract.m_pkArtist).SetObject(0xff);

    uint32_t nCount = 0;
    Utils::Vector<uint8_t> vData;

    Env::LogReader r(k0, k1);
    for (; ; nCount++)
    {
        uint32_t nData = 0, nKey = sizeof(k0);
        if (!r.MoveNext(&k0, nKey, nullptr, nData, 0))
            break;

        vData.Prepare(vData.m_Count + nData);
        r.MoveNext(&k0, nKey, vData.m_p + vData.m_Count, nData, 1);
        vData.m_Count += nData;
    }

    Env::DocGroup gr2("");

    if (nCount)
    {
        Env::DocAddBlob("data", vData.m_p, vData.m_Count);
    }
    else
    {
        On_error("not found");
    }
}

void On_action_load_all_pictures_id(const ContractID& cid)
{
    // creating key for collections
    Env::Key_T<Dogs::Collections::Key> key;
    _POD_(key.m_Prefix.m_Cid) = cid;
    _POD_(key.m_KeyInContract.pubKey) = 1;

    // loading serialized buf of collections
    uint32_t valueLen = 0, keyLen = sizeof(Dogs::Collections::Key);

    Env::VarReader reader(key, key);
    if (!reader.MoveNext(&key, keyLen, nullptr, valueLen, 0))
    {
        return On_error("error of move next");
    }

    auto buf = std::make_unique<uint8_t[]>(valueLen + 1); // 0-term
    if (!reader.MoveNext(&key, keyLen, buf.get(), valueLen, 1))
    {
        return On_error("error of move next");
    }

    // deserialization of collections
    std::vector<std::string> hashes = Serialization::deserialize<std::vector<std::string>>(buf.get() + sizeof(Serialization::Buffer),
        valueLen - sizeof(Serialization::Buffer));


    Env::DocGroup gr2("");
    Env::DocArray gr0("items");

    for (auto hash : hashes )
    {
        Env::DocGroup gr1("");
        Env::DocAddText("id", hash.c_str());
    }
}

void On_action_get_my_pkey(const ContractID& cid)
{
    PubKey my_key;
    Env::DerivePk(my_key, &cid, sizeof(cid));
    Env::DocGroup root("");
    {
        Env::DocAddBlob_T("My public key", my_key);
    }
}

void On_action_get_artist_pkey(const ContractID& cid)
{
    // rewrite
    PubKey my_key;
    Env::DerivePk(my_key, &cid, sizeof(cid));
    Env::DocGroup root("");
    {
        Env::DocAddBlob_T("key", my_key);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

                ///////////////////////////
                {
                    Env::DocGroup grMethod("upload");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("pkArtist", "pubKey");
                    Env::DocAddText("data", "blob");
                }
                {
                    Env::DocGroup grMethod("download");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("id", "uin32");
                }
                ///////////////////////////
            }

            ////////////////////////////////////////////
            {
                Env::DocGroup grRole("artist");

                {
                    Env::DocGroup grMethod("save_attribute_picture");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("attribute_name", "string");
                    Env::DocAddText("data", "blob");
                }
                {
                    Env::DocGroup grMethod("load_all_attribute_pictures");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("attribute_name", "string");
                }
                {
                    Env::DocGroup grMethod("get_key");
                    Env::DocAddText("cid", "ContractID");
                }
            }

            {
                Env::DocGroup grRole("user");

                {
                    Env::DocGroup grMethod("download");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("id", "uin32");
                }
            }
            ////////////////////////////////////////////

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
                {
                    Env::DocGroup grMethod("add_picture");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("collection_name", "string");
                    Env::DocAddText("attribute_name", "string");
                    Env::DocAddText("data", "blob");
                }
                {
                    Env::DocGroup grMethod("get_all_pictures_ids");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("collection_name", "string");
                    Env::DocAddText("attribute_name", "string");
                }
                {
                    Env::DocGroup grMethod("view_picture");
                    Env::DocAddText("cid", "ContractID");
                    Env::DocAddText("id", "string");
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
        {"view_attributes", On_action_view_attributes_by_collection},
        {"add_picture", On_action_add_image_to_attribute},
        {"get_all_pictures_ids", On_action_get_all_images_ids},
        {"view_picture", On_action_view_image_by_id}
    };

    const std::vector<std::pair<const char*, Action_func_t>> VALID_MANAGER_ACTIONS = 
    {
        {"create_contract", On_action_create_contract},
        {"destroy_contract", On_action_destroy_contract},
        {"view_contracts", On_action_view_contracts}
        
        ///////////////////////////////////////////////////
        ,
        {"upload", On_action_save_attribute_picture},
        {"download", On_action_load_attribute_picture}
        ///////////////////////////////////////////////////
    };

    ///////////////////////////////////////////////////
    const std::vector<std::pair<const char*, Action_func_t>> VALID_ARTIST_ACTIONS =
    {
        {"save_attribute_picture", On_action_save_attribute_picture},
        {"load_attribute_picture", On_action_load_attribute_picture},
        {"get_key", On_action_get_artist_pkey}
    };

    const std::vector<std::pair<const char*, Action_func_t>> VALID_USER_ACTIONS =
    {
        {"get_my_pkey", On_action_get_my_pkey},
        {"view_all", On_action_load_all_pictures_id},
        {"download", On_action_load_attribute_picture}
    };
    ///////////////////////////////////////////////////

    const std::vector<std::pair<const char*, const std::vector<std::pair<const char*, Action_func_t>>&>> VALID_ROLES = 
    {
        {"player", VALID_PLAYER_ACTIONS},
        {"manager", VALID_MANAGER_ACTIONS}

        ///////////////////////////////////////////////////
        ,
        {"user", VALID_USER_ACTIONS},
        {"artist", VALID_ARTIST_ACTIONS}
        ///////////////////////////////////////////////////
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
