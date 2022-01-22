#ifndef LL_TABLE_GENERATOR_HPP
#define LL_TABLE_GENERATOR_HPP

#include <vector>
#include <unordered_set>
#include <list>

class CFG;

struct RuleID {
	unsigned head;
	unsigned prodNo;
};

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

	void getExprFirst(const std::vector<unsigned>& expr, std::unordered_set<unsigned>& out) const;

	void addRuleToTable(unsigned nonTerm, unsigned term, RuleID rule);
	const std::list<RuleID>& getRuleFromTable(unsigned nonTerm, unsigned term) const;

	void generateTable();
	void printTable() const;
	std::string ruleToString(RuleID ruleId) const;

	bool isAmbigious() const { return maxProdsInACell > 1; }

private:
	void calculateDirectlyNullable();
	void calculateNullableHelper(unsigned);
	void calculateFirstHelper(unsigned);
	void calculateFollowHelper(unsigned);

	void calculateNullable();
	void calculateFirst();
	void calculateFollow();

private:
	CFG* cfg;
	int* nullable;
	std::unordered_set<unsigned>* first;
	std::unordered_set<unsigned>* follow;
	unsigned maxProdsInACell;

	std::list<RuleID>* table;
};


#endif