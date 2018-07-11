#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <ctime>
#include "groupsAlgorithm_cpp.h"
using namespace std;  

vector<int> unassigned_points;
Point** points = NULL;
int dim;
int num_points;
double eps;
int min_pts;
vector<Group*> groupList;
int group_id = 0;
int unassigned_size = 0;
int noise = 0;
vector<int> clusterpoints;

// void printPoint(Point* p)
// {
// 	cout << "Id: " << p->point_id << endl;
// 	cout << "Cluster: " << p->cluster_id << endl;
// 	cout << "Group: " << p->group_id << endl;
// 	cout << "Type: " << p->type << endl;
// 	cout << "Coordinates: ";

// 	for(int i=0;i<dim;i++)
// 	{
// 		cout << p->coordinates[i] << " ";
// 	}
	
// 	cout << endl;
// 	cout << "Print Point ENDS" << endl; 
// }

// void printGroup(Group* g)
// {
// 	cout << "Id: " << g->group_id << endl;
// 	cout << "NumPoints: " << g->num_points << endl;
// 	cout << "Master Point: " << g->master_point << endl;
// 	cout << "Point List: " << endl;
// 	cout << g->slave_points.size() << endl;

// 	for(int i=0;i<g->slave_points.size();i++)
// 		cout << g->slave_points[i] << " ";
// 	cout << endl;

// 	cout << g->reachable_groups.size() << endl;
// 	cout << "Reachable Groups: " << endl;

// 	for(int i=0;i<g->reachable_groups.size();i++)
// 		cout << g->reachable_groups[i] << " ";
// 	cout << endl;

// 	cout << "Threshold: " << g->threshold << endl;

// 	cout << "Print Group Ends" << endl;
// }

Group* createNewGroup(Point* p)
{
	Group* group = new Group;
	group->group_id = group_id++;
	group->num_points = 1;
	group->master_point = p->point_id;
	group->threshold = 0;
	p->group_id = group->group_id;
	group->slave_points.clear();
	group->reachable_groups.clear();
	return group;
}

void addPointToGroup(Point* p, Group* g)
{
	double dist = distance(p, points[g->master_point]);
	if(dist > eps)return;

	if(dist > g->threshold)g->threshold = dist;
		g->num_points++;

	p->group_id = g->group_id;
	g->slave_points.push_back(p->point_id);
}

long double distance(Point* p1, Point* p2)
{
	long double sum = 0;
	for(int i=0;i<dim;i++)
		sum += ((p1->coordinates[i] - p2->coordinates[i])*(p1->coordinates[i] - p2->coordinates[i]));
	
	return sqrt(sum);
}

void createGroupList()
{
	if(groupList.size())
	{
		groupList.clear();
	}
}


Group* findGroup(int id)
{
	return groupList[id];
}

vector<int> findNeighbours(int point_id, double eps)
{
	Point* p = points[point_id];
	vector<int> neighbours; //STORES ID's of the points that are neighbours.
	int group_id = p->group_id;
	Group* g = findGroup(group_id);

		if(point_id != g->master_point)
		{
			neighbours.push_back(g->master_point);
		}

		for(int i=0;i<g->slave_points.size();i++)
		{
			if(point_id != g->slave_points[i])
			{
				if(distance(points[point_id], points[g->slave_points[i]]) <= eps)
				{
					neighbours.push_back(g->slave_points[i]);
				}
			}
		}

		for(int i=0;i<g->reachable_groups.size();i++)
		{
			Group* rg = findGroup(g->reachable_groups[i]);
			if(distance(points[g->master_point], points[rg->master_point]) <= rg->threshold + eps + distance(points[g->master_point], points[point_id]))
			{
				if(distance(points[rg->master_point], points[point_id]) <= eps)
				{
					neighbours.push_back(rg->master_point);
				}
				
				for(int j=0;j<rg->slave_points.size();j++)
				{
					if(distance(points[rg->slave_points[j]], points[point_id]) <= eps)
					{
						neighbours.push_back(rg->slave_points[j]);
					}
				}
			}
		}
	return neighbours;
}
