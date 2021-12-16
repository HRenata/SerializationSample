#pragma once

#ifndef HOST_BUILD
#define _LITTLE_ENDIAN
#include "exception_base.hpp" // hack
#define __YAS_THROW_EXCEPTION(type, msg) Env::Halt();
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>
#endif

#include <memory>

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
	Buffer* serialize(const ObjT& object)
	{
		yas::count_ostream cos;
		yas::binary_oarchive<yas::count_ostream, YAS_FLAGS> sizeCalc(cos);
		sizeCalc& object;

		auto paramSize = sizeof(Buffer) + cos.total_size;
		std::vector<char> v(paramSize, '\0');
		Buffer* buf = reinterpret_cast<Buffer*>(v.data());
		buf->size = cos.total_size;

		yas::mem_ostream mos(reinterpret_cast<char*>(buf + 1), buf->size);
		yas::binary_oarchive<yas::mem_ostream, YAS_FLAGS> ar(mos);
		ar& object;

		return buf;
	}

	//template<typename ObjT>
	//std::unique_ptr<Buffer> serialize(const ObjT& object)
	//{
	//	yas::count_ostream cos;
	//	yas::binary_oarchive<yas::count_ostream, YAS_FLAGS> sizeCalc(cos);
	//	sizeCalc& object;

	//	auto paramSize = sizeof(Buffer) + cos.total_size;
	//	
	//	std::unique_ptr<Buffer> buf(static_cast<Buffer*>(::operator new(paramSize)));
	//	
	//	yas::mem_ostream mos(reinterpret_cast<char*>(buf.get() + 1), buf.get()->size);
	//	yas::binary_oarchive<yas::mem_ostream, YAS_FLAGS> ar(mos);
	//	ar& object;

	//	return buf;
	//}

	
	template<typename ObjT>
	ObjT deserialize(const void* ptr, std::size_t size)
	{
		yas::mem_istream mis(ptr, size);
		yas::binary_iarchive<yas::mem_istream, YAS_FLAGS> iar(mis);

		ObjT obj;
		iar& obj;

		return obj;
	}

#pragma pack (pop)

}
