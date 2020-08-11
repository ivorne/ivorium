#include "ToText_TableDebugView.hpp"

#include "TextDebugView.hpp"

#include "../Basics/StringIOIndex.hpp"
#include "../Basics/utils.hpp"
#include "../Instancing/Instance.hpp"

#include <map>

namespace iv
{

ToText_TableDebugView::ToText_TableDebugView( Context * context ) :
    TableDebugView( context )
{
}

std::string ToText_TableDebugView::write( std::any const & value )
{
    return ::iv::StringIOIndex::Write( value, this->context() );
}

void ToText_TableDebugView::Write_AsLines( TextDebugView * out, bool print_table_names )
{
    std::unordered_map< TableId, TableData > const & tables = this->Tables();
    
    size_t max_width = 0;
    
    // compute base width
    for( auto & [ table_id, table ] : tables )
        for( auto & row : table.rows )
            for( auto & [ column_name, column ] : row.columns )
            {
                (void)table_id;
                (void)column;
                std::string base_str = column_name + ": ";
                max_width = std::max( max_width, utf8_size( base_str ) );
                
                for( size_t i = 0; i < column.hints.size(); i++ )
                {
                    std::string const & hint_name = column.hints[ i ].name;
                    std::string base_str = "   " + hint_name + ": ";
                    max_width = std::max( max_width, utf8_size( base_str ) );
                }
            }
    
    // print
    for( auto & [ table_id, table ] : tables )
        for( auto & row : table.rows )
        {
            if( print_table_names )
                out->out() << "-" << table_id.persistent_value() << "-" << std::endl;
        
            for( auto & [ column_name, column ] : row.columns )
            {
                // create name string
                std::string base_str;
                //base_str += table_id.persistent_value() + "::" + column_name + ": ";
                base_str += column_name + ": ";
                
                // print name string and whitespace fill
                out->out() << base_str;
                for( size_t i = utf8_size( base_str ); i < max_width; i++ )
                    out->out() << " ";
                
                // print column value
                std::string pref = "";
                for( size_t i = 0; i < max_width; i++ )
                    pref += " ";
                out->prefix_push( pref.c_str() );
                out->out() << this->write( column.value );
                out->prefix_pop();
                
                //
                out->out() << std::endl;
                
                // print hints
                for( size_t i = 0; i < column.hints.size(); i++ )
                {
                    std::string const & hint_name = column.hints[ i ].name;
                    std::any const & hint_val = column.hints[ i ].hint;
                    
                    std::string base_str = "   " + hint_name + ": ";
                    
                    out->out() << base_str;
                    for( size_t i = utf8_size( base_str ); i < max_width; i++ )
                        out->out() << " ";
                    
                    out->out() << this->write( hint_val );
                    out->out() << std::endl;
                }
            }
            
            if( print_table_names )
                out->out() << std::endl;
        }
}

void ToText_TableDebugView::Write_AsTables( TextDebugView * context )
{
    std::unordered_map< TableId, TableData > const & tables = this->Tables();
    
    std::map< std::string, size_t > columns;    // currently ordered alphabeticaly by column name; we might want to somehow retrieve aggregate order in which columns were written and use that order; or use some explicit ordering
    for( auto & [ table_id, table ] : tables )
    {
        //---- fetch columns and their widths ----
        columns.clear();
        for( auto & row : table.rows )
        {
            for( auto & [ column_name, column ] : row.columns )
            {
                size_t & line_size = columns[ column_name ];
                
                // adjust for column header
                line_size = std::max( line_size, utf8_size( column_name ) + 4 );
                
                // adjust for column content
                std::string strcol = this->write( column.value );
                line_size = std::max( line_size, utf8_size( strcol ) + 2 );
                
                // adjust for hints
                for( auto & [ hint_name, hint ] : column.hints )
                {
                    std::string strhint = this->write( hint );
                    size_t hintsize = 4 + utf8_size( hint_name ) + 2 + utf8_size( this->write( hint ) ) + 1;
                    line_size = std::max( line_size, hintsize );
                }
            }
        }
        
        // add at least one column so that it is easier to write (it will not be filled with anything anyway)
        if( columns.empty() )
            columns[ "" ] = 0;
        
        // adjust last column width for table title
        size_t title_req_width = 4 + utf8_size( table_id.persistent_value() ) + 2;
        size_t total_width = 1;
        for( auto & [ colname, colwidth ] : columns )
        {
            (void)colname;
            total_width += colwidth + 1;
        }
        
        if( total_width < title_req_width )
        {
            if( columns.empty() )
            {
                columns[ "" ] = 6;
            }
            else
            {
                size_t & last_col_width = columns.rbegin()->second;
                last_col_width += title_req_width - total_width;
            }
        }
        
        //-------------- helpers ----------------------
        auto write_vertical_separator = [&]( const char * left, const char * separator, const char * right, const char * filler )
        {
            bool first = true;
            context->out() << left;
            for( auto & [ colname, colwidth ] : columns )
            {
                (void)colname;
                if( first )
                    first = false;
                else
                    context->out() << separator;
                    
                for( size_t i = 0; i < colwidth; i++ )
                    context->out() << filler;
            }
            context->out() << right << std::endl;
        };
        
        auto write_table_title = [&]( std::string const & title )
        {
            //
            size_t i = 0;
            auto write_symbol = [&]( const char * symbol )
            {
                const size_t offset = 3;
                if( i < offset )
                    context->out() << symbol;
                else if( i == offset )
                    context->out() << "╸";
                else if( i <= offset + title.size() )
                    context->out() << title[ i - offset - 1 ];
                else if( i <= offset + title.size() + 1 )
                    context->out() << "╺";
                else
                    context->out() << symbol;
                i++;
            };
            
            //
            bool first = true;
            write_symbol( "┏" );
            for( auto & [ colname, colwidth ] : columns )
            {
                (void)colname;
                if( first )
                    first = false;
                else
                    write_symbol( "┯" );
                    
                for( size_t i = 0; i < colwidth; i++ )
                    write_symbol( "━" );
            }
            write_symbol( "┓" );
            context->out() << std::endl;
        };
        
        auto write_column_titles = [&]()
        {
            bool first = true;
            context->out() << "┃";
            for( auto & [ colname, colwidth ] : columns )
            {
                if( first )
                    first = false;
                else
                    context->out() << "│";
                
                context->out() << " -" << colname << "- ";
                    
                for( size_t i = utf8_size( colname ) + 4; i < colwidth; i++ )
                    context->out() << ' ';
            }
            context->out() << "┃" << std::endl;
        };
        
        auto write_column_values = [&]( table_debug_context::RowData const & row ) -> bool  // returns true if some column has some hints, false if no column has any hints
        {
            bool result = false;
            
            bool first = true;
            context->out() << "┃";
            for( auto & [ colname, colwidth ] : columns )
            {
                if( first )
                    first = false;
                else
                    context->out() << "│";
                
                auto it_coldata = row.columns.find( colname );
                if( it_coldata != row.columns.end() )
                {
                    table_debug_context::ColumnData const & coldata = it_coldata->second;
                    if( coldata.hints.size() )
                        result = true;
                    
                    std::string coldata_str = this->write( coldata.value );
                    context->out() << ' ' << coldata_str << ' ';
                    for( size_t i = utf8_size( coldata_str ) + 2; i < colwidth; i++ )
                        context->out() << ' ';
                }
                else
                {
                    for( size_t i = 0; i < colwidth; i++ )
                        context->out() << ' ';
                }
            }
            context->out() << "┃" << std::endl;
            
            return result;
        };
        
        auto write_column_hints = [&]( table_debug_context::RowData const & row, size_t hint_id ) -> bool   // returns true if some column has a hint on position hint_id+1 (next hint)
        {
            bool result = false;
            
            bool first = true;
            context->out() << "┃";
            for( auto & [ colname, colwidth ] : columns )
            {
                if( first )
                    first = false;
                else
                    context->out() << "│";
                
                auto it_coldata = row.columns.find( colname );
                if( it_coldata != row.columns.end() )
                {
                    table_debug_context::ColumnData const & coldata = it_coldata->second;
                    if( coldata.hints.size() > hint_id )
                    {
                        // check if it is last hint
                        bool last = !( coldata.hints.size() > hint_id + 1 );
                        if( !last )
                            result = true;
                        
                        // retrieve hint data
                        auto & [ hint_name, hint_any ] = coldata.hints[ hint_id ];
                        std::string hint_str = this->write( hint_any );
                        
                        // write hint
                        size_t i = 0;
                        if( last )
                            context->out() << "  └╴";
                        else
                            context->out() << "  ├╴";
                        i += 4;
                        
                        if( !hint_name.empty() )
                        {
                            context->out() << hint_name << ": ";
                            i += utf8_size( hint_name ) + 2;
                        }
                        
                        context->out() << hint_str << ' ';
                        i += utf8_size( hint_str ) + 1;
                        
                        // fill with spaces
                        for( ; i < colwidth; i++ )
                            context->out() << ' ';
                    }
                    else
                    {
                        for( size_t i = 0; i < colwidth; i++ )
                            context->out() << ' ';
                    }
                }
                else
                {
                    for( size_t i = 0; i < colwidth; i++ )
                        context->out() << ' ';
                }
            }
            
            context->out() << "┃" << std::endl;
            
            return result;
        };
        
        //-------- print table --------
        // top line
        write_table_title( table_id.persistent_value() );
        
        // colnames
        write_column_titles();
        
        // header - body separator
        write_vertical_separator( "┣", "┿", "┫", "━" );
        
        // rows
        bool first_row = true;
        for( auto const & row : table.rows )
        {
            // row separator
            if( first_row )
                first_row = false;
            else
                write_vertical_separator( "┠", "┼", "┨", "─" );
            
            // column values and hints
            if( write_column_values( row ) )
            {
                // column hints
                for( size_t i = 0; write_column_hints( row, i ); i++ );
            }
        }
        
        // table ending line
        write_vertical_separator( "┗", "┷", "┛", "━" );
    }
}

}
