#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "groupsAlgorithm_c.h"
#include "dbscan_c.h"
#include "vector_c.h"

// #define PRINT 1
// #define DEBUG 1

extern vector* unassigned_points;

extern Point** points;
extern int dim;
extern int num_points;
extern double eps;
extern int min_pts;
extern vector* groupList;
extern int group_id;
extern int unassigned_size;
extern int noise;
extern vector* clusterpoints;
extern int* addhelper;

FILE* output;

void destroy_neighbours(vector* neighbours)
{
    VECTOR_FREE(neighbours);
}

void printGroupInfo(vector* groupList, FILE* outfile)
{
    int i, j;
    for(i=0;i<VECTOR_TOTAL(groupList);i++)
    {
        Group g = groupList->groupItems[i];
        fprintf(outfile, "GroupID: %d \t TotalPoints: %d\n", g->group_id, g->num_points);
        fprintf(outfile, "PointIDs: \n");
        for(j=0;j<VECTOR_TOTAL(g->slave_points);j++)
        {
            fprintf(outfile, "%d\t", g->slave_points->intItems[j]);
            if(j!=0 && j%5==0)fprintf(outfile, "\n");
        }
        fprintf(outfile, "\n");
        fprintf(outfile, "ReachableGroups: \n");
        for(j=0;j<VECTOR_TOTAL(g->reachable_groups);j++)
        {
            fprintf(outfile, "%d\t", g->reachable_groups->intItems[j]);
            if(j!=0 && j%5==0)fprintf(outfile, "\n");
        }
        fprintf(outfile, "\n\n");
    }
}

void destroydata(Point** points, vector* groupList, Cluster** clusters, int clustersize)
{

    int i;
    for(i=0;i<num_points;i++)
    {
        free(points[i]->coordinates);
        free(points[i]);
    }
    free(points);

    for(i=0;i<VECTOR_TOTAL(groupList);i++)
    {
        VECTOR_FREE(groupList->groupItems[i]->reachable_groups);
        VECTOR_FREE(groupList->groupItems[i]->slave_points);
    }
    free(groupList);

    for(i=0;i<clustersize;i++)
    {
        VECTOR_FREE(clusters[i]->point_ids);
        free(clusters[i]);
    }
    free(clusters);
}

void dbscan(double epsilon, int min_pts)
{
    int totalCore = 0;
    int cluster_id = 0;
    int i;
    for (i = 0; i < num_points; ++i) 
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
    clusterpoints = (vector*)malloc(sizeof(vector));
    
    VECTOR_INIT(clusterpoints, INTEGER);
    addhelper[0] = 0;

    for(i=0;i<cluster_id;i++)
    {
        clusters[i] = (Cluster*) malloc(sizeof(Cluster));
        clusters[i]->cluster_id = i;
        clusters[i]->core_pts = 0;

        clusters[i]->point_ids = (vector*)malloc(sizeof(vector));
        VECTOR_INIT(clusters[i]->point_ids, INTEGER);

        VECTOR_ADD(clusterpoints, addhelper);
    }

    noise = 0;
    for(i=0;i<num_points;i++)
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
            addhelper[0] = i;
            VECTOR_ADD(clusters[points[i]->cluster_id]->point_ids, addhelper);

            if(points[i]->type == CORE)
            {
                totalCore++;
                clusters[points[i]->cluster_id]->core_pts++;
            }
            clusterpoints->intItems[points[i]->cluster_id]++;
        }
    }

    fprintf(output, "TotalClusters: %d\n",cluster_id);
    fprintf(output, "TotalNoise: %d\n", noise);
    fprintf(output, "TotalCore: %d\n", totalCore);
    // fprintf(stdout, "TotalClusters: %d\n",cluster_id);
    // fprintf(stdout, "TotalNoise: %d\n", noise);
    // fprintf(stdout, "TotalCore: %d\n", totalCore);

    for(i=0;i<cluster_id;i++)
    {
        fprintf(output, "Cluster: %d \t Total: %d \t Core: %d\n", i, VECTOR_TOTAL(clusters[i]->point_ids), clusters[i]->core_pts);
    }
}

