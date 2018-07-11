/**************************     HEADER FILE     **************************/
#ifndef DBSCAN_H
#define DBSCAN_H

void destroy_neighbours(vector<int> neighbours);
void dbscan(Point** points, int num_points, double epsilon, int min_pts);
int expand(int index, int cluster_id, Point** points, int num_points, double epsilon, int min_pts);

#endif