#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

BEAM_EXPORT void Ctor(void*) 
{
	// creating empty collections
	Serialize::Collections collections;

	// serialization of collections
	yas::count_ostream cs;
	yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
	sizeCalc& collections;

	auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
	std::vector<char> v(paramSize, '\0');
	Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(v.data());
	buf->size = cs.total_size;

	yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
	yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(ms);
	ar& collections;

	// creating key
	Serialize::Key1 key;
	key.key = 0;

	// saving serialized buffer
	Env::SaveVar(&key, sizeof(key), buf, paramSize, KeyTag::Internal);
}

BEAM_EXPORT void Dtor(void*) {}

BEAM_EXPORT void Method_2(Serialize::Buffer& paramsBuffer)
{
	// creating key for saving collections
	Serialize::Key1 key;
	key.key = 0;
	
	// saving collections
	Env::SaveVar(&key, sizeof(key), &paramsBuffer, sizeof(Serialize::Buffer) + paramsBuffer.size, KeyTag::Internal);

	// deserialization of collections
	Serialize::Collections collections;

	yas::mem_istream ms(paramsBuffer.data, paramsBuffer.size);
	yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

	iar& collections;

	// creating of attributes with empty vector for last collection
	Serialize::Attributes attributes;
	attributes.collectionName = collections[collections.size() - 1].name;

	// serialization of attributes for last collection
	yas::count_ostream cs;
	yas::binary_oarchive<yas::count_ostream, Serialize::YAS_FLAGS> sizeCalc(cs);
	sizeCalc& attributes;

	auto paramSize = sizeof(Serialize::Buffer) + cs.total_size;
	std::vector<char> v(paramSize, '\0');
	Serialize::Buffer* buf = reinterpret_cast<Serialize::Buffer*>(v.data());
	buf->size = cs.total_size;

	yas::mem_ostream mos(reinterpret_cast<char*>(buf + 1), buf->size);
	yas::binary_oarchive<yas::mem_ostream, Serialize::YAS_FLAGS> ar(mos);
	ar& attributes;

	//creating key for saving attributes
	Serialize::Hash256 name_hash = Serialize::get_name_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Serialize::Key key_(name_hash);

	// saving attributes
	Env::SaveVar(&key_, sizeof(key_), buf, paramSize, KeyTag::Internal);
}

BEAM_EXPORT void Method_3(Serialize::Buffer& paramsBuffer)
{
	// deserialization of attributes
	Serialize::Attributes attributes;

	yas::mem_istream ms(paramsBuffer.data, paramsBuffer.size);
	yas::binary_iarchive<yas::mem_istream, Serialize::YAS_FLAGS> iar(ms);

	iar& attributes;

	//creating key
	Serialize::Hash256 name_hash = Serialize::get_name_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Serialize::Key key_(name_hash);
	
	// saving attributes
	Env::SaveVar(&key_, sizeof(key_), &paramsBuffer, sizeof(Serialize::Buffer) + paramsBuffer.size, KeyTag::Internal);
}