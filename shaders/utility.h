#pragma once
#include <string>
#include <vector>

namespace Utility
{

    using Hash256 = Opaque<32>;
}

template<typename Ar>
void serialize(Ar& ar, Utility::Hash256& obj)
{
    ar& obj.m_p;
}

template <uint32_t nBytes>
bool operator==(const Opaque<nBytes>& lhs, const  Opaque<nBytes>& rhs)
{
	for (size_t i = 0; i < nBytes; ++i)
	{
		if (lhs.m_p[i] != rhs.m_p[i])
			return false;
	}
	return true;
}

namespace Utility
{
	Hash256 get_hash(const void* ptr, size_t len)
	{
		Hash256 res;
		HashProcessor::Sha256 hp;
		hp.Write(ptr, len);
		hp >> res;
		return res;
	}
}
