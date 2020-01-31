#!/bin/bash
start_omp=`date +%s`
    FILES_OMP="build/bin/*_omp"
    for file in $FILES_OMP; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    done
end_omp=`date +%s`
runtime=$((end_omp-start_omp))
if [ "$runtime" -gt "5" ]
then
    echo "Alert: runtime > 5 sec. runtime = $runtime sec."
    exit 1
fi

start_tbb=`date +%s`
    FILES_TBB="build/bin/*_tbb"
    for file in $FILES_TBB; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    done
end_tbb=`date +%s`
runtime=$((end_tbb-start_tbb))
if [ "$runtime" -gt "5" ]
then
    echo "Alert: runtime > 5 sec. runtime = $runtime sec."
    exit 1
fi

start_std=`date +%s`
    FILES_STD="build/bin/*_std"
    for file in $FILES_STD; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        ./$file --gtest_repeat=10
    done
end_std=`date +%s`
runtime=$((end_std-start_std))
if [ "$runtime" -gt "5" ]
then
    echo "Alert: runtime > 5 sec. runtime = $runtime sec."
    exit 1
fi

start_mpi=`date +%s`
    FILES_MPI="build/bin/*_mpi"
    for file in $FILES_MPI; do
        echo "--------------------------------"
        echo $(basename $file)
        echo "--------------------------------"
        if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
            NUM_PROC=$(cat /proc/cpuinfo|grep processor|wc -l)
        elif [[ $TRAVIS_OS_NAME == 'osx' ]]; then
            NUM_PROC=$(sysctl -a | grep machdep.cpu | grep thread_count | cut -d ' ' -f 2)
        else
            echo "Unknown OS"
            NUM_PROC="1"
        fi
        echo "NUM_PROC: " $NUM_PROC
        for i in {1..10}; do
            mpirun -np $NUM_PROC $file || exit 1
        done
    done
end_mpi=`date +%s`
runtime=$((end_mpi-start_mpi))
if [ "$runtime" -gt "5" ]
then
    echo "Alert: runtime > 5 sec. runtime = $runtime sec."
    exit 1
fi
