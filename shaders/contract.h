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

	struct Test
	{
		static const uint32_t s_iMethod = 2;
		struct Att
		{
            int type;
            std::string name;
            bool operator==(const Att& other) const
            {
				return type == other.type && name == other.name;
            }
            template<typename Ar>
            void serialize(Ar& ar)
            {
                ar& type& name;
            }
		};
	};

	struct Key
	{
		int key;
	};

	/////////////////////////////////////////////////////////////////////////////////////////

	namespace Actions
	{
		struct AddCollection
		{
			static const uint32_t s_iMethod = 4;
			std::string name;

			template<typename Ar>
			void serialize(Ar& ar)
			{
				ar& name;
			}
		};
	}
#pragma pack (pop)
}
