#!/bin/bash

if [[ $OSTYPE == "linux-gnu" ]]; then
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/core_func_tests
  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/reference_func_tests
#  valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./build/bin/tbb_func_tests
fi

./build/bin/core_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
./build/bin/reference_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
./build/bin/tbb_func_tests --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating

FILES_SEQ="build/bin/*_seq"
for file in $FILES_SEQ; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        if [[ $OSTYPE == "linux-gnu" ]]; then
          valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
        fi
        ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_STL="build/bin/*_stl"
for file in $FILES_STL; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        if [[ $OSTYPE == "linux-gnu" ]]; then
          valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
        fi
        ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
       echo "--------------------------------"
       echo $(basename $file)
       echo "--------------------------------"
#        if [[ $OSTYPE == "linux-gnu" ]]; then
#          valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
#        fi
      ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
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

    if [[ $OSTYPE == "linux-gnu" ]]; then
        mpirun --oversubscribe -np $NUM_PROC $file --gtest_repeat=10 || exit 1
    elif [[ $OSTYPE == "darwin"* ]]; then
        mpirun -np $NUM_PROC $file --gtest_repeat=10 || exit 1
    fi

done
