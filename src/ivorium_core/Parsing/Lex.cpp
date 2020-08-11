#include "Lex.hpp"

#include "../Defs.hpp"

#include "../Basics/SS.hpp"
#include "../Instancing/Instance.hpp"

#include <cctype>

namespace iv
{

Lex::Lex( Instance * inst ) :
    cm( inst, this, "Lex" ),
    inst( inst ),
    _single_line( false ),
    _numbers_enabled( true ),
    in( nullptr ),
    line( 0 ),
    column( 0 ),
    prev_line( 0 ),
    prev_column( 0 ),
    failed( false ),
    newlines( 0 ),
    newline_skipping( true ),
    token( Eof )
{
}

Instance * Lex::instance() const
{
    return this->inst;
}

void Lex::DefineKeyword( const char * keyword )
{
    this->keywords.insert( keyword );
}

void Lex::DefineOperator( const char * op )
{
    this->operators.insert( op );
}

void Lex::DisableNewlineSkipping()
{
    this->newline_skipping = false;
}

void Lex::EnableNewlineSkipping()
{
    this->newline_skipping = true;
}

void Lex::numbers_enabled( bool enabled )
{
    this->_numbers_enabled = enabled;
}

void Lex::Init( std::istream & in, bool single_line )
{
    this->cm.log( SRC_INFO, Defs::Log::Lex, "Init", ( single_line ? " single_line" : "" ), "." );
    
    this->_single_line = single_line;
    
    this->in = &in;
    this->line = 0;
    this->column = 0;
    this->failed = false;
    
    this->PrecomputeOperators();
    
    this->NextChar();
    this->NextToken();
}

void Lex::Close()
{
    this->cm.log( SRC_INFO, Defs::Log::Lex, "Close." );
    
    this->in = nullptr;
    this->token = Eof;
    this->newlines = 0;
}

//------------------------------------------ lexan ----------------------------------------------------------------
void Lex::NextChar()
{
    if( this->failed || !this->in || !this->in->good() )
    {
        this->c = std::char_traits< char >::eof();
        return;
    }
    
    this->c = this->in->get();
    
    this->column++;
    if( this->c == '\n' )
    {
        this->line++;
        this->column = 0;
    }
    
    if( !this->in->eof() && this->in->fail() )
    {
        this->c = std::char_traits< char >::eof();
        this->Error( "Input error: Error reading input stream." );
        this->failed = true;
        this->Close();
    }
}

void Lex::PrecomputeOperators()
{
    for( auto & op : this->operators )
    {
        this->operators_begins.insert( op[0] );
        
        std::string prefix;
        for( char a : op )
        {
            prefix += a;
            this->operators_prefixes.insert( prefix );
        }
    }
}

void Lex::ReadNumber( bool negative )
{
    std::string str;
    
    if( negative )
        str += '-';
    
    // read numbers
    while( this->c >= '0' && this->c <= '9' )
    {
        str += this->c;
        this->NextChar();
    }
    
    if( this->c != '.' )
    { // Integer
        this->token = Integer;
        this->token_str = str;
    }
    else
    { // Double
        str += '.';
        this->NextChar();
        
        // read numbers after dot
        while( this->c >= '0' && this->c <= '9' )
        {
            str += this->c;
            this->NextChar();
        }
        
        if( this->c == 'e' )
        {
            this->NextChar();
            if( this->c == '-' )
            {
                str += this->c;
                NextChar();
            }
            
            if( this->c < '0' || this->c > '9' )
            {
                this->Error( "Lex error: No number after 'e' in floating point literal." );
                this->failed = true;
                this->Close();
                return;
            }
            
            while( this->c >= '0' && this->c <= '9' )
            {
                str += this->c;
                this->NextChar();
            }
        }
        
        this->token = Double;
        this->token_str = str;
    }
    
    if( !this->isspace( this->c ) && !this->operators_begins.count( this->c ) )
    {
        this->Error( "Lex error: Number must be followed by a whitespace character or by an operator." );
        this->failed = true;
        this->Close();
        return;
    }
}

void Lex::ReadIdentifier()
{
    char first = this->c;
    this->NextChar();
    
    std::string str;
    str += first;
    
    if( this->operators_begins.count( first ) )
    {
        while( this->c != std::char_traits< char >::eof() && this->operators_prefixes.count( str + (char)this->c ) )
        {
            str += this->c;
            this->NextChar();
        }
        
        this->token = Operator;
        this->token_str = str;
        return;
    }
    else
    {
        while( this->c != std::char_traits< char >::eof() && !this->isspace( this->c ) && !this->operators_begins.count( this->c ) )
        {
            str += this->c;
            this->NextChar();
        }
        
        this->token_str = str;
        if( this->keywords.count( str ) )
            this->token = Keyword;
        else
            this->token = Name;
        return;
    }
}

bool Lex::isspace( char c )
{
    return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

void Lex::NextToken()
{
    this->prev_line = this->line;
    this->prev_column = this->column;
    this->NextTokenImpl();
    this->cm.log( SRC_INFO, Defs::Log::Lex, "NextToken: ", this->TokenToStr( this->token, this->token_str.c_str() ), "  ", this->token_str );
}

void Lex::NextTokenImpl()
{
    // reset things
    this->newlines = 0;
    this->tested_tokens.clear();
    this->tested_keywords.clear();
    this->tested_operators.clear();
    
    // read next token
    while( 1 )
    {
        if( this->failed )
        { // fail
            return;
        }
        else if( this->c == std::char_traits< char >::eof() )
        { // eof
            this->token = Eof;
            this->token_str = "";
            this->NextChar();
            return;
        }
        else if( this->c == '\n' )
        { // newline
            this->newlines++;
            if( this->_single_line )
            {
                this->Close();
                this->newlines = 1;
                return;
            }
            else
            {
                this->NextChar();
            }
        }
        else if( this->c == '#' )
        { // comment
            while( this->c != '\n' && this->c != std::char_traits< char >::eof() )
                this->NextChar();
        }
        else if( this->isspace( this->c ) )
        { // whitespace
            this->NextChar();
        }
        else if( this->_numbers_enabled && !this->operators_begins.count( '-' ) && this->c == '-' )
        {
            this->NextChar();
            this->ReadNumber( true );
            return;
        }
        else if( this->_numbers_enabled && this->c >= '0' && this->c <= '9' )
        { // number
            this->ReadNumber( false );
            return;
        }
        else if( this->c == '"' || this->c == '\'' )
        { // string
            std::string str;
            char end = this->c;
            this->NextChar();
            bool escaped = false;
            while( this->c != end || escaped )
            {
                if( escaped )
                {
                    escaped = false;
                    if( this->c == '\\' )
                        str += '\\';
                    else if( this->c == 'n' )
                        str += '\n';
                    else if( this->c == 'r' )
                        str += '\r';
                    else if( this->c == 't' )
                        str += '\t';
                    else if( this->c == '"' )
                        str += '"';
                    else if( this->c != std::char_traits< char >::eof() )
                    {
                        this->Error( SS() << "Lex error: Invalid escaped character in string: \\" << this->c << "." << SS::c_str() );
                        this->failed = true;
                        this->Close();
                        return;
                    }
                }
                else
                {
                    if( this->c == std::char_traits< char >::eof() )
                    {
                        this->Error( "Lex error: Unexpected end of input, string not closed." );
                        this->failed = true;
                        this->Close();
                        return;
                    }
                    else if( this->c == '\n' )
                    {
                        this->Error( "Lex error: Unexpected end of line, string not closed." );
                        this->failed = true;
                        this->Close();
                        return;
                    }
                    else if( end == '"' && this->c == '\\' )
                    {
                        escaped = true;
                    }
                    else
                    {
                        str += this->c;
                    }
                }
                
                this->NextChar();
            }
            
            this->NextChar();
            this->token = String;
            this->token_str = str;
            return;
        }
        else
        { // something else
            this->ReadIdentifier();
            return;
        }
    }
}

//------------------------------------------ parser -------------------------------------------

bool Lex::IsNext( Token token )
{
    if( this->failed )
        return token==Eof;
    
    this->tested_tokens.push_back( token );
    
    if( !this->newline_skipping && this->newlines && token != Newline )
        return false;
    
    if( token == Newline )
        return this->newlines;
    else if( token == Double && this->token == Integer )
        return true;
    else
        return this->token == token;
}

bool Lex::IsNextKeyword( const char * keyword )
{
    if( this->failed )
        return false;
    
    this->tested_keywords.push_back( keyword );
    
    if( !this->newline_skipping && this->newlines )
        return false;
    
    return this->token == Keyword && this->token_str == keyword;
}

bool Lex::IsNextOperator( const char * op )
{
    if( this->failed )
        return false;
    
    this->tested_operators.push_back( op );
    
    if( !this->newline_skipping && this->newlines )
        return false;
    
    return this->token == Operator && this->token_str == op;
}

Lex::Token Lex::GetNextToken()
{
    if( !this->newline_skipping && this->newlines )
        return Newline;
    
    return this->token;
}

std::string Lex::GetNextTokenValue()
{
    if( !this->newline_skipping && this->newlines )
        return "\n";
    
    return this->token_str;
}

void Lex::AcceptFail()
{
    if( this->failed )
        return;
        
    this->ParserFail();
}

void Lex::Accept( Token token )
{
    if( this->failed )
        return;
    
    if( !this->newline_skipping && token != Newline && this->newlines )
    {
        this->tested_tokens.push_back( token );
        this->ParserFail();
        return;
    }
    
    if( this->token == token )
    {
        this->NextToken();
        return;
    }
    else if( this->token == Integer && token == Double )
    {
        this->NextToken();
        return;
    }
    else if( token == Newline && this->newlines )
    {
        this->newlines--;
        return;
    }
    else
    {
        this->tested_tokens.push_back( token );
        this->ParserFail();
        return;
    }
}

void Lex::AcceptKeyword( const char * keyword )
{
    if( !this->keywords.count( keyword ) )
    {
        this->Error( SS()<<"Trying to accept keyword '"<<keyword<<"' that was not registered using method Lex::DefineKeyword."<<SS::c_str() );
        return;
    }
    
    if( this->failed )
        return;
        
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_keywords.push_back( keyword );
        this->ParserFail();
        return;
    }
    
    if( this->token == Keyword && this->token_str == keyword )
    {
        this->NextToken();
    }
    else
    {
        this->tested_keywords.push_back( keyword );
        this->ParserFail();
        return;
    }
}

void Lex::AcceptOperator( const char * op )
{
    if( !this->operators.count( op ) )
    {
        this->Error( SS()<<"Trying to accept operator '"<<op<<"' that was not registered using method Lex::DefineOperator."<<SS::c_str() );
        return;
    }
    
    if( this->failed )
        return;
        
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_operators.push_back( op );
        this->ParserFail();
        return;
    }
    
