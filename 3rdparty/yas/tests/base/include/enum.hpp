
// Copyright (c) 2010-2021 niXman (github dot nixman at pm dot me). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__tests__base__include__enum_hpp
#define __yas__tests__base__include__enum_hpp

/***************************************************************************/

template<typename archive_traits>
bool enum_test(std::ostream &log, const char *archive_type, const char *test_name) {
	typename archive_traits::oarchive oa1;
	archive_traits::ocreate(oa1, archive_type);

	enum enum_test_enum1 { _1_1, _1_2, _1_3, _1_4 };
	enum class enum_test_enum2: char { _2_1, _2_2, _2_3, _2_4 };

	oa1 & YAS_OBJECT_NVP("obj", ("e11", _1_2), ("e14", _1_4));

	enum_test_enum1 e11{}, e14{};
	typename archive_traits::iarchive ia1;
	archive_traits::icreate(ia1, oa1, archive_type);

	ia1 & YAS_OBJECT_NVP("obj", ("e11", e11), ("e14", e14));
	if ( e11 != _1_2 || e14 != _1_4 ) {
		YAS_TEST_REPORT(log, archive_type, test_name);
		return false;
	}

	typename archive_traits::oarchive oa2;
	archive_traits::ocreate(oa2, archive_type);
	oa2 & YAS_OBJECT_NVP(
         "obj"
        ,("e21", enum_test_enum2::_2_1)
        ,("e23", enum_test_enum2::_2_3)
	);

	enum_test_enum2 e21{}, e23{};
	typename archive_traits::iarchive ia2;
	archive_traits::icreate(ia2, oa2, archive_type);
    ia2 & YAS_OBJECT_NVP(
         "obj"
        ,("e21", e21)
        ,("e23", e23)
    );
	if ( e21 != enum_test_enum2::_2_1 || e23 != enum_test_enum2::_2_3) {
		YAS_TEST_REPORT(log, archive_type, test_name);
		return false;
	}

	return true;
}

/***************************************************************************/

#endif // __yas__tests__base__include__enum_hpp
