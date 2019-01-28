python scripts/lint.py

if [ ! -d "build" ]; then
    mkdir build
fi

cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ..
