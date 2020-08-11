#include "Prefsize.hpp"

namespace iv
{

Prefsize::Prefsize( Instance * inst ) :
    OneChildElem< SlotChild >( inst ),
    SlotChild( this ),
    cm( inst, this, "Prefsize" ),
    attr_prefsizeX( &this->cm, std::nullopt ),
    attr_prefsizeY( &this->cm, std::nullopt ),
    attr_prefsizeZ( &this->cm, std::nullopt ),
    attr_prefsizeScale( &this->cm, float3( 1, 1, 1 ) )
{
    this->cm.inherits( this->OneChildElem< SlotChild >::cm, this->SlotChild::cm );
}

void Prefsize::first_pass_impl( ElementRenderer * er )
{
    if( this->child.Get() )
    {
        if( this->child.dirty() || this->expectedSize.dirty() )
        {
            this->expectedSize.clear_dirty();
            
            auto exp = this->expectedSize.Get();
            
            if( this->attr_prefsizeX.Get().has_value() )
                exp.x = std::min( exp.x, this->attr_prefsizeX.Get().value() );
                
            if( this->attr_prefsizeY.Get().has_value() )
                exp.y = std::min( exp.y, this->attr_prefsizeY.Get().value() );
                
            if( this->attr_prefsizeZ.Get().has_value() )
                exp.z = std::min( exp.z, this->attr_prefsizeZ.Get().value() );
            
            this->child.Get()->expectedSize.Set( exp );
        }
        
        this->child.Get()->elem()->first_pass( er );
    }
    
    if( this->attr_prefsizeX.clear_dirty() + this->attr_prefsizeY.clear_dirty() + this->attr_prefsizeZ.clear_dirty() + this->attr_prefsizeScale.clear_dirty() + this->child.dirty() + ( this->child.Get() && this->child.Get()->preferredSize.dirty() ) )
    {
        //
        float3 prefsize( 0, 0, 0 );
        
        // set child to prefsize
        if( this->child.Get() )
        {
            this->child.Get()->preferredSize.clear_dirty();
            prefsize = this->child.Get()->preferredSize.Get();
        }
        
        // override with initialization parameters
        if( this->attr_prefsizeX.Get().has_value() )
            prefsize.x = this->attr_prefsizeX.Get().value();
            
        if( this->attr_prefsizeY.Get().has_value() )
            prefsize.y = this->attr_prefsizeY.Get().value();
            
        if( this->attr_prefsizeZ.Get().has_value() )
            prefsize.z = this->attr_prefsizeZ.Get().value();
        
        // set synthesized parameter
        this->preferredSize.Set( prefsize * this->attr_prefsizeScale.Get() );
    }
    
    if( this->child.Get() )
        if( this->child.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
            er->QueueSecondPass( this );
}

void Prefsize::second_pass_impl( ElementRenderer * er )
{
    //
    if( !this->child.Get() )
        return;
    
    //
    if( this->child.dirty() || this->size.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
    {
        this->child.clear_dirty();
        this->size.clear_dirty();
        this->modelTransform.clear_dirty();
        this->scissor.clear_dirty();
        
        this->child.Get()->elem()->modelTransform.Set( this->modelTransform.Get() );
        this->child.Get()->elem()->scissor.Set( this->scissor.Get() );
        this->child.Get()->size.Set( this->size.Get() );
        this->child.Get()->elem()->second_pass( er );
    }
}

Prefsize * Prefsize::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

Prefsize * Prefsize::prefsizeScale( float3 val )
{
    this->attr_prefsizeScale.Set( val );
    return this;
}

Prefsize * Prefsize::prefsize( float3 val )
{
    this->attr_prefsizeX.Set( val.x );
    this->attr_prefsizeY.Set( val.y );
    this->attr_prefsizeZ.Set( val.z );
    return this;
}

Prefsize * Prefsize::prefsizeX( std::optional< float > val )
{
    this->attr_prefsizeX.Set( val );
    return this;
}

Prefsize * Prefsize::prefsizeY( std::optional< float > val )
{
    this->attr_prefsizeY.Set( val );
    return this;
}

Prefsize * Prefsize::prefsizeZ( std::optional< float > val )
{
    this->attr_prefsizeZ.Set( val );
    return this;
}

}
