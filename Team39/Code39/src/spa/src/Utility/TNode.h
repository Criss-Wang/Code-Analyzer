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

	virtual ~TNode() = default;
	TNode() {
		this->data = "";
	}

	TNode(string data) {
		this->data = data;
	}

	TNode(string data, vector<TNode*> nodes) {
		this->data = data;
		this->childNodes = nodes;
	}

	TNode(string data, TNode* nodes) {
		this->data = data;
		this->childNodes.push_back(nodes);
	}

	/* WIP, should merge into one insert function only. */
	void insertFront(TNode* node);

	void insertBack(TNode* node);

	string getData();

	vector<TNode*> getChildNodes();

	bool isEqual(TNode* node);

};

class varTNode : public TNode {
public:
	varTNode(string var_name) : TNode(var_name) {
	}
};

class readTNode : public TNode {
public:
	readTNode(string line_no, string var_name) : TNode(line_no, new varTNode(var_name)) { }
};

class printTNode : public TNode {
public:
	printTNode(string line_no, string var_name) : TNode(line_no, new varTNode(var_name)) { }
};

class stmtLstTNode : public TNode {
public:
	stmtLstTNode(vector<TNode*> nodes) : TNode("stmtLst", nodes) { }

	stmtLstTNode(TNode* nodes) : TNode("stmtLst", nodes) { }
};

class AST {

private:

	TNode* root = nullptr;

public:

	AST() {	};

	AST(TNode* node) {
		root = node;
	};

	TNode* getRoot();

	string print(string prefix, TNode* node, int pos);

	void buildtree(list<string> input);

	bool isEqual(AST ast);
};
