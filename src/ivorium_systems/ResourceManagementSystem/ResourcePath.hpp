#pragma once

#include <string>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    This is used mainly internaly in ResourceManagementSystem and in  communication between RMS and ResourceProviders.
    This is always absolute path (must begin with '/') to the resource (typicaly file, dirs are unusual) in a single-root (no C:, D: letters) virtual file system (it is aggregated from all providers registered to ResourceManagementSystem).
    To support relative paths and paths to directories, another class (and conversions from/to ResourcePath) should be implemented (or std::path with proper conversions added can be used).
    
    TODO - make sure that this can not point above the virtual root (might hapen if path is form example "/../../..").
            - this should be verified using some platform-aware api that verifies the location of target file
            - maybe I should move IO handling to some file like this that would do verifications also
            - maybe get_neighbour_path is not the best; probably there should be more specific function "create ResourcePath from given string considering that we call it from file with this ResourcePath"
                - that would evaluate if the string is absolute or relative and either set absolute virtual path or use "current" ResourcePath to resolve relative path
*/
class ResourcePath
{
public:
                            ResourcePath();
                            ResourcePath( const char * abspath );
                            ResourcePath( std::string const & abspath );
                            
    ResourcePath &          operator=( const char * abspath );
    ResourcePath &          operator=( std::string const & abspath );
    
    ResourcePath            operator+( std::string const & append ) const;
    ResourcePath &          operator+=( std::string const & append );
    
    ResourcePath            operator/( std::string const & join ) const;
    ResourcePath &          operator/=( std::string const & join );
                            
    std::string const &     string() const;
    
    bool                    empty() const;
    
    /**
        Transforms virtual path contained in this ResourcePath into real path by joining it with given path to virtual root.
    */
    std::string             to_real_path( const char * vroot ) const;
    
    /**
        Last part of this resource path is removed and the 'relative' parameter is appended and the result is returned.
    */
    ResourcePath            get_neighbour_path( const char * relative ) const;
    
    /**
        If this ends with a '/', then it returns itself.
        If this is not a directory, it will return parent directory.
    */
    ResourcePath            get_directory() const;
    
    bool                    operator==( ResourcePath const & other ) const;
    bool                    operator!=( ResourcePath const & other ) const;
    bool                    operator<( ResourcePath const & other ) const;

    static void             Clean( std::string & path );
    
private:
    mutable std::string path;
};

inline std::ostream & operator<<( std::ostream & out, iv::ResourcePath const & path )
{
    out << path.string();
    return out;
}

class Resource_LogTrace : public LogTrace
{
public:
    Resource_LogTrace( ResourcePath const & path ) :
        path( path )
    {
    }
    
    virtual void PrintTraceLine( std::ostream & out ) override
    {
        out << "Resource '" << this->path << "':" << std::endl;
    }
    
private:
    ResourcePath path;
};

/**
 * Can be absolute or relative to instanced path.
 * We define it in this header, because ResourcePath type is one of the arguments to string conversion (so it would cause include dependency problems if it were in another header.
*/
template<>
struct StringIO< ResourcePath >
{
    ResourcePath Read( const char * name, Context const * context )
    {
        ResourcePath target_path;
        std::string filename = name;
        if( filename.size() > 0 && filename[ 0 ] == '/' )
        { // absolute path
            target_path = filename;
        }
        else
        { // relative path
            //target_path = cwd.get_neighbour_path( filename.c_str() );
            context->warning( SRC_INFO, "Reading relative paths to ResourcePath is not currently implemented." );
        }
        
        return target_path;
    }
    
    std::string Write( ResourcePath const & value, Context const * context ) const
    {
        return value.string();
    }
};

}

namespace std
{

template<>
struct hash< iv::ResourcePath >
{
    size_t operator()( iv::ResourcePath const & val ) const
    {
        return std::hash< std::string >()( val.string() );
    }
};

}
