#include<stdio.h>
#include <iostream>
#include <typeinfo>
#include "TNode.h"

void TNode::insertFront(TNode* node) {
	this->childNodes.insert(childNodes.begin(), node);
}

void TNode::insertBack(TNode* node) {
	this->childNodes.push_back(node);
}

string TNode::getData()
{
	return this->data;
}

vector<TNode*> TNode::getChildNodes()
{
	return this->childNodes;
}

bool TNode::isEqual(TNode* node) {

	if (typeid(*this) == typeid(*node) && this->data == node->data) {

		while (!this->childNodes.empty()) {
			if (node->childNodes.empty()) {
				return false;
			}
			else {
				vector<TNode*> myChild = this->childNodes;
				vector<TNode*> compareChild = node->childNodes;

				int pos = 0;
				bool equal = true;
				for each (TNode* myNode in myChild) {
					equal = equal && myNode->isEqual(compareChild.at(pos));
					pos++;
				}
				return equal;
			}
		}

		if (node->childNodes.empty()) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

TNode* AST::getRoot()
{
	return this->root;
}

string AST::print(string prefix, TNode* node, int pos) {

	if (node != nullptr)
	{
		cout << prefix;

		cout << ("---");

		// print the type and value of the node
		cout << node->getData() << ":" << typeid(*node).name() << endl;

		// enter the next tree level - left and right branch
		int pos = node->getChildNodes().size() - 1;
		for each (TNode* child in node->getChildNodes()) {
			print(prefix + (pos > -1 ? "|   " : "    "), node->getChildNodes().at(pos), pos);
			pos--;
		}
	}

	return "";
}

void AST::buildtree(list<string> input) {
	/* TODO: use actual tokenizer implemented in Parser */

	vector<TNode*> stmts;

	int line_no = 1;
	for (auto it = input.begin(); it != input.end(); ++it)
	{
		if (*it == "read") {
			auto var = next(it, 1);
			stmts.push_back(new readTNode(to_string(line_no), *var));
			line_no++;
		}
		else if (*it == "print") {
			auto var = next(it, 1);
			stmts.push_back(new printTNode(to_string(line_no), *var));
			line_no++;
		}
		else {
			continue;
		}
	}

	this->root = new stmtLstTNode(stmts);
}

bool AST::isEqual(AST ast)
{
	return this->root->isEqual(ast.root);
}
