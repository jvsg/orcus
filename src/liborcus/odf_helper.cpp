/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_helper.hpp"
#include <sstream>
#include <orcus/spreadsheet/types.hpp>
#include <orcus/measurement.hpp>

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

bool odf_helper::extract_border_details(const orcus::pstring& value,
                                                length_t                          &m_border_width,
                                                spreadsheet::border_style_t       &m_border_style,
                                                spreadsheet::color_elem_t         &m_border_red,
                                                spreadsheet::color_elem_t         &m_border_green,
                                                spreadsheet::color_elem_t         &m_border_blue
                                        )
{   orcus::pstring color_code;
    orcus::pstring width;
    orcus::pstring style;

    std::stringstream ss(value.str());
    std::string buffer;

    std::getline(ss,buffer,' ');
    width=buffer.c_str();
    m_border_width=to_length(width);

    std::getline(ss,buffer,' ');

    if(buffer=="unknown"                 ) m_border_style =  spreadsheet::border_style_t::unknown;
    else if(buffer=="none"               ) m_border_style = spreadsheet::border_style_t::none;
    else if(buffer=="dash_dot"           ) m_border_style = spreadsheet::border_style_t::dash_dot;
    else if(buffer=="dash_dot_dot"       ) m_border_style = spreadsheet::border_style_t::dash_dot_dot;
    else if(buffer=="dashed"             ) m_border_style = spreadsheet::border_style_t::dashed;
    else if(buffer=="dotted"             ) m_border_style = spreadsheet::border_style_t::dotted;
    else if(buffer=="double_border"      ) m_border_style = spreadsheet::border_style_t::double_border;
    else if(buffer=="hair"               ) m_border_style = spreadsheet::border_style_t::hair;
    else if(buffer=="medium"             ) m_border_style = spreadsheet::border_style_t::medium;
    else if(buffer=="medium_dash_dot"    ) m_border_style = spreadsheet::border_style_t::medium_dash_dot;
    else if(buffer=="medium_dash_dot_dot") m_border_style = spreadsheet::border_style_t::medium_dash_dot_dot;
    else if(buffer=="medium_dashed"      ) m_border_style = spreadsheet::border_style_t::medium_dashed;
    else if(buffer=="slant_dash_dot"     ) m_border_style = spreadsheet::border_style_t::slant_dash_dot;
    else if(buffer=="thick"              ) m_border_style = spreadsheet::border_style_t::thick;
    else if(buffer=="thin"               ) m_border_style = spreadsheet::border_style_t::thin;
    
    std::getline(ss,buffer,' ');
    color_code=buffer.c_str();
    
    if(!convert_fo_color(color_code,m_border_red, m_border_green,m_border_blue))
        return false;
    
    return true;
}
        
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
