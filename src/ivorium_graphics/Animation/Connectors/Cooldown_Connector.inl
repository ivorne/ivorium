namespace iv
{

template< class T >
Cooldown_Connector< T >::Cooldown_Connector( Instance * inst ) :
    Transform_ConnectorI< T, T >( inst ),
    cm( inst, this, "Cooldown_Connector" ),
    _cooldown_increasing( 0.0 ),
    _cooldown_decreasing( 0.0 ),
    _time( 0.0 )
{
    this->cm.inherits( this->Transform_ConnectorI< T, T >::cm );
}

template< class T >
Cooldown_Connector< T > * Cooldown_Connector< T >::cooldown_increasing( Anim_float value )
{
    this->_cooldown_increasing = value;
    this->Activate();
    return this;
}

template< class T >
Anim_float Cooldown_Connector< T >::cooldown_increasing()
{
    return this->_cooldown_increasing;
}

template< class T >
Cooldown_Connector< T > * Cooldown_Connector< T >::cooldown_decreasing( Anim_float value )
{
    this->_cooldown_decreasing = value;
    this->Activate();
    return this;
}

template< class T >
Anim_float Cooldown_Connector< T >::cooldown_decreasing()
{
    return this->_cooldown_decreasing;
}

template< class T >
void Cooldown_Connector< T >::UpdatePass_Down()
{
    if( !this->parent || !this->child )
        return;
    
    //
    this->_time += this->parent->Step();
    
    //
    auto current_target = this->child->Target();
    auto next_target = this->parent->Target();
    if( next_target != current_target )
    {
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | cooldown." );
    
        // requires compare operator
        Anim_float cooldown;
        if( next_target < current_target )
            cooldown = this->_cooldown_decreasing;
        else
            cooldown = this->_cooldown_increasing;
        
        //
        if( this->_time >= cooldown )
        {
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Events, "Transfer target to child." );
            this->child->Target( next_target );
            this->_time = 0.0;
        }
    }
    else
    {
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | identified, step ", this->parent->Step(), "." );
        this->child->Connector_RequestStep( this->parent->Step() );
    }
}

template< class T >
void Cooldown_Connector< T >::UpdatePass_Up()
{
    if( !this->parent || !this->child )
        return;
    
    auto current_target = this->child->Target();
    auto next_target = this->parent->Target();
    if( next_target != current_target )
    {
        
        // requires compare operator
        Anim_float cooldown;
        if( next_target < current_target )
            cooldown = this->_cooldown_decreasing;
        else
            cooldown = this->_cooldown_increasing;
        
        //
        Anim_float distance = std::max( this->_time - cooldown, std::numeric_limits< Anim_float >::epsilon() );
        
        //
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Up | cooldown ", cooldown, ", distance ", distance, "." );
        
        //
        this->parent->Connector_UpdateDistance( distance );
    }
    else
    {
        Anim_float distance = this->child->Distance();
        if( this->_time < std::max( this->cooldown_increasing(), this->cooldown_decreasing() ) )
            distance = std::max( distance, std::numeric_limits< Anim_float >::epsilon() );
        
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Up | identified, distance ", distance, "." );
        
        this->parent->Connector_UpdateDistance( distance );
    }
}

}
