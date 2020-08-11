#include "ResourcePath.hpp"
#include "../fs.hpp"

#include <optional>

namespace iv
{

void ResourcePath::Clean( std::string & path )
{
    int from = 0;
    int to = 0;
    bool keep_root_slash = path.size() > 0 && path[ 0 ] == '/';
    
    std::optional< size_t > dots = 0;
    
    while( true )
    {
        if( path[ from ] == '.' )
        {
            if( dots.has_value() )
                dots = dots.value() + 1;
        }
        else if( path[ from ] == '/' || from == path.size() )
        {
            if( dots.has_value() )
            {
                if( dots.value() == 0 )
                {
                    if( !keep_root_slash || to > 0 )
                        to--;
                }
                else if( dots.value() == 1 )
                {
                    to -= 2;
                }
                else if( dots.value() == 2 )
                {
                    // trace back until I locate previous descriptor and move 'to' to it
                    int newto = to - 1;
                    size_t slash_cnt = 0;
                    
                    while( true )
                    {
                        if( ( newto == -1 && path[ 0 ] != '/' ) || path[ newto ] == '/' )
                        {
                            slash_cnt++;
                            if( slash_cnt == 2 )
                            {
                                to = newto;
                                break;
                            }
                        }
                        
                        if( newto == -1 )
                            break;
                        
                        newto--;
                    }
                }
            }
            
            // next path fraction
            dots = 0;
        }
        else
        {
            dots = std::nullopt;
        }
        
        //
        if( from == path.size() )
        {
            if( path.size() && path[ 0 ] == '/' )
                to = std::max( to, 1 );
            
            path.resize( std::max( to, 0 ) );
            break;
        }
        
        // next iteration
        if( to >= 0 )
            path[ to ] = path[ from ];
        to++;
        from++;
    }
}

ResourcePath::ResourcePath() :
    path("")
{
}
    
ResourcePath::ResourcePath( const char * path ) :
    //path( ( memset( this, 0, sizeof( ResourcePath ) ), std::string( "/" ) + path ) )
    path( std::string( "/" ) + path )
{
}

ResourcePath::ResourcePath( std::string const & path ) :
    //path( ( memset( this, 0, sizeof( ResourcePath ) ), std::string( "/" ) + path ) )
    path( std::string( "/" ) + path )
{
}

ResourcePath & ResourcePath::operator=( std::string const & path )
{
    this->path = std::string( "/" ) + path;
    return *this;
}

ResourcePath & ResourcePath::operator=( const char * path )
{
    this->path = std::string( "/" ) + path;
    return *this;
}

ResourcePath ResourcePath::operator+( std::string const & append ) const
{
    return ResourcePath( this->path + append );
}

ResourcePath & ResourcePath::operator+=( std::string const & append )
{
    this->path += append;
    return *this;
}

ResourcePath ResourcePath::operator/( std::string const & join ) const
{
    ResourcePath res( this->path + "/" + join );
    return res;
}

ResourcePath & ResourcePath::operator/=( std::string const & join )
{
    this->path += "/";
    this->path += join;
    return *this;
}

std::string const & ResourcePath::string() const
{
    ResourcePath::Clean( this->path );
    return this->path;
}

bool ResourcePath::empty() const
{
    return (*this) == ResourcePath();
}

std::string ResourcePath::to_real_path( const char * vroot ) const
{
    std::string result = vroot;
    
    if( result.size() == 0 )
        result = ".";
    
    result = result + "/" + this->path;
    
    ResourcePath::Clean( result );
    
    
#if IV_CONFIG_FS_ENABLED
    fs::path absolute = fs::absolute( fs::path( result ) );
    return absolute.string();
#else
    return result;
#endif
}

bool ResourcePath::operator==( ResourcePath const & other ) const
{
    ResourcePath::Clean( this->path );
    ResourcePath::Clean( other.path );
    return this->path == other.path;
}

bool ResourcePath::operator!=( ResourcePath const & other ) const
{
    ResourcePath::Clean( this->path );
    ResourcePath::Clean( other.path );
    return this->path != other.path;
}

bool ResourcePath::operator<( ResourcePath const & other ) const
{
    ResourcePath::Clean( this->path );
    ResourcePath::Clean( other.path );
    return this->path < other.path;
}

ResourcePath ResourcePath::get_neighbour_path( const char * relative ) const
{
    return (*this) / ".." / relative;
}

ResourcePath ResourcePath::get_directory() const
{
    if( this->path.size() == 0 )
        return *this;
    
    if( this->path[ this->path.size() - 1 ] == '/' )
        return *this;
    
    return (*this) / "..";
}

}
