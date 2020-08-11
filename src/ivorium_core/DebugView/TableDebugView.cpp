#include "TableDebugView.hpp"
#include <algorithm>

namespace iv
{

using namespace table_debug_context;

//---------------------------------- TableId -------------------------------------------------------
RuntimeIdDictionary TableId::Dictionary;

//---------------------------------- TableDebugView ---------------------------------------------
TableDebugView::TableDebugView( Context * context ) :
    DebugView( context )
{
}

TableHandle TableDebugView::Table( TableId table_id )
{
    return TableHandle( this, table_id );
}

bool TableDebugView::IsEmpty() const
{
    return this->tables.empty();
}

void TableDebugView::Clear()
{
    this->tables.clear();
}

std::unordered_map< TableId, TableData > const & TableDebugView::Tables() const
{
    return this->tables;
}

std::unordered_map< TableId, TableData > & TableDebugView::Tables()
{
    return this->tables;
}

namespace table_debug_context
{

//---------------------------------- TableHandle ---------------------------------------------------
TableHandle::TableHandle( TableDebugView * context, TableId table_id ) :
    _table( &context->Tables()[ table_id ] )
{
}

RowHandle TableHandle::Row( std::any const & row_id )
{
    return RowHandle( this->_table, row_id );
}

//---------------------------------- RowHandle -----------------------------------------------------
RowHandle::RowHandle( TableData * _table, std::any const & row_id )
{
    _table->rows.push_back( RowData() );
    this->_row = &_table->rows[ _table->rows.size() - 1 ];
    this->_row->identity = row_id;
}

}

}
