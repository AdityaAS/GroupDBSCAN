/**************************     HEADER FILE     **************************/
#ifndef GROUPDBSCAN_H
#define GROUPDBSCAN_H
#include <vector>
using namespace std;

#define UNCLASSIFIED -2

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3

#define NOGROUP -5

enum TYPE {NOISE=-1, BORDER, CORE};
//BORDER = 0 //CORE = 1

typedef long long int ll;

struct Cluster
{
	int cluster_id;
	int core_pts;
	vector<int> point_ids;
};

struct Point
{
	int point_id;
	int cluster_id;
	int group_id;
	TYPE type;
	vector<double> coordinates;
};

struct Group
{
	int group_id;
	int num_points;
	int master_point;
	double threshold;
	vector<int> slave_points;
	vector<int> reachable_groups; //CONTAINS ONLY ID's OF REACHABLE GROUPS. GROUPS CAN BE ACCESSED USING THIS ID.
	vector<int> eps_groups; 	  //int reachable_groups_size;
};

struct GroupList
{
	Group* curr;
	GroupList* next;
};

//METHOD DECLARATIONS
// void printPoint(Point* p);
// void printGroup(Group* g);
Group* createNewGroup(Point* p);
void addPointToGroup(Point* p, Group* g);
long double distance(Point* p1, Point* p2);
void createGroupList();
void GroupsAlgorithm(double eps, int min_pts);
Group* findGroup(int id);
vector<int> findNeighbours(int point_id, double eps);

/**************************     HEADER FILE ENDS     **************************/
#endif