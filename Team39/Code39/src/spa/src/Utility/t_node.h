#pragma once
#include <string>
#include <vector>
#include <list>
using namespace std;

class TNode {

private:
	
	string data;
	vector<TNode*> childNodes;

public:

	/* virtual destructor for polymorphic class*/
	virtual ~TNode() = default;

	TNode() {
		this->data = "";
		this->childNodes = { };
	}

	TNode(string data) {
		this->data = data;
		this->childNodes = { };
	}

	TNode(string data, vector<TNode*> nodes) {
		this->data = data;
		this->childNodes = nodes;
	}

	TNode(string data, TNode* node) {
		this->data = data;
		this->childNodes = { node };
	}

	/* WIP, should merge into one insert function only. */
	void insertFront(TNode* node);
	void insertBack(TNode* node);

	/*Returns the data of TNode*/
	string getData();

	/*Returns all the children of TNode*/
	vector<TNode*> getChildNodes();

	/*Compares the type and data of 2 TNodes. Used for testing.*/
	bool isEqual(TNode* node);
};

// TODO(Zhenlin): [Implementation] Currently treating all the expr/rel_expr/constant as variables (non-statements)
class varTNode : public TNode {
public:

	/*data = var_name, childNodes = empty.*/
	varTNode(string var_name) : TNode(var_name) { }
};

class readTNode : public TNode {
public:

	/*data = line_no, childNodes = varTNode.*/
	readTNode(string line_no, string var_name) : TNode(line_no, new varTNode(var_name)) { }

};

class printTNode : public TNode {
public:

	/*data = line_no, childNodes = varTNode.*/
	printTNode(string line_no, string var_name) : TNode(line_no, new varTNode(var_name)) { }

};

class stmtLstTNode : public TNode {
public:

	/*data = "stmtLst", childNodes = list of nodes.*/
	stmtLstTNode(vector<TNode*> nodes) : TNode("stmtLst", nodes) { }
};
