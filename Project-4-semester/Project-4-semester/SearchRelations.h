#pragma once
#include <list>
using namespace std;

class SearchRelations
{
public:
	SearchRelations(bool **picture);
	~SearchRelations();

private:
	bool **picture;
	list<int> *vertex;
};

