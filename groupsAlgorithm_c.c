#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// #include <vector>

#include "groupsAlgorithm_c.h"

vector* unassigned_points;
Point** points = NULL;
int dim;
int num_points;
double eps;
int min_pts;
vector* groupList;
int group_id = 0;
int unassigned_size = 0;
int noise = 0;
int* addhelper;

vector* clusterpoints;
// vector<int> clusterpoints;

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

Group createNewGroup(Point* p)
{
	Group group = (Group)malloc(sizeof(struct group));
	group->group_id = group_id++;
	group->num_points = 1;
	group->master_point = p->point_id;
	group->threshold = 0;
	group->slave_points = (vector*)malloc(sizeof(vector));
	group->reachable_groups = (vector*)malloc(sizeof(vector));

	VECTOR_INIT(group->slave_points, INTEGER);
	VECTOR_INIT(group->reachable_groups, INTEGER);

	p->group_id = group->group_id;
	
	return group;
}

void addPointToGroup(Point* p, Group g)
{
	double dist = distance(p, points[g->master_point]);
	if(dist > eps)return;

	if(dist > g->threshold)g->threshold = dist;
		g->num_points++;

	p->group_id = g->group_id;
	addhelper[0] = p->point_id;
	VECTOR_ADD(g->slave_points, addhelper);

}

long double distance(Point* p1, Point* p2)
{
	int i;
	long double sum = 0;
	for(i=0;i<dim;i++)
		sum += ((p1->coordinates[i] - p2->coordinates[i])*(p1->coordinates[i] - p2->coordinates[i]));
	
	return sqrt(sum);
}

void createGroupList()
{
	groupList = (vector*)malloc(sizeof(vector));
	VECTOR_INIT(groupList, GROUP);
}


Group findGroup(int id)
{
	if(id >=0 && id <= groupList->total)
		return groupList->groupItems[id];
}

vector* findNeighbours(int point_id, double eps)
{
	Point* p = points[point_id];

	vector* neighbours = (vector*)malloc(sizeof(vector));
	VECTOR_INIT(neighbours, INTEGER);

	int i, j;
	int group_id = p->group_id;
	Group g = findGroup(group_id);

	if(point_id != g->master_point)
	{
		addhelper[0] = g->master_point;
		VECTOR_ADD(neighbours, addhelper);
	}

	for(i=0;i<VECTOR_TOTAL(g->slave_points);i++)
	{
		if((point_id != g->slave_points->intItems[i]) && distance(points[point_id], points[g->slave_points->intItems[i]]) <= eps)
		{
			addhelper[0] = g->slave_points->intItems[i];
			VECTOR_ADD(neighbours, addhelper);
		}
	}

	for(i=0;i<VECTOR_TOTAL(g->reachable_groups);i++)
	{
		Group rg = findGroup(g->reachable_groups->intItems[i]);
		if(distance(points[g->master_point], points[rg->master_point]) <= rg->threshold + eps + distance(points[g->master_point], points[point_id]))
		{
			if(distance(points[rg->master_point], points[point_id]) <= eps)
			{
				addhelper[0] = rg->master_point;
				VECTOR_ADD(neighbours, addhelper);
			}
			
			for(j=0;j<VECTOR_TOTAL(rg->slave_points);j++)
			{
				if(distance(points[rg->slave_points->intItems[j]], points[point_id]) <= eps)
				{
					addhelper[0] = (rg->slave_points->intItems[j]);
					VECTOR_ADD(neighbours, addhelper);
				}
			}
		}
	}
	return neighbours;
}
