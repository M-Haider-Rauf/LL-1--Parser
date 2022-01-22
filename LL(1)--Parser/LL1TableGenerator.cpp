#include "LL1TableGenerator.hpp"

#include "CFG.hpp"

#include <iostream>

LL1TableGenerator::LL1TableGenerator() 
	: cfg(nullptr), nullable(nullptr)
	, first(nullptr), follow(nullptr),
	maxProdsInACell(0), table(nullptr)
{
}

LL1TableGenerator::~LL1TableGenerator()
{
	delete[] follow;
	delete[] first;
	delete[] nullable;
	delete[] table;
}

void LL1TableGenerator::setCFG(CFG* cfg)
{
	this->cfg = cfg;

	delete[] nullable;
	delete[] first;
	delete[] follow;
	delete[] table;
	maxProdsInACell = 0;

	const unsigned nonTermCnt = cfg->getNonTermCnt();
	const unsigned termCnt = cfg->getTermCnt();

	nullable = new int[nonTermCnt];
	std::memset(nullable, 0xFF, nonTermCnt * sizeof(int));  //set whole nullable array to -1

	first = new std::unordered_set<unsigned>[nonTermCnt];
	follow = new std::unordered_set<unsigned>[nonTermCnt];
	table = new std::list<RuleID>[nonTermCnt * (termCnt + 1)];
}

void LL1TableGenerator::calculateNullableHelper(unsigned head)
{
	if (nullable[head - CFG::NONTERM_OFF] >= 0) {
		return;
	}

	const auto& rules = cfg->getRules();
	const auto& body = rules.find(head)->second;

	bool ruleNull = false;

	for (const auto& expr : body) {
		bool exprNull = true;

		for (unsigned sym : expr) {
			if (cfg->isIdNonTerm(sym)) {
				calculateNullableHelper(sym);

				if (nullable[sym - CFG::NONTERM_OFF] == 0) {
					exprNull = false;
					break;
				}
			}
			else {
				exprNull = false;
				break;
			}
		}

		if (exprNull) {
			ruleNull = true;
			break;
		}
	}

	if (ruleNull) {
		nullable[head - CFG::NONTERM_OFF] = 1;
	}
	else {
		
		nullable[head - CFG::NONTERM_OFF] = 0;
	}
}

void LL1TableGenerator::calculateDirectlyNullable()
{
	const auto& rules = cfg->getRules();

	for (const auto& rule : rules) {
		unsigned head = rule.first;
		const auto& body = rule.second;

		for (const auto& expr : body) {
			if (expr.front() == NULL_ID) {
				nullable[head - CFG::NONTERM_OFF] = 1;
			}
		}
	}
}

void LL1TableGenerator::calculateNullable()
{
	calculateDirectlyNullable();

	for (const auto& termPair : cfg->getRules()) {
		calculateNullableHelper(termPair.first);
	}
}

void LL1TableGenerator::calculateFirstHelper(unsigned head)
{
	const auto& rules = cfg->getRules();
	const auto& body = rules.find(head)->second;
	auto& headFirst = first[head - CFG::NONTERM_OFF];

	for (const auto& expr : body) {
		bool calcRestfirst = true;
		const int len = expr.size();
		unsigned firstSym = expr[0];

		if (CFG::isIdNonTerm(firstSym)) {
			calculateFirstHelper(firstSym);
			auto& firstSymFirst = first[firstSym - CFG::NONTERM_OFF];
			headFirst.insert(firstSymFirst.begin(), firstSymFirst.end());
		}
		else {
			headFirst.insert(firstSym);
			calcRestfirst = false;
		}

		
		if (calcRestfirst) {
			for (int i = 0; i < len - 1; ++i) {
				unsigned sym = expr[i];
				if (CFG::isIdNonTerm(sym) && nullable[sym - CFG::NONTERM_OFF]) {
					unsigned nextSym = expr[i + 1];
					if (CFG::isIdNonTerm(nextSym)) {
						calculateFirstHelper(nextSym);
						auto& nextSymFirst = first[nextSym - CFG::NONTERM_OFF];
						headFirst.insert(nextSymFirst.begin(), nextSymFirst.end());
					}
					else {
						headFirst.insert(nextSym);
					}
				}
				else {
					break;
				}
			}
		}
	}
}

