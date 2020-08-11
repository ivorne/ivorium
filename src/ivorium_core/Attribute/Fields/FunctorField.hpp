#pragma once

#include "../Field.hpp"
#include <functional>

namespace iv
{

template< class T >
class FunctorField : public Field< T >
{
public:
                                FunctorField( Instance * inst, std::function< void( bool real ) > const & OnChange = nullptr );
    void                        OnChange( std::function< void( bool real ) > const & );
    
    T                           Get();
    void                        Modify( T const & );
    
protected:
    virtual void                OnChanged( bool real ) override;
    
private:
    std::function< void( bool ) > change_reporter;
};

}

//================================================================

#include "FunctorField.inl"
