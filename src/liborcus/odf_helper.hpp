/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_ODF_HELPER_HPP
#define INCLUDED_ORCUS_ODF_HELPER_HPP

#include <orcus/pstring.hpp>
#include <orcus/spreadsheet/types.hpp>
#include <orcus/measurement.hpp>

namespace orcus {

class odf_helper
{

public:
    struct border_properties
    {
        enum class odf_border_style_t
        {
            none,
            hidden,
            dotted,
            dashed,
            solid,
            double_border,
            groove,
            ridge,
            inset,
            outset
        };

        spreadsheet::color_elem_t red;
        spreadsheet::color_elem_t green;
        spreadsheet::color_elem_t blue;

        bool color_set;

        odf_border_style_t border_style;
        bool style_set;

        length_t width;
        bool width_set;

        border_properties():
            color_set(false), style_set(false), width_set(false) {}
    };

public:
    static bool convert_fo_color(const orcus::pstring& value, orcus::spreadsheet::color_elem_t& red,
            orcus::spreadsheet::color_elem_t& green, orcus::spreadsheet::color_elem_t& blue);

    static border_properties read_border_properties(const orcus::pstring& value);

};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
