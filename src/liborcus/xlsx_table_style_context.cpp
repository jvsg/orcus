/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_table_style_context.hpp"
#include "xlsx_helper.hpp"
#include "xlsx_types.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"

#include "orcus/global.hpp"
#include "orcus/exception.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/measurement.hpp"

#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>

namespace orcus {

namespace {

typedef mdds::sorted_string_map<spreadsheet::table_style_element_t> table_style_element_type_map;

table_style_element_type_map::entry table_style_element_type_entries[] =
{
    { MDDS_ASCII("blankRow"), spreadsheet::table_style_element_blank_row },
    { MDDS_ASCII("firstColumn"), spreadsheet::table_style_element_first_column },
    { MDDS_ASCII("firstColumnStripe"), spreadsheet::table_style_element_first_column_stripe },
    { MDDS_ASCII("firstColumnSubheading"), spreadsheet::table_style_element_first_column_subheading },
    { MDDS_ASCII("firstHeaderCell"), spreadsheet::table_style_element_first_header_cell },
    { MDDS_ASCII("firstRowStripe"), spreadsheet::table_style_element_first_row_stripe },
    { MDDS_ASCII("firstRowSubheading"), spreadsheet::table_style_element_first_row_subheading },
    { MDDS_ASCII("firstSubtotalColumn"), spreadsheet::table_style_element_first_subtotal_column },
    { MDDS_ASCII("firstSubtotalRow"), spreadsheet::table_style_element_first_subtotal_row },
    { MDDS_ASCII("firstTotalCell"), spreadsheet::table_style_element_first_total_cell },
    { MDDS_ASCII("headerRow"), spreadsheet::table_style_element_header_row },
    { MDDS_ASCII("lastColumn"), spreadsheet::table_style_element_last_column },
    { MDDS_ASCII("lastHeaderCell"), spreadsheet::table_style_element_last_header_cell },
    { MDDS_ASCII("lastTotalCell"), spreadsheet::table_style_element_last_total_cell },
    { MDDS_ASCII("pageFieldLabels"), spreadsheet::table_style_element_page_field_labels },
    { MDDS_ASCII("pageFieldValues"), spreadsheet::table_style_element_page_field_values },
    { MDDS_ASCII("secondColumnStripe"), spreadsheet::table_style_element_second_column_stripe },
    { MDDS_ASCII("secondColumnSubheading"), spreadsheet::table_style_element_second_column_subheading },
    { MDDS_ASCII("secondRowStripe"), spreadsheet::table_style_element_second_row_stripe },
    { MDDS_ASCII("secondRow_subheading"), spreadsheet::table_style_element_second_row_subheading },
    { MDDS_ASCII("secondSubtotalColumn"), spreadsheet::table_style_element_second_subtotal_column },
    { MDDS_ASCII("secondSubtotalRow"), spreadsheet::table_style_element_second_subtotal_row },
    { MDDS_ASCII("thirdColumnSubheading"), spreadsheet::table_style_element_third_column_subheading },
    { MDDS_ASCII("thirdRowSubtotal"), spreadsheet::table_style_element_third_row_subtotal },
    { MDDS_ASCII("thirdSubtotalColumn"), spreadsheet::table_style_element_third_subtotal_column },
    { MDDS_ASCII("thirdSubtotalRow"), spreadsheet::table_style_element_third_subtotal_row },
    { MDDS_ASCII("totalRow"), spreadsheet::table_style_element_total_row },
    { MDDS_ASCII("wholeTable"), spreadsheet::table_style_element_whole_table }
};

class table_style_element_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    table_style_element_attr_parser():
        m_type(spreadsheet::table_style_element_dont_know),
        m_size(1),
        m_dxf(0)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_type:
            {
                table_style_element_type_map type_map(table_style_element_type_entries,
                        sizeof(table_style_element_type_entries)/sizeof(table_style_element_type_entries[0]),
                        spreadsheet::table_style_element_dont_know);
                m_type = type_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_size:
                m_size = to_long(attr.value);
            break;
            case XML_dxfId:
                m_dxf = to_long(attr.value);
            break;
        }
    }

    void finalize(spreadsheet::iface::import_table_style* table_style,
            const dxf_id_map& dxf_map)
    {
        table_style->set_band_size(m_size);
        dxf_id_map::const_iterator itr = dxf_map.find(m_dxf);
        if (itr != dxf_map.end())
        {
            table_style->set_dxf_id(itr->second);
        }
        else
            table_style->set_dxf_id(m_dxf);

        table_style->set_element_type(m_type);
    }

private:

    spreadsheet::table_style_element_t m_type;
    size_t m_size;
    size_t m_dxf;
};

class table_style_attr_parser
{
public:
    table_style_attr_parser():
        m_pivot(true),
        m_table(true)
    {
    }
    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_name:
                m_name = attr.value;
            break;
            case XML_pivot:
                m_pivot = to_bool(attr.value);
            break;
            case XML_table:
                m_table = to_bool(attr.value);
            break;
        }
    }

    void finalize(spreadsheet::iface::import_table_style* table_style)
    {
        table_style->set_pivot_style(m_pivot);
        table_style->set_table_style(m_table);
        table_style->set_name(m_name.get(), m_name.size());
    }

private:
    pstring m_name;
    bool m_pivot;
    bool m_table;
};

class table_styles_attr_parser
{
public:
    table_styles_attr_parser(spreadsheet::iface::import_table_style* table_style):
        m_table_styles(table_style)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_count:
            {
                size_t count = to_long(attr.value);
                m_table_styles->set_table_style_count(count);
            }
            break;
            case XML_defaultTableStyle:
                m_table_styles->set_default_table_style(attr.value.get(),
                        attr.value.size());
            break;
            case XML_defaultPivotStyle:
                m_table_styles->set_default_pivot_style(attr.value.get(),
                        attr.value.size());
            break;
        }
    }

private:
    spreadsheet::iface::import_table_style* m_table_styles;
};

}

xlsx_table_styles_context::xlsx_table_styles_context(session_context& session_cxt, const tokens& tokens,
        spreadsheet::iface::import_table_style* import_table_styles):
    xml_context_base(session_cxt, tokens),
    m_table_styles(import_table_styles)
{
}

xlsx_table_styles_context::~xlsx_table_styles_context()
{
}

bool xlsx_table_styles_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_table_styles_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_table_styles_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_table_styles_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    switch (name)
    {
        case XML_tableStyles:
        {
            std::for_each(attrs.begin(), attrs.end(), table_styles_attr_parser(m_table_styles));
        }
        break;
        case XML_tableStyle:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_tableStyles);
            table_style_attr_parser parser = 
                std::for_each(attrs.begin(), attrs.end(), table_style_attr_parser());
            parser.finalize(m_table_styles);
        }
        break;
        case XML_tableStyleElement:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_tableStyle);
            table_style_element_attr_parser parser = 
                std::for_each(attrs.begin(), attrs.end(), table_style_element_attr_parser());
            parser.finalize(m_table_styles, m_local_to_global_dxf);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_table_styles_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_tableStyles:
        break;
        case XML_tableStyle:
            m_table_styles->commit_table_style();
        break;
        case XML_tableStyleElement:
            m_table_styles->commit_element();
        break;
        default:
        break;
    }

    return pop_stack(ns, name);
}

void xlsx_table_styles_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}
    
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
