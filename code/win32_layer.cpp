#include "datatypes.h"

#if _INTERNAL
    #define output_string(s, ...)        {char Buffer[100];sprintf_s(Buffer, s, __VA_ARGS__);OutputDebugStringA(Buffer);}
    #define throw_error_and_exit(e, ...) {output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__); getchar(); global_error = true;}
    #define throw_error(e, ...)           output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__)
    #define inform(i, ...)                output_string(" ------------------------------[INFO] "    ## i, __VA_ARGS__)
    #define warn(w, ...)                  output_string(" ------------------------------[WARNING] " ## w, __VA_ARGS__)
    #define assert(expr) if(!(expr)) {*(int *)0 = 0;}
#else
    #define output_string(s, ...)
    #define throw_error_and_exit(e, ...)
    #define throw_error(e, ...)
    #define inform(i, ...)
    #define warn(w, ...)
    #define assert(expr)
#endif

#include <string>

enum Token {
    Tok_EOI = -1,
    Tok_EOF = -2,

    Tok_Def = -3,

    Tok_Identifier = -4,
    Tok_Number     = -5,
};

using namespace std;

// ===============================================================
// Lexer
// ===============================================================

static string identifier_str;
static double numeric_value;

static FILE *source;

static int current_tok;

static int get_tok() {
tokenization_beg: 
    static int last_char = ' ';

    while (isspace(last_char))
        last_char = fgetc(source);

    if (last_char == EOF) {
        return Tok_EOF;
    }
    if (last_char == ';') {
        last_char = fgetc(source);
        return Tok_EOI;
    }
    else if (last_char == '/') {
        last_char = fgetc(source);
        // @todo: handle division
        if (last_char == '/') // comment
        {
            do
                last_char = fgetc(source);
            while (last_char != EOF && last_char != '\n' && last_char != '\r');
        }

        goto tokenization_beg;
    }
    else if (last_char == ':') {
        last_char = fgetc(source);
        if (last_char == ':') // comment
        {
            return Tok_Def;
        }
    }
    else if (isalpha(last_char))
    {
        identifier_str = (char)last_char;

        while (isalnum(last_char = fgetc(source)))
            identifier_str += (char)last_char;

        return Tok_Identifier;
    }
    else if (isdigit(last_char))
    {
        string num_str;
        do {
            num_str  += (char)last_char;
            last_char = fgetc(source);
        } while (isdigit(last_char) || last_char == '.');

        numeric_value = strtod(num_str.c_str(), 0);

        return Tok_Number;
    }

    int this_char = last_char;
    last_char = fgetc(source);
    return this_char;
}

static int get_next_tok() {
    return current_tok = get_tok();
}

// ===============================================================
// Parser
// ===============================================================

struct AST_Expression {
    virtual ~AST_Expression() {}
};

class AST_Number : public AST_Expression {
    AST_Number(double _value) : value(_value) {}

    double value;
};

class AST_Variable : public AST_Expression {
    AST_Variable(string _name) : value(_name) {}

    string name;
};

class AST_Binary_Expr : public AST_Expression {
    AST_Binary_Expr(char _op,
                    unique_ptr<AST_Expression> _lhs,
                    unique_ptr<AST_Expression> _rhs)
        : op(_op),
          lhs(move(_lhs)),
          rhs(move(_rhs)) {}

    unique_ptr<AST_Expression> lhs;
    unique_ptr<AST_Expression> rhs;
    char op;
};

class AST_Func_Call : public AST_Expression {
    AST_Func_Call(string _func_name,
                  vector<unique_ptr<AST_Expression>> _args)
        : func_name(_func_name),
          args(_args) {}

    string func_name;
    vector<unique_ptr<AST_Expression>> args;
};

struct AST_Func_Prototype {
    PrototypeAST(string _name, vector<string> _args)
        : name(_name), args(move(_args)) {}

    const std::string &getName() const { return Name; }

    string name;
    vector<string> args;
};

struct AST_Function_Body {
    unique_ptr<AST_Func_Prototype> prototype;
    unique_ptr<AST_Expression>     body;

    AST_Function_Body(unique_ptr<AST_Func_Prototype> _prototype,
                      unique_ptr<AST_Expression>     _body)
        : prototype(move(_prototype)),
          body(move(_body)) {}
};

/// LogError* - These are little helper functions for error handling.
unique_ptr<ExprAST> log_error(char *str) {
  fprintf(stderr, "LogError: %s\n", str);
  return nullptr;
}
unique_ptr<PrototypeAST> log_error_p(char *str) {
  log_error(str);
  return nullptr;
}

// Parsing

static unique_ptr<AST_Expression> parse_num_expr() {
    auto result = make_unique<AST_Number>(numeric_value);
    get_next_tok();
    return move(result);
}

static unique_ptr<AST_Expression> parse_paren_expr() {
    get_next_tok();

    auto v = parse_expression();

    if (!v)
        return nullptr;

    if (current_tok != ')')
        return log_error("expected ')'");
    get_next_tok();

    return v;
}

static unique_ptr<AST_Expression> parse_identifier_expr() {
    string id_name;

    get_next_tok();

    if (current_tok != '(') // simple variable ref.
        return make_unique<AST_Variable>(id_name);

    // function call
    get_next_tok();
    vector<unique_ptr<AST_Expression>> args;
    if (current_tok != ')') {
        while (true) {
            if (auto arg = parse_expression()) {
                args.push_back(move(arg));
            }
            else return nullptr;

            if (current_tok == ')')
                break;

            if (current_tok != ',')
                return log_error("expected ')' or ',' in argument list");

            get_next_tok();
        }
    }

    get_next_tok();
    return make_unique<AST_Func_Call>(id_name, move(args));
}

static unique_ptr<AST_Expression> parse_current_token() {
    switch(current_tok) {
        default:             return log_error("Unknown token");
        case Tok_Identifier: return parse_identifier_expr();
        case Tok_Number:     return parse_num_expr();
        case '(':            return parse_paren_expr();
    }
}

int main() {
    auto b = isalnum(':');
    fopen_s(&source, "main.kal", "r");

    while (true) {
        auto tok = get_tok();
        if (tok == Tok_EOF)
            break;
    }
}