int expand(int index, int cluster_id, Point** points, int num_points, double epsilon, int min_pts)
{
    int return_value = NOT_CORE_POINT;
    vector* seeds = findNeighbours(index, epsilon);
    Group g = findGroup(points[index]->group_id);
    int i;
    if (VECTOR_TOTAL(seeds) < min_pts)
    {
        points[index]->cluster_id = NOISE;
    }
    else
    {
        points[index]->type = CORE;
        points[index]->cluster_id = cluster_id;
        vector* queue = seeds;
        int head = 0;

        for(i=0;i<VECTOR_TOTAL(seeds);i++)
            points[seeds->intItems[i]]->cluster_id = cluster_id;

        while(head != VECTOR_TOTAL(seeds))
        {
            int id = queue->intItems[head];
            head++;
            vector* spread = findNeighbours(id, epsilon);
            
            if(VECTOR_TOTAL(spread) >= min_pts) 
            {
                points[id]->type = CORE;
                for(i=0;i<VECTOR_TOTAL(spread);i++)
                {
                    if (points[spread->intItems[i]]->cluster_id == NOISE || points[spread->intItems[i]]->cluster_id == UNCLASSIFIED) 
                    {
                        if (points[spread->intItems[i]]->cluster_id == UNCLASSIFIED)
                        {
                            addhelper[0] = (points[spread->intItems[i]]->point_id);
                            VECTOR_ADD(queue, addhelper);
                        }
                        points[spread->intItems[i]]->cluster_id = cluster_id;
                    }
                }
            }
            VECTOR_FREE(spread);
            free(spread);
        }

        return_value = CORE_POINT;
    }
    VECTOR_FREE(seeds);
    free(seeds);

    return return_value;
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char const *argv[])
{
    clock_t global_t = clock();
    addhelper = (int*)malloc(sizeof(int)*1);

    if(argc != 5)
    {
        fprintf(stderr, "Usage: ./<Program> <InputFile> <Eps> <MinPts> <ClusterOutputFile>\n");
        fprintf(stderr, "Exiting\n");
        return 0;
    }

    unassigned_points = (vector*)malloc(sizeof(vector));
    VECTOR_INIT(unassigned_points, INTEGER);

    groupList = (vector*)malloc(sizeof(vector));
    VECTOR_INIT(groupList, GROUP);

    eps = strtod(argv[2], NULL);
    min_pts = strtod(argv[3], NULL);

    char input_file[100];
    strcpy(input_file, argv[1]);
    FILE* input=fopen(argv[1], "r");
    output = fopen(argv[4], "w");
    // FILE* groupInfoFile = fopen("groupInfo_C.txt", "w");

    if(fscanf(input, "%d %d", &num_points, &dim)!=2)
    {
        fprintf(stderr, "Unable to read input from file %s", input_file);
        return -1;
    }

    points=(Point**)malloc(num_points*sizeof(Point*));  
    clock_t local_t  = clock();
    int i, j, k;
	
    for(i=0;i<num_points;i++)
    {
        points[i] = (Point*)malloc(sizeof(Point));
        points[i]->coordinates = (double*)malloc(sizeof(double)*dim);
        points[i]->point_id = i;
        points[i]->group_id = NOGROUP;
        points[i]->cluster_id = UNCLASSIFIED;
        points[i]->type = BORDER;
   
        for(j=0;j<dim;j++)
        {
            double p;
            fscanf(input, "%lf", &p);
            points[i]->coordinates[j] = p;
        }

        bool flag = false;
        flag = false;

        if(VECTOR_ISEMPTY(groupList))
        {
            Group g = createNewGroup(points[i]);
            VECTOR_ADD(groupList, g);
        }
        else
        {
            for(j=0;j<VECTOR_TOTAL(groupList);j++)
            {
                if(distance(points[groupList->groupItems[j]->master_point], points[i]) <= eps)
                {
                    addPointToGroup(points[i], groupList->groupItems[j]);   
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
                for(j=0; j<VECTOR_TOTAL(groupList); j++)
                {
                    if(distance(points[groupList->groupItems[j]->master_point], points[i]) <= 2*eps)
                    {
                        flag = true;
                        break;
                    }
                }
                if(!flag)
                {
                    Group g = createNewGroup(points[i]);
                    VECTOR_ADD(groupList, g);
                }
                else
                {
                    addhelper[0] = i;
                    VECTOR_ADD(unassigned_points, addhelper);
                }
            }   
        }
    }

    bool uflag;
    for(i=0;i<VECTOR_TOTAL(unassigned_points);i++)
    {
        uflag = false;
        int size;
        size = VECTOR_TOTAL(groupList);
        for(j=0;j<size;j++)
        {
            if(distance(points[groupList->groupItems[j]->master_point], points[unassigned_points->intItems[i]]) <= eps)
            {
                uflag = true;
                addPointToGroup(points[unassigned_points->intItems[i]], groupList->groupItems[j]);
                break;
            }
        }
        if(!uflag)
        {
            Group g = createNewGroup(points[unassigned_points->intItems[i]]);
            VECTOR_ADD(groupList, g); 
        }
    }
    VECTOR_FREE(unassigned_points);
    free(unassigned_points);
    // local_t = clock() - local_t;
    // fprintf(output, "Time for Groups DS Formation: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);


    // local_t = clock();
    for(i=0;i<VECTOR_TOTAL(groupList);i++)
    {
        for(k=i+1;k<VECTOR_TOTAL(groupList);k++)
        {         
            if(distance(points[groupList->groupItems[i]->master_point],
                        points[groupList->groupItems[k]->master_point]) <= eps + groupList->groupItems[i]->threshold + groupList->groupItems[k]->threshold)
            {
                addhelper[0] = (groupList->groupItems[k]->group_id);
                VECTOR_ADD(groupList->groupItems[i]->reachable_groups, addhelper);

                addhelper[0] = (groupList->groupItems[i]->group_id);
                VECTOR_ADD(groupList->groupItems[k]->reachable_groups, addhelper);

            }
        }
    }


    // local_t = clock() - local_t;
    // fprintf(output, "Time for reachable groups identification: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);


    /******************** INPUT READ DONE  ********************/
    // local_t = clock();
    
    // printGroupInfo(groupList, groupInfoFile);

    dbscan(eps, min_pts);
    
    // local_t = clock() - local_t;
    // fprintf(output, "Time for DBSCAN: %Lf sec\n", (long double)local_t/CLOCKS_PER_SEC);


    fclose(input);
    global_t = clock() - global_t;
    fprintf(output, "Total Program Time: %Lf sec\n", (long double)global_t/CLOCKS_PER_SEC);

    // FILE* neighboursFile = fopen("neighboursFile.txt", "w");
    // for(i=0;i<num_points;i++)
    // {
    //     fprintf(neighboursFile, "POINT: %d\n", i);
    //     fprintf(neighboursFile, "NEIGHBOURS:\n");
    //     vector* seeds = findNeighbours(i, eps);
    //     qsort(seeds->intItems, VECTOR_TOTAL(seeds), sizeof(int), cmpfunc);
    //     int j=0;
    //     for(j=0;j<VECTOR_TOTAL(seeds);j++)
    //     {
    //         fprintf(neighboursFile, "%d ", seeds->intItems[j]);
    //     }
    //     VECTOR_FREE(seeds);
    //     free(seeds);
    //     fprintf(neighboursFile, "\n\n");
    // }

    for(i=0; i<VECTOR_TOTAL(groupList); i++)
    {
        Group group = groupList->groupItems[i];
        VECTOR_FREE(group->slave_points);
        VECTOR_FREE(group->reachable_groups);
        free(group->reachable_groups);
        free(group->slave_points);
        free(group);
    }

    return 0;
}










