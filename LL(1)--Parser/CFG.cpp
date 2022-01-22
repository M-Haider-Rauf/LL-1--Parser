#include "CFG.hpp"

#include "Util.hpp"

#include <iostream>

unsigned CFG::addNewTerminal(const std::string& str)
{
	//update maps
	term2IdMap.insert({ str, nextTermId });
	id2TermMap.push_back(str);

	++termCnt;

	return nextTermId++; //return ID of newly added terminal
}

unsigned CFG::addNewNonTerminal(const std::string& str)
{
	nonTerm2IdMap.insert({ str, nextNonTermId });
	id2NonTermMap.push_back(str);

	++nonTermCnt;

	return nextNonTermId++;
}


const std::string& CFG::id2Term(unsigned id) const
{
	return id2TermMap[id - TERM_OFF];  //id is offset by this constant
}

const std::string& CFG::id2NonTerm(unsigned id) const
{
	return id2NonTermMap[id - NONTERM_OFF];
}


//convert any valid ID to symbol
std::string CFG::id2Sym(unsigned id) const
{
	switch (id) {
	case  NULL_ID: return NULL_STR;
	case EOI_ID: return EOI_STR;

	default:
		if (isIdNonTerm(id)) return id2NonTerm(id);
		else return id2Term(id);
	}
}

void CFG::addRule(const std::string& str)
{
	std::vector<std::string> tokens;
	tokenizeBySpaces(str, tokens);

	std::string headStr = tokens[0]; //consume head
	std::string equals = tokens[1]; //consume equals-sign

	unsigned headId;
	auto nonTermIter = nonTerm2IdMap.find(headStr); //check if head symbol already added to grammar

	if (nonTermIter == nonTerm2IdMap.end()) {
		headId = addNewNonTerminal(headStr); //add to grammar if not present
	}
	else {
		headId = nonTermIter->second; //terminal already present, get id
	}

	Body* body = NULL;
	auto headIter = rules.find(headId);

	if (headIter == rules.end()) {
		body = &(rules.insert({ headId, {} }).first->second);
	}
	else {
		body = &(headIter->second);
	}

	std::vector<unsigned> expr;

	size_t i = 2;
	const size_t len = tokens.size();

	while (i < len) {
		const std::string& tok = tokens[i];

		if (tok == NULL_STR) {
			expr.push_back(NULL_ID);
		}
		else if (tok == "|") {
			body->push_back(expr);
			expr.clear();
		}
		else if (isTerm(tok)) {
			unsigned newTermId;
			auto termIter = term2IdMap.find(tok);

			if (termIter == term2IdMap.end()) {
				newTermId = addNewTerminal(tok);
			}
			else {
				newTermId = termIter->second;
			}

			expr.push_back(newTermId);
		}
		else {
			unsigned newNonTermId;
			auto nonTermIter = nonTerm2IdMap.find(tok);

			if (nonTermIter == nonTerm2IdMap.end()) {
				newNonTermId = addNewNonTerminal(tok);
			}
			else {
				newNonTermId = nonTermIter->second;
			}

			expr.push_back(newNonTermId);
		}

		++i;
	}

	body->push_back(expr);
}

bool CFG::isIdTerm(unsigned id)
{
	return id > EOI_ID && id < NONTERM_OFF;
}

bool CFG::isIdNonTerm(unsigned id)
{
	return id >= NONTERM_OFF;
}

const RuleList& CFG::getRules()
{
	return rules;
}

const Body& CFG::getStartProd() const
{
	return rules.find(NONTERM_OFF)->second;
}

void CFG::printInfo() const
{
	for (auto rule : rules) {
		std::cout << id2NonTerm(rule.first) << " -> ";

		for (auto expr : rule.second) {
			for (auto sym : expr) {
				std::cout << id2Sym(sym);
			}

			std::cout << " | ";
		}
		
		std::cout << "\b \b\b \b";
		std::cout << "\n";
	}


	std::cout << "\n";
	std::cout << "Start Symbol: " << id2NonTermMap[0] << "\n";
	std::cout << "Terminals: " << termCnt << "\n";
	std::cout << "Non-Terminals: " << nonTermCnt << "\n";
}

void CFG::clear()
{
	term2IdMap.clear();
	nonTerm2IdMap.clear();
	id2NonTermMap.clear();
	id2TermMap.clear();
	rules.clear();

	termCnt = nonTermCnt = 0;

	nextTermId = TERM_OFF;
	nextNonTermId = NONTERM_OFF;
}
