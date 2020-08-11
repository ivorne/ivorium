#pragma once

#include <functional>

namespace iv
{

class Instance;

/**
 *  \ingroup ComponentSystem
 */
class instance_ptr_interface
{
friend class Instance;
protected:
    virtual void    invalidate ( Instance * inst ) = 0;
    void            addSelfToSC( Instance * inst );
    void            removeSelfFromSC( Instance * inst );
};

/**
*/
class instance_ptr : public instance_ptr_interface
{
public:
                            instance_ptr    ( Instance * inst = nullptr );
                            instance_ptr    ( instance_ptr const & other );
                            ~instance_ptr   ( );

    instance_ptr &          operator =      ( instance_ptr const & other );
    instance_ptr &          operator =      ( Instance * instance );
    
    
    bool                    operator ==     ( instance_ptr const & other );
    bool                    operator !=     ( instance_ptr const & other );
    bool                    operator <      ( instance_ptr const & other );
    bool                    operator >      ( instance_ptr const & other );
    bool                    operator <=     ( instance_ptr const & other );
    bool                    operator >=     ( instance_ptr const & other );
                        
    Instance *              get             ( ) const;
    Instance &              operator *      ( ) const;
    Instance *              operator ->     ( ) const;
                            operator bool   ( ) const;

protected:
    virtual void invalidate( Instance * inst ) override;

private:
    Instance * inst;

};



/**
    Careful - there is no guarantee about where the callback might get called.
    So do as little as possible from the callback, preferably just set some dirty flag and process the change from better controlled stack.
*/
class callback_instance_ptr : public instance_ptr
{
public:
using instance_ptr::instance_ptr;

    callback_instance_ptr & operator=( callback_instance_ptr const & other );
    callback_instance_ptr & operator=( Instance * instance );
    
    void                    set_change_callback( std::function< void() > const & );
    
protected:
    virtual void            invalidate( Instance * inst ) override;
    
private:
    std::function< void() > _change_callback;
};


}

namespace std
{
    template<>
    struct hash< iv::instance_ptr >
    {
        size_t operator()( iv::instance_ptr const & v ) const
        {
            return std::hash< iv::Instance * >()( v.get() );
        }
    };
}
