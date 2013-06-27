#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include <fstream>

using namespace std;

namespace Tok
{
enum TOKEN_TYPES
{
    TOK_EOF = 0,
    TOK_DELIM,
    TOK_NUMBER,
    TOK_IDENT,
    TOK_LITERAL,
};

class Tokenizer
{
public:
    Tokenizer();
    ~Tokenizer();
    bool open(string name);
    int getToken();
    const string tok();
    int type();
    int getLine() const;
    bool eof();
    bool check(const char *str, bool consume=true);
    bool checkNumber(bool consume=true);
    bool checkLiteral(bool consume=true);
    int toInt();
    float toFloat();
    void consumeToken();
    string rest();
    void rewind();
private:
    bool logLine = false;
    // variável responsável por nos dizer se o token pretendido tem
    // que ser lido do arquivo.
    bool isTokenInBuffer;
    int tokType;
    bool isWhiteLine();
    bool loadLine();
    bool isDelimiter(char c);
    ifstream input;
    string line;
    string token;
    size_t linePos;
    size_t currentLine;
    size_t lastToken;
    bool operator==(const char *str);
    bool operator!=(const char *str);
public:
    void setLogLine(bool v)
    {
        logLine = v;
    }
};

}

#endif //__TOKENIZER_HPP__
