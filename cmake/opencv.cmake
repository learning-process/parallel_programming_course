# Build OpenCV components
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/opencv/include)
include(ExternalProject)
ExternalProject_Add(ppc_opencv
        SOURCE_DIR        "${CMAKE_SOURCE_DIR}/3rdparty/opencv"
        PREFIX            "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/build"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/install"
        CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/3rdparty/opencv/" -B "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/build/"
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -G${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -D CMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
        -D CMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER} -DCMAKE_CXX_FLAGS="-w" -DCMAKE_C_FLAGS="-w" -DENABLE_CCACHE=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF
        -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DWITH_CUDA=OFF -DWITH_OPENCL=OFF -DBUILD_opencv_calib3d=OFF -DBUILD_opencv_dnn=OFF -DBUILD_opencv_ml=OFF -DBUILD_opencv_objdetect=OFF
        -DBUILD_opencv_java=OFF -DBUILD_opencv_gapi=OFF -DBUILD_opencv_video=OFF -DBUILD_opencv_world=OFF -DBUILD_opencv_features2d=OFF -DBUILD_opencv_flann=OFF -DBUILD_opencv_photo=OFF
        BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/build" --config ${CMAKE_BUILD_TYPE} --parallel
        INSTALL_COMMAND   "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/build" --prefix "${CMAKE_CURRENT_BINARY_DIR}/ppc_opencv/install")
