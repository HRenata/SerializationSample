#include "Shaders/common.h"
#include "Shaders/Math.h"
#include "contract.h"

#include "pictures.h"

BEAM_EXPORT void Ctor(void*) 
{
	// creating empty collections
	Dogs::Collections collections;

	// serialization of collections
	auto serializedBuffer = Serialization::serialize(collections);

	// creating key
	Dogs::Collections::Key key (0);

	// saving serialized buffer
	Env::SaveVar(&key, sizeof(key), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);
}

BEAM_EXPORT void Dtor(void*) {}

BEAM_EXPORT void Method_2(Serialization::Buffer& paramsBuffer)
{
	// creating key for saving collections
	Dogs::Collections::Key key (0);
	
	// saving collections
	Env::SaveVar(&key, sizeof(key), &paramsBuffer, sizeof(Serialization::Buffer) + paramsBuffer.size, KeyTag::Internal);

	// deserialization of collections
	Dogs::Collections collections = Serialization::deserialize<Dogs::Collections>(paramsBuffer.data, paramsBuffer.size);

	// creating of attributes with empty vector for last collection
	Dogs::Attributes attributes;
	attributes.collectionName = collections.collections[collections.collections.size() - 1].name;

	// serialization of attributes for last collection
	auto serializedBuffer = Serialization::serialize(attributes);

	//creating key for saving attributes
	Utility::Hash256 nameHash = Utility::get_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Dogs::Attributes::Key keyAtt(0, nameHash);

	// saving attributes
	Env::SaveVar(&keyAtt, sizeof(keyAtt), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);
}

BEAM_EXPORT void Method_3(Serialization::Buffer& paramsBuffer)
{
	// deserialization of attributes
	Dogs::Attributes attributes = Serialization::deserialize<Dogs::Attributes>(paramsBuffer.data, paramsBuffer.size);

	//creating key for saving attributes
	Utility::Hash256 collectionNameHash = Utility::get_hash(attributes.collectionName.c_str(), attributes.collectionName.size());
	Dogs::Attributes::Key keyAtt(0, collectionNameHash);
	
	// saving attributes
	Env::SaveVar(&keyAtt, sizeof(keyAtt), &paramsBuffer, sizeof(Serialization::Buffer) + paramsBuffer.size, KeyTag::Internal);


	// creating of attributes with empty vector of images for last collection
	Dogs::ImagesInfos images;
	images.collectionName = attributes.collectionName;
	images.attributeName = attributes.attributes[attributes.attributes.size() - 1].name;

	// serialization of attributes for last collection
	auto serializedBuffer = Serialization::serialize(images);

	//creating key for saving images
	Utility::Hash256 attributeNameHash = Utility::get_hash(images.attributeName.c_str(), images.attributeName.size());
	Dogs::ImagesInfos::Key keyIm(0, collectionNameHash, attributeNameHash);

	// saving images
	Env::SaveVar(&keyIm, sizeof(keyIm), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);
}

BEAM_EXPORT void Method_4(Serialization::Buffer& paramsBuffer)
{
	// deserialization of attributes
	Dogs::ImagesInfos images = Serialization::deserialize<Dogs::ImagesInfos>(paramsBuffer.data, paramsBuffer.size);

	//creating key for saving attributes
	Utility::Hash256 collectionNameHash = Utility::get_hash(images.collectionName.c_str(), images.collectionName.size());
	Utility::Hash256 attributeNameHash = Utility::get_hash(images.attributeName.c_str(), images.attributeName.size());
	Dogs::ImagesInfos::Key keyIm(0, collectionNameHash, attributeNameHash);

	// saving attributes
	Env::SaveVar(&keyIm, sizeof(keyIm), &paramsBuffer, sizeof(Serialization::Buffer) + paramsBuffer.size, KeyTag::Internal);
}

BEAM_EXPORT void Method_5(const Gallery::Image& image)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
const T& min(const T& a, const T& b)
{
	return (b < a) ? b : a;
}
BEAM_EXPORT void Method_6(const Gallery::Image& image)
{
	// picture to uint8_t*
	auto pData = reinterpret_cast<const uint8_t*>(&image + 1);
	uint32_t nData = image.m_Size;


	// key for saving picture info
	Gallery::Masterpiece::Key mk;
	auto x = Utility::get_hash(pData, nData);
	
	Env::Memcpy(&mk.m_ID,&x,sizeof(x));
	//mk.m_ID = Utility::EncodeBase64(x.m_p, sizeof(x.m_p));
	//mk.m_ID = std::string(reinterpret_cast<char const*>(x.m_p), sizeof x.m_p);






	// creating empty collections
	std::vector<Utility::Hash256> hashes;


	// creating key
	Dogs::Collections::Key key (1);

	//deserialization
	if (Env::LoadVar(&key, sizeof(key), nullptr, 0, KeyTag::Internal)) {
		unsigned long long paramSize = 0;
		Serialization::Buffer* buf = nullptr;
		paramSize = Env::LoadVar(&key, sizeof(key), buf, paramSize, KeyTag::Internal);

		std::vector<char> v(paramSize, '\0');
		buf = reinterpret_cast<Serialization::Buffer*>(v.data());

		Env::LoadVar(&key, sizeof(key), buf, paramSize, KeyTag::Internal);

		yas::mem_istream ms(v.data() + sizeof(Serialization::Buffer), v.size() - sizeof(Serialization::Buffer));
		yas::binary_iarchive<yas::mem_istream, Serialization::YAS_FLAGS> iar(ms);

		iar& hashes;
	}



	hashes.push_back(mk.m_ID);

	// serialization 
	auto serializedBuffer = Serialization::serialize(hashes);

	// saving serialized buffer
	Env::SaveVar(&key, sizeof(key), serializedBuffer,
		sizeof(Serialization::Buffer) + serializedBuffer->size, KeyTag::Internal);



	//// info about picture
	//Gallery::Masterpiece m;
	//_POD_(m).SetZero();
	//_POD_(m.m_pkOwner) = image.m_pkArtist;

	// saveing picture info by key mk
	//Env::SaveVar_T(mk, m);

	////////////////////////////////////////////////////////////////////////////

	// key for saving picture to log
	Gallery::Events::Add::Key eak;
	Env::Memcpy(&eak.m_ID, &x, sizeof(mk.m_ID));
	//eak.m_ID = mk.m_ID;
	_POD_(eak.m_pkArtist) = image.m_pkArtist;

	uint32_t nMaxEventSize = 0x2000; // TODO: max event size is increased to 1MB from HF4

	// saving picture to logs
	while (true)
	{
		Env::EmitLog(&eak, sizeof(eak), pData, min(nData, nMaxEventSize), KeyTag::Internal);

		if (nData <= nMaxEventSize)
			break;

		nData -= nMaxEventSize;
		pData += nMaxEventSize;
	}
}