#ifndef DEF_H
#define DEF_H

// #include "vector.h"
#include <stdbool.h>

#define UNCLASSIFIED -2

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3

#define NOGROUP -5

typedef long long int ll;
typedef struct Cluster Cluster;
typedef struct Point Point;
typedef struct group* Group;
typedef struct GroupList GroupList;
typedef struct vector vector;
typedef enum VECTORTYPE VECTORTYPE;
typedef enum TYPE TYPE;

enum VECTORTYPE {INTEGER, GROUP};

enum TYPE {NOISE=-1, BORDER, CORE};//BORDER = 0 //CORE = 1

// vector->items[i] gives the pointer for the ith element
struct vector 
{
    VECTORTYPE type;
    Group* groupItems;
    int* intItems; 

    int capacity;
    int total;

};

struct Cluster
{
	int cluster_id;
	int core_pts;
	vector* point_ids;
};

struct Point
{
	int point_id;
	int cluster_id;
	int group_id;
	TYPE type;
	double* coordinates;
};

struct group
{
	int group_id;
	int num_points;
	int master_point;
	double threshold;
	vector* slave_points;
	vector* reachable_groups; //CONTAINS ONLY ID's OF REACHABLE GROUPS. GROUPS CAN BE ACCESSED USING THIS ID.
};

struct GroupList
{
	Group* curr;
	GroupList* next;
};

#endif