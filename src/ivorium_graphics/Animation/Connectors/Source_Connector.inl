namespace iv
{
    
template< class T >
Source_Connector< T >::Source_Connector( Instance * inst, T const & default_value, AnimNode< T > * child, Attr< T > * source ) :
    Field< T >( inst ),
    cm( inst, this, "Source_Connector" ),
    inst( inst ),
    child( child ),
    default_value( default_value )
{
    this->cm.inherits( this->Field< T >::cm );
    
    if( source )
        this->Assign_Attribute_R( source );
}

template< class T >
Instance * Source_Connector< T >::instance() const
{
    return this->inst;
}

template< class T >
void Source_Connector< T >::SetChild( AnimNode< T > * child )
{
    this->child = child;
    this->OnChanged(); 
}

template< class T >
void Source_Connector< T >::OnChanged( bool real )
{
    if( this->child )
    {
        auto mode = this->Field< T >::Mode();
        
        if( mode == Attribute::ValueMode::Disabled )
        {
            this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "Target ", this->default_value, " (default value)." );
            this->child->Target( this->default_value );
        }
        else
        {
            this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "Target ", this->Field< T >::Get(), "." );
            this->child->Target( this->Field< T >::Get() );
        }
    }
}

}
