#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

BEAM_EXPORT void Ctor(void*) 
{
	//Serialize::Collections collections;

	//yas::count_ostream cs;
	//yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
	//sizeCalc& collections;

	//auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
	//std::vector<char> v(paramSize, '\0');
	//Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(v.data());
	//buf->size = cs.total_size;

	//yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
	//yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(ms);
	//ar& collections;

	//Env::SaveVar("collections", sizeof("collections"), &buf, paramSize, KeyTag::Internal);
}

BEAM_EXPORT void Dtor(void*) {}

//serialization
BEAM_EXPORT void Method_2(Serialize::Buffer& paramsBuffer)
{
	Serialize::Key key;
	key.key = 0;

	Env::SaveVar(&key, sizeof(key), &paramsBuffer, sizeof(Serialize::Buffer) + paramsBuffer.size, KeyTag::Internal);
}

//deserialization
BEAM_EXPORT void Method_3(void*)
{
	Serialize::Key key;
	key.key = 0;

	Serialize::Test::Att  obj;

	unsigned long long paramSize = 0;
	Serialize::Buffer* buf = nullptr;
	paramSize = Env::LoadVar(&key, sizeof(key), buf, paramSize, KeyTag::Internal);

	std::vector<char> v(paramSize, '\0');
	buf = reinterpret_cast<Serialize::Buffer*>(v.data());

	Env::LoadVar(&key, sizeof(key), buf, paramSize, KeyTag::Internal);

	yas::mem_istream ms(v.data() + sizeof(Serialize::Buffer), v.size() - sizeof(Serialize::Buffer));
	yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

	iar& obj;
}

BEAM_EXPORT void Method_4(Serialize::Buffer& paramsBuffer)
{
	Env::SaveVar("collections", sizeof("collections"), &paramsBuffer, sizeof(Serialize::Buffer) + paramsBuffer.size, KeyTag::Internal);
}
#include <deque>
BEAM_EXPORT void Method_5(void*)
{
	std::deque<Serialize::Collection> k;
	//Serialize::Collections collections;

	//yas::count_ostream cs;
	//yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
	//sizeCalc& collections;

	//auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
	//std::vector<char> v(paramSize, '\0');
	//Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(v.data());
	//buf->size = cs.total_size;

	//yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
	//yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(ms);
	//ar& collections;

	//Env::SaveVar("collections", sizeof("collections"), &buf, paramSize, KeyTag::Internal);
}
