/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_helper.hpp"

#include "string_helper.hpp"

#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>

#include <orcus/global.hpp>

namespace orcus {

namespace {

bool is_valid_hex_digit(const char& character, orcus::spreadsheet::color_elem_t& val)
{
    if ('0' <= character && character <= '9')
    {
        val += character - '0';
        return true;
    }

    if ('A' <= character && character <= 'F')
    {
        val += character - 'A' + 10;
        return true;
    }

    if ('a' <= character && character <= 'f')
    {
        val += character - 'a' + 10;
        return true;
    }

    return false;
}

// converts two characters starting at index to a color value
bool convert_color_digits(const pstring& value, orcus::spreadsheet::color_elem_t& color_val, size_t index)
{
    const char& high_val = value[index];
    color_val = 0;
    if (!is_valid_hex_digit(high_val, color_val))
        return false;
    color_val *= 16;
    const char& low_val = value[++index];
    return is_valid_hex_digit(low_val, color_val);
}

namespace {

typedef mdds::sorted_string_map<odf_helper::border_properties::odf_border_style_t> odf_border_style_map;

odf_border_style_map::entry odf_border_style_entries[] =
{
    {MDDS_ASCII("dashed"), odf_helper::border_properties::odf_border_style_t::dashed},
    {MDDS_ASCII("dotted"), odf_helper::border_properties::odf_border_style_t::dotted},
    {MDDS_ASCII("double"), odf_helper::border_properties::odf_border_style_t::double_border},
    {MDDS_ASCII("groove"), odf_helper::border_properties::odf_border_style_t::groove},
    {MDDS_ASCII("hidden"), odf_helper::border_properties::odf_border_style_t::hidden},
    {MDDS_ASCII("inset"), odf_helper::border_properties::odf_border_style_t::inset},
    {MDDS_ASCII("none"), odf_helper::border_properties::odf_border_style_t::none},
    {MDDS_ASCII("outset"), odf_helper::border_properties::odf_border_style_t::outset},
    {MDDS_ASCII("ridge"), odf_helper::border_properties::odf_border_style_t::ridge},
    {MDDS_ASCII("solid"), odf_helper::border_properties::odf_border_style_t::solid}
};

}

void parse_border_property(const pstring& property, odf_helper::border_properties& border_properties)
{
    if (property.empty())
        return;

    size_t prop_size = property.size();
    if (prop_size == 7 && property[0] == '#')
    {
        // color property
        border_properties.color_set = odf_helper::convert_fo_color(property,
                border_properties.red, border_properties.green,
                border_properties.blue);
    }
    else if (property[0] >= '0' && property[0] <= '9')
    {
        // border width
        border_properties.width_set = true;
        border_properties.width = orcus::to_length(property);
    }
    else
    {
        // assume it is the style
        border_properties.style_set = true;
        odf_border_style_map border_style_map(odf_border_style_entries, ORCUS_N_ELEMENTS(odf_border_style_entries), odf_helper::border_properties::odf_border_style_t::none);
        border_properties.border_style = border_style_map.find(property.get(), property.size());
    }
}

}

bool odf_helper::convert_fo_color(const pstring& value, orcus::spreadsheet::color_elem_t& red,
        orcus::spreadsheet::color_elem_t& green, orcus::spreadsheet::color_elem_t& blue)
{
    // first character needs to be '#'
    if (value.size() != 7)
        return false;

    if (value[0] != '#')
        return false;

    if (!convert_color_digits(value, red, 1))
        return false;

    if (!convert_color_digits(value, green, 3))
        return false;

    return convert_color_digits(value, blue, 5);
}

odf_helper::border_properties odf_helper::read_border_properties(const orcus::pstring& value)
{
    odf_helper::border_properties border_properties;

    std::vector<pstring> split_properties = string_helper::split_string(value, ' ');

    for (const pstring& property : split_properties)
        parse_border_property(property, border_properties);

    return border_properties;
}
        
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
