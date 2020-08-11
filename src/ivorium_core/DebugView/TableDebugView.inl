namespace iv
{

namespace table_debug_context
{

//---------------------------------- RowHandle -----------------------------------------------------
template< class ColumnT >
ColumnHandle RowHandle::Column( char const * col_name, ColumnT && value )
{
    return ColumnHandle( this->_row, col_name, value );
}

//---------------------------------- ColumnHandle --------------------------------------------------
template< class ColumnT >
ColumnHandle::ColumnHandle( RowData * _row, const char * col_name, ColumnT && value )
{
    //
    StringIOIndex::Register< ColumnT >();
    
    //
    auto it = _row->columns.insert( std::pair( std::string( col_name ), ColumnData() ) ).first;
    it->second.value = std::any( value );
    this->_column = &it->second;
}

template< class HintT >
ColumnHandle & ColumnHandle::Hint( const char * hint_name, HintT && value )
{
    //
    StringIOIndex::Register< HintT >();
    
    //
    HintData data;
    data.name = hint_name;
    data.hint = std::any( value );
    this->_column->hints.push_back( data );
    return *this;
}

}

}
