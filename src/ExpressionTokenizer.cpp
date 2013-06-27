//==============================================================================
//
//==============================================================================
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "ExpressionTokenizer.hpp"

#define strcmpi strcasecmp

using namespace std;

namespace Stat
{


//==============================================================================
// /////////////////////////////////////////////////////////////////////////////
//                   Analisador léxico específico para expressões
// /////////////////////////////////////////////////////////////////////////////
//==============================================================================

//==============================================================================
// ctor
//==============================================================================
StTokenizer::StTokenizer()
{
    isTokenInBuffer = false;
    tokType = TOK_EOF;
    token = "";
    isFloat = false;
}

//==============================================================================
// dtor
//==============================================================================
StTokenizer::~StTokenizer()
{
}

//==============================================================================
// Retorna o tipo do token atual
//==============================================================================
int StTokenizer::type()
{
    return tokType;
}

//==============================================================================
// Retorna verdadeiro se o fim do arquivo ja foi alcançado
//==============================================================================
bool StTokenizer::eof()
{
    return (linePos >= line.size());
}

//==============================================================================
// Retorna o valor inteiro do token atual
//==============================================================================
int StTokenizer::toInt()
{
    const char *str = token.c_str();
    if (str)
        return atoi(str);
    return 0;
}

//==============================================================================
// Retorna o valor em float do token atual
//==============================================================================
float StTokenizer::toFloat()
{
    const char *str = token.c_str();
    if (str)
        return atof(str);
    return 0.0f;
}

//==============================================================================
// Retorna verdadeiro se o número é um float
//==============================================================================
bool StTokenizer::isFloatNumber()
{
    return isFloat;
}

//==============================================================================
// Retorna o token atual
//==============================================================================
const string StTokenizer::tok()
{
    return token;
}

//==============================================================================
// Verifica se o token atual é igual a str, se consume for true, então se o
// token for igual a str esse token será consumido, e o próximo token será lido
// do buffer.
//==============================================================================
bool StTokenizer::check(const char *str, bool consume)
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
bool StTokenizer::checkNumber(bool consume)
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
bool StTokenizer::checkLiteral(bool consume)
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
void StTokenizer::consumeToken()
{
    isTokenInBuffer = true;
}

//==============================================================================
// Verifica se um carácter é um delimitador
//==============================================================================
bool StTokenizer::isDelimiter(char c)
{
    if (!c)
        return false;
    if (strchr("[]!=<>,:()+-*/&|~%%^", c))
        return true;
    return false;
}

//==============================================================================
// Faz a análise léxica de uma delimitador
//==============================================================================
void StTokenizer::lexForDelim()
{
    // verifica delimitadores de dois caracteres
    if ((linePos + 1) < line.size())
    {
        if (!isDelimiter(line[linePos + 1]))
            goto _normal;
        linePos++;
        switch (line[linePos - 1])
        {
        case '&':
            if (line[linePos] == '&')
            {
                token = "&&";
                linePos++;
            }
            else
                token ="&";
            break;

        case '|':
            if (line[linePos] == '|')
            {
                token = "||";
                linePos++;
            }
            else
                token ="|";
            break;

        case '^':
            if (line[linePos] == '^')
            {
                token = "^^";
                linePos++;
            }
            else
                token ="^";
            break;

        case '*':
            if (line[linePos] == '*')
            {
                token = "**";
                linePos++;
            }
            else
                token ="*";
            break;

        case ':':
            if (line[linePos] == '=')
            {
                token = ":=";
                linePos++;
            }
            else
                token =":";
            break;

        case '!':
            if (line[linePos] == '=')
            {
                token = "!=";
                linePos++;
            }
            else
                token ="!";
            break;

        case '>':
            if (line[linePos] == '=')
            {
                token = ">=";
                linePos++;
            }
            else
                token =">";
            break;

        case '<':
            if (line[linePos] == '=')
            {
                token = "<=";
                linePos++;
            }
            else
                token ="<";
            break;

        default:
            token = line[linePos - 1];
            break;
        }
    }
    else
    {
_normal:
        token = line[linePos];
        linePos++;
    }
}
//==============================================================================
// Função principal do lexer, responsável por analisar o buffer, e chamar o
// a alimentador do buffer sempre que necessário.
//==============================================================================
int StTokenizer::getToken()
{
    if (!isTokenInBuffer)
        return tokType;

    isTokenInBuffer = false;
    tokType = TOK_EOF;
    token = "";
    isFloat = false;

    // verifica se a string é nula ou a posição está no fim
    if ((line == "") || (linePos >= line.size()))
    {
        return tokType;
    }

    // ignora espaços em branco
    while ((linePos < line.size() && isspace(line[linePos])))
        linePos++;

    // se acabou a string, então temos EOF
    if (linePos >= line.size())
        return tokType;

    // Primeiro verificamos se é um delimitador
    if (isDelimiter(line[linePos]))
    {
        tokType = TOK_DELIM;
        lexForDelim();

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

        tokType = TOK_NUMBER;
        size_t finalPos = linePos;
        while (isdigit(line[finalPos]) || line[finalPos] == '.')
        {
            if (line[finalPos] == '.')
                isFloat = true;
            finalPos++;
        }
        // copia a parte da string que é numérica
        token += line.substr(linePos, (finalPos - linePos));
        linePos = finalPos;


        // Verifica se é do tipo alpha
    }
    else if (isalpha(line[linePos]))
    {
        size_t finalPos = linePos;

        while (!isDelimiter(line[finalPos]) && finalPos < line.size())
        {
            // por alguma razão que desconheço utilizar isspace não funcioná,
            // porém fazendo essa simples comparação funciona muito bem.
            if (isspace(line[finalPos]))
                break;
            if ((line[finalPos] == ' ') || (line[finalPos] == '\t'))
                break;
            finalPos++;
        }
        token = line.substr(linePos, (finalPos - linePos));
        if (isspace(line[finalPos]))
            finalPos++;
        linePos = finalPos;
        tokType = TOK_IDENT;

        // verifica por números sinalizados, apenas copiamos o sinal e saltamos para
        // a parte encarregada de copiar os digitos.
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
void StTokenizer::setLineBuf(string rdbuf)
{
    line = rdbuf;
    linePos = 0;
    isTokenInBuffer = true;
}

//==============================================================================
// operador utilizado para comparar diretamente o token com a str
//==============================================================================
bool StTokenizer::operator==(const char *str)
{
    if (!strcmpi(str, token.c_str()))
        return true;
    return false;
}

//==============================================================================
// operador utilizado para verificar a desigualdade diretamente do token.
//==============================================================================
bool StTokenizer::operator!=(const char *str)
{
    if (strcmpi(str, token.c_str()))
        return true;
    return false;
}

}
