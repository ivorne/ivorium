#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <typeindex>

#include "static_warning.hpp"
#include "hash.hpp"

namespace iv
{

class Context;

namespace runtime_id
{

//------------------------------ RuntimeId ------------------------------------
class Dictionary
{
public:
    static const constexpr int InvalidRuntimeId = 0;
                    
                    Dictionary();
    int             create( const char * persistent_name, char const * type_name );
    
    std::string     runtime_to_persistent( int runtime );
    int             persistent_to_runtime( const char * persistent );
    int             runtime_validate( int runtime );
    size_t          ids_count();    ///< returns value bigger than all of currently registered runtime ids
    void            lock();
    
private:
    int next_free_runtime_id;
    bool locked;
    std::unordered_map< std::string, int > p2r;
    std::unordered_map< int, std::string > r2p;
};

}

//-----------------------------------------------------------------------------
/**
    This must be allocated in nulled memory (static memor).
*/
struct RuntimeIdDictionary
{
public:
    runtime_id::Dictionary *         dict();

private:
    runtime_id::Dictionary *         _dict;
};

//--------------------------------- RuntimeId ---------------------------------
/**
    Specialization example:
        in .hpp:
            class AbilityInfoId : public RuntimeId< AbilityInfoId >
            {
            public:
                static constexpr char TypeName[] = "AbilityInfoId";
                using RuntimeId< AbilityInfoId >::RuntimeId;
                static RuntimeIdDictionary Dictionary;
            };
            
        in .cpp:
            RuntimeIdDictionary AbilityInfoId::Dictionary;

*/
template< class Me >
class RuntimeId
{
public:
    // type control - should be overloaded in type Me
    static constexpr char   TypeName[] = "Unknown";
    
    // instance control
                            RuntimeId();
    explicit                RuntimeId( int runtime );
    explicit                RuntimeId( const char * persistent );                       ///< No warning if conversion fails.
    explicit                RuntimeId( const char * persistent, Context * context );    ///< Emmits warning if conversion fails.
    
    static Me               create( const char * persistent_name );                     ///< If RuntimeId was already locked, this will emmit runtime_warning.
    static void             lock();                                                     ///< This tells us that something used ids_count for something that will not be changed, so that functionality of newer ids might be incomplete. Creating new ids after locking the RuntimeId will emmit runtime_warning.
    static size_t           ids_count();                                                ///< How many ids exist, including invalid id 0. Min id is ids_count - 1. All ids between 0 and ids_count are valid ids.
    
    // Relation operators, defined mostly for use in structured containers, more can be added if required.
    bool                    operator==( Me const & other ) const;
    bool                    operator!=( Me const & other ) const;
    bool                    operator<( Me const & other ) const;
    
    // value access
    int                     runtime_value() const;
    std::string             persistent_value() const;
    bool                    valid() const;                  ///< Same result as comparing it to default value, eg `myid != MyRuntimeId()`.
    
private:
    static runtime_id::Dictionary * dict();
    int _runtime_value;
};

// used internally by RuntimeId
void RuntimeId_WarningIdDoesNotExist( Context * context, char const * type_name, char const * persistent_name );

template< class Me >
struct hash< Me, std::enable_if_t< std::is_base_of< iv::RuntimeId< Me >, Me >::value > >
{
    size_t operator()( Me const & val ) const
    {
        return std::hash< int >()( val.runtime_value() );
    }
};

}

template< class Me >
std::ostream & operator<<( std::ostream & out, iv::RuntimeId< Me > const & id );


#include "RuntimeId.inl"
