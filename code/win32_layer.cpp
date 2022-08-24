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

static std::string identifier_str;
static double numeric_value;

static FILE *source;

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
        std::string num_str;
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

int main() {
    auto b = isalnum(':');
    fopen_s(&source, "main.kal", "r");

    while (true) {
        auto tok = get_tok();
        if (tok == Tok_EOF)
            break;
    }
}
