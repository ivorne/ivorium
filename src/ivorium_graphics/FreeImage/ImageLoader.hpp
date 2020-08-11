#pragma once

#include <iostream>
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

class StreamResource;

struct ImageLoader_Metadata
{
    int2            size;
    PixelFormat     pixel_format;
};

/**
    Name is only used optionaly to determine file type.
    All data are read from given istream.
    Given callback is not stored, it is called from within the ImageLoader_Load function call when loading succeeds.
    Logs all errors and warnings to given logger.
    Returns true if image was succesfully read and callback was called.
*/
bool ImageLoader_Load( StreamResource * resource, PixelFormat format, std::function< void( ImageLoader_Metadata const &, uint8_t * bytes, size_t bytes_count ) > const & );

}
