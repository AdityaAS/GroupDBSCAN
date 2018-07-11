path='/path/to/data/'
echo $1
echo $2
echo $3

make -f makefile_c

./output_c $path$1 $2 $3 FINAL/output_$1_eps=$2_minpts=$3.txt

gprof output_c gmon.out >  FINAL/profile_$1_eps=$2_minpts=$3.txt

rm gmon.out
make -f makefile_c clean
