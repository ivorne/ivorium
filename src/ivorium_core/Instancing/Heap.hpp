#pragma once

#include <vector>
#include "I.hpp"
#include "../Basics/static_warning.hpp"

namespace iv
{

/**
    All instances created by Heap will be deleted in Heap destructor.
*/
class Heap
{
public:
                    /**
                        \p inst
                            Default instance that will be used as parent when creating new instances using Heap::createInst.
                            If this is nullptr, then createInst should not be used.
                        \p cm
                            Client that will be parent of new clients created with Heap::createClient.
                            This client should be client in Instance given as first parameter.
                    */
                    Heap( Instance * inst = nullptr, ClientMarker * cm = nullptr );
                    ~Heap();
    
    /**
        Deletes any object created by this Heap, otherwise emmits warning.
        Works as operator delete, so for example polymorphic objects can be destroyed using typed pointer to its parent.
    */
    template< class T >
    void            destroy( T * );
    
    /**
        Deletes everything in reverse order than in which it was created.
    */
    void            clear();
    
    /**
        Creates instance of T using operator new and constructor T::T( CArgs const & );
    */
    template< class T, class ... CArgs >
    T *             create( CArgs const & ... );
    
    /**
        Asserts that Instance passed to constructor is not nullptr.
        Creates child instance with given name and creates root client for it.
        
        Type T must comply with Client concept:
                            T::T( Instance *, Cargs ... );
            Instance *      T::instance() const;
            ClientMarker    T::cm;
    */
    template< class T, class ... CArgs >
    T *             createInstance( std::string const & name, CArgs const & ... );
    
    /**
        Asserts that both Instance and ClientMarker passed to constructor are not nullptr.
        Creates client in the Instance and adds it as child to given ClientMarker.
        
        Type T must comply with Client concept:
                            T::T( Instance *, Cargs ... );
            Instance *      T::instance() const;
            ClientMarker    T::cm;
    */
    template< class T, class ... CArgs >
    T *             createClient( CArgs const & ... );
    
private:
    using Deleter = void(*)(void*);
    
    struct Item
    {
        void * identity;
        Deleter deleter;
        
        Item() : identity( nullptr ), deleter( nullptr ){}
        Item( void * i, Deleter d ) : identity( i ), deleter( d ){}
    };
    
    template< class T >
    static void DeleterImpl( void * ptr );
    
    template< class T, std::enable_if_t< std::is_polymorphic_v< T >, int > = 0 >
    static void * identity_cast( T * ptr );
    
    template< class T, std::enable_if_t< !std::is_polymorphic_v< T >, int > = 0 >
    static void * identity_cast( T * ptr );
    
private:
    Instance * inst;
    ClientMarker * cm;
    std::vector< Item > items;
};

}

#include "Heap.inl"
