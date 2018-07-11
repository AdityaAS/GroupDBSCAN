/**************************     HEADER FILE     **************************/
#ifndef DBSCAN_H
#define DBSCAN_H

#include "vector_c.h"

void destroy_neighbours(vector* neighbours);
void dbscan(double epsilon, int min_pts);
int expand(int index, int cluster_id, Point** points, int num_points, double epsilon, int min_pts);

#endif