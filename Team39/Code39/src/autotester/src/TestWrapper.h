#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "AbstractWrapper.h"
#include "../../spa/src/PKB/pkb.h"

class TestWrapper : public AbstractWrapper
{
private:
	Pkb pkb;

public:
	// default constructor
	TestWrapper();

	// destructor
	~TestWrapper();

	// method for parsing the SIMPLE source
	void parse(std::string filename) override;

	// method for evaluating a query
	void evaluate(std::string query, std::list<std::string>& results) override;
};

#endif
