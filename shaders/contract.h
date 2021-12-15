#pragma once

#include "serialization.h"
#include <string>
#include <vector>

namespace Dogs
{
#pragma pack (push, 1)

	struct Collection
	{
		std::string name;

		bool operator==(const Collection& other) const
		{
			return name == other.name;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& name;
		}
	};
	using Collections = std::vector<Dogs::Collection>;

	struct Attribute
	{
		std::string name;

		bool operator==(const Attribute& other) const
		{
			return name == other.name;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& name;
		}
	};
	struct Attributes
	{
		std::string collectionName;
		std::vector<Dogs::Attribute> attributes; 

		bool operator==(const Attributes& other) const
		{
			return collectionName == other.collectionName && attributes == other.attributes;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& collectionName& attributes;
		}
	};


	using Hash256 = Opaque<32>;
	Hash256 get_name_hash(const char* name, size_t len)
	{
		Hash256 res;
		HashProcessor::Sha256 hp;
		hp.Write(name, len);
		hp >> res;
		return res;
	}

	struct Key
	{
		Hash256 name_hash;
		Key(const Hash256& key)
		{
			Env::Memcpy(&name_hash, &key, sizeof(name_hash));
		}
	};

	struct Key1
	{
		int key;
	};
	/////////////////////////////////////////////////////////////////////////////////////////

	namespace Actions
	{
		struct AddCollection
		{
			static const uint32_t s_iMethod = 2;
		};
		struct AddAttribute
		{
			static const uint32_t s_iMethod = 3;
		};
	}
#pragma pack (pop)
}
