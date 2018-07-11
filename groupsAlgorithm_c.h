/**************************     HEADER FILE     **************************/
#ifndef GROUPDBSCAN_H
#define GROUPDBSCAN_H

#include "Def.h"
#include "vector_c.h"
//METHOD DECLARATIONS
// void printPoint(Point* p);
// void printGroup(Group* g);
Group createNewGroup(Point* p);
void addPointToGroup(Point* p, Group g);
long double distance(Point* p1, Point* p2);
void createGroupList();
void GroupsAlgorithm(double eps, int min_pts);
Group findGroup(int id);
vector* findNeighbours(int point_id, double eps);

/**************************     HEADER FILE ENDS     **************************/
#endif