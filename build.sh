python scripts/lint.py

if [ ! -d "build" ]; then
    mkdir build
fi

cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cppcheck -j4 --project=compile_commands.json &> log_cppcheck
python ../scripts/static_analysis.py log_cppcheck
cmake --build .
cd ..
