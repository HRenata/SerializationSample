#pragma once

#ifndef HOST_BUILD
#define _LITTLE_ENDIAN
#include "exception_base.hpp" // hack
#define __YAS_THROW_EXCEPTION(type, msg) Env::Halt();
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>

#endif

namespace Serialization
{
	constexpr size_t YAS_FLAGS = yas::binary | yas::no_header | yas::elittle | yas::compacted;

#pragma pack (push, 1)

	struct Buffer
	{
		size_t size;
		char data[0];
	};

	template<typename ObjT>
	Serialization::Buffer* serialize(const ObjT& object)
	{
		yas::count_ostream cs;
		yas::binary_oarchive<yas::count_ostream, Serialization::YAS_FLAGS> sizeCalc(cs);
		sizeCalc& object;

		auto paramSize = sizeof(Serialization::Buffer) + cs.total_size;
		std::vector<char> ov(paramSize, '\0');
		Serialization::Buffer* buf = reinterpret_cast<Serialization::Buffer*>(ov.data());
		buf->size = cs.total_size;

		yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
		yas::binary_oarchive<yas::mem_ostream, Serialization::YAS_FLAGS> ar(ms);
		ar& object;

		return buf;
	}


	template<typename ObjT>
	ObjT deserialize(const void* ptr, std::size_t size)
	{
		yas::mem_istream ms(ptr, size);
		yas::binary_iarchive<yas::mem_istream, Serialization::YAS_FLAGS> iar(ms);

		ObjT obj;
		iar& obj;

		return obj;
	}

#pragma pack (pop)

}
