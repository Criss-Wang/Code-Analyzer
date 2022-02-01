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

				bool equal = true;
				for (int pos = 0; pos < myChild.size(); pos++) {
					equal = equal && myChild.at(pos)->isEqual(compareChild.at(pos));
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
