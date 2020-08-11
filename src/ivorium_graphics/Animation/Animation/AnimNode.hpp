#pragma once

#include "AnimConnector.hpp"
#include "../../Defs.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

extern TableId AnimNode_DebugTable;
class AnimSystem;

/**
*/
class AnimNodeI
{
public:
ClientMarker cm;

                                AnimNodeI( Instance * inst );
                                ~AnimNodeI();
                            
    Instance *                  instance() const;
    void                        status( TableDebugView * view );
    
    /**
        Label for the value that is stored in this AnimNode.
    */
    AnimNodeI *                 label( std::string const & );
    std::string const &         label() const;
    
    /**
    */
    virtual void                StringIO_SetTarget( const char * source, Context const * context ){}
    virtual std::string         StringIO_GetTarget( Context const * context ){ return ""; }
    
//------------------------- anim graph ----------------------------------
    /**
    */
    void                        anim_setParent( AnimConnector * );
    void                        anim_unsetParent( AnimConnector * );
    AnimConnector *             anim_getParent();
    
    /**
    */
    void                        anim_addChild( AnimConnector * );
    void                        anim_removeChild( AnimConnector * );
    void                        anim_eachChild( std::function< void( AnimConnector * ) > const & );
    
//------------------------- update algorithm ----------------------------------------

    /**
        Called from AnimSystem before connectors start updating the node.
    */
    void                        System_UpdateStart();
    
    /**
        Called after updates on node finished.
    */
    void                        System_UpdateEnd();
    
    /**
        Returns value if node is between System_UpdateStart and System_UpdateEnd calls and returns update_id of AnimSystem when update started.
    */
    std::optional< unsigned >   System_UpdateId() const;
    
    /**
        Connector requests this child node to be updated during AnimConnector::UpdatePass_Down.
    */
    void                        Connector_RequestStep( Anim_float step_distance );
    
    /**
        Connector reports new distance of parent node during Animconnector::UpdatePass_Up.
    */
    void                        Connector_UpdateDistance( Anim_float new_distance );
    
//------------------------- update read fields -------------------------------------------
    /**
        Advance step distance requested through Connector_RequestStep.
    */
    Anim_float                  Step();
    
    /**
        Wheter the node is updated each frame.
        Does not change during frame, only in System_UpdateEnd.
    */
    bool                        Active();
    
    /**
        Start updating node until it stabilizes (IsInTarget returns true for few frames).
    */
    void                        Activate();
    
    /**
        Current distance to target.
        Cached from last Advance.
    */
    Anim_float                  Distance();
    
    /**
    */
    bool                        IsInTarget();
    
    /**
        Transforms argument of AnimNode::Advance and return value of AnimNode::Distance which relatively changes the transition speed of this node.
    */
    Anim_float                  Speed() const;
    AnimNodeI *                 Speed( Anim_float );
    AnimNodeI *                 Speed_inf();    ///< Shortcut for AnimNodeI::Speed( std::numeric_limits< Anim_float >::infinity() ).
    
protected:
    // AnimNodeI
    virtual void UpdateLastTarget(){};
    virtual bool TargetStabilized(){ return true; }
    
    static const constexpr unsigned ActiveCooldownFrames = 5;
    
protected:
    // identity
    Instance *  inst;
    AnimSystem * as;
    std::string _label;
    Anim_float  _speed;
    bool _anim_initialized;
    
    // structure
    AnimConnector * _parent;
    std::unordered_set< AnimConnector * > _children;
    
    //
    std::optional< unsigned >   _activation_id;         ///< Update_id when this should be activated.
    unsigned                    _active_cooldown;       ///< How many more frames of inactivity on this node to sleep it.
    bool                        _active;                ///< If it is actively updated each frame. Changed only during 
    
    Anim_float  _distance_start;
    Anim_float  _distance_end;
    Anim_float  _distance_working;
    
    Anim_float  _requested_step;
    
    std::optional< unsigned > _update_id;
    
    bool in_target_notified;
};

/**
*/
template< class T >
class AnimNode : public AnimNodeI
{
public:
ClientMarker cm;
                                        AnimNode( Instance * inst, T const & initial_value );
    void                                status( TableDebugView * view );
    
    AnimNode< T > *                     label( std::string const & );
    std::string const &                 label() const;
    
    virtual void                        StringIO_SetTarget( const char * source, Context const * context ) override;
    virtual std::string                 StringIO_GetTarget( Context const * context ) override;
    
    //--------------------- anim state and update ----------------------------------------
    /**
    */
    Anim_float                          Speed() const;
    AnimNode< T > *                     Speed( Anim_float );
    AnimNode< T > *                     Speed_inf();            ///< Shortcut for AnimNodeI::Speed( std::numeric_limits< Anim_float >::infinity() ).
    
    /**
        Target to which we want to be updated next time.
    */
    AnimNode< T > *                     Target( T const & target );
    T const &                           Target();
    
    /**
        Target to which this was last time updated.
    */
    T const &                           LastTarget();
    
    /**
    */
    bool                                IsInTarget();
    bool                                IsInTarget( T const & target );
    
private:
    virtual void                        UpdateLastTarget() override;
    virtual bool                        TargetStabilized() override;
    
private:
    T _target;
    T _last_target;
};

}

#include "AnimNode.inl"
