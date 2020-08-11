#include "Axis.hpp"

namespace iv
{

const StringIO< AxisOrder >::ValuesType StringIO< AxisOrder >::Values = 
{
    { AxisOrder::Incremental, "Incremental" },
    { AxisOrder::Decremental, "Decremental" },
};

const StringIO< Axis >::ValuesType StringIO< Axis >::Values = 
{
    { Axis::X, "X" },
    { Axis::Y, "Y" },
    { Axis::Z, "Z" },
};

const StringIO< AxisDirection >::ValuesType StringIO< AxisDirection >::Values = 
{
    { AxisDirection::X_Inc, "X_Inc" },
    { AxisDirection::X_Dec, "X_Dec" },
    { AxisDirection::Y_Inc, "Y_Inc" },
    { AxisDirection::Y_Dec, "Y_Dec" },
    { AxisDirection::Z_Inc, "Z_Inc" },
    { AxisDirection::Z_Dec, "Z_Dec" },
};

}
