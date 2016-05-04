#include <orcus/orcus_import_ods.hpp>

#include <orcus/spreadsheet/styles.hpp>

#include <orcus/string_pool.hpp>
#include "orcus/stream.hpp"
#include "odf_helper.hpp"
#include <cassert>
#include <iostream>
#include <orcus/spreadsheet/types.hpp>
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

}

int main()
{   orcus::string_pool string_pool;
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


    orcus::string_pool string_pool2;
    const char* path2 = SRCDIR"/test/ods/styles/improved-cell-styles.xml";
    std::string content2 = orcus::load_file_content(path2);
    orcus::spreadsheet::import_styles styles2(string_pool2);
    orcus::import_ods::read_styles(content2.c_str(), content2.size(), &styles2);

    size_t border_count = styles2.get_border_count();
    assert(border_count==3);

    const orcus::spreadsheet::border_t* border_data = styles2.get_border(2);
    orcus::spreadsheet::border_style_t fst =(*border_data).top.style;
    
    assert(fst ==orcus::spreadsheet::border_style_t::thick);

    const orcus::spreadsheet::border_t* border_data1 = styles2.get_border(1);
    orcus::spreadsheet::color_t clrs =(*border_data1).top.border_color;

    assert(int(clrs.blue)==204);

    return 0;
}
