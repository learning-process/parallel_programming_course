#!/bin/bash

FILES_SEQ="build/bin/*_seq"
for file in $FILES_SEQ; do
    if [ "$file" = "build/bin/*_seq" ]
    then
        continue
    fi
    start_seq=`date +%s`
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    end_seq=`date +%s`
    runtime=$((end_seq-start_seq))
    if [ "$runtime" -gt "5" ]
    then
        echo "Alert: runtime > 5 sec. runtime = $runtime sec."
        exit 1
    fi
done


FILES_OMP="build/bin/*_omp"
for file in $FILES_OMP; do
    if [ "$file" = "build/bin/*_omp" ]
    then
        continue
    fi
    start_omp=`date +%s`
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    end_omp=`date +%s`
    runtime=$((end_omp-start_omp))
    if [ "$runtime" -gt "5" ]
    then
        echo "Alert: runtime > 5 sec. runtime = $runtime sec."
        exit 1
    fi
done


FILES_TBB="build/bin/*_tbb"
for file in $FILES_TBB; do
    if [ "$file" = "build/bin/*_tbb" ]
    then
        continue
    fi
    start_tbb=`date +%s`
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    end_tbb=`date +%s`
    runtime=$((end_tbb-start_tbb))
    if [ "$runtime" -gt "5" ]
    then
        echo "Alert: runtime > 5 sec. runtime = $runtime sec."
        exit 1
    fi
done


FILES_STD="build/bin/*_std"
for file in $FILES_STD; do
    if [ "$file" = "build/bin/*_std" ]
    then
        continue
    fi
    start_std=`date +%s`
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    end_std=`date +%s`
    runtime=$((end_std-start_std))
    if [ "$runtime" -gt "5" ]
    then
        echo "Alert: runtime > 5 sec. runtime = $runtime sec."
        exit 1
    fi
done

FILES_MPI="build/bin/*_mpi"
for file in $FILES_MPI; do
    if [ "$file" = "build/bin/*_mpi" ]
    then
        continue
    fi
    start_mpi=`date +%s`
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        if [[ $OSTYPE == 'linux' ]]; then
            NUM_PROC=$(cat /proc/cpuinfo|grep processor|wc -l)
        elif [[ $OSTYPE == 'osx' ]]; then
            NUM_PROC=$(sysctl -a | grep machdep.cpu | grep thread_count | cut -d ' ' -f 2)
        else
            echo "Unknown OS"
            NUM_PROC="1"
        fi
        echo "NUM_PROC: " $NUM_PROC
        for i in {1..10}; do
            mpirun -np $NUM_PROC $file || exit 1
        done
    end_mpi=`date +%s`
    runtime=$((end_mpi-start_mpi))
    if [ "$runtime" -gt "5" ]
    then
        echo "Alert: runtime > 5 sec. runtime = $runtime sec."
        exit 1
    fi
done
