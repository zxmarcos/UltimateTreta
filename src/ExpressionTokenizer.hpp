#ifndef __StTokenizer_HPP__
#define __StTokenizer_HPP__

#include <string>
#include <fstream>

using namespace std;

namespace Stat
{
enum TOKEN_TYPES
{
	TOK_EOF = 0,
	TOK_DELIM,
	TOK_NUMBER,
	TOK_IDENT,
	TOK_LITERAL,
};

class StTokenizer
{
public:
	StTokenizer();
	~StTokenizer();
	int getToken();
	const string tok();
	int type();
	bool eof();
	bool check(const char *str, bool consume=true);
	bool checkNumber(bool consume=true);
	bool checkLiteral(bool consume=true);
	int toInt();
	float toFloat();
	void consumeToken();
	void setLineBuf(string rdbuf);
	bool isFloatNumber();
private:

	void lexForDelim();
	// variável responsável por nos dizer se o token pretendido tem
	// que ser lido do arquivo.
	bool isTokenInBuffer;
	int tokType;
	bool isDelimiter(char c);
	string line;
	string token;
	bool isFloat;
	size_t linePos;
	bool operator==(const char *str);
	bool operator!=(const char *str);
};

}

#endif //__StTokenizer_HPP__
