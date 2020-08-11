namespace iv
{

template< class T >
AnimNode< T > * AnimHeap::Node( T const & initial_value )
{
    auto node = this->heap.template createClient< iv::AnimNode< T > >( initial_value );
    this->nodes.insert( node );
    return node;
}

template< class T >
AnimNode< T > * AnimHeap::Attribute_SourceNode( Attr< T > * attr, T const & default_value )
{
    auto node = this->heap.template createClient< iv::Source_AnimNode< T > >( default_value );
    node->Assign_Attribute_R( attr );
    this->nodes.insert( node );
    return node;
}

template< class T >
AnimNode< T > * AnimHeap::Attribute_DestinationNode( Attr< T > * attr, T const & initial_value )
{
    auto node = this->heap.template createClient< iv::Destination_AnimNode< T > >( attr, initial_value );
    this->nodes.insert( node );
    return node;
}

template< class T >
AnimNode< T > * AnimHeap::State_DestinationNode( T const & initial_value )
{
    auto node = this->heap.template createClient< iv::State_AnimNode< T > >( initial_value );
    this->nodes.insert( node );
    return node;
}

template< class T >
Cooldown_Connector< T > * AnimHeap::Make_Cooldown_Connector( AnimNode< T > * parent, AnimNode< T > * child )
{
    auto connector = this->heap.template createClient< iv::Cooldown_Connector< T > >();
    connector->SetParent( parent );
    connector->SetChild( child );
    return connector;
}

template< class T >
TimeTransform_Connector< T > * AnimHeap::Make_TimeTransform_Connector( TimeTransform const & transform, AnimNode< T > * parent, AnimNode< T > * child )
{
    auto connector = this->heap.template createClient< iv::TimeTransform_Connector< T > >();
    connector->transform( transform );
    connector->SetParent( parent );
    connector->SetChild( child );
    return connector;
}

}
