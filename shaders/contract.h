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
			Utility::Hash256 collectionNameHash;

			Key() = default;
			Key(const int key, const Utility::Hash256& collectionName) : Collections::Key(key)
			{
				Env::Memcpy(&collectionNameHash, &collectionName, sizeof(collectionNameHash));
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

	struct Image
	{
		uint32_t m_Size;
		// followed by the data
	};

	struct ImageInfo
	{
		std::string hash;

		bool operator==(const ImageInfo& other) const
		{
			return hash == other.hash;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& hash;
		}
	};

	struct ImagesInfos
	{
		struct Key : public Attributes::Key
		{
			Utility::Hash256 attributeNameHash;

			Key() = default;
			Key(const int key, const Utility::Hash256& collectionName, const Utility::Hash256& attributeName) 
				: Attributes::Key(key, collectionName)
			{
				Env::Memcpy(&attributeNameHash, &attributeName, sizeof(attributeNameHash));
			}
		};

		std::string collectionName;
		std::string attributeName;
		std::vector<Dogs::ImageInfo> images;

		bool operator==(const ImagesInfos& other) const
		{
			return collectionName == other.collectionName && attributeName == other.attributeName 
				&& images == other.images;
		}

		template<typename Ar>
		void serialize(Ar& ar)
		{
			ar& collectionName& attributeName& images;
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
		struct AddImageInfo
		{
			static const uint32_t s_iMethod = 4;
		};
		struct AddImage
		{
			static const uint32_t s_iMethod = 5;
		};
	}
#pragma pack (pop)
}
