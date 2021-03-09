#!/bin/bash

# shellcheck disable=SC2164
cd build
ctest --extra-verbose --repeat-until-fail 10 --timeout 10 --build-and-test
# shellcheck disable=SC2103
cd ..

FILES_SEQ="build/bin/*_seq"
for file in $FILES_SEQ; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
done

# FILES_OMP="build/bin/*_omp"
# for file in $FILES_OMP; do
#         echo "--------------------------------"
#         echo $(basename $file)
#         echo "--------------------------------"
#         valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
# done

# FILES_TBB="build/bin/*_tbb"
# for file in $FILES_TBB; do
#         echo "--------------------------------"
#         echo $(basename $file)
#         echo "--------------------------------"
#         valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
# done

FILES_STD="build/bin/*_std"
for file in $FILES_STD; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
done

FILES_MPI="build/bin/*_mpi"
for file in $FILES_MPI; do
    if [ "$file" = "build/bin/*_mpi" ]; then continue; fi
    echo "--------------------------------"
    # shellcheck disable=SC2046
    echo $(basename $file)
    echo "--------------------------------"
    if [[ $OSTYPE == "linux-gnu" ]]; then
        NUM_PROC=$(cat /proc/cpuinfo|grep processor|wc -l)
    elif [[ $OSTYPE == "darwin"* ]]; then
        NUM_PROC=$(sysctl -a | grep machdep.cpu | grep thread_count | cut -d ' ' -f 2)
    else
        echo "Unknown OS"
        NUM_PROC="1"
    fi
    echo "NUM_PROC: " $NUM_PROC
    # shellcheck disable=SC2034
    for i in {1..10}; do
        mpirun -np $NUM_PROC $file || exit 1
    done
done
