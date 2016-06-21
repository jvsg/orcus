/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "orcus/spreadsheet/conditional_format.hpp"
#include "orcus/string_pool.hpp"

#include <algorithm>
#include <vector>

namespace orcus {

class string_pool;

namespace spreadsheet {

void condition_t::reset()
{
    *this = condition_t();
}

import_conditional_format::import_conditional_format(string_pool& sp) : m_string_pool(sp) {}

import_conditional_format::~import_conditional_format()
{
}

condition_t::condition_t()
{}

void import_conditional_format::set_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
	m_cur_conditional_format.color = color_t (alpha, red, green, blue);
}

void import_conditional_format::set_style_name(const char*  p, size_t n)
{
    m_cur_conditional_format.mapped_name = pstring(p, n);
}

void import_conditional_format::set_formula(const char* p, size_t n)
{
	m_cur_conditional_format.formula = pstring(p, n);
}

void import_conditional_format::set_condition_type(orcus::spreadsheet::condition_type_t type) {}

void import_conditional_format::set_operator(orcus::spreadsheet::condition_operator_t operator_type)
{
	m_cur_conditional_format.condition_operator = operator_type;
}

size_t import_conditional_format::commit_condition()
{
    m_conditional_formats.push_back(m_cur_conditional_format);
    m_cur_conditional_format.reset();
    return m_conditional_formats.size() - 1;
}

void import_conditional_format::set_type(orcus::spreadsheet::conditional_format_t type)
{
    m_cur_conditional_format.condition_type = type;
}

void import_conditional_format::set_values(std::vector<size_t> v)
{
    m_cur_conditional_format.values = v;
}

void import_conditional_format::set_date(orcus::spreadsheet::condition_date_t) {}
void import_conditional_format::set_icon_name(const char* p, size_t n) {}

void import_conditional_format::set_databar_gradient(bool gradient) {}
void import_conditional_format::set_databar_axis(orcus::spreadsheet::databar_axis_t axis) {}
void import_conditional_format::set_databar_color_positive(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue) {}
void import_conditional_format::set_databar_color_negative(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue) {}
void import_conditional_format::set_min_databar_length(double length) {}
void import_conditional_format::set_max_databar_length(double length) {}
void import_conditional_format::set_show_value(bool show) {}
void import_conditional_format::set_iconset_reverse(bool reverse) {}
void import_conditional_format::set_xf_id(size_t xf) {}
void import_conditional_format::commit_entry() {}
void import_conditional_format::set_range(const char* p, size_t n) {}
void import_conditional_format::set_range(orcus::spreadsheet::row_t row_start, orcus::spreadsheet::col_t col_start,
            orcus::spreadsheet::row_t row_end, orcus::spreadsheet::col_t col_end) {}
void import_conditional_format::commit_format() {}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
