#include <orcus/orcus_import_ods.hpp>

#include <orcus/spreadsheet/styles.hpp>

#include <orcus/string_pool.hpp>
#include "orcus/stream.hpp"

#include <cassert>
#include <iostream>

namespace {

const orcus::spreadsheet::cell_style_t* find_cell_style_by_name(const orcus::pstring& name, orcus::spreadsheet::import_styles* styles)
{
    size_t n = styles->get_cell_styles_count();
    for (size_t i = 0; i < n; ++i)
    {
        const orcus::spreadsheet::cell_style_t* cur_style = styles->get_cell_style(i);
        if (cur_style->name == name)
            return cur_style;
    }

    assert(false);
    return NULL;
}

void check_border(const orcus::spreadsheet::border_attrs_t& border, orcus::spreadsheet::color_elem_t red,
        orcus::spreadsheet::color_elem_t green, orcus::spreadsheet::color_elem_t blue, orcus::spreadsheet::color_elem_t alpha)
{
    assert(border.border_color.red == red);
    assert(border.border_color.green == green);
    assert(border.border_color.blue == blue);
    assert(border.border_color.alpha == alpha);
}

}

int main()
{
    orcus::string_pool string_pool;
    const char* path = SRCDIR"/test/ods/styles/cell-styles.xml";
    std::string content = orcus::load_file_content(path);
    orcus::spreadsheet::import_styles styles(string_pool);
    orcus::import_ods::read_styles(content.c_str(), content.size(), &styles);


    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Note", &styles);
    assert(style->parent_name == "Text");
    size_t xf = style->xf;
    std::cerr << std::hex << (int)xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);
    size_t fill = cell_format->fill;
    std::cerr << std::hex << (int)fill;
    const orcus::spreadsheet::fill_t* cell_fill = styles.get_fill(fill);
    assert(cell_fill);
    std::cerr << std::hex << (int)cell_fill->bg_color.red;
    assert(cell_fill->bg_color.red == 0xfe);
    assert(cell_fill->bg_color.green == 0xff);
    assert(cell_fill->bg_color.blue == 0xcc);

    size_t border = cell_format->border;
    std::cerr << border << std::endl;
    const orcus::spreadsheet::border_t* cell_border = styles.get_border(border);
    assert(cell_border);
    const orcus::spreadsheet::border_attrs_t& top = cell_border->top;
    check_border(top, 0x12, 0xea, 0x4b, 0);
    const orcus::spreadsheet::border_attrs_t& bottom = cell_border->bottom;
    check_border(bottom, 0x12, 0xea, 0x4b, 0);
    const orcus::spreadsheet::border_attrs_t& right = cell_border->right;
    check_border(right, 0x12, 0xea, 0x4b, 0);
    const orcus::spreadsheet::border_attrs_t& left = cell_border->left;
    check_border(left, 0x12, 0xea, 0x4b, 0);
    return 0;
}
