#!/bin/bash

FILES_REF="build/bin/*_ref"
for file in $FILES_REF; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$file
        ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_SEQ="build/bin/*_seq"
for file in $FILES_SEQ; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_STD="build/bin/*_std"
for file in $FILES_STD; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
       echo "--------------------------------"
       echo $(basename $file)
       echo "--------------------------------"
      ./$file --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating
done

FILES_TBB="build/bin/*_tbb"
for file in $FILES_TBB; do
       echo "--------------------------------"
       echo $(basename $file)
       echo "--------------------------------"
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

    mpirun -np $NUM_PROC $file --gtest_repeat=10 || exit 1
done
