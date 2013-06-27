#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Tokenizer.hpp"

#define strcmpi strcasecmp

using namespace std;

namespace Tok
{


//==============================================================================
// /////////////////////////////////////////////////////////////////////////////
//                            Analisador léxico
// /////////////////////////////////////////////////////////////////////////////
//==============================================================================

//==============================================================================
// ctor
//==============================================================================
Tokenizer::Tokenizer()
{
    isTokenInBuffer = false;
    tokType = TOK_EOF;
    token = "";
    line = "";
    linePos = 0;
    currentLine = 0;
    lastToken = 0;
}

//==============================================================================
// dtor
//==============================================================================
Tokenizer::~Tokenizer()
{
}

//==============================================================================
// Retorna a linha atual no arquivo
//==============================================================================
int Tokenizer::getLine() const
{
    return currentLine;
}

//==============================================================================
// Retorna o tipo do token atual
//==============================================================================
int Tokenizer::type()
{
    return tokType;
}

//==============================================================================
// Retorna verdadeiro se o fim do arquivo ja foi alcançado
//==============================================================================
bool Tokenizer::eof()
{
    return input.eof() && (linePos >= line.size());
}

//==============================================================================
// Retorna o valor inteiro do token atual
//==============================================================================
int Tokenizer::toInt()
{
    const char *str = token.c_str();
    if (str)
        return atoi(str);
    return 0;
}

//==============================================================================
// Retorna o valor em float do token atual
//==============================================================================
float Tokenizer::toFloat()
{
    const char *str = token.c_str();
    if (str)
        return atof(str);
    return 0.0f;
}

//==============================================================================
// Retorna o token atual
//==============================================================================
const string Tokenizer::tok()
{
    return token;
}

//==============================================================================
// Verifica se o token atual é igual a str, se consume for true, então se o
// token for igual a str esse token será consumido, e o próximo token será lido
// do buffer.
//==============================================================================
bool Tokenizer::check(const char *str, bool consume)
{
    // Verifica se é necessário buscar o token no buffer
    if (isTokenInBuffer)
        if (!getToken())
            return false;
    if (!strcmpi(token.c_str(), str))
    {
        if (consume)
            isTokenInBuffer = true;
        return true;
    }
    return false;
}

//==============================================================================
// Verifica se o token atual é um número, se consume for true, então esse
// token será consumido, e o próximo token será lido do buffer
//==============================================================================
bool Tokenizer::checkNumber(bool consume)
{
    if (isTokenInBuffer)
        if (!getToken())
            return false;
    if (tokType == TOK_NUMBER)
    {
        if (consume)
            isTokenInBuffer = true;
        return true;
    }
    return false;
}

//==============================================================================
// Verifica se o token atual é um literal, se consume for true, então esse
// token será consumido, e o próximo token será lido do buffer
//==============================================================================
bool Tokenizer::checkLiteral(bool consume)
{
    if (isTokenInBuffer)
        if (!getToken())
            return false;
    if (tokType == TOK_LITERAL)
    {
        if (consume)
            isTokenInBuffer = true;
        return true;
    }
    return false;
}

//==============================================================================
// Consome o token atual, note que isso apenas torna verdadeiro o flag
// isTokenInBuffer, pois quando getToken for chamado verificará por isso.
//==============================================================================
void Tokenizer::consumeToken()
{
    isTokenInBuffer = true;
}

//==============================================================================
// Abre um arquivo para ser utilizado como buffer.
//==============================================================================
bool Tokenizer::open(string name)
{
    if (input.is_open())
        input.close();
    input.open(name.c_str(), ios_base::in);
    if (!input.is_open())
    {
        cerr << "Nao foi possivel carregar o arquivo de animacoes" << endl;
        return false;
    }
    currentLine = 0;
    isTokenInBuffer = true;
    return true;
}

//==============================================================================
// Verifica se um carácter é um delimitador
//==============================================================================
bool Tokenizer::isDelimiter(char c)
{
    if (!c)
        return false;
    if (strchr("[]=,:", c))
        return true;
    return false;
}

//==============================================================================
// Função principal do lexer, responsável por analisar o buffer, e chamar o
// a alimentador do buffer sempre que necessário.
//==============================================================================
int Tokenizer::getToken()
{
    if (!isTokenInBuffer)
        return tokType;

    isTokenInBuffer = false;
    tokType = TOK_EOF;

    // ignora espaços em branco
    while ((linePos < line.size() && isspace(line[linePos])))
        linePos++;

    // verifica se a string é nula ou a posição está no fim
    if ((line == "") || (linePos >= line.size()))
    {
        if (!loadLine())
            return tokType;
    }

    // ignora espaços em branco
    while ((linePos < line.size() && isspace(line[linePos])))
        linePos++;


    token = "";

    // Primeiro verificamos se é um delimitador
    if (isDelimiter(line[linePos]))
    {
        tokType = TOK_DELIM;
        token = line[linePos];
        linePos++;

        // Verifica se temos um literal (quoted strings)
    }
    else if (line[linePos] == '"')
    {
        linePos++;
        size_t finalPos = linePos + 1;
        while (line[finalPos] != '"' && line[finalPos])
            finalPos++;
        tokType = TOK_LITERAL;
        token = line.substr(linePos, (finalPos - linePos));
        if (line[finalPos] == '"')
            finalPos++;
        linePos = finalPos;

        // Agora verificamos se é um número
    }
    else if (isdigit(line[linePos]))
    {
_number:
        size_t finalPos = linePos;
        while (isdigit(line[finalPos]) || line[finalPos] == '.')
            finalPos++;
        // copia a parte da string que é numérica
        token += line.substr(linePos, (finalPos - linePos));
        linePos = finalPos;
        tokType = TOK_NUMBER;

        // Verifica se é do tipo alpha
    }
    else if (isalpha(line[linePos]) && !isspace(line[linePos]))
    {
        size_t finalPos = linePos;

        while (!isDelimiter(line[finalPos]) && finalPos < line.size() && !isspace(line[finalPos]))
        {
            // por alguma razão que desconheço utilizar isspace não funcioná,
            // porém fazendo essa simples comparação funciona muito bem.
            finalPos++;
        }
        if (finalPos != linePos)
            token = line.substr(linePos, (finalPos - linePos));
        //if (isspace(line[finalPos]))
        //    finalPos++;
        linePos = finalPos;
        tokType = TOK_IDENT;

        // verifica por números sinalizados, apenas copiamos o sinal e saltamos para
        // a parte encarregada de copiar os digitos.
    }
    else if ((line[linePos] == '-') || (line[linePos] == '+'))
    {
        if ((linePos + 1) < line.size())
        {
            if (isdigit(line[linePos + 1]))
            {
                token += line[linePos++];
                goto _number;
            }
        }
    }
    else
        linePos++;
#if 0
    cout << tokType << " :: {" << token << "}" << endl;
#endif
    return tokType;
}

//==============================================================================
// Função responsável por carregar uma nova linha para o buffer de análise,
// é nosso alimentador.
//==============================================================================
bool Tokenizer::loadLine()
{
    if (input.eof())
        return false;
    getline(input, line);
    if (logLine)
        cout << "Line: " << line << endl;
    // verifica se encontra o começo de algum comentário
    size_t pos = line.find_first_of(';', 0);
    if (pos != string::npos)
    {
        line = line.substr(0, pos);
    }
    currentLine++;
    //cout << "carregando linha " << currentLine << endl;
    // se essa linha ainda for branca, leia outra linha...
    if (isWhiteLine())
        loadLine();
    else linePos = 0;
    return true;
}

//==============================================================================
// Verifica se a linha atual não contém nada útil
//==============================================================================
bool Tokenizer::isWhiteLine()
{
    size_t pos = 0;
    while (pos < line.size())
    {
        if (!isspace(line[pos]))
            return false;
        pos++;
    }
    return true;
}

void Tokenizer::rewind()
{
    linePos = 0;
}

//==============================================================================
// operador utilizado para comparar diretamente o token com a str
//==============================================================================
bool Tokenizer::operator==(const char *str)
{
    if (!strcmpi(str, token.c_str()))
        return true;
    return false;
}

//==============================================================================
// operador utilizado para verificar a desigualdade diretamente do token.
//==============================================================================
bool Tokenizer::operator!=(const char *str)
{
    if (strcmpi(str, token.c_str()))
        return true;
    return false;
}

string Tokenizer::rest()
{
    if (linePos < line.size())
    {
        string str = line.substr(linePos, line.size() - linePos);
        loadLine();
        isTokenInBuffer = true;
        return str;
    }
    else
        return string("");
}

}
