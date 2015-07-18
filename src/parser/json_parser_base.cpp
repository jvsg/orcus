/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_parser_base.hpp"
#include "orcus/global.hpp"
#include "orcus/cell_buffer.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace orcus { namespace json {

escape_char_t get_escape_char_type(char c)
{
    switch (c)
    {
        case '"':
        case '\\':
        case '/':
            return escape_char_t::legal;
        case 'b': // backspace
        case 'f': // formfeed
        case 'n': // newline
        case 'r': // carriage return
        case 't': // horizontal tab
            return escape_char_t::control_char;
        default:
            ;
    }

    return escape_char_t::illegal;
}

parse_error::parse_error(const std::string& msg) : ::orcus::parse_error(msg) {}

void parse_error::throw_with(const char* msg_before, char c, const char* msg_after)
{
    throw parse_error(build_message(msg_before, c, msg_after));
}

void parse_error::throw_with(
    const char* msg_before, const char* p, size_t n, const char* msg_after)
{
    throw parse_error(build_message(msg_before, p, n, msg_after));
}

struct parser_base::impl
{
    cell_buffer m_buffer;
};

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(make_unique<impl>()) {}

parser_base::~parser_base() {}

void parser_base::parse_true()
{
    static const char* expected = "true";
    if (!parse_expected(expected))
        throw parse_error("parse_true: boolean 'true' expected.");

    skip_blanks();
}

void parser_base::parse_false()
{
    static const char* expected = "false";
    if (!parse_expected(expected))
        throw parse_error("parse_false: boolean 'false' expected.");

    skip_blanks();
}

void parser_base::parse_null()
{
    static const char* expected = "null";
    if (!parse_expected(expected))
        throw parse_error("parse_null: null expected.");

    skip_blanks();
}

long parser_base::parse_long_or_throw()
{
    const char* p = mp_char;
    long v = parse_integer(p, remaining_size());
    if (p == mp_char)
        throw parse_error("parse_integer_or_throw: failed to parse long integer value.");

    m_pos += p - mp_char;
    mp_char = p;
    return v;
}

double parser_base::parse_double_or_throw()
{
    double v = parse_double();
    if (std::isnan(v))
        throw parse_error("parse_double_or_throw: failed to parse double precision value.");
    return v;
}

void parser_base::skip_blanks()
{
    skip(" \t\n\r");
}

void parser_base::reset_buffer()
{
    mp_impl->m_buffer.reset();
}

void parser_base::append_to_buffer(const char* p, size_t n)
{
    if (!p || !n)
        return;

    mp_impl->m_buffer.append(p, n);
}

const char* parser_base::get_buffer() const
{
    return mp_impl->m_buffer.get();
}

size_t parser_base::get_buffer_size() const
{
    return mp_impl->m_buffer.size();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