void LL1TableGenerator::calculateFirst()
{
	for (const auto& termPair : cfg->getRules()) {
		calculateFirstHelper(termPair.first);
	}
}

void LL1TableGenerator::calculateFollowHelper(unsigned head)
{
	auto& rules = cfg->getRules();
	auto& headFollow = follow[head - CFG::NONTERM_OFF];



	for (const auto& rule : rules) {
		unsigned ruleHead = rule.first;


		for (const auto& expr : rule.second) {
			const int len = expr.size();
			int i = 0;

			for (i = 0; i < len; ++i) {
				unsigned sym = expr[i];
				if (sym == head) {
					bool copyFollow = false;

					if (i < len - 1) {
						std::vector<unsigned> restExpr(expr.begin() + i + 1, expr.end());
						std::unordered_set<unsigned> restExprFirst;

						getExprFirst(restExpr, restExprFirst);
						headFollow.insert(restExprFirst.begin(), restExprFirst.end());

						if (restExprFirst.find(NULL_ID) != restExprFirst.end()) {
							copyFollow = true;
						}
					}
					else {
						copyFollow = true;
					}

					if (copyFollow && ruleHead != head) {
						calculateFollowHelper(ruleHead);
						auto& ruleHeadFollow = follow[ruleHead - CFG::NONTERM_OFF];

						headFollow.insert(ruleHeadFollow.begin(), ruleHeadFollow.end());
					}
				}
			}
		}
	}
}

void LL1TableGenerator::calculateFollow()
{
	follow[0].insert(EOI_ID);

	for (const auto& termPair : cfg->getRules()) {
		calculateFollowHelper(termPair.first);
	}

	for (size_t i = 0; i < cfg->getNonTermCnt(); ++i) {
		auto& followSet = follow[i];

		if (followSet.find(NULL_ID) != followSet.end()) {
			followSet.erase(NULL_ID);
		}
	}
}

void LL1TableGenerator::getExprFirst(const std::vector<unsigned>& expr, std::unordered_set<unsigned>& out) const
{
	const int len = expr.size();
	int i;

	unsigned firstSym = expr.front();
	if (cfg->isIdNonTerm(firstSym)) {
		auto& firstSymFirst = first[firstSym - CFG::NONTERM_OFF];
		out.insert(firstSymFirst.begin(), firstSymFirst.end());
	}
	else {
		out.insert(firstSym);
	}


	for (i = 0; i < len - 1; ++i) {
		unsigned sym = expr[i];
		unsigned nextSym = expr[i];

		if (cfg->isIdNonTerm(sym) && nullable[sym - CFG::NONTERM_OFF]) {
			auto& nextSymFirst = first[nextSym - CFG::NONTERM_OFF];
			out.insert(nextSymFirst.begin(), nextSymFirst.end());
		}
		else {
			break;
		}
	}


	bool allNull = false;
	if (i == len - i) {
		unsigned lastSym = expr[len - 1];
		if (cfg->isIdNonTerm(lastSym) && nullable[lastSym - CFG::NONTERM_OFF]) {
			out.insert(NULL_ID);
			allNull = true;
		}
	}

	if (!allNull) {
		if (out.find(EOI_ID) != out.end()) {
			out.erase(EOI_ID);
		}
	}
}


void LL1TableGenerator::printNullable() const
{
	for (unsigned i = 0; i < cfg->getNonTermCnt(); ++i) {
		std::cout << "Nullable(";
		std::cout << cfg->id2NonTerm(i + CFG::NONTERM_OFF) << "):\t";

		int d = nullable[i];

		if (d == 0) {
			std::cout << "No";
		}
		else if (d == 1) {
			std::cout << "Yes";
		}
		else {
			std::cout << "N/A";
		}

		std::cout << "\n";
	}
}

