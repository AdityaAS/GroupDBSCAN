#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <deque>
#include <ctime>
#include "groupsAlgorithm_cpp.h"
#include "dbscan_cpp.h"

// #define PRINT 1

using namespace std;

extern vector<int> unassigned_points;
extern Point** points;
extern int dim;
extern int num_points;
extern double eps;
extern int min_pts;
extern vector<Group*> groupList;
extern int group_id;
extern int unassigned_size;
extern int noise;
extern vector<int> clusterpoints;
FILE* output;

void destroy_neighbours(vector<int> neighbours)
{
    neighbours.clear();
}
void printGroupInfo(vector<Group*> groupList, FILE* outfile)
{
    int i, j;
    for(i=0;i<groupList.size();i++)
    {
        Group* g = groupList[i];
        fprintf(outfile, "GroupID: %d \t TotalPoints: %d\n", g->group_id, g->num_points);
        fprintf(outfile, "PointIDs: \n");
        for(j=0;j<g->slave_points.size();j++)
        {
            fprintf(outfile, "%d\t", g->slave_points[j]);
            if(j!=0 && j%5==0)fprintf(outfile, "\n");
        }
        fprintf(outfile, "\n");
        fprintf(outfile, "ReachableGroups: \n");
        for(j=0;j<g->reachable_groups.size();j++)
        {
            fprintf(outfile, "%d\t", g->reachable_groups[j]);
            if(j!=0 && j%5==0)fprintf(outfile, "\n");
        }
        fprintf(outfile, "\n\n");
    }
}
void destroydata(Point** points, vector<Group*> groupList, Cluster** clusters, int clustersize)
{
    for(int i=0;i<num_points;i++)
    {
        points[i]->coordinates.clear();
        delete points[i];
    }
    delete points;
    for(int i=0;i<groupList.size();i++)
    {
        groupList[i]->reachable_groups.clear();
        groupList[i]->slave_points.clear();
        groupList[i]->eps_groups.clear();
        delete groupList[i];
    }
    for(int i=0;i<clustersize;i++)
    {
        clusters[i]->point_ids.clear();
        delete clusters[i];
    }
    delete clusters;
}

void dbscan(double epsilon, int min_pts)
{
    int cluster_id = 0;

    for (int i = 0; i < num_points; ++i) 
    {
        if (points[i]->cluster_id == UNCLASSIFIED) 
        {
            if (expand(i, cluster_id, points, num_points, epsilon, min_pts) == CORE_POINT)
            {
                cluster_id+=1;
            }
        }
    }

    Cluster** clusters = (Cluster**)malloc(sizeof(Cluster*)*cluster_id);

    for(int i=0;i<cluster_id;i++)
    {
        clusters[i] = new Cluster;
        clusters[i]->cluster_id = i;
        clusters[i]->core_pts = 0;
        clusters[i]->point_ids.clear();
        clusterpoints.push_back(0);
    }

    noise = 0;
    for(int i=0;i<num_points;i++)
    {
        if(points[i]->cluster_id == UNCLASSIFIED)
        {
            fprintf(output, "Cluster Unassigned Error\n");
            return ;
        }

        if(points[i]->cluster_id == NOISE)
        {
            noise++;
        }

        if(points[i]->cluster_id >=0)
        {
            clusters[points[i]->cluster_id]->point_ids.push_back(i);

            if(points[i]->type == CORE)clusters[points[i]->cluster_id]->core_pts++;
            
            clusterpoints[points[i]->cluster_id]++;
        }
    }

    fprintf(output, "No of Clusters: %d\n",cluster_id);
    fprintf(output, "Noise: %d\n", noise);

    for(int i=0;i<cluster_id;i++)
    {
        fprintf(output, "Cluster: %d \t Total: %lu \t Core: %d\n", i, clusters[i]->point_ids.size(), clusters[i]->core_pts);
    }

    destroydata(points, groupList, clusters, cluster_id);
}

int expand(int index, int cluster_id, Point** points, int num_points, double epsilon, int min_pts)
{
    int return_value = NOT_CORE_POINT;
    vector<int> seeds = findNeighbours(index, epsilon);
    Group* g = findGroup(points[index]->group_id);

    if (seeds.size() < min_pts)
    {
        points[index]->cluster_id = NOISE;
    }
    else
    {
        points[index]->type = CORE;
        points[index]->cluster_id = cluster_id;
        queue<int, deque<int> > q (deque<int> (seeds.begin(),seeds.end())) ;

        for(int i=0;i<seeds.size();i++)
            points[seeds[i]]->cluster_id = cluster_id;

        while(!q.empty())
        {
            int id = q.front();
            q.pop();
            vector<int> spread = findNeighbours(id, epsilon);
            
            if(spread.size() >= min_pts) 
            {
                points[id]->type = CORE;
                for(int i=0;i<spread.size();i++)
                {
                    if (points[spread[i]]->cluster_id == NOISE || points[spread[i]]->cluster_id == UNCLASSIFIED) 
                    {
                        if (points[spread[i]]->cluster_id == UNCLASSIFIED)
                        {
                            q.push(points[spread[i]]->point_id);
                        }
                        points[spread[i]]->cluster_id = cluster_id;
                    }
                }
            }
            spread.clear();
        }

        return_value = CORE_POINT;
    }
    seeds.clear();
    return return_value;
}