    if( this->token == Operator && this->token_str == op )
    {
        this->NextToken();
    }
    else
    {
        this->tested_operators.push_back( op );
        this->ParserFail();
        return;
    }
}

std::string Lex::AcceptString()
{
    if( this->failed )
        return "";
    
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_tokens.push_back( String );
        this->ParserFail();
        return "";
    }
    
    if( this->token == String )
    {
        std::string result = this->token_str;
        this->NextToken();
        return result;
    }
    else
    {
        this->tested_tokens.push_back( String );
        this->ParserFail();
        return "";
    }
}

std::string Lex::AcceptName()
{
    if( this->failed )
        return "";
    
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_tokens.push_back( Name );
        this->ParserFail();
        return "";
    }
    
    if( this->token == Name )
    {
        auto result = this->token_str;
        this->NextToken();
        return result;
    }
    else
    {
        this->tested_tokens.push_back( Name );
        this->ParserFail();
        return "";
    }
}

int Lex::AcceptInteger()
{
    if( this->failed )
        return 0;
    
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_tokens.push_back( Integer );
        this->ParserFail();
        return 0;
    }
    
    if( this->token == Integer )
    {
        auto result = atoi( this->token_str.c_str() );
        this->NextToken();
        return result;
    }
    else
    {
        this->tested_tokens.push_back( Integer );
        this->ParserFail();
        return 0;
    }
}

