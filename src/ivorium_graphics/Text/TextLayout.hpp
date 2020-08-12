#pragma once

#include "FontMesh.hpp"

#include "../Elements/Elem.hpp"
#include "../Elements/ElementRenderer.hpp"
#include "../Elements/Slot.hpp"

namespace iv
{


class TextSegment
{
public:
ClientMarker cm;
                                        TextSegment( Elem * elem );
    Instance *                          instance() const;
    
    Elem *                              elem();
    Elem const *                        elem() const;
    
    
// synthesized parameters
    /**
        This triggers first pass and second pass layout recomputation.
        Acts like Dirty< void > (just dirty flag without real value).
        It is set to true when geometry_Compute method would return a different result with the same input than before.
    */
    bool                                geometry_dirty;
    
    /**
    */
    virtual FontMesh::Geometry          geometry_Compute( FontMesh::Location const & ) = 0;
    
// inherited parameters
    /**
    */  
    DirtyAttr< FontMesh::Location >     location;
    
private:
    Elem * _elem;
};

/**
    \ingroup Elements
    This whole layouting algorithm does not work well when there are multiple font sizes on one line, but that is bad typography anyway, so it should not be a problem.
*/
class TextLayout : public VectorChildrenElem< TextSegment >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
                                    TextLayout( Instance * );
    
// initialization parameters
    DirtyAttr< float >              attr_lineSpacing;           ///< Space between lines in pixels. Baselines are ( ascender + descender + spacing ) pixels apart.
    
// utility methods
    TextLayout *                    line_spacing( float );
    
protected:
    virtual void                    first_pass_impl( ElementRenderer * ) override;
    virtual void                    second_pass_impl( ElementRenderer * ) override;
    
private:
    void RepositionChildren();
    
private:
    Heap heap;
    bool geometry_dirty;
};

}
