#pragma once

#include <array>
#include <vector>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class Type, class SubType >
class SubId
{
public:
    SubId();
    SubId( SubId const & ) = delete;
    size_t idx() const;
    size_t idx_count() const;
    
private:
    size_t _idx;
};

/**
*/
class SubDirectory
{
public:
    size_t next;
};

/**
*/
class Sub_StructorFunctions
{
public:
    size_t sizeof_v;
    size_t alignof_v;
    void * (*copy_new) ( void * src );
    void * (*copy_inplace) ( void * place, void * src );
    void (*destroy_inplace) ( void * self );
    void (*destroy_delete) ( void * self );
    void (*assign_copy) ( void * self, void * src );
    void (*assign_default) ( void * self );
    //void * (*move_new) ( void * src );
    //void * (*move_inplace) ( void * place, void * src );
    //void (*assign_move) ( void * self, void * src );
};

/**
    \ingroup ModelTools
    \brief Allow other modules to have something like a private member variables in your class.
    
Class that can contain sub-data:
\code{.cpp}
    // in .hpp
    class Item
    {
    public:
        iv::Sub< Item > sub;                       //< This says that Item must have static member SubdirectoryInst of type SubDirectory.
        static iv::SubDirectory SubDirectoryInst;
    };

    // in .cpp
    iv::SubDirectory Item::SubDirectoryInst;
\endcode

Defining data that can be added:
\code{.cpp}
    // in .hpp
    struct ExtraValue
    {
        static const iv::SubId< Item, ExtraValue > Id;
        
        int extra_value;
    };

    // in .cpp
    const iv::SubId< Item, ExtraValue > ExtraValue::Id;
\endcode

Accessing data:
\code{.cpp}
    Item item;
    ExtraValue & data = item.sub[ ExtraValue::Id ];
    data.extra_value = 125;
\endcode
*/
template< class Type, size_t LocalItems = 0, size_t LocalStorage = 0 >
class Sub
{
public:
                                Sub();
                                ~Sub();
                                Sub( Sub const & src );
    Sub &                       operator=( Sub const & src );
                                
    void                        clear();        ///< Removes all content, effectively resetting it to default values.
    
    template< class SubType >
    SubType &                   operator[]( SubId< Type, SubType > const & id );
    
private:
    struct S
    {
        S() : inst( nullptr ), functions( nullptr ), dynamic( false ){}
        
        void * inst;
        Sub_StructorFunctions * functions;
        bool dynamic;
    };
    
private:
    S *                         getS( size_t idx );
    void                        copy( size_t idx, S const & s );
    
private:
    std::array< S, LocalItems > dir_local;
    std::array< char, LocalStorage > mem_local;
    std::vector< S > dir_dynamic;
    size_t static_mem_used;
};

}

#include "Sub.inl"