double Lex::AcceptDouble()
{
    if( this->failed )
        return 0.0;
    
    if( !this->newline_skipping && this->newlines )
    {
        this->tested_tokens.push_back( Double );
        this->ParserFail();
        return 0.0;
    }
    
    if( this->token == Double )
    {
        auto result = atof( this->token_str.c_str() );
        this->NextToken();
        return result;
    }
    else if( this->token == Integer )
    {
        auto result = atoi( this->token_str.c_str() );
        this->NextToken();
        return double( result );
    }
    else
    {
        this->tested_tokens.push_back( Double );
        this->ParserFail();
        return 0.0;
    }
}


int Lex::GetLine() const
{
    return this->prev_line;
}

int Lex::GetColumn() const
{
    return this->prev_column;
}

std::string Lex::TokenToStr( Token token, const char * str )
{
    switch( token )
    {
        case Keyword:
            return SS()<<"Keyword( "<<str<<" )"<<SS::str();
        case Operator:
            return SS()<<"Operator( "<<str<<" )"<<SS::str();
        case Newline:
            return "Newline";
        case Eof:
            return "Eof";
        case String:
            return SS()<<"String \""<<str<<"\""<<SS::str();
        case Name:
            return SS()<<"Name( "<<str<<" )"<<SS::str();
        case Integer:
            return SS()<<"Integer "<<str<<SS::str();
        case Double:
            return SS()<<"Double "<<str<<SS::str();
        default:
            return "-unknown-";
    }
}

