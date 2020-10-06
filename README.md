# GroupDBSCAN

This repository contains the C and C++ implementations of the algorithm described in the paper `A fast DBSCAN clustering algorithm by accelerating neighbor searching using Groups method` [Paper Link](https://dl.acm.org/citation.cfm?id=2952211)

The core contribution of the algorithm is to define groups of points and then transfer all processing from point level computing to group level computing thus reducing the overall complexity of the spatial clustering algorithm.


## Setup
1. Clone the repository
2. Install dependencies (gcc/g++ - latest versions should work)

## Running the algorithm
1. Re-format your input according to the template below and store the file in a folder `/datasets/`

```
<number_of_data_points>
<dimension>
<data_1_dim_1> <data_1_dim_2> ... <data_1_dim_dimension>
.
.
.
...
```

For example
```
10
2
1 20
2 20
2 19
8 15
8 14
7 15
9 14
9 17
12 17
11 18
```

2. Running the algorithm
```shell
./run.sh <dataset> <epsilon> <minpts> <MinDegree Rtree> <MaxDegree Rtree>

where
- <dataset> is the name of the file formatted according to 1.
- <epsilon> represents the neighbourhood parameter (Anything within epislon distance from given point is considered a neighbour)
- <minpts> represents the density parameter. It defines the minimum number of neighbours required for a point to be classified as `dense`
- <MinDegree Rtree> and <MaxDegree Rtree> hyper-parameters and correspond to the minimum and maximum degree of the custom defined μC-RTree
```
