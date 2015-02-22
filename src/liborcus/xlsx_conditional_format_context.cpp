/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_conditional_format_context.hpp"
#include "xlsx_helper.hpp"
#include "xlsx_types.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"

#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <mdds/sorted_string_map.hpp>

namespace orcus {

namespace {

class color_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_rgb:
            {
                to_rgb(attr.value, m_alpha, m_red, m_green, m_blue);
            }
            break;
            default:
            break;
        }
    }

    void get_colors(spreadsheet::color_elem_t& alpha, spreadsheet::color_elem_t& red,
            spreadsheet::color_elem_t& green, spreadsheet::color_elem_t& blue)
    {
        alpha = m_alpha;
        red = m_red;
        green = m_green;
        blue = m_blue;
    }

private:
    spreadsheet::color_elem_t m_alpha;
    spreadsheet::color_elem_t m_red;
    spreadsheet::color_elem_t m_green;
    spreadsheet::color_elem_t m_blue;
};

enum xlsx_cond_format_type
{
    none = 0,
    expression,
    cellIs,
    colorScale,
    dataBar,
    iconSet,
    top10,
    uniqueValues,
    duplicateValues,
    containsText,
    notContainsText,
    beginsWith,
    endsWith,
    containsBlanks,
    notContainsBlanks,
    containsErrors,
    notContainsErrors,
    timePeriod,
    aboveAverage
};

enum xlsx_cond_format_boolean
{
    boolean_default = 0,
    boolean_true,
    boolean_false
};

typedef mdds::sorted_string_map<xlsx_cond_format_type> cond_format_type_map;

typedef mdds::sorted_string_map<xlsx_cond_format_boolean> cond_format_boolean_map;

cond_format_type_map::entry cond_format_type_entries[] =
{
    { ORCUS_ASCII("expression"), expression },
    { ORCUS_ASCII("cellIs"), cellIs },
    { ORCUS_ASCII("colorScale"), colorScale },
    { ORCUS_ASCII("dataBar"), dataBar },
    { ORCUS_ASCII("iconSet"), iconSet },
    { ORCUS_ASCII("top10"), top10 },
    { ORCUS_ASCII("uniqueValues"), uniqueValues },
    { ORCUS_ASCII("duplicateValues"), duplicateValues },
    { ORCUS_ASCII("containsText"), containsText },
    { ORCUS_ASCII("notContainsText"), notContainsText },
    { ORCUS_ASCII("beginsWith"), beginsWith },
    { ORCUS_ASCII("endsWith"), endsWith },
    { ORCUS_ASCII("containsBlanks"), containsBlanks },
    { ORCUS_ASCII("containsErrors"), containsErrors },
    { ORCUS_ASCII("notContainsErrors"), notContainsErrors },
    { ORCUS_ASCII("timePeriod"), timePeriod },
    { ORCUS_ASCII("aboveAverage"), aboveAverage },
};

cond_format_boolean_map::entry cond_format_boolean_entries[] =
{
    { ORCUS_ASCII("0"), boolean_false },
    { ORCUS_ASCII("1"), boolean_true },
    { ORCUS_ASCII("true"), boolean_false },
    { ORCUS_ASCII("false"), boolean_true }
};

bool parse_boolean_flag(const xml_token_attr_t& attr, bool default_value)
{
    static const cond_format_boolean_map boolean_map(cond_format_boolean_entries, sizeof(cond_format_boolean_entries)/sizeof(cond_format_boolean_entries[0]), boolean_default);    
    xlsx_cond_format_boolean val = boolean_map.find(attr.value.get(), attr.value.size());
    switch (val)
    {
        case boolean_default:
            return default_value;
        break;
        case boolean_true:
            return true;
        break;
        case boolean_false:
            return false;
        break;
    }
}

class cfRule_attr_parser : public std::unary_function<xml_token_attr_t, void>
{

    cfRule_attr_parser(spreadsheet::iface::import_conditional_format& cond_format):
        m_cond_format(cond_format),
        m_above_average(true),
        m_percent(false),
        m_bottom(false)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_type:
            {
                cond_format_type_map type_map(cond_format_type_entries, sizeof(cond_format_type_entries)/sizeof(cond_format_type_entries[0]), none);
                m_type = type_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_dxfId:
            break;
            case XML_aboveAverage:
            {
                m_above_average = parse_boolean_flag(attr, true);
            }
            break;
            case XML_percent:
                m_percent = parse_boolean_flag(attr, false);
            break;
            case XML_bottom:
                m_bottom = parse_boolean_flag(attr, false);
            break;
            case XML_operator:
            break;
            case XML_text:
            break;
            case XML_timePeriod:
            break;
            case XML_rank:
            break;
            case XML_stdDev:
            break;
            case XML_equalAverage:
            {

            }
            break;

            default:
                break;
        }
    }

private:
    spreadsheet::iface::import_conditional_format& m_cond_format;
    xlsx_cond_format_type m_type;
    bool m_above_average;
    bool m_percent;
    bool m_bottom;
};

}

xlsx_conditional_format_context::xlsx_conditional_format_context(
        session_context& session_cxt, const tokens& tokens,
        spreadsheet::iface::import_conditional_format& import_cond_format):
    xml_context_base(session_cxt, tokens),
    m_cond_format(import_cond_format)
{
}

xlsx_conditional_format_context::~xlsx_conditional_format_context()
{
}

bool xlsx_conditional_format_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_conditional_format_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_conditional_format_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_conditional_format_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    switch (name)
    {
        case XML_conditionalFormatting:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_worksheet);
        break;
        case XML_cfRule:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_conditionalFormatting);
        }
        break;
        case XML_cfvo:
        break;
        case XML_dataBar:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cfRule);
            m_cond_format.set_type(spreadsheet::conditional_format_databar);
        }
        break;
        case XML_iconSet:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cfRule);
            m_cond_format.set_type(spreadsheet::conditional_format_iconset);
        }
        break;
        case XML_color:
        {
            color_attr_parser func = for_each(attrs.begin(), attrs.end(), color_attr_parser());
            spreadsheet::color_elem_t alpha, red, green, blue;
            func.get_colors(alpha, red, green, blue);
            m_cond_format.set_color(alpha, red, green, blue);
        }
        break;
        case XML_formula:
        break;
        default:
            warn_unhandled();
    }

}

bool xlsx_conditional_format_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_conditionalFormatting:
        {
            m_cond_format.commit_format();
        }
        break;
        case XML_cfRule:
        {
            m_cond_format.commit_entry();
        }
        break;
        case XML_cfvo:
        {
            m_cond_format.commit_condition();
        }
        break;
        case XML_color:
        break;
        case XML_formula:
        {
            m_cond_format.set_formula(m_cur_str.get(), m_cur_str.size());
            m_cond_format.commit_condition();
        }
        break;
        default:
            ;
    }

    m_cur_str.clear();
    return pop_stack(ns, name);
}

void xlsx_conditional_format_context::characters(const pstring& str, bool transient)
{
    m_cur_str = str;
    if (transient)
        m_cur_str = m_pool.intern(m_cur_str).first;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
