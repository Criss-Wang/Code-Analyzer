#include "Parser.h"
#include "Utility/TNode.h"

#include "catch.hpp"
using namespace std;
void require(bool b) {
    REQUIRE(b);
}

/* test case for assignment is work in progress 
TEST_CASE("1st Test: Assignment") {
	
	string input = "x = y + 1";

	AST expectedAST(new TNode("assign"));
	(*expectedAST.root).insertFront(new TNode("x"));
	(*expectedAST.root).insertBack(new TNode("+", { new TNode("y"), new TNode("1") }));
	
	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.root, false);

    require((*actualAST.root).isEqual(expectedAST.root));
} */

TEST_CASE("Read - Test 1") {

	list<string> input = { "read", "a", ";" };

	TNode* stmt = new readTNode("1", "a");
	AST expectedAST(new stmtLstTNode(stmt));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Read - Test 2") {

	list<string> input = { "read", "a", ";" };

	TNode* stmt = new printTNode("1", "a");
	AST expectedAST(new stmtLstTNode(stmt));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require(!(actualAST).isEqual(expectedAST));
}

TEST_CASE("Read - Test 3") {

	list<string> input = { "read", "a", ";", "read", "b", ";" };

	TNode* stmt1 = new readTNode("1", "a");
	TNode* stmt2 = new readTNode("2", "b");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);

	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Read - Test 4") {

	list<string> input = { "read", "a", ";", "read", "b", ";" };

	TNode* stmt1 = new readTNode("1", "a");
	TNode* stmt2 = new printTNode("2", "b");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);

	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require(!(actualAST).isEqual(expectedAST));
}

TEST_CASE("Read - Test 5") {

	list<string> input = { "read", "a", ";", "read", "b", ";", "read", "c", ";", "read", "d", ";" };

	TNode* stmt1 = new readTNode("1", "a");
	TNode* stmt2 = new readTNode("2", "b");
	TNode* stmt3 = new readTNode("3", "c");
	TNode* stmt4 = new readTNode("4", "d");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);
	stmts.push_back(stmt3);
	stmts.push_back(stmt4);

	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Print - Test 1") {

	list<string> input = { "print", "a", ";" };

	TNode* stmt = new printTNode("1", "a");
	AST expectedAST(new stmtLstTNode(stmt));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Print - Test 2") {

	list<string> input = { "print", "a", ";" };

	TNode* stmt = new readTNode("1", "a");
	AST expectedAST(new stmtLstTNode(stmt));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require(!(actualAST).isEqual(expectedAST));
}

TEST_CASE("Print - Test 3") {

	list<string> input = { "print", "a", ";", "print", "b", ";" };

	TNode* stmt1 = new printTNode("1", "a");
	TNode* stmt2 = new printTNode("2", "b");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);
	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Print - Test 4") {

	list<string> input = { "print", "a", ";", "print", "b", ";" };

	TNode* stmt1 = new printTNode("1", "a");
	TNode* stmt2 = new readTNode("2", "b");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);
	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require(!(actualAST).isEqual(expectedAST));
}

TEST_CASE("Print - Test 5") {

	list<string> input = { "print", "a", ";", "print", "b", ";", "print", "c", ";", "print", "d", ";" };

	TNode* stmt1 = new printTNode("1", "a");
	TNode* stmt2 = new printTNode("2", "b");
	TNode* stmt3 = new printTNode("3", "c");
	TNode* stmt4 = new printTNode("4", "d");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);
	stmts.push_back(stmt3);
	stmts.push_back(stmt4);
	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}

TEST_CASE("Read and Print - Test 1") {

	list<string> input = { "print", "a", ";", "read", "b", ";", "print", "c", ";", "print", "d", ";" };

	TNode* stmt1 = new printTNode("1", "a");
	TNode* stmt2 = new readTNode("2", "b");
	TNode* stmt3 = new printTNode("3", "c");
	TNode* stmt4 = new printTNode("4", "d");
	vector<TNode*> stmts;
	stmts.push_back(stmt1);
	stmts.push_back(stmt2);
	stmts.push_back(stmt3);
	stmts.push_back(stmt4);
	AST expectedAST(new stmtLstTNode(stmts));

	AST actualAST;
	actualAST.buildtree(input);

	actualAST.print("", actualAST.getRoot(), false);

	require((actualAST).isEqual(expectedAST));
}


