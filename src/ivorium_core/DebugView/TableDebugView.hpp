#pragma once

#include "DebugView.hpp"

#include "../Basics/RuntimeId.hpp"
#include "../Basics/StringIOIndex.hpp"
#include "../Instancing/instance_ptr.hpp"

#include <string>
#include <vector>
#include <any>

//----------------------------------------------------------------------------------------------------------------------
//------------- TableId ------------------------------------------------------------------------------------------------
namespace iv
{

class TableId : public RuntimeId< TableId >
{
public:
    static constexpr char TypeName[] = "TableId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< TableId >::RuntimeId;
};

}

namespace std
{
template<>
struct hash< iv::TableId > : iv::hash< iv::TableId >{};
}

namespace iv
{

class TableDebugView;

namespace table_debug_context
{

struct HintData;
struct ColumnData;
struct RowData;
struct TableData;

//---------------------- Handles -------------------------------------------------------------------
class ColumnHandle
{
public:
                        template< class ColumnT >
                        ColumnHandle( RowData * _row, const char * col_name, ColumnT && value );
    
    template< class HintT >
    ColumnHandle &      Hint( const char * hint_name, HintT && value );
    
private:
    ColumnData * _column;
};

class RowHandle
{
public:
                        RowHandle( TableData * _table, std::any const & row_id );

    template< class ColumnT >
    ColumnHandle        Column( char const * col_name, ColumnT && value );
    
private:
    RowData * _row;
};

class TableHandle
{
public:
                        TableHandle( TableDebugView * context, TableId table_id );

    RowHandle           Row( std::any const & row_id );
    
private:
    TableData * _table;
};

//---------------------- Data ----------------------------------------------------------------------
struct HintData
{
    std::string name;
    std::any hint;
};

struct ColumnData
{
    std::any value;
    std::vector< HintData > hints;
};

struct RowData
{
    std::any identity;
    std::unordered_map< std::string, ColumnData > columns;  ///< ColumnData::name values in the vector are unique.
};

struct TableData
{
    std::vector< RowData > rows;
};

}

//----------------------------------------------------------------------------------------------------------------------
//------------- TableDebugView --------------------------------------------------------------------------------------
/**
    It is meant to be used by debugged code - something writes to this and this is then used by debug display to fill TextDebugView or GuiDebugView.
*/
class TableDebugView : public DebugView
{
public:
                                        TableDebugView( Context * context );
    
    table_debug_context::TableHandle    Table( TableId table_id );
    
    bool                                IsEmpty() const;
    void                                Clear();    ///< Removes all tables.
    
    //
    using TableData = table_debug_context::TableData;
    std::unordered_map< TableId, TableData > const &    Tables() const;
    std::unordered_map< TableId, TableData > &          Tables();
    
private:
    std::unordered_map< TableId, TableData > tables;
};

//----------------------------------------------------------------------------------------------------------------------
//------------- Handles --------------------------------------------------------------------------------------------
}

#include "TableDebugView.inl"
