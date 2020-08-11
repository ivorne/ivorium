#pragma once

#include "../Instancing/ClientMarker.hpp"
#include "../Basics/LogTrace.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

namespace iv
{

class Instance;

/**
    Lexical analyzer and framework for simple LL parsers.
*/
class Lex
{
public:
ClientMarker cm;

                    Lex( Instance * inst );
    Instance *      instance() const;
    
    //----------------- configuration --------------------------
    void            DefineKeyword( const char * keyword );
    void            DefineOperator( const char * op );
    
    /**
        Numbers are enabled by default.
    */
    void            numbers_enabled( bool enabled );
    
    //----------------- tokens ---------------------------------
    /**
        Comments begin with # and end with newline.
    */
    enum Token
    {
        Keyword,        // keyword must have whitespace characters on both sides to be considered keyword ; can not begin with a number ; can not contain whitespaces and operators
        Operator,       // operator that does not have to be separated with whitespace characters from variables ; can not begin with a number ; can not contain whitespaces ; all prefixes of specified operators are also operators
        Newline,        // newline is skipped if not explicitly requested
        Eof,            // end of input
        
        String,         // anything that has quotes or apostrophes on both sides; quoted strings support escaping with backslash: \" -> ", \n -> newline, \r -> cr, \t -> tab, \\ -> backslash; Other escaped characters yield error
        Name,           // anything not containing whitespace characters that is not anything else ; can not begin with a number ; can not contain whitespaces and operators
        Integer,        // sequence of numbers (0-9); Whenever Double is requested in Lex's interface, Integer can substitute for it. Negative values work under the same conditions as for Double numbers.
        Double          // Examples: 1.3, 1e3, 1e-3, 1.e-5, 5.5e-5, if operator '-' is defined, then this can only be positive (and it is up on user code to parse the minus and negate the number).
    };
    
    //----------------- parsing --------------------------------
    /**
        Starts parsing of given input.
        First token is retrieved and pointer to stream is saved.
        Stream is released when Token::Eof is read or when Fail is called.
        So if we stop parsing before Eof is reached, Fail should be called.
        Param fileid is only used in error reports to tell user, which file contains given syntactic error.
    */
    void            Init( std::istream & in, bool single_line = false );
    void            Close();
    
    /**
        Newline skipping is enabled by default.
        When newline skipping is enabled, the newlines will be processed (skipped) as whitespaces.
        Otherwise each newline is considered a specific token Newline and needs to be accepted before processing tokens that follow on next line.
    */
    void            DisableNewlineSkipping();
    void            EnableNewlineSkipping();
    
    /**
        Checks if next token is that what is given.
        Parser remembers which tokens were tested so they can be reported on potential Fail.
    */
    bool            IsNext( Token token );
    bool            IsNextKeyword( const char * keyword );
    bool            IsNextOperator( const char * op );
    
    /**
        Try not to use these two methods.
        They do not give information about acceptable values in errors.
        They also do not support the fact that Integers can be used in place where Double is requested.
    */
    Token           GetNextToken();
    std::string     GetNextTokenValue();
    
    /**
        Accept - reads input and goes to next input.
        Might change FAIL flag to true.
    */
    void            Accept( Token token );
    void            AcceptKeyword( const char * keyword );
    void            AcceptOperator( const char * op );
    void            AcceptFail();   ///< No more options for accept are available. This accept always fails.
    
    std::string     AcceptString();
    std::string     AcceptName();
    int             AcceptInteger();
    double          AcceptDouble();
    
    /**
        This can be used to recover after a parser or logic fail (will not be effective in case of lexan or input fail).
        If Lex is not failed, this will do nothing.
        If Lex is failed, tokens will be read until specified token is found (or Eof occures).
        This token will be the next token (IsNext, can be Accepted) after recovery.
        In case of unrecoverable fail (IO or lexan fail), this will also have no effect.
    */
    void            Recover( Token token );
    void            RecoverKeyword( const char * keyword );
    void            RecoverOperator( const char * op );
    
    /**
        Use this if there is possibility that this will be passed to LogicFail later.
    */
    int             GetLine() const;
    int             GetColumn() const;
    
    /**
        Sets FAIL flag to true, reports current line and column and which is current token and reports which tokens were tested using IsNext* methods.
    */
    void            LogicFail( const char * message );
    
    /**
    */
    void            ParserFail( const char * prefix  = nullptr);
    
    /**
        Returns value of a FAIL flag.
    */
    bool            Failed();
    
private:
    void            NextChar();
    void            NextToken();
    void            NextTokenImpl();
    void            Error( const char * msg );
    void            ReadNumber( bool negative );
    void            ReadIdentifier();   ///< reads Keyword, Operator or Name; first char of that is given as parameter (current this->c is second char)
    void            PrecomputeOperators();
    std::string     TokenToStr( Token token, const char * associated_token_str );
    bool            isspace( char c );
    
private:
    Instance * inst;
    bool            _single_line;

    std::unordered_set< std::string >   keywords;
    std::unordered_set< std::string >   operators;
    bool                                _numbers_enabled;
    
    std::istream *  in;
    int             line;
    int             column;
    int             prev_line;
    int             prev_column;
    
    bool            failed;
    
    // current token
    int             newlines;       ///< newlines are usually skipped; this contains number of newlines skipped between current token and previous token
    bool            newline_skipping;
    Token           token;
    std::string     token_str;      ///< valid if current token is Keyword, Operator, String, Name, Integer or Double
    
    // current char
    int             c;
    
    // tested tokens
    std::vector< Token >        tested_tokens;
    std::vector< std::string >  tested_keywords;
    std::vector< std::string >  tested_operators;
    
    // precomputed operators
    std::unordered_set< char > operators_begins;
    std::unordered_set< std::string > operators_prefixes;   ///< also full operators ; this is used as set of operators when parsing
};

class Lex_LogTrace : public LogTrace
{
public:
    Lex_LogTrace( Lex const & lex ) :
        lex( lex )
    {
    }
    
protected:
    virtual void PrintTraceLine( std::ostream & out ) override
    {
        out << "Line "<< ( lex.GetLine()+1 ) << ":" << ( lex.GetColumn()+1 ) << ":" << std::endl;
    }
    
private:
    Lex const & lex;
};

}
