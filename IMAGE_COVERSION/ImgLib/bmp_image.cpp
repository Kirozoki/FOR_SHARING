#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    char b = 'B', m = 'M';
    uint32_t img_sum_size; // stride x height + headers
    uint32_t reserve_space = 0; // by 0
    uint32_t img_begin = 54; // after 
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t info_header_size = 40;
    uint32_t img_width;
    uint32_t img_height;
    uint16_t planes_amount = 1;
    uint16_t pxl_bits = 24;
    uint32_t compress = 0;
    uint32_t img_bytes; // stride x height
    uint32_t hor_resolution = 11811;
    uint32_t ver_resolution = 11811;
    uint32_t used_colors = 0;
    uint32_t valued_colors = 0x1000000;
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию - не работает!!!
bool SaveBMP(const Path& file, const Image& image) {
    ofstream out(file, ios::binary);
    if (!out) {
        return false;
    }

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    if (image.GetHeight() == 0) {
        file_header.img_sum_size = 54;
        info_header.img_width, info_header.img_height, info_header.img_bytes = 0;
        out.write((const char*)&file_header, sizeof(file_header));
        out.write((const char*)&info_header, sizeof(info_header));
        out.close();
        return out.good();
    } 
    
    const int image_w = image.GetWidth();
    const int image_h = image.GetHeight();
    int stride_s = GetBMPStride(image_w);

    file_header.img_sum_size = stride_s * image_h + 54;
    info_header.img_width = image_w;
    info_header.img_height = image_h;
    info_header.img_bytes = stride_s * image_h;

    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    vector<char> buffer(stride_s);

    for (int i = image_h; i > 0; --i) {
        const Color* line = image.GetLine(i - 1);
        for (int j = 0; j < image_w; ++j) {
            buffer[j * 3] = static_cast<char>(line[j].b);
            buffer[j * 3 + 1] = static_cast<char>(line[j].g);
            buffer[j * 3 + 2] = static_cast<char>(line[j].r);
        }
        out.write(buffer.data(), buffer.size());
    }

    out.close();

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream in(file, ios::binary);
    if (!in) {
        return Image();
    }

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    if (!in.read((char*)(&file_header), sizeof(file_header)) 
        || file_header.b != 'B' || file_header.m != 'M') {
        return Image();
    }
    if (!in.read((char*)(&info_header), sizeof(info_header))
        || info_header.img_height == 0) {
        return Image();
    }
    
    int stride = info_header.img_bytes / info_header.img_height;
    Image result_img(stride / 3, info_header.img_height, img_lib::Color::Black());
    vector<char> buffer(stride);

    for (int i = info_header.img_height; i > 0 ; --i ) {
        Color* line = result_img.GetLine(i - 1);
        in.read(buffer.data(), buffer.size());

        for (int j = 0; j < stride / 3; ++j) {
            line[j].b = static_cast<byte>(buffer[3 * j]);
            line[j].g = static_cast<byte>(buffer[3 * j + 1]);
            line[j].r = static_cast<byte>(buffer[3 * j + 2]);
        }
    }
    return result_img;
}

}  // namespace img_lib