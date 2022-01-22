#ifndef CFG_HPP
#define CFG_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

//typedefs
using SymbolMap = std::unordered_map<std::string, unsigned>;
using Body = std::vector<std::vector<unsigned>>;
using RuleList = std::map<unsigned, Body>;


//constants
constexpr unsigned NULL_ID = 0u;	//empty string
constexpr unsigned EOI_ID = 1u;		//EOI == End-of-Input aka $ in books

#define NULL_STR "#" //# will represent empty string or epsilon
#define EOI_STR "$" //$ will represent End-of-Input marker


/* This class is used to represent a context-free Grammar.
*  You can add statements like S -> b | c | A | etc. to the grammar
*  Separate the individual symbols by spaces.
*  Symbols starting with a capital letter are treated as non-terminals.
*  All other symbols are treates as terminals.
*  Exceptions are, # means empty string, and $ is reserved as end-of-input marker.
*/
class CFG {
public:
	CFG() { clear(); }

	//disable copying
	CFG(const CFG&) = delete;
	CFG& operator=(const CFG&) = delete;

public:
	//internally, each symbol is assigned a unique ID.
	static const unsigned TERM_OFF = 2; //terminal IDs start from here.
	static const unsigned NONTERM_OFF = ((~0u - 2u) / 2u) + 2u; //non-terminal IDs start here upto MAX_UNSIGNED


	//get number of terminals and non-terminals in grammar respectively
	unsigned getTermCnt() const { return termCnt; }
	unsigned getNonTermCnt() const { return nonTermCnt; }

	//get the maps holding terminals and non-terminals
	const SymbolMap& getTermMap() const { return term2IdMap; }
	const SymbolMap& getNonTermMap() const { return nonTerm2IdMap; }

	//these functions convert a valid ID to string rep of symbol...
	const std::string& id2Term(unsigned) const;
	const std::string& id2NonTerm(unsigned) const;
	std::string id2Sym(unsigned) const;

	//add a grammar rule
	void addRule(const std::string& str);

	//given an integer ID, these functions check if it is bound to terminal or non-terminal
	static bool isIdTerm(unsigned);
	static bool isIdNonTerm(unsigned);

	const RuleList& getRules(); //get rules
	const Body& getStartProd() const; //get the starting rule


	//print basic stuff about grammar...
	//including rules, no.of terminals, no.of non-terminals, and start symbol
	void printInfo() const;

	void clear(); //remove all rules from grammar

private:
	unsigned addNewTerminal(const std::string& str);
	unsigned addNewNonTerminal(const std::string& str);

private:
	//no. of terminals and non-terminals in grammar
	unsigned termCnt;
	unsigned nonTermCnt;

	//counter to assign IDs to grammar symbols
	unsigned nextTermId;
	unsigned nextNonTermId;

	//map symbol to ID
	SymbolMap term2IdMap;
	SymbolMap nonTerm2IdMap;

	//map ID to symbol
	std::vector<std::string> id2TermMap;
	std::vector<std::string> id2NonTermMap;

	RuleList rules;
};


#endif