void LL1TableGenerator::printFirst() const
{
	for (unsigned i = 0; i < cfg->getNonTermCnt(); ++i) {
		std::cout << "FIRST(";
		std::cout << cfg->id2NonTerm(i + CFG::NONTERM_OFF) << "):\t";

		const auto& set = first[i];

		for (unsigned term : set) {
			std::cout << cfg->id2Sym(term) << "  ";
		}

		std::cout << "\n";
	}
}

void LL1TableGenerator::printFollow() const
{
	for (unsigned i = 0; i < cfg->getNonTermCnt(); ++i) {
		std::cout << "FOLLOW(";
		std::cout << cfg->id2NonTerm(i + CFG::NONTERM_OFF) << "):\t";

		const auto& set = follow[i] ;

		for (unsigned term : set) {
			std::cout << cfg->id2Sym(term) << "  ";
		}

		std::cout << "\n";
	}
}

void LL1TableGenerator::addRuleToTable(unsigned nonTerm, unsigned term, RuleID ruleID)
{
	const size_t termCnt = cfg->getTermCnt();
	const size_t offset = termCnt + 1;

	size_t yIdx = nonTerm - CFG::NONTERM_OFF;
	size_t xIdx = term - EOI_ID;

	auto& cell = table[yIdx * offset + xIdx];
	cell.push_back(ruleID);

	if (maxProdsInACell > cell.size()) {
		maxProdsInACell = cell.size();
	}
}

const std::list<RuleID>& LL1TableGenerator::getRuleFromTable(unsigned nonTerm, unsigned term) const
{
	const size_t termCnt = cfg->getTermCnt();
	const size_t offset = termCnt + 1;

	size_t yIdx = nonTerm - CFG::NONTERM_OFF;
	size_t xIdx = term - EOI_ID;

	return table[yIdx * offset + xIdx];
}

void LL1TableGenerator::generateTable()
{
	calculateNullable();
	calculateFirst();
	calculateFollow();

	const auto& rules = cfg->getRules();

	for (const auto& rule : rules) {
		unsigned head = rule.first;

		const auto& body = rule.second;
		const size_t len = body.size();

		for (size_t prodNo = 0; prodNo < len; ++prodNo) {
			const auto& prod = body[prodNo];

			std::unordered_set<unsigned> first;
			getExprFirst(prod, first);

			for (unsigned nt : first) {
				if (nt != NULL_ID) {
					addRuleToTable(head, nt, { head, (unsigned)prodNo });
				}
			}

			if (first.find(NULL_ID) != first.end()) {
				const auto& followHead = follow[head - CFG::NONTERM_OFF];

				for (unsigned nt : followHead) {
					addRuleToTable(head, nt, { head, (unsigned)prodNo });
				}
			}
		}
	}
}

void LL1TableGenerator::printTable() const
{
	const size_t termCnt = cfg->getTermCnt() + 1;
	const size_t nonTermCnt = cfg->getNonTermCnt();
	const auto& rules = cfg->getRules();

	std::cout << "\t";

	for (size_t t = 0; t < termCnt; ++t) {
		std::cout << cfg->id2Sym(t + EOI_ID) << "\t";
	}

	std::cout << "\n";
	for (unsigned nt = 0; nt < nonTermCnt; ++nt) {
		std::cout << cfg->id2NonTerm(nt + CFG::NONTERM_OFF) << "\t";

		for (unsigned t = 0; t < termCnt; ++t) {
			const auto& ruleIDs = getRuleFromTable(nt + CFG::NONTERM_OFF, t + EOI_ID);

			
			for (auto ruleID : ruleIDs) {
				std::cout << ruleIdToString(ruleID);
			}

			std::cout << "\t";
		}

		std::cout << "\n\n";
	}
}

std::string LL1TableGenerator::ruleIdToString(RuleID ruleId) const
{
	std::string str;

	const auto& rule = *(cfg->getRules().find(ruleId.head));

	str += cfg->id2NonTerm(rule.first);
	str += " -> ";

	for (unsigned sym : rule.second[ruleId.prodNo]) {
		str += cfg->id2Sym(sym);
	}

	return str;
}
