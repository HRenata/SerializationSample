// Copyright 2018-2021 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions andB
// limitations under the License.

#define HOST_BUILD

#include "../beam/core/block_rw.h"
#include "../beam/bvm/bvm2_impl.h"
#include <cmath>
#include <cstddef>

#include <string>
#include <vector>
#include <algorithm>
#include <deque>


namespace Shaders {

#ifdef _MSC_VER
#	pragma warning (disable : 4200 4702) // unreachable code
#endif // _MSC_VER

#define BEAM_EXPORT

#include "Shaders/common.h"
}

/// YAS
//#define _LITTLE_ENDIAN
#include "../shaders/exception_base.hpp" // hack
#define __YAS_THROW_EXCEPTION(type, msg) Shaders::Env::Halt();
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>
/// YAS

namespace ECC {

	void SetRandom(uintBig& x)
	{
		GenRandom(x);
	}

	void SetRandom(Scalar::Native& x)
	{
		Scalar s;
		while (true)
		{
			SetRandom(s.m_Value);
			if (!x.Import(s))
				break;
		}
	}

	void SetRandom(Key::IKdf::Ptr& pRes)
	{
		uintBig seed;
		SetRandom(seed);
		HKdf::Create(pRes, seed);
	}

}


namespace Shaders {

#include "../shaders/contract.h"

	template <bool bToShader> void Convert(Shaders::Serialize::Buffer& x)
	{

	}
	
	namespace Contract {
#include "../shaders/contract_sid.i"
#include "../shaders/contract.cpp"
	}

#ifdef _MSC_VER
#	pragma warning (default : 4200 4702)
#endif // _MSC_VER
}

int g_TestsFailed = 0;

void TestFailed(const char* szExpr, uint32_t nLine)
{
	printf("Test failed! Line=%u, Expression: %s\n", nLine, szExpr);
	g_TestsFailed++;
	fflush(stdout);
}

#define verify_test(x) \
	do { \
		if (!(x)) \
			TestFailed(#x, __LINE__); \
	} while (false)

#define fail_test(msg) TestFailed(msg, __LINE__)

using namespace beam;
using namespace beam::bvm2;


#include "unittest/contract_test_processor.h"

namespace beam {
	namespace bvm2 {

		struct MyProcessor
			:public ContractTestProcessor
		{

			struct Code
			{
				ByteBuffer m_Contract;
			} m_Code;

			ContractID m_cid;


			void CallFar(const ContractID& cid, uint32_t iMethod, Wasm::Word pArgs, uint8_t bInheritContext) override
			{

				if (cid == m_cid)
				{
					TempFrame f(*this, cid);
					switch (iMethod)
					{
					case 0: Shaders::Contract::Ctor(nullptr); return;
					case 1: Shaders::Contract::Dtor(nullptr); return;
					case 2: Shaders::Contract::Method_2(CastArg<Shaders::Serialize::Buffer>(pArgs)); return;
					case 3: Shaders::Contract::Method_3(nullptr); return;
					}
				}

				ProcessorContract::CallFar(cid, iMethod, pArgs, bInheritContext);
			}


			void TestContract();

			void TestAll();
		};

		template <>
		struct MyProcessor::Converter<beam::Zero_>
			:public Blob
		{
			Converter(beam::Zero_&)
			{
				p = nullptr;
				n = 0;
			}
		};

		void MyProcessor::TestAll()
		{
			AddCode(m_Code.m_Contract, "../shaders/shaders/contract.wasm");

			TestContract();
		}

		struct CidTxt
		{
			char m_szBuf[Shaders::ContractID::nBytes * 5];

			void Set(const Shaders::ContractID& x)
			{
				char* p = m_szBuf;
				for (uint32_t i = 0; i < x.nBytes; i++)
				{
					if (i)
						*p++ = ',';

					*p++ = '0';
					*p++ = 'x';

					uintBigImpl::_Print(x.m_pData + i, 1, p);
					p += 2;
				}

				assert(p - m_szBuf < (long int)_countof(m_szBuf));
				*p = 0;
			}
		};

		static void VerifyId(const ContractID& cidExp, const ContractID& cid, const char* szName)
		{
			if (cidExp != cid)
			{
				CidTxt ct;
				ct.Set(cid);

				printf("Incorrect %s. Actual value: %s\n", szName, ct.m_szBuf);
				g_TestsFailed++;
				fflush(stdout);
			}
		}

#define VERIFY_ID(exp, actual) VerifyId(exp, actual, #exp)


		void MyProcessor::TestContract()
		{
			Zero_ zero;
			verify_test(ContractCreate_T(m_cid, m_Code.m_Contract, zero));

			Shaders::Serialize::Test::Att obj;
			obj.type = 90;
			obj.name = "Name";

			yas::count_ostream cs;
			yas::binary_oarchive<yas::count_ostream, Shaders::Serialize::YAS_FLAGS> sizeCalc(cs);
			sizeCalc& obj;

			auto paramSize = sizeof(Shaders::Serialize::Buffer) + cs.total_size;
			std::vector<char> v(paramSize, '\0');
			Shaders::Serialize::Buffer* buf = reinterpret_cast<Shaders::Serialize::Buffer*>(v.data());
			buf->size = cs.total_size;

			yas::mem_ostream ms(reinterpret_cast<char*>(buf + 1), buf->size);
			yas::binary_oarchive<yas::mem_ostream, Shaders::Serialize::YAS_FLAGS> ar(ms);
			ar& obj;
			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			Blob blob;
			Shaders::Convert<true>(*buf);
			blob.p = buf;
			blob.n = paramSize;
			RunGuarded(m_cid, 2, blob, nullptr);

			//verify_test(RunGuarded_T(m_cid, 2, *buf));

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			verify_test(RunGuarded_T(m_cid, 3, zero));
			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//MyProcessor proc;
			//MyManager man(proc.m_Vars);
			//man.InitMem();
			//man.TestHeap();

			//ByteBuffer buf;
			//MyProcessor::AddCodeEx(buf, "vault/app.wasm", Processor::Kind::Manager);
			//man.m_Code = buf;

			//man.RunGuarded(0); // get scheme
			//std::cout << man.m_Out.str();
			//man.m_Out.str("");

			//man.m_Args["role"] = "manager";
			//man.m_Args["action"] = "view_accounts";
			//man.set_ArgBlob("cid", Shaders::Vault::s_CID);

			//man.RunGuarded(1);
			//std::cout << man.m_Out.str();
			//man.m_Out.str("");

		}

	} // namespace bvm2
} // namespace beam

int main()
{
	try
	{
		ECC::PseudoRandomGenerator prg;
		ECC::PseudoRandomGenerator::Scope scope(&prg);

		MyProcessor proc;

		proc.TestAll();
	}
	catch (const std::exception& ex)
	{
		printf("Expression: %s\n", ex.what());
		g_TestsFailed++;
	}

	return g_TestsFailed ? -1 : 0;
}
