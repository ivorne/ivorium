#include "ImageLoader.hpp"
#include "StreamTexture.hpp"
#include <FreeImage.h>

namespace iv
{

static unsigned l_ReadProc( void *buffer, unsigned size, unsigned count, fi_handle handle )
{
    std::istream * in = (std::istream *)handle;
    in->read( (char*)buffer, size * count );
    return in->gcount();
}

static unsigned l_WriteProc( void *buffer, unsigned size, unsigned count, fi_handle handle )
{
    runtime_warning( SRC_INFO, "Writing to input stream is not supported." );
    return 0;
}

static int l_SeekProc( fi_handle handle, long offset, int origin )
{
    std::ios_base::seekdir way;
    if( origin == SEEK_SET )
        way = std::ios_base::beg;
    else if( origin == SEEK_CUR )
        way = std::ios_base::cur;
    else
        way = std::ios_base::end;
    
    std::istream * in = (std::istream *)handle;
    in->seekg( offset, way );
    return 0;
}

static long l_TellProc( fi_handle handle )
{
    std::istream * in = (std::istream *)handle;
    return in->tellg();
}

static FreeImageIO l_IStreamIO = 
{
    .read_proc = l_ReadProc,
    .write_proc = l_WriteProc,
    .seek_proc = l_SeekProc,
    .tell_proc = l_TellProc
};

bool ImageLoader_Load( StreamResource * resource, PixelFormat, std::function< void( ImageLoader_Metadata const &, uint8_t * bytes, size_t bytes_count ) > const & f )
{
    // determine type
    FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
    resource->with_resource_stream( [&]( std::istream & in )
    {
        image_format = FreeImage_GetFileTypeFromHandle( &l_IStreamIO, &in, 0 );
        if( image_format == FIF_UNKNOWN )
            image_format = FreeImage_GetFIFFromFilename( resource->resource_path().string().c_str() );
    } );
    
    if( image_format == FIF_UNKNOWN )
    {
        resource->cm.log( SRC_INFO, iv::Defs::Log::Warning, "Can not determine image format." );
        return false;
    }
    
    FIBITMAP * bitmap = nullptr;
    resource->with_resource_stream( [&]( std::istream & in )
    {
        bitmap = FreeImage_LoadFromHandle( image_format, &l_IStreamIO, &in, 0 );
    } );
    
    if( bitmap != nullptr )
    {
        // decide on format
        PixelFormat format;
        #if defined( FREEIMAGE_BIGENDIAN ) || FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
            format = PixelFormat::RGBA;
        #else
            format = PixelFormat::BGRA;
        #endif
        
        // convert to suitable format
        if( format == PixelFormat::RGBA || format == PixelFormat::BGRA )
        {
            if( FreeImage_GetImageType( bitmap ) != FIT_BITMAP || FreeImage_GetBPP( bitmap ) != 32 )
            {
                resource->cm.log( SRC_INFO, iv::Defs::Log::Performance, "Converting image to PixelFormat::RGBA or PixelFormat::BGRA." );
                FIBITMAP * old_bitmap = bitmap;
                bitmap = FreeImage_ConvertTo32Bits( bitmap );
                FreeImage_Unload( old_bitmap );     /// ???? Should it be unloaded or is it automatically unloaded during conversion?
            }
        }
        else
        {
            resource->cm.log( SRC_INFO, iv::Defs::Log::Warning, "PixelFormat ", format," is not supported in ImageLoader implementation." );
        }
        
        // extract metadata
        ImageLoader_Metadata metadata;
        metadata.size.x = FreeImage_GetWidth( bitmap );
        metadata.size.y = FreeImage_GetHeight( bitmap );
        metadata.pixel_format = format;
        int bpp = FreeImage_GetBPP( bitmap ) / 8;
        
        // report
        f( metadata, FreeImage_GetBits( bitmap ), metadata.size.x * metadata.size.y * bpp );
        
        // clean up
        FreeImage_Unload( bitmap );
    }
    
    return false;
}

}
