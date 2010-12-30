/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __ORCUS_ODSCONTEXT_HPP__
#define __ORCUS_ODSCONTEXT_HPP__

#include "xml_context.hpp"
#include "model/sheet.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

class ods_content_xml_context : public xml_context_base
{
public:
    struct row_attr
    {
        uint32_t number_rows_repeated;
        row_attr();
    };

    struct cell_attr
    {
        uint32_t number_columns_repeated;
        cell_attr();
    };

    ods_content_xml_context(const tokens& tokens);
    virtual ~ods_content_xml_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

    void print_html(const ::std::string& filepath) const;

private:
    void start_table(const xml_attrs_t& attrs, const xml_token_pair_t& parent);
    void end_table();

    void start_column(const xml_attrs_t& attrs, const xml_token_pair_t& parent);
    void end_column();

    void start_row(const xml_attrs_t& attrs, const xml_token_pair_t& parent);
    void end_row();

    void start_cell(const xml_attrs_t& attrs, const xml_token_pair_t& parent);
    void end_cell();

private:
    ::boost::ptr_vector<model::sheet> m_tables;

    row_attr    m_row_attr;
    cell_attr   m_cell_attr;

    uint32_t m_row;
    uint32_t m_col;
    pstring m_para_content; // store the content of current <text:p> element.
};

}

#endif