#include "Pratt.hpp"

#include <functional>

namespace iv
{

Pratt::Pratt( Instance * inst ) :
    cm( inst, this, "Pratt" ),
    inst( inst ),
    lex( nullptr ),
    _root( nullptr ),
    value_name( -1 )
{
}

Pratt::~Pratt()
{
    this->clear();
}

Instance * Pratt::instance()
{
    return this->inst;
}

void Pratt::define_operators( Lex * lex )
{
    this->operators.erase( "" );
    for( std::string const & op : this->operators )
        lex->DefineOperator( op.c_str() );
}

void Pratt::read_expression( Lex * lex )
{
    this->clear();
    this->lex = lex;
    this->_root = this->expr( 0, "" );
    if( this->lex->Failed() )
        this->clear();
    this->lex = nullptr;
}

Pratt::Expression const * Pratt::root()
{
    return this->_root;
}

void Pratt::clear()
{
    std::function< void( Expression const * ) > del = [ &del ]( Expression const * expr )
    {
        if( !expr )
            return;
        
        auto * unary = expr->ToUnary();
        if( unary )
            del( unary->child );
        
        auto * binary = expr->ToBinary();
        if( binary )
        {
            del( binary->left );
            del( binary->right );
        }
        
        delete expr;
    };
    
    del( this->_root );
}

void Pratt::def_value( int name )
{
    this->value_name = name;
}

void Pratt::def_unary( int name, int precedence, const char * op_left, bool child_mandatory, const char * op_right, bool phantom )
{
    Def * def = new Def;
    def->name = name;
    def->precedence = precedence;
    def->unary = true;
    def->phantom = phantom;
    def->op_left = op_left;
    def->child_mandatory = child_mandatory;
    def->op_middle = "";
    def->right_child_mandatory = false;
    def->op_right = op_right;
    
    // checks and inserts
    if( def->op_left.size() )
    {
        if( this->defs_prefix.count( def->op_left ) )
        {
            this->cm.warning( SRC_INFO, "Can not define unary operation. An operation with the same prefix operand '", def->op_left, "' already exists." );
            delete def;
            return;
        }
        
        this->defs_prefix[ def->op_left ] = def;
    }
    else if( def->op_right.size() )
    {
        if( this->defs_nonprefix.count( def->op_right ) )
        {
            this->cm.warning( SRC_INFO, "Can not define unary operation. An operation with the same non-prefix operand '", def->op_right, "' already exists." );
            delete def;
            return;
        }
        
        if( !child_mandatory )
        {
            if( this->defs_prefix.count( def->op_right ) )
            {
                this->cm.warning( SRC_INFO, "Can not define unary operation. An operation with the same prefix operand '", def->op_right, "' already exists. Consider making child expression mandatory for this operation (it will not colide with prefix operations)." );
                delete def;
                return;
            }
            
            this->defs_prefix[ def->op_right ] = def;
        }
        
        this->defs_nonprefix[ def->op_right ] = def;
    }
    else
    {
        this->cm.warning( SRC_INFO, "Can not define unary operation. Neither prefix nor postfix operator is specified." );
        delete def;
        return;
    }
    
    this->defs.insert( def );
    
    this->operators.insert( op_left );
    this->operators.insert( op_right );
}

void Pratt::def_binary( int name, int precedence, const char * op_left, bool left_child_mandatory, const char * op_middle, bool right_child_mandatory, const char * op_right )
{
    Def * def = new Def;
    def->name = name;
    def->precedence = precedence;
    def->unary = false;
    def->phantom = false;
    def->op_left = op_left;
    def->child_mandatory = left_child_mandatory;
    def->op_middle = op_middle;
    def->right_child_mandatory = right_child_mandatory;
    def->op_right = op_right;
    
    if( def->op_middle.empty() )
    {
        this->cm.warning( SRC_INFO, "Can not define binary operation. Infix operator is not specified." );
        delete def;
        return;
    }
    
    if( def->op_left.size() )
    {
        if( this->defs_prefix.count( def->op_left ) )
        {
            this->cm.warning( SRC_INFO, "Can not define binary operation. An operation with the same prefix operand '", def->op_left, "' already exists." );
            delete def;
            return;
        }
        
        this->defs_prefix[ def->op_left ] = def;
    }
    else
    {
        if( this->defs_nonprefix.count( def->op_middle ) )
        {
            this->cm.warning( SRC_INFO, "Can not define binary operation. An operation with the same non-prefix operand '", def->op_middle, "' already exists." );
            delete def;
            return;
        }
        
        if( !def->child_mandatory )
        {
            if( this->defs_prefix.count( def->op_middle ) )
            {
                this->cm.warning( SRC_INFO, "Can not define binary operation. An operation with the same prefix operand '", def->op_middle, "' already exists. Consider making left child expression mandatory for this operation (it will not colide with prefix operations)." );
                delete def;
                return;
            }
            
            this->defs_prefix[ def->op_middle ] = def;
        }
        
        this->defs_nonprefix[ def->op_middle ] = def;
    }
    
    this->defs.insert( def );
    
    this->operators.insert( op_left );
    this->operators.insert( op_middle );
    this->operators.insert( op_right );
}

Pratt::Expression * Pratt::expr( int precedence, const char * terminator )
{
    if( this->lex->Failed() )
        return nullptr;
    
    if( this->lex->IsNext( Lex::Operator ) )
    {
        std::string next = this->lex->GetNextTokenValue();
        if( next == terminator )
        {
            return nullptr;
        }
        else if( this->defs_prefix.count( next ) )
        { // prefix operator
            int line = this->lex->GetLine();
            int column = this->lex->GetColumn();
    
            this->lex->Accept( Lex::Operator );
            Def * def = this->defs_prefix.at( next );
            if( def->unary )
            {
                if( def->op_left.size() )
                { // unary prefix operator
                    Expression * child;
                    if( def->op_right.size() )
                        child = this->expr( -1, def->op_right.c_str() );
                    else
                        child = this->expr( precedence, terminator );
                    
                    if( def->child_mandatory && !child )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Unary operator '"<<next<<"' is missing an operand."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    if( def->op_right.size() )
                        this->lex->AcceptOperator( def->op_right.c_str() );
                    
                    if( def->phantom )
                    {
                        return child;
                    }
                    else
                    {
                        Unary * unary = new Unary;
                        unary->line = line;
                        unary->column = column;
                        unary->name = def->name;
                        unary->child = child;
                        return unary;
                    }
                }
                else
                { // unary postfix operator without child
                    if( def->child_mandatory )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Code error: Unary operator '"<<next<<"' is in defs_prefix but it has empty op_left and mandatory child."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    if( def->phantom )
                    {
                        return nullptr;
                    }
                    else
                    {
                        Unary * unary = new Unary;
                        unary->line = line;
                        unary->column = column;
                        unary->name = def->name;
                        unary->child = nullptr;
                        return unary;
                    }
                }
            }
            else
            {
                if( def->op_left.size() )
                { // binary prefix operator
                    // read child_left (op_middle is mandatory)
                    Expression * child_left = this->expr( -1, def->op_middle.c_str() );
                    
                    if( def->child_mandatory && !child_left )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Binary operator '"<<next<<"' is missing left operand."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    // read op_middle
                    this->lex->AcceptOperator( def->op_middle.c_str() );
                    
                    // read child_right
                    Expression * child_right;
                    if( def->op_right.size() )
                        child_right = this->expr( -1, def->op_right.c_str() );
                    else
                        child_right = this->expr( precedence, terminator );
                    
                    if( def->right_child_mandatory && !child_right )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Binary operator '"<<next<<"' is missing right operand."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    // accept right operator
                    if( def->op_right.size() )
                        this->lex->AcceptOperator( def->op_right.c_str() );
                    
                    // return Expression
                    Binary * binary = new Binary;
                    binary->line = line;
                    binary->column = column;
                    binary->name = def->name;
                    binary->left = child_left;
                    binary->right = child_right;
                    return binary;
                }
                else
                { // binary postfix operator without left child
                    if( def->child_mandatory )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Code error: Binary operator '"<<next<<"' is in defs_prefix but it has empty op_left and mandatory child."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    // read child_right
                    Expression * child_right;
                    if( def->op_right.size() )
                        child_right = this->expr( -1, def->op_right.c_str() );
                    else
                        child_right = this->expr( precedence, terminator );
                    
                    if( def->right_child_mandatory && !child_right )
                    {
                        this->lex->LogicFail( SS()<<"Pratt error: Binary operator '"<<next<<"' is missing right operand."<<SS::c_str() );
                        return nullptr;
                    }
                    
                    // accept right operator
                    if( def->op_right.size() )
                        this->lex->AcceptOperator( def->op_right.c_str() );
                    
                    // return Expression
                    Binary * binary = new Binary;
                    binary->line = line;
                    binary->column = column;
                    binary->name = def->name;
                    binary->left = nullptr;
                    binary->right = child_right;
                    return binary;
                }
            }
        }
        else
        { // terminate
            return nullptr;
        }
    }
    else if( this->lex->IsNext( Lex::Eof ) )
    { // terminate
        return nullptr;
    }
    else
    { // value
        // read value
        Value * value = new Value;
        value->line = this->lex->GetLine();
        value->column = this->lex->GetColumn();
        value->name = this->value_name;
        value->token = this->lex->GetNextToken();
        value->token_str = this->lex->GetNextTokenValue();
        
        this->lex->Accept( this->lex->GetNextToken() );
        
        Expression * top = value;
        
        // check next operator (non-prefix)
        while( !this->lex->Failed() && this->lex->IsNext( Lex::Operator ) )
        {
            std::string next = this->lex->GetNextTokenValue();
            
            if( next == terminator )
                break;
            
            if( !this->defs_nonprefix.count( next ) )
                break;
            
            Def * def = this->defs_nonprefix.at( next );
            if( def->precedence <= precedence )
                break;
            
            // accept operator
            int line = this->lex->GetLine();
            int column = this->lex->GetColumn();
            this->lex->Accept( Lex::Operator );
            
            if( def->unary )
            { // unary non-prefix operator
                if( def->phantom )
                {
                    ;   // keep 'top' as it is
                }
                else
                {
                    Unary * unary = new Unary;
                    unary->line = line;
                    unary->column = column;
                    unary->name = def->name;
                    unary->child = top;
                    top = unary;
                }
            }
            else
            { // binary non-prefix operator
                // read right expression
                Expression * child_right;
                if( def->op_right.size() )
                    child_right = this->expr( -1, def->op_right.c_str() );
                else
                    child_right = this->expr( precedence, terminator );
                
                if( def->right_child_mandatory && !child_right )
                {
                    this->lex->LogicFail( SS()<<"Pratt error: Binary operator '"<<next<<"' is missing right operand."<<SS::c_str() );
                    continue;
                }
                
                if( def->op_right.size() )
                    this->lex->AcceptOperator( def->op_right.c_str() );
                
                // create expression
                Binary * binary = new Binary;
                binary->line = line;
                binary->column = column;
                binary->name = def->name;
                binary->left = top;
                binary->right = child_right;
                top = binary;
            }
        }
        
        return top;
    }
}

}
