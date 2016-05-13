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
    static bool convert_fo_color(const orcus::pstring& value, orcus::spreadsheet::color_elem_t& red,
            orcus::spreadsheet::color_elem_t& green, orcus::spreadsheet::color_elem_t& blue);

    /* extracts border style,width and colors out of the pstring provided to it */
    static void extract_border_details(const orcus::pstring& value,length_t                           &m_border_width,
                                                                    spreadsheet::border_style_t       &m_border_style,
                                                                    spreadsheet::color_elem_t         &m_border_red,
                                                                    spreadsheet::color_elem_t         &m_border_green,
                                                                    spreadsheet::color_elem_t         &m_border_blue
										);

};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
