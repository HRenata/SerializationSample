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
	struct Collections
	{
		struct Key
		{
			int pubKey;
			Key() = default;
			Key(const int key) : pubKey(key) {}
		};

		std::vector<Dogs::Collection> collections;

		bool operator==(const Collections& other) const
		{
			return collections == other.collections;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& collections;
		}
	};

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
		struct Key : public Collections::Key
		{
			Utility::Hash256 name_hash;

			Key() = default;
			Key(const int key, const Utility::Hash256& name) : Collections::Key(key)
			{
				Env::Memcpy(&name_hash, &name, sizeof(name_hash));
			}
		};

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
