#include "TestWrapper.h"
#include "Parser.h"

using namespace std;

#include "../../spa/src/PQL/Parser.h"
#include "../../spa/src/PQL/QueryEvaluator.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
	Pkb pkb = Pkb();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
  // ...rest of your code...
  ifstream input_file(filename);
  std::string input = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
  Parse(input, this->pkb);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  // call your evaluator to evaluate the query here
    // ...code to evaluate query...

    // store the answers to the query in the results list (it is initially empty)
    // each result must be a string.

  pql::Parser parser = pql::Parser(query);
  parser.Parse();
  std::list<std::string> res = evaluateQuery(parser.getQuery(), this->pkb);

  for (string s : res) {
    results.push_back(s);
  }
}
