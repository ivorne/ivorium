#include "Rectangle_LumaButton.hpp"
#include "LumaText.hpp"

namespace iv
{
    
Rectangle_LumaButton::Rectangle_LumaButton( iv::Instance * inst, ResourcePath const & icon, char const * label, LumaStyleId style_id ) :
    LumaButton( inst, style_id ),
    cm( inst, this, "Rectangle_LumaButton" ),
    img_icon( nullptr ),
    text( nullptr )
{
    this->cm.inherits( this->LumaButton::cm );
    
    this->attr_surface.Set( ResourcePath( "/ivorium_UI/LumaButton/surface.png" ) );
    
    //-------------------------------------------
    auto divider = this->surface_slot.createChild< iv::Align >()
        ->dontExpand( true )
        ->createChild< iv::Divider >()
            ->axis( Axis::X )
            ->axisOrder( AxisOrder::Incremental );
        
        // icon
        if( !icon.empty() )
        {
            this->img_icon = divider->createChild< DividerSlot >()
                ->lowerOrder( 0 )
                ->createChild< iv::Align >()
                    ->keepAspect( true )
                    ->createChild< iv::Prefsize >()
                        ->prefsizeX( 16 )
                        ->prefsizeY( 16 )
                        ->createChild< iv::Image >()
                            ->filename( icon );
        }
        
        // label
        std::string label_trimmed = iv::string_trim( std::string( label ) );
        if( !label_trimmed.empty() )
        {
            if( !icon.empty() )
            {
                divider->createChild< DividerSlot >()
                    ->lowerOrder( 2 )
                    ->createChild< Prefsize >()
                        ->prefsizeX( 4 );
            }
            
            this->text = divider->createChild< DividerSlot >()
                ->lowerOrder( 1 )
                ->createChild< iv::Border >()
                    ->createChild< iv::TextLayout >()
                        ->createChild< iv::LumaText >()
                            ->text( label_trimmed )
                            ->fontSize( 15 );
        }
    
    //
    this->CallColorsChanged();
}

void Rectangle_LumaButton::ColorsChanged( float4 surface, float4 on_surface )
{
    if( this->img_icon )
    {
        this->img_icon->preblend( surface );
        this->img_icon->colorTransform( ColorTransform::Scale( on_surface ) );
    }
    
    if( this->text )
    {
        this->text->preblend( surface );
        this->text->color( on_surface );
    }
}

}
