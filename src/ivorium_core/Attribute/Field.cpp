#include "Field.hpp"

namespace iv
{

const StringIO< FieldI::Assignment >::ValuesType StringIO< FieldI::Assignment >::Values = 
{
    { FieldI::Assignment::Unassigned, "Unassigned" },
    { FieldI::Assignment::Constant, "Constant" },
    { FieldI::Assignment::Attribute_R, "Attribute_R" },
    { FieldI::Assignment::Attribute_RW, "Attribute_RW" }
};

void FieldI::Assign_AnyAttribute_R( Attribute * source )
{
    this->AnySource_R_Impl( source );
}

void FieldI::Assign_AnyAttribute_RW( Attribute * source )
{
    this->AnySource_RW_Impl( source );
}

std::type_index FieldI::Type()
{
    return this->Type_Impl();
}

Attribute::ValueMode FieldI::Mode() const
{
    return this->mode_Impl();
}

FieldI::Assignment FieldI::AssignmentState() const
{
    return this->AssignmentState_impl();
}

}
