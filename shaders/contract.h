#pragma once

#ifndef HOST_BUILD

#include <cstddef>

#include <string>
#include <vector>

#define _LITTLE_ENDIAN
#include "exception_base.hpp" // hack
#define __YAS_THROW_EXCEPTION(type, msg) Env::Halt();
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>

#endif

namespace Serialize
{
	constexpr size_t YAS_FLAGS = yas::binary | yas::no_header | yas::elittle | yas::compacted;

#pragma pack (push, 1)

	struct Buffer
	{
		size_t size;
		char data[0];
	};
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
	using Collections = std::vector<Serialize::Collection>;

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
		std::vector<Serialize::Attribute> attributes; 

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
