#include "LL1Parser.hpp"
#include "Util.hpp"
#include "LL1TableGenerator.hpp"
#include "CFG.hpp"

#include <stack>
#include <iostream>

LL1Parser::LL1Parser() : table(nullptr), cfg(nullptr)
{
}

void LL1Parser::setTable(LL1TableGenerator* table)
{
	this->table = table;
}

void LL1Parser::setCFG(CFG* cfg)
{
	this->cfg = cfg;
}

void LL1Parser::parse(const std::string& string)
{
	std::vector<unsigned> toks;
	runLexer(string, toks);

	if (!toks.empty()) { //empty token array == Lexer Error
		toks.push_back(EOI_ID);
		runLL1Parser(toks);
	}
}

void LL1Parser::runLexer(const std::string& str, std::vector<unsigned>& outToks)
{
	bool lexError = false;
	std::vector<std::string> tokens;
	tokenizeBySpaces(str, tokens);

	const auto& terminals = cfg->getTermMap();

	for (const std::string& tok : tokens) {
		auto tokIter = terminals.find(tok);

		if (tokIter == terminals.end()) {
			std::cerr << "Unknown Token: " << tok << "\n";
			lexError = true;
		}
		else if (!lexError) {
			outToks.push_back(tokIter->second);
		}
	}

	if (lexError) {
		outToks.clear();
	}
}

void LL1Parser::runLL1Parser(const std::vector<unsigned>& input)
{
	std::stack<unsigned> stack;
	stack.push(EOI_ID);  //push $
	stack.push(CFG::NONTERM_OFF); //push start symbols

	const size_t len = input.size();
	size_t inP = 0; //input pointer

	if (table->isAmbigious()) {
		std::cerr << "WARNING: Grammar is ambigious! May not parse correctly.\n";
	}

	while (stack.top() != EOI_ID) {
		if (stack.top() == input[inP]) {
			std::cout << "MATCH: " << cfg->id2Term(stack.top()) << "\n";
			stack.pop();
			++inP;
		}
		else if (cfg->isIdTerm(stack.top())) {
			std::cerr << "PASRSE ERROR: Terminal on top of stack: " << cfg->id2Sym(stack.top()) << "\n";
			break;
		}
		else if (table->getRuleFromTable(stack.top(), input[inP]).empty()) {
			std::cerr << "PARSE ERROR: No rule in ParseTable[";
			std::cerr << cfg->id2Sym(stack.top()) << ", " << cfg->id2Sym(input[inP]) << "]\n";
			break;
		}
		else {
			const auto& ruleId = table->getRuleFromTable(stack.top(), input[inP]).front();
			const auto& rule = cfg->getRules().find(ruleId.head)->second[ruleId.prodNo];

			std::cout << "OUTPUT: " << table->ruleToString(ruleId) << "\n";
			stack.pop();

		
			if (rule.front() != NULL_ID) {  //use the rule added first
				for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
					stack.push(*it);
				}
			}
		}
	}

	if (stack.size() > 1) { //at the end, only $ should be in stack
		std::cout << "Unsuccessful Parsing!\n";
	}
}
