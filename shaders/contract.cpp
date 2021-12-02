#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

BEAM_EXPORT void Ctor(void*) {}

BEAM_EXPORT void Dtor(void*) {}

//serialization
BEAM_EXPORT void Method_2(Serialize::Buffer& paramsBuffer)
{
	std::vector<char> v(paramsBuffer.size, '\0');
	auto u = v.data();
	u= reinterpret_cast<char*> (paramsBuffer.data);

	Serialize::Test::Att  params;
	yas::mem_istream ms(paramsBuffer.data, paramsBuffer.size);
	yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

	iar& params;

	Env::SaveVar_T("0", params);
}

//deserialization
BEAM_EXPORT void Method_3(void*)
{
	Serialize::Test::Att  params;
	Env::LoadVar_T("0", params);
	params;
}
