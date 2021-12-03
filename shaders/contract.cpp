#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

BEAM_EXPORT void Ctor(void*) {}

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
