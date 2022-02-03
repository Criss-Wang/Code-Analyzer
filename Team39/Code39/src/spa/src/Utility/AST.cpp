#include<stdio.h>
#include <iostream>
#include <typeinfo>
#include "AST.h"

TNode* AST::getRoot() {
	return this->root;
}

string AST::print(string prefix, TNode* node, int pos) {

	if (node != nullptr) {
		cout << prefix;

		cout << ("---");

		// print the type and value of the node
		string type = typeid(*node).name();
		cout << node->getData() << ":" << type.substr(6) << endl;

		// enter the next tree level
		for (int pos = node->getChildNodes().size() - 1; pos > -1; pos--) {
			print(prefix + (pos > -1 ? "|   " : "    "), node->getChildNodes().at(pos), pos);
		}
	}

	return "";
}

void AST::buildtree(list<string> input) {
	/* TODO: use actual tokenizer implemented in Parser */

	vector<TNode*> stmts;

	int line_no = 1;
	for (auto it = input.begin(); it != input.end(); ++it) {
		if (*it == "read") {
			auto var = next(it, 1);
			stmts.push_back(new readTNode(to_string(line_no), *var));
			line_no++;
		} else if (*it == "print") {
			auto var = next(it, 1);
			stmts.push_back(new printTNode(to_string(line_no), *var));
			line_no++;
		} else {
			continue;
		}
	}

	this->root = new stmtLstTNode(stmts);
}

bool AST::isEqual(AST ast) {
	return this->root->isEqual(ast.root);
}


