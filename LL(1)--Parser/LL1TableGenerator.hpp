#ifndef LL_TABLE_GENERATOR_HPP
#define LL_TABLE_GENERATOR_HPP

#include <vector>
#include <unordered_set>
#include <list>

class CFG;

/* represent a single rule.
* a head can have multiple productions
*/
struct RuleID {
	unsigned head;  //head of production
	unsigned prodNo;//which of multiple productions
};


/*
* Given a CFG, this class can generate Nullabe, FIRST and FOLLOW sets.
* Then it will generate an LL(1) Parsing Table
*/
class LL1TableGenerator {
public:
	LL1TableGenerator();
	~LL1TableGenerator();

	LL1TableGenerator(const LL1TableGenerator&) = delete;
	LL1TableGenerator& operator=(const LL1TableGenerator&) = delete;


public:
	void setCFG(CFG* cfg);

	void printNullable() const;
	void printFirst() const;
	void printFollow() const;

	//given a compunf expression like ET, generate FIRST
	void getExprFirst(const std::vector<unsigned>& expr, std::unordered_set<unsigned>& out) const;

	//add a rule to table at M[nonterm, term] = rule
	void addRuleToTable(unsigned nonTerm, unsigned term, RuleID rule);

	//get rule at M[nonterm, term]
	const std::list<RuleID>& getRuleFromTable(unsigned nonTerm, unsigned term) const;

	void generateTable();
	void printTable() const;

	//convert a rule ID to rule string
	std::string ruleIdToString(RuleID ruleId) const;

	bool isAmbigious() const { return maxProdsInACell > 1; }

private:
	void calculateDirectlyNullable(); //which prods are nullable directly S -> #

	//calculate for given non-terminal
	void calculateNullableHelper(unsigned); 
	void calculateFirstHelper(unsigned);
	void calculateFollowHelper(unsigned);

	//calculate for all non-terminals
	void calculateNullable();
	void calculateFirst();
	void calculateFollow();

private:
	CFG* cfg;

	int* nullable;
	std::unordered_set<unsigned>* first;
	std::unordered_set<unsigned>* follow;

	std::list<RuleID>* table;
	unsigned maxProdsInACell;
};


#endif