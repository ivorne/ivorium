namespace iv
{

template< class T >
TimeTransform_Connector< T >::TimeTransform_Connector( Instance * inst ) :
    Transform_ConnectorI< T, T >( inst ),
    cm( inst, this, "TimeTransform_Connector", ClientMarker::Status() ),
    current_source_time( 0.0 ),
    current_total_time( 0.0 ),
    current_distance( 0.0 ),
    reset_remainder_distance( std::nullopt )
{
    this->cm.inherits( this->Transform_ConnectorI< T, T >::cm );
}

template< class T >
void TimeTransform_Connector< T >::status( iv::TableDebugView * view )
{
    auto row = view->Table( TimeTransform_Connector_DebugTable ).Row( this );
    row.Column( "current_source_time", this->current_source_time );
    row.Column( "current_total_time", this->current_total_time );
    row.Column( "current_distance", this->current_distance );
    row.Column( "reset_remainder_distance", this->reset_remainder_distance );
}

template< class T >
void TimeTransform_Connector< T >::transform( TimeTransform const & transform )
{
    this->_transform = transform;
}

template< class T >
void TimeTransform_Connector< T >::UpdatePass_Down()
{
    if( !this->parent || !this->child )
        return;
        
    bool dirty = this->treeChanged_Get() || this->parentChanged_Get();
    this->treeChanged_Clear();
    this->parentChanged_Clear();
    
    Anim_float distance = this->parent->Step();
    if( distance == std::numeric_limits< Anim_float >::infinity() )
    {
        if( dirty )
        {
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Events, "Reset target - target changed to ",this->parent->Target(),", warping." );
            this->child->Target( this->parent->Target() );
        }
        
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | Warping." );
        
        this->child->Connector_RequestStep( distance );
        
        this->current_total_time = 0.0;
        this->current_source_time = 0.0;
        this->current_distance = 0.0;
    }
    else
    {
        if( dirty )
        {
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Events, "Reset target - target changed to ", this->parent->Target(), "." );
            this->child->Target( this->parent->Target() );
            this->current_total_time = 0.0;
            this->current_source_time = 0.0;
            this->reset_remainder_distance = distance;
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | Changing target." );
            return;
        }
        
        if( !this->reset_remainder_distance.has_value() ) // Only consider retargeting due to critical distance deviation when the current_total_time was fetched and has an actual value.
        {
            auto expected_target_time = this->_transform.Transform( this->current_source_time, this->current_total_time );
            auto expected_target_distance = std::max( Anim_float( 0.0 ), this->current_total_time - expected_target_time );
            auto current_target_distance = this->child->Distance();
            auto difference = iv::abs( expected_target_distance - current_target_distance );
            
            if( ( this->current_total_time == 0 && current_target_distance != 0 )|| difference / this->current_total_time > DistanceChangeRatioThatCausesReset )
            {
                this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "Reset target - critical deviation of ", difference, " (", ( difference / this->current_total_time * 100 ) ,"%)." );
                this->reset_remainder_distance = 0.0f;
                this->current_source_time = 0.0;
            }
        }
        
        //
        if( this->reset_remainder_distance.has_value() )
        {
            this->current_total_time = this->child->Distance();
            distance += this->reset_remainder_distance.value();
            this->reset_remainder_distance = std::nullopt;
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Events, "New current_total_time is ", this->current_total_time, "." );
        }
        
        // current time
        auto current_target_time = this->_transform.Transform( this->current_source_time, this->current_total_time );
        
        // next time
        auto next_source_time = this->current_source_time + distance;
        auto next_target_time = this->_transform.Transform( next_source_time, this->current_total_time );
        
        // distance
        auto target_distance = next_target_time - current_target_time;
        
        //
        this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | Step ",distance,", child_step ",target_distance,"." );
        
        // advance
        this->current_source_time = next_source_time;
        this->child->Connector_RequestStep( target_distance );
        
        // return new distance
        this->current_distance = std::max( Anim_float( 0.0 ), this->current_total_time - this->current_source_time );
    }
}

template< class T >
void TimeTransform_Connector< T >::UpdatePass_Up()
{
    auto distance = this->current_distance;
    if( this->child->Distance() > 0.0f )
        distance = std::max( Anim_float( 0.001f ), distance );
    
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Up | Distance ",distance,"." );
    
    this->parent->Connector_UpdateDistance( distance );
}

}
