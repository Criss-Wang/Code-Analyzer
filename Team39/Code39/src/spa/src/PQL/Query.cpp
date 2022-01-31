

#include "Query.h"

Query::Query() {

}

Query::~Query() {

}

Query::EntityType Query::getSelectType() {
	return Query::EntityType::STMT;
}