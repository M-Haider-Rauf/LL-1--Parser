#include <iostream>

#include "CFG.hpp"
#include "LL1TableGenerator.hpp"
#include "LL1Parser.hpp"
#include "Util.hpp"

int main()
{
	CFG cfg;
	LL1TableGenerator gen;
	LL1Parser parser;
	
	//add rules to grammar
	//separate symbols by spaces
	cfg.addRule("E -> T E'");
	cfg.addRule("E' -> + T E' | #");
	cfg.addRule("T -> F T'");
	cfg.addRule("T' -> * F T' | #");
	cfg.addRule("F -> ( E ) | id");
	cfg.printInfo();
	std::cout << "\n";
	
	gen.setCFG(&cfg);
	gen.generateTable();
	gen.printNullable();
	gen.printFirst();
	gen.printFollow();
	
	//string to parse
	//separate tokens by spaces
	std::string str = "id + id * id";
	parser.setCFG(&cfg);
	parser.setTable(&gen);

	std::cout << "\n\nParsing: " << str << "\n";
	parser.parse(str);
}