void Lex::LogicFail( const char * message )
{
    if( this->failed )
        return;
    
    this->Error( message );
    this->failed = true;
}

void Lex::ParserFail( const char * prefix )
{
    // we only report the first error
    if( this->failed )
        return;
    
    std::string current_token_string;
    if( !this->newline_skipping && this->newlines )
        current_token_string = this->TokenToStr( Newline, "\n" );
    else
        current_token_string = this->TokenToStr( this->token, this->token_str.c_str() );
    
    SS ss;
    if( prefix )
        ss << prefix << ": ";
    ss << "Unexpected token " << current_token_string << ". Expecting one of these tokens: ";
    
    bool first = true;
    
    for( Token t : this->tested_tokens )
    {
        if( first )
            first = false;
        else
            ss << ", ";
        
        ss << this->TokenToStr( t, "" );
    }
    
    for( auto & str : this->tested_keywords )
    {
        if( first )
            first = false;
        else
            ss << ", ";
        
        ss << this->TokenToStr( Keyword, str.c_str() );
    }
    
    for( auto & str : this->tested_operators )
    {
        if( first )
            first = false;
        else
            ss << ", ";
        
        ss << this->TokenToStr( Operator, str.c_str() );
    }
    
    ss << ".";
    this->Error( ss << SS::c_str() );
    
    this->failed = true;
}

bool Lex::Failed()
{
    return this->failed;
}

void Lex::Error( const char * msg )
{
    this->cm.warning( SRC_INFO, msg );
}

void Lex::Recover( Token token )
{
    if( !this->failed )
        return;
    
    if( !this->in )
        return;
    
    this->failed = false;
    
    if( token == Newline )
    {
        while( this->newlines==0 && this->token != Eof )
            this->NextToken();
    }
    else if( token == Double )
    {
        while( this->token != Double && this->token != Integer && this->token != Eof )
            this->NextToken();
    }
    else
    {
        while( this->token != token && this->token != Eof )
            this->NextToken();
    }
}

void Lex::RecoverKeyword( const char * keyword )
{
    if( !this->failed )
        return;
    
    if( !this->in )
        return;
    
    this->failed = false;
    
    while( !( this->token == Keyword && this->token_str == keyword ) && this->token != Eof )
        this->NextToken();
}

void Lex::RecoverOperator( const char * op )
{
    if( !this->failed )
        return;
    
    if( !this->in )
        return;
    
    this->failed = false;
    
    while( !( this->token == Operator && this->token_str == op ) && this->token != Eof )
        this->NextToken();
}
    
}
