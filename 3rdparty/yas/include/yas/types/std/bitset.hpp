
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

#ifndef __yas__types__std__std_bitset_serializers_hpp
#define __yas__types__std__std_bitset_serializers_hpp

#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/serializer.hpp>
#include <yas/detail/io/serialization_exceptions.hpp>

#include <bitset>
#include <vector>

namespace yas {
namespace detail {

/***************************************************************************/

template<std::size_t F, std::size_t N>
struct serializer<
    type_prop::not_a_fundamental,
    ser_case::use_internal_serializer,
    F,
    std::bitset<N>
> {
    template<typename Archive>
    static Archive& save(Archive& ar, const std::bitset<N>& bits) {
        __YAS_CONSTEXPR_IF ( F & yas::json ) {
            std::vector<std::uint8_t> result((N + 7) >> 3);
            for ( std::size_t idx = 0; idx < N; ++idx ) {
                result[idx >> 3] = __YAS_SCAST(std::uint8_t, result[idx >> 3] | (bits[idx] << (idx & 7)));
            }

            ar.write("[", 1);
            ar & result;
            ar.write("]", 1);
        } else {
            ar.write_seq_size(N);
            std::vector<std::uint8_t> result((N + 7) >> 3);
            for ( std::size_t idx = 0; idx < N; ++idx ) {
                result[idx >> 3] = __YAS_SCAST(std::uint8_t, result[idx >> 3] | (bits[idx] << (idx & 7)));
            }

            ar & result;
        }

        return ar;
    }

    template<typename Archive>
    static Archive& load(Archive& ar, std::bitset<N>& bits) {
        __YAS_CONSTEXPR_IF ( F & yas::json ) {
            __YAS_THROW_IF_WRONG_JSON_CHARS(ar, "[");

            std::vector<std::uint8_t> buf;
            ar & buf;

            if ( buf.size() != ((N + 7) >> 3) ) {
                __YAS_THROW_WRONG_BITSET_STORAGE_SIZE();
            }

            for ( std::size_t idx = 0; idx < N; ++idx ) {
                bits[idx] = ((buf[idx >> 3] >> (idx & 7)) & 1);
            }

            __YAS_THROW_IF_WRONG_JSON_CHARS(ar, "]");
        }  else {
            const auto size = ar.read_seq_size();
            if ( size != N ) { __YAS_THROW_WRONG_BITSET_SIZE(); }

            std::vector<std::uint8_t> buf;
            ar & buf;

            if ( buf.size() != ((N + 7) >> 3)) {
                __YAS_THROW_WRONG_BITSET_STORAGE_SIZE();
            }

            for ( std::size_t idx = 0; idx < N; ++idx ) {
                bits[idx] = ((buf[idx >> 3] >> (idx & 7)) & 1);
            }
        }

        return ar;
    }
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // __yas__types__std__std_bitset_serializers_hpp
