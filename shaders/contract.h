#pragma once

#include <string>
#include <vector>

#include "serialization.h"
#include "utility.h"

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


	/////////////////////////////////////////////////////////////////////////////////////////


	struct Key
	{
		Utility::Hash256 name_hash;
		Key(const Utility::Hash256& key)
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
