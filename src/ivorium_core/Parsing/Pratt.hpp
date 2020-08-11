#pragma once

#include "Lex.hpp"

#include <unordered_map>

namespace iv
{

/**
    This reads expression from given Lex analyzer and builds its syntactic tree.
    Expression supports unary prefix, binary infix and unary postfix operators with custom precedence, parentheses (to change operator precedence) and funcalls .
    A funcall begins with a left parenthesis after an expression. Inside parentheses is a list of arguments (expressions) separated with ',' symbol.
    Symbol ',' used as function argument separator will always take precedence over its use as prefix, infix or postfix operator (use double parentheses to force its use as postfix, infix or prefix operator).
    
    When reading expression, Pratt reads exactly one expression (expression not folowed by infix or postfix operator).
*/
class Pratt
{
public:
    struct Unary;
    struct Binary;
    struct Value;
    
    struct Expression
    {
        int line;
        int column;
        
        int name;
        
        virtual ~Expression(){}
        virtual Unary const * ToUnary() const   { return nullptr; }
        virtual Binary const * ToBinary() const { return nullptr; }
        virtual Value const * ToValue() const { return nullptr; }
    };
    
    struct Unary : public Expression
    {
        Expression const * child;
        
        virtual Unary const * ToUnary() const override { return this; }
    };

    struct Binary : public Expression
    {
        Expression const * left;
        Expression const * right;
        
        virtual Binary const * ToBinary() const override { return this; }
    };
    
    struct Value : public Expression
    {
        /**
            Token can be: Keyword, String, Variable, Integer or Double.
            Use atoi on token_str to get Integer value, use atof on token_str to get Double value.
        */
        Lex::Token token;
        std::string token_str;
        
        virtual Value const * ToValue() const override { return this; }
    };

public:
ClientMarker cm;

                    Pratt( Instance * inst );
                    ~Pratt();

    Instance *      instance();

    //------------- define expression structure ------------------------------------------------------
    /**
        Define any unary (prefix or postfix) operation.
        Higher precedence means that the operation is more inside.
        \param phantom If set to true, operators will be ignored and child expression will be passed as child of parent expression directly.
            Use this for operators that change precedence but have no other effect, typicaly parentheses.
        \param precedence Not valid for every operators (operators with both op_left and op_right does not use parameter precedence).
            Default precedence is -1, so do not use negative precedence of operators.
        \param name
        \param op_left
        \param child_mandatory
        \param op_right
    */
    void            def_unary( int name, int precedence, const char * op_left, bool child_mandatory, const char * op_right, bool phantom=false );
    
    /**
        Similar to def_unary.
        Op_middle is mandatory.
        Operator is also registered as unary operator if op_left is empty and left_child_mandatory is false.
    */
    void            def_binary( int name, int precedence, const char * op_left, bool left_child_mandatory, const char * op_middle, bool right_child_mandatory, const char * op_right );
    void            def_value( int name );
    
    //----------------------------------------------------------------------------------------------------------
    /**
        Tells given Lex about defined operators.
    */
    void            define_operators( Lex * lex );
    
    /**
        Parses one expression from given Lex instance.
        The parsed syntax tree is accessible through Pratt::root method.
        Pratt::define_operators should be called on this instance before read_expression is called.
    */
    void            read_expression( Lex * lex );
    
    /**
        Deletes syntax tree created in Pratt::read_expression method.
        Does not have to be called; Pratt::read_expression clears old instance; Pratt::~Pratt also calls does this.
    */
    void            clear();
    
    //------------------------------------------------------------------------------------------------------
    /**
        Returns syntax tree created by Pratt::read_expression method.
        Returns nullptr if read_expression was not called yet or if there was an error during reading or if there was just nothing to read (for example if current Lex token is operator unknown to Pratt parser).
    */
    Expression const *  root();

//------------------------------------------------------------------------------------------------------
private:
    Expression *    expr( int precedence, const char * terminator );
    
private:
    struct Def
    {
        int name;
        int precedence;
        bool unary;
        bool phantom;                   ///< valid only if unary==true
        
        std::string op_left;
        bool child_mandatory;
        std::string op_middle;          ///< valid only if unary==false
        bool right_child_mandatory;     ///< valid only if unary==false
        std::string op_right;
    };
    
private:
    Instance * inst;
    Lex * lex;
    Expression * _root;
    std::unordered_set< Def * > defs;
    
    std::unordered_set< std::string > operators;
    
    std::unordered_map< std::string, Def * > defs_prefix;   ///< key is op_left (if it is set) or op_right (if op_left is not set and child_mandatory==false)
    std::unordered_map< std::string, Def * > defs_nonprefix;    ///< key is either op_middle (for binary operator) or op_right (for unary operator)
    int value_name;
};

}
