#include "SearchRelations.h"


SearchRelations::SearchRelations(bool **picture) {
	this->picture = picture;
	vertex = new list<int>();
}


SearchRelations::~SearchRelations() {
	delete *picture;
	delete picture;
}
