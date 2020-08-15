namespace iv
{

//=================================== ComponentAttr_Index =========================================================
template< class Component >
ComponentAttr_Index< Component >::ComponentAttr_Index( iv::Instance * inst ) :
    cm( inst, this, "ComponentAttr_Index" ),
    inst( inst ),
    iterating( false )
{
}

template< class Component >
iv::Instance * ComponentAttr_Index< Component >::instance() const
{
    return this->inst;
}

template< class Component >
void ComponentAttr_Index< Component >::AttributeEvent( iv::AttributeEventProcessor::Event event, Component * component, iv::Attribute * attr, iv::AttributeListener * listener )
{
    switch( event )
    {
        case iv::AttributeEventProcessor::Event::AddListener:
        {
            if( this->iterating )
                this->cm.warning( SRC_INFO, "Should not add listeners when iterating over them." );
            this->listeners.insert( std::pair( attr, listener ) );
        } break;
        
        case iv::AttributeEventProcessor::Event::RemoveListener:
        {
            if( this->iterating )
                this->cm.warning( SRC_INFO, "Should not remove listeners when iterating over them." );
                
            auto range = this->listeners.equal_range( attr );
            for( auto it = range.first; it != range.second; ++it )
                if( it->second == listener )
                {
                    this->listeners.erase( it );
                    break;
                }
        } break;
        
        case iv::AttributeEventProcessor::Event::Changed:
        {
            this->iterating += 1;
            auto range = this->listeners.equal_range( attr );
            for( auto it = range.first; it != range.second; ++it )
                it->second->Attribute_Changed( attr );
            this->iterating -= 1;
            
            this->Component_AttrChanged( component, attr );
        } break;
        
        case iv::AttributeEventProcessor::Event::Activated:
        {
        } break;
        
        case iv::AttributeEventProcessor::Event::Deactivated:
        {
            this->iterating += 1;
            auto range = this->listeners.equal_range( attr );
            for( auto it = range.first; it != range.second; ++it )
                it->second->Attribute_Deactivated( attr );
            this->iterating -= 1;
        } break;
    }
}

//=================================== ComponentAttr =========================================================
template< class Component, class T >
ComponentAttr< Component, T >::ComponentAttr( iv::ClientMarker * cm, T const & initial_value ) :
    iv::AttributeEventProcessor(),
    iv::SharedAttr_I< T >( cm, this, initial_value ),
    component( nullptr ),
    index( nullptr )
{
}

template< class Component, class T >
ComponentAttr< Component, T >::~ComponentAttr()
{
    this->Index();
}

template< class Component, class T >
ComponentAttr< Component, T >::ComponentAttr( ComponentAttr< Component, T > const & src ) :
    iv::AttributeEventProcessor(),
    iv::SharedAttr_I< T >( src.owner(), this, src.Get() ),
    component( nullptr ),
    index( nullptr )
{
}

template< class Component, class T >
ComponentAttr< Component, T > & ComponentAttr< Component, T >::operator=( ComponentAttr< Component, T > const & src )
{
    this->Modify( src.owner(), src.Get() );
}

template< class Component, class T >
void ComponentAttr< Component, T >::Index( Component * component, ComponentAttr_Index< Component > * index )
{
    if( bool( index ) == bool( this->index ) )
        return;
    
    if( !index )
        this->Notify_Deactivated();
    
    this->component = component;
    this->index = index;
    
    if( index )
        this->Notify_Activated();
}

template< class Component, class T >
void ComponentAttr< Component, T >::Index()
{
    this->Index( nullptr, nullptr );
}

template< class Component, class T >
void ComponentAttr< Component, T >::AttributeEvent( iv::AttributeEventProcessor::Event event, iv::Attribute * attr, iv::AttributeListener * listener )
{
    if( !this->index )
    {
        if( event == iv::AttributeEventProcessor::Event::AddListener || event == iv::AttributeEventProcessor::Event::RemoveListener )
            this->owner()->warning( SRC_INFO, "Event ", event, " on ComponentAttr can not be served because it is not currently registered to an Index." );
        return;
    }
    
    this->index->AttributeEvent( event, this->component, attr, listener );
}

}
