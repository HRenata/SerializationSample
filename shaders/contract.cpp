#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

BEAM_EXPORT void Ctor(void*) 
{
	// creating empty collections
	Dogs::Collections collections;

	// serialization of collections
	auto serializedBuffer = Serialization::serialize(collections);

	// creating key
	Dogs::Key1 key;
	key.key = 0;

	// saving serialized buffer
	Env::SaveVar(&key, sizeof(key), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);
}

BEAM_EXPORT void Dtor(void*) {}

BEAM_EXPORT void Method_2(Serialization::Buffer& paramsBuffer)
{
	// creating key for saving collections
	Dogs::Key1 key;
	key.key = 0;
	
	// saving collections
	Env::SaveVar(&key, sizeof(key), &paramsBuffer, sizeof(Serialization::Buffer) + paramsBuffer.size, KeyTag::Internal);

	// deserialization of collections
	Dogs::Collections collections = Serialization::deserialize<Dogs::Collections>(paramsBuffer.data, paramsBuffer.size);

	// creating of attributes with empty vector for last collection
	Dogs::Attributes attributes;
	attributes.collectionName = collections[collections.size() - 1].name;

	// serialization of attributes for last collection
	auto serializedBuffer = Serialization::serialize(attributes);

	//creating key for saving attributes
	Utility::Hash256 nameHash = Utility::get_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Dogs::Key keyAtt(nameHash);

	// saving attributes
	Env::SaveVar(&keyAtt, sizeof(keyAtt), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);
}

BEAM_EXPORT void Method_3(Serialization::Buffer& paramsBuffer)
{
	// deserialization of attributes
	Dogs::Attributes attributes = Serialization::deserialize<Dogs::Attributes>(paramsBuffer.data, paramsBuffer.size);

	//creating key
	Utility::Hash256 nameHash = Utility::get_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Dogs::Key keyAtt(nameHash);
	
	// saving attributes
	Env::SaveVar(&keyAtt, sizeof(keyAtt), &paramsBuffer, sizeof(Serialization::Buffer) + paramsBuffer.size, KeyTag::Internal);
}
