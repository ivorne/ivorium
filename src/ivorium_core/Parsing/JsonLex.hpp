#pragma once

#include "Lex.hpp"

#include <stack>

namespace iv
{

class JsonLex
{
public:
ClientMarker cm;

    enum Token
    {   
        Integer,
        Double,
        String,
        Bool,
        Null,
        
        ArrayBegin,
        ArrayEnd,
        
        TableBegin,
        TableEnd,
        TableKey,
        
        Eof
    };

                    JsonLex( Instance * inst );
    Instance *      instance() const;
                  
    Lex const &     lex() const;
    
    void            Init( std::istream & in );
    bool            Failed();
    void            LogicFail( const char * message ){ this->_lex.LogicFail( message ); }
    
    // IsNext
    bool            IsNext( Token );
    
    // Accept
    void            Accept( Token );
    
    std::string     AcceptTableKey();
    
    int             AcceptInteger();
    double          AcceptDouble();
    std::string     AcceptString();
    bool            AcceptBool();
    void            AcceptNull();
    
private:
    enum class Next
    {
        Value,
        TableKey,
        
        TableEnd,
        ArrayEnd,
        Eof
    };

    enum class LayerType
    {
        Array,
        Table
    };

    void value_ended();
    void JsonFail();
    
private:
    Lex _lex;
    Next next;
    std::stack< LayerType > layer_types;
};

}
