#include "JsonLex.hpp"

namespace iv
{

JsonLex::JsonLex( Instance * inst ) :
    cm( inst, this, "JsonLex" ),
    _lex( inst ),
    next( Next::Value ),
    layer_types()
{
    this->cm.owns( this->_lex.cm );
    
    this->_lex.DefineOperator( "," );
    this->_lex.DefineOperator( ":" );
    this->_lex.DefineOperator( "{" );
    this->_lex.DefineOperator( "}" );
    this->_lex.DefineOperator( "[" );
    this->_lex.DefineOperator( "]" );
    
    this->_lex.DefineKeyword( "true" );
    this->_lex.DefineKeyword( "false" );
    this->_lex.DefineKeyword( "null" );
}

Instance * JsonLex::instance() const
{
    return this->_lex.instance();
}
                  
Lex const & JsonLex::lex() const
{
    return this->_lex;
}
    
void JsonLex::Init( std::istream & in )
{
    this->next = Next::Value;
    this->layer_types = std::stack< LayerType >();
    this->_lex.Init( in );
}

bool JsonLex::Failed()
{
    return this->_lex.Failed();
}
    
bool JsonLex::IsNext( Token token )
{
    switch( token )
    {
        case Token::Integer:
            return this->next == Next::Value && this->_lex.IsNext( Lex::Integer );
            
        case Token::Double:
            return this->next == Next::Value && this->_lex.IsNext( Lex::Double );
            
        case Token::String:
            return this->next == Next::Value && this->_lex.IsNext( Lex::String );
            
        case Token::Bool:
            return this->next == Next::Value && ( this->_lex.IsNextKeyword( "true" ) || this->_lex.IsNextKeyword( "false" ) );
           
        case Token::Null:
            return this->next == Next::Value && this->_lex.IsNextKeyword( "null" );
            
        case Token::ArrayBegin:
            return this->next == Next::Value && this->_lex.IsNextOperator( "[" );
            
        case Token::ArrayEnd:
            return this->next == Next::ArrayEnd && this->_lex.IsNextOperator( "]" );
        
        case Token::TableBegin:
            return this->next == Next::Value && this->_lex.IsNextOperator( "{" );
            
        case Token::TableEnd:
            return this->next == Next::TableEnd && this->_lex.IsNextOperator( "}" );
            
        case Token::TableKey:
            return this->next == Next::TableKey && this->_lex.IsNext( Lex::String );
            
        case Token::Eof:
            return this->next == Next::Eof && this->_lex.IsNext( Lex::Eof );
        
        default:
            return false;
    }
}

void JsonLex::value_ended()
{
    if( this->layer_types.empty() )
    {
        this->next = Next::Eof;
    }
    else
    {
        LayerType type = this->layer_types.top();
        if( type == LayerType::Array )
        {
            if( this->_lex.IsNextOperator( "," ) )
            {
                this->_lex.AcceptOperator( "," );
                this->next = Next::Value;
            }
            else
            {
                this->next = Next::ArrayEnd;
            }
        }
        else
        {
            if( this->_lex.IsNextOperator( "," ) )
            {
                this->_lex.AcceptOperator( "," );
                this->next = Next::TableKey;
            }
            else
            {
                this->next = Next::TableEnd;
            }
        }
    }
}

void JsonLex::JsonFail()
{
    //this->_lex.LogicFail( "Json structure problem." );
    this->_lex.ParserFail( "JsonFail" );
}

void JsonLex::Accept( Token token )
{
    if( !this->IsNext( token ) )
    {
        this->JsonFail();
        return;
    }
    
    switch( token )
    {
        case Token::Integer:
            this->_lex.Accept( Lex::Integer );
            this->value_ended();
            break;
            
        case Token::Double:
            this->_lex.Accept( Lex::Double );
            this->value_ended();
            break;
            
        case Token::String:
            this->_lex.Accept( Lex::String );
            this->value_ended();
            break;
            
        case Token::Bool:
            if( this->_lex.IsNextKeyword( "true" ) )
                this->_lex.AcceptKeyword( "true" );
            else
                this->_lex.AcceptKeyword( "false" );
            this->value_ended();
            break;
        
        case Token::Null:
            this->_lex.AcceptKeyword( "null" );
            this->value_ended();
            break;
            
        case Token::ArrayBegin:
            this->_lex.AcceptOperator( "[" );
            this->layer_types.push( LayerType::Array );
            if( this->_lex.IsNextOperator( "]" ) )
                this->next = Next::ArrayEnd;
            else
                this->next = Next::Value;
            break;
        
        case Token::ArrayEnd:
            this->_lex.AcceptOperator( "]" );
            this->layer_types.pop();
            this->value_ended();
            break;
        
        case Token::TableKey:
            this->_lex.Accept( Lex::String );
            this->_lex.AcceptOperator( ":" );
            this->next = Next::Value;
            break;
        
        case Token::TableBegin:
            this->_lex.AcceptOperator( "{" );
            this->layer_types.push( LayerType::Table );
            if( this->_lex.IsNextOperator( "}" ) )
                this->next = Next::TableEnd;
            else
                this->next = Next::TableKey;
            break;
        
        case Token::TableEnd:
            this->_lex.AcceptOperator( "}" );
            this->layer_types.pop();
            this->value_ended();
            break;
        
        case Token::Eof:
            this->_lex.Accept( Lex::Eof );
            break;
    }
}
    
std::string JsonLex::AcceptTableKey()
{
    if( !this->IsNext( Token::TableKey ) )
    {
        this->JsonFail();
        return "";
    }
    
    auto result = this->_lex.AcceptString();
    this->_lex.AcceptOperator( ":" );
    this->next = Next::Value;
    return result;
}
    
int JsonLex::AcceptInteger()
{
    if( !this->IsNext( Token::Integer ) )
    {
        this->JsonFail();
        return 0;
    }
    
    auto result = this->_lex.AcceptInteger();
    this->value_ended();
    return result;
}

double JsonLex::AcceptDouble()
{
    if( !this->IsNext( Token::Double ) )
    {
        this->JsonFail();
        return 0.0;
    }
    
    auto result = this->_lex.AcceptDouble();
    this->value_ended();
    return result;
}

std::string JsonLex::AcceptString()
{
    if( !this->IsNext( Token::String ) )
    {
        this->JsonFail();
        return "";
    }
    
    std::string result = this->_lex.AcceptString();
    this->value_ended();
    return result;
}

bool JsonLex::AcceptBool()
{
    if( !this->IsNext( Token::Bool ) )
    {
        this->JsonFail();
        return false;
    }
    
    bool result;
    if( this->_lex.IsNextKeyword( "true" ) )
    {
        this->_lex.AcceptKeyword( "true" );
        result = true;
    }
    else
    {
        this->_lex.AcceptKeyword( "false" );
        result = false;
    }
    this->value_ended();
    return result;
}

void JsonLex::AcceptNull()
{
    if( !this->IsNext( Token::Null ) )
    {
        this->JsonFail();
        return;
    }
    
    this->_lex.AcceptKeyword( "null" );
    this->value_ended();
}

}
