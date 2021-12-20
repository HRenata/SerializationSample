#pragma once
#include "utility.h"
#include "serialization.h"

namespace Gallery
{
#pragma pack (push, 1)

    struct Masterpiece
    {
        struct Key {
            Utility::Hash256 m_ID;
            //std::string m_ID;
        };

        PubKey m_pkOwner;
    };


    struct State
    {
        static const uint8_t s_Key = 0;

        std::string m_Exhibits;
    };

    struct Events
    {
        struct Add {
            struct Key {
                uint8_t m_Prefix = 0;
                Utility::Hash256 m_ID;
                //std::string m_ID;
                PubKey m_pkArtist;
            };
            // data is the exhibit itself
        };
    };

    struct Image
    {
        PubKey m_pkArtist;
        uint32_t m_Size;
        Utility::Hash256 m_ID;
        // followed by the data
    };


    namespace Method
    {
        struct AddExhibit
        {
            static const uint32_t s_iMethod = 6;
            PubKey m_pkArtist;
            uint32_t m_Size;
            // followed by the data
        };
    }
#pragma pack (pop)
}
