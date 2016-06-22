/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SPREADSHEET_CONDITIONAL_FORMAT_HPP__
#define __ORCUS_SPREADSHEET_CONDITIONAL_FORMAT_HPP__

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/spreadsheet/styles.hpp"
#include "orcus/pstring.hpp"
#include "orcus/env.hpp"
#include <orcus/measurement.hpp>
#include <vector>

namespace orcus {

class string_pool;

namespace spreadsheet {

struct ORCUS_SPM_DLLPUBLIC condition_t
{
    pstring mapped_name;
    condition_operator_t condition_operator;
    conditional_format_t condition_type;
    color_t color;
    pstring formula;
    std::vector<size_t> values;

    condition_t();
    void reset();
};

class ORCUS_SPM_DLLPUBLIC import_conditional_format : public iface::import_conditional_format
{
public:
    ORCUS_DLLPUBLIC virtual ~import_conditional_format();
    import_conditional_format(string_pool& sp);

    virtual void set_color(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue);
    virtual void set_style_name(const char*  p, size_t n);
    virtual void set_values(std::vector<size_t> v);
    virtual void set_formula(const char* p, size_t n);
    virtual void set_condition_type(orcus::spreadsheet::condition_type_t type);
    virtual void set_date(orcus::spreadsheet::condition_date_t date);
    virtual size_t commit_condition();
    virtual void set_icon_name(const char* p, size_t n);
    virtual void set_databar_gradient(bool gradient);
    virtual void set_databar_axis(orcus::spreadsheet::databar_axis_t axis);
    virtual void set_databar_color_positive(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue);
    virtual void set_databar_color_negative(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue);
    virtual void set_min_databar_length(double length);
    virtual void set_max_databar_length(double length);
    virtual void set_show_value(bool show);
    virtual void set_iconset_reverse(bool reverse);
    virtual void set_xf_id(size_t xf);
    virtual void set_operator(orcus::spreadsheet::condition_operator_t operator_type);
    virtual void set_type(orcus::spreadsheet::conditional_format_t type);
    virtual void commit_entry();
    virtual void set_range(const char* p, size_t n);
    virtual void set_range(orcus::spreadsheet::row_t row_start, orcus::spreadsheet::col_t col_start,
            orcus::spreadsheet::row_t row_end, orcus::spreadsheet::col_t col_end);
    virtual void commit_format();

    const condition_t* get_conditional_format(size_t index);


private:
    string_pool& m_string_pool;
    condition_t m_cur_conditional_format;
    ::std::vector<condition_t> m_conditional_formats;
};

}}


#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
