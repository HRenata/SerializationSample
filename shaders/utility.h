#pragma once

namespace Utility
{
	using Hash256 = Opaque<32>;
	Hash256 get_hash(const void* ptr, size_t len)
	{
		Hash256 res;
		HashProcessor::Sha256 hp;
		hp.Write(ptr, len);
		hp >> res;
		return res;
	}
}