int main(int argc, char const *argv[])
{
    clock_t global_t = clock();

    if(argc != 5)
    {
        fprintf(stderr, "Usage: <Program> <InputFile> <Eps> <MinPts> <ClusterOutputFile>\n");
        fprintf(stderr, "Exiting\n");
        return 0;
    }

    eps = strtod(argv[2], NULL);
    min_pts = strtod(argv[3], NULL);

    char input_file[100];
    strcpy(input_file, argv[1]);
    FILE* input=fopen(argv[1], "r");
    output = fopen(argv[4], "w");
    FILE* groupInfoFile = fopen("groupInfo_CPP.txt", "w");
    output = stdout;
    if(fscanf(input, "%d %d", &num_points, &dim)!=2)
    {
        fprintf(stderr, "Unable to read input from file %s", input_file);
        return -1;
    }

    points=(Point**)malloc(num_points*sizeof(Point*));  
    clock_t local_t  = clock();

    for(int i=0;i<num_points;i++)
    {
        points[i] = new Point;
        points[i]->coordinates.clear();
        points[i]->point_id = i;
        points[i]->group_id = NOGROUP;
        points[i]->cluster_id = UNCLASSIFIED;
        points[i]->type = BORDER;
   
        for(int j=0;j<dim;j++)
        {
            double p;
            fscanf(input, "%lf", &p);
            points[i]->coordinates.push_back(p);
        }

        bool flag = false;
        flag = false;
        if(groupList.empty())
        {
            Group* g = createNewGroup(points[i]);
            groupList.push_back(g);
        }
        else
        {
            for(int j=0;j<groupList.size();j++)
            {
                if(distance(points[groupList[j]->master_point], points[i]) <= eps)
                {
                    addPointToGroup(points[i], groupList[j]);   
                    flag = true;
                    break;
                }
            }
            if(flag)
            {
                ;
            }
            else
            {
                flag = false;
                for(int j=0; j<groupList.size(); j++)
                {
                    if(distance(points[groupList[j]->master_point], points[i]) <= 2*eps)
                    {
                        flag = true;
                        break;
                    }
                }
                if(!flag)
                {
                    Group* g = createNewGroup(points[i]);
                    groupList.push_back(g); 
                }
                else
                {
                    unassigned_points.push_back(i);
                }
            }   
        }
    }

    bool uflag;
    for(int i=0;i<unassigned_points.size();i++)
    {
        uflag = false;
        int size;
        size = groupList.size();
        for(int j=0;j<size;j++)
        {
            if(distance(points[groupList[j]->master_point], points[unassigned_points[i]]) <= eps)
            {
                uflag = true;
                addPointToGroup(points[unassigned_points[i]], groupList[j]);
                break;
            }
        }
        if(!uflag)
        {
            Group* g = createNewGroup(points[unassigned_points[i]]);
            groupList.push_back(g);
        }
    }
    unassigned_points.clear();
    local_t = clock() - local_t;
    fprintf(output, "Time for Groups DS Formation: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);


    local_t = clock();
    for(int i=0;i<groupList.size();i++)
    {
        for(int k=i+1;k<groupList.size();k++)
        {         
            if(distance(points[groupList[i]->master_point],
                        points[groupList[k]->master_point]) <= eps + groupList[i]->threshold + groupList[k]->threshold)
            {
                groupList[i]->reachable_groups.push_back(groupList[k]->group_id);
                groupList[k]->reachable_groups.push_back(groupList[i]->group_id);
            }
        }
    }
    local_t = clock() - local_t;
    fprintf(output, "Time for reachable groups identification: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);

    /******************** INPUT READ DONE  ********************/
    printGroupInfo(groupList, groupInfoFile);
    
    local_t = clock();
    dbscan(eps, min_pts);
    local_t = clock() - local_t;
    fprintf(output, "Time for DBSCAN: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);


    fclose(input);
    global_t = clock() - global_t;
    fprintf(output, "Total Program Time: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);

      return 0;
}










