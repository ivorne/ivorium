#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

class LumaSystem;

/**
*/
class LumaStyleId : public iv::RuntimeId< LumaStyleId >
{
public:
    static constexpr char TypeName[] = "LumaStyleId";
    using iv::RuntimeId< LumaStyleId >::RuntimeId;
    static iv::RuntimeIdDictionary Dictionary;
};

}

namespace std
{
template<>
struct hash< iv::LumaStyleId > : public iv::hash< iv::LumaStyleId >{};
}

namespace iv
{

/**
*/
struct LumaStyle
{
public:
    LumaStyle();

    iv::float4 surface;         ///< Surface of elements. Base neutral background color.
    iv::float4 highlight;           ///< Surface of activatable or highlighted elements. Or text on surface that should be highlighted.
    iv::float4 error;               ///< Sufrace color in case of an error. Or text on surface in case of an error.
    
    iv::float4 on_surface;          ///< Text and symbols on a normal sufrace.
    iv::float4 on_highlight;        ///< Text and symbols on sufraces with highlight color.
    iv::float4 on_error;            ///< Text and symbols on sufraces with error color.
    
    ResourcePath font_normal;
    ResourcePath font_monospace;
};

/**
    Listens to one style for changes.
*/
class LumaListener
{
public:
ClientMarker cm;

                            LumaListener( Instance * inst, LumaStyleId );
                            ~LumaListener();
    Instance *              instance() const;
    
    LumaStyleId             style_id() const;
    LumaStyle const &       style() const;
    
    virtual void            LumaStyleChanged( LumaStyle const & ) = 0;
    
    
private:
    Instance *  inst;
    LumaStyleId id;
};

/**
*/
class LumaStyler
{
public:
ClientMarker cm;
                            LumaStyler( Instance * inst );
    Instance *              instance() const;
    
    /**
    */
    void                    current_style( LumaStyleId );
    LumaStyleId             current_style() const;
    
    /**
    */
    void                    style( LumaStyleId, LumaStyle const & );
    LumaStyle const &       style( LumaStyleId ) const;
    
private:
    Instance * inst;
    LumaSystem * ls;
};

/**
*/
class LumaSystem : public iv::System
{
public:
                            LumaSystem( SystemContainer * sc );
    virtual std::string     debug_name() const override { return "LumaSystem"; }
    
    /**
        All elements that use luma styling will read current default style from system and use it, unless style is explicitly specified in constructor.
    */
    void                    current_style( LumaStyleId );
    LumaStyleId             current_style() const;
    
    /**
    */
    void                    style( LumaStyleId, LumaStyle const & );
    LumaStyle const &       style( LumaStyleId ) const;
    
    /**
        Notified when style changes.
    */
    void                    AddListener( LumaListener * );
    void                    RemoveListener( LumaListener * );
    
private:
    LumaStyleId _current_style;
    std::unordered_map< LumaStyleId, LumaStyle > _styles;
    iv::volatile_set< LumaListener * > _listeners;
};

}
