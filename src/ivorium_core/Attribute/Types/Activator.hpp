#pragma once

#include "../../Basics/StringIO.hpp"
#include "../../Basics/StringIO_defs.hpp"

#include <tuple>

namespace iv
{

/**
    Counts number of times it was activated (typicaly a button was pressed).
    Client code will have its own counter copying this counter and therefore knowing about any new activations.
    The counter value can only increase (eventualy overflow), so do not compare using operator< but using operator!=.
*/
struct Activator
{
public:
                    Activator();
                    Activator( unsigned char initial );
    
    // relational operators just for hashing, stable ordering and identity
    bool            operator==( Activator const & other ) const;
    bool            operator!=( Activator const & other ) const;
    bool            operator<( Activator const & other ) const;
    
    //------------- activations ------------------------------------
    /**
        One activation.
    */
    Activator       MakeActivated() const;

    /**
        Get how many activations are ther in this instance compared to localy-stored reference Activator.
    */
    unsigned char   ReadActivations( Activator const & reference ) const;
    
    /**
        Read activations from this instance and then copy them into the localy-stored reference Activator.
    */
    unsigned char   CopyActivations( Activator & reference ) const;
    
    /**
        Copy one activation to reference.
    */
    bool            CopyActivation( Activator & reference ) const;
    
    //---------------------
    unsigned char   Value() const;
    
private:
    unsigned char _value;
};

/**
*/
template<>
struct StringIO< Activator >
{
    Activator Read( const char * source, Context const * context ) const
    {
        int value = StringIO< int >().Read( source, context );
        return Activator( value );
    }
    
    std::string Write( Activator const & value, Context const * context ) const
    {
        return StringIO< int >().Write( value.Value(), context );
    }
};

}
