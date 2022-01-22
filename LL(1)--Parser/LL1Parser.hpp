#ifndef LL1_PARSER_HPP
#define LL1_PARSER_HPP

class LL1TableGenerator;
class CFG;

#include <string>
#include <vector>


/*
* This class needs a CFG and LL1TableGenerator.
* Then it can parse a string from that grammar.
*/

class LL1Parser {
public:
	LL1Parser();
	
	LL1Parser(const LL1Parser&) = delete;
	LL1Parser& operator=(const LL1Parser&) = delete;

public:
	void setTable(LL1TableGenerator* table);
	void setCFG(CFG* cfg);

	void parse(const std::string& string);  //parse the string from grammar

private:
	void runLexer(const std::string& str, std::vector<unsigned>& toks); //break into tokens
	void runLL1Parser(const std::vector<unsigned>& input);	//run parser on input...

private:
	LL1TableGenerator* table;
	CFG* cfg;
};

#endif // !LL1_PARSER_HPP
