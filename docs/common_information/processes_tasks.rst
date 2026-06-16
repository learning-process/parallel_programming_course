Processes parallelism tasks
===========================

First task
----------

+----------------+---------------------------------------------------------------------------------+
| Variant Number | Task                                                                            |
+================+=================================================================================+
| 1              | Sum of vector elements                                                          |
+----------------+---------------------------------------------------------------------------------+
| 2              | Calculating the average value of vector elements                                |
+----------------+---------------------------------------------------------------------------------+
| 3              | Maximum value of vector elements                                                |
+----------------+---------------------------------------------------------------------------------+
| 4              | Minimum value of vector elements                                                |
+----------------+---------------------------------------------------------------------------------+
| 5              | Finding the number of sign alternations between adjacent elements of the vector |
+----------------+---------------------------------------------------------------------------------+
| 6              | Finding the number of order violations between adjacent elements of the vector  |
+----------------+---------------------------------------------------------------------------------+
| 7              | Finding the most similar adjacent elements of the vector                        |
+----------------+---------------------------------------------------------------------------------+
| 8              | Finding the most different adjacent elements of the vector                      |
+----------------+---------------------------------------------------------------------------------+
| 9              | Scalar product of vectors                                                       |
+----------------+---------------------------------------------------------------------------------+
| 10             | Sum of matrix elements                                                          |
+----------------+---------------------------------------------------------------------------------+
| 11             | Sum of values by rows in the matrix                                             |
+----------------+---------------------------------------------------------------------------------+
| 12             | Sum of values by columns in the matrix                                          |
+----------------+---------------------------------------------------------------------------------+
| 13             | Maximum value of matrix elements                                                |
+----------------+---------------------------------------------------------------------------------+
| 14             | Minimum value of matrix elements                                                |
+----------------+---------------------------------------------------------------------------------+
| 15             | Finding maximum values by rows in the matrix                                    |
+----------------+---------------------------------------------------------------------------------+
| 16             | Finding maximum values by columns in the matrix                                 |
+----------------+---------------------------------------------------------------------------------+
| 17             | Finding minimum values by rows in the matrix                                    |
+----------------+---------------------------------------------------------------------------------+
| 18             | Finding minimum values by columns in the matrix                                 |
+----------------+---------------------------------------------------------------------------------+
| 19             | Integration – rectangle method                                                  |
+----------------+---------------------------------------------------------------------------------+
| 20             | Integration – trapezoidal method                                                |
+----------------+---------------------------------------------------------------------------------+
| 21             | Integration – Monte Carlo method                                                |
+----------------+---------------------------------------------------------------------------------+
| 22             | Counting the number of alphabetical characters in a string                      |
+----------------+---------------------------------------------------------------------------------+
| 23             | Counting the frequency of a character in a string                               |
+----------------+---------------------------------------------------------------------------------+
| 24             | Counting the number of words in a string                                        |
+----------------+---------------------------------------------------------------------------------+
| 25             | Counting the number of sentences in a string                                    |
+----------------+---------------------------------------------------------------------------------+
| 26             | Checking lexicographical order of two strings                                   |
+----------------+---------------------------------------------------------------------------------+
| 27             | Counting the number of differing characters between two strings                 |
+----------------+---------------------------------------------------------------------------------+

Second task
-----------

+----------------+-------------------------------------------------------------------------------------+
| Variant Number | Task                                                                                |
+================+=====================================================================================+
| 1              | Broadcast (one to all transfer)                                                     |
+----------------+-------------------------------------------------------------------------------------+
| 2              | Reduce (all to one transfer)                                                        |
+----------------+-------------------------------------------------------------------------------------+
| 3              | Allreduce (all to one and broadcast)                                                |
+----------------+-------------------------------------------------------------------------------------+
| 4              | Scatter (one to all transfer)                                                       |
+----------------+-------------------------------------------------------------------------------------+
| 5              | Gather (all to one transfer)                                                        |
+----------------+-------------------------------------------------------------------------------------+
| 6              | Line                                                                                |
+----------------+-------------------------------------------------------------------------------------+
| 7              | Ring                                                                                |
+----------------+-------------------------------------------------------------------------------------+
| 8              | Star                                                                                |
+----------------+-------------------------------------------------------------------------------------+
| 9              | Torus Grid                                                                          |
+----------------+-------------------------------------------------------------------------------------+
| 10             | Hypercube                                                                           |
+----------------+-------------------------------------------------------------------------------------+
| 11             | Horizontal strip scheme - matrix-vector multiplication                              |
+----------------+-------------------------------------------------------------------------------------+
| 12             | Vertical strip scheme - matrix-vector multiplication                                |
+----------------+-------------------------------------------------------------------------------------+
| 13             | Horizontal strip scheme – partitioning only matrix A - matrix-matrix multiplication |
+----------------+-------------------------------------------------------------------------------------+
| 14             | Horizontal strip scheme A, vertical strip scheme B - matrix-matrix multiplication   |
+----------------+-------------------------------------------------------------------------------------+
| 15             | Gaussian method – horizontal strip scheme                                           |
+----------------+-------------------------------------------------------------------------------------+
| 16             | Gaussian method – vertical strip scheme                                             |
+----------------+-------------------------------------------------------------------------------------+
| 17             | Gauss-Jordan method                                                                 |
+----------------+-------------------------------------------------------------------------------------+
| 18             | Iterative methods (Jacobi)                                                          |
+----------------+-------------------------------------------------------------------------------------+
| 19             | Iterative methods (Gauss-Seidel)                                                    |
+----------------+-------------------------------------------------------------------------------------+
| 20             | Iterative methods (Simple)                                                          |
+----------------+-------------------------------------------------------------------------------------+
| 21             | Bubble sort (odd-even transposition algorithm)                                      |
+----------------+-------------------------------------------------------------------------------------+
| 22             | Image smoothing                                                                     |
+----------------+-------------------------------------------------------------------------------------+
| 23             | Contrast enhancement                                                                |
+----------------+-------------------------------------------------------------------------------------+

Third task
----------

+----------------+----------------------------------------------------------------------------------------------------------+
| Variant Number | Task                                                                                                     |
+================+==========================================================================================================+
| 1              | Dense matrix multiplication. Elements of data type double. Block scheme, Cannon's algorithm.             |
+----------------+----------------------------------------------------------------------------------------------------------+
| 2              | Dense matrix multiplication. Elements of data type double. Block scheme, Fox's algorithm.                |
+----------------+----------------------------------------------------------------------------------------------------------+
| 3              | Dense matrix multiplication. Elements of data type double. Strassen's algorithm.                         |
+----------------+----------------------------------------------------------------------------------------------------------+
| 4              | Sparse matrix multiplication. Elements of data type double. Matrix storage format – row format (CRS).    |
+----------------+----------------------------------------------------------------------------------------------------------+
| 5              | Sparse matrix multiplication. Elements of data type double. Matrix storage format – column format (CCS). |
+----------------+----------------------------------------------------------------------------------------------------------+
| 6              | Solving systems of linear equations using the conjugate gradient method.                                 |
+----------------+----------------------------------------------------------------------------------------------------------+
| 7              | Computing multidimensional integrals using a multistep scheme (rectangle method).                        |
+----------------+----------------------------------------------------------------------------------------------------------+
| 8              | Computing multidimensional integrals using a multistep scheme (trapezoidal method).                      |
+----------------+----------------------------------------------------------------------------------------------------------+
| 9              | Computing multidimensional integrals using a multistep scheme (Simpson's method).                        |
+----------------+----------------------------------------------------------------------------------------------------------+
| 10             | Computing multidimensional integrals using the Monte Carlo method.                                       |
+----------------+----------------------------------------------------------------------------------------------------------+
| 11             | Global search algorithm (Strongin's) for one-dimensional optimization problems. Parallelization by       |
|                | characteristics.                                                                                         |
+----------------+----------------------------------------------------------------------------------------------------------+
| 12             | Multistep scheme for solving two-dimensional global optimization problems. Parallelization by dividing   |
|                | the search area.                                                                                         |
+----------------+----------------------------------------------------------------------------------------------------------+
| 13             | Multistep scheme for solving two-dimensional global optimization problems. Parallelization by            |
|                | characteristics.                                                                                         |
+----------------+----------------------------------------------------------------------------------------------------------+
| 14             | Quick sort with simple merging.                                                                          |
+----------------+----------------------------------------------------------------------------------------------------------+
| 15             | Quick sort with odd-even merging (Batcher's method).                                                     |
+----------------+----------------------------------------------------------------------------------------------------------+
| 16             | Shell sort with simple merging.                                                                          |
+----------------+----------------------------------------------------------------------------------------------------------+
| 17             | Shell sort with odd-even merging (Batcher's method).                                                     |
+----------------+----------------------------------------------------------------------------------------------------------+
| 18             | Radix sort for integers with simple merging.                                                             |
+----------------+----------------------------------------------------------------------------------------------------------+
| 19             | Radix sort for integers with odd-even merging (Batcher's method).                                        |
+----------------+----------------------------------------------------------------------------------------------------------+
| 20             | Radix sort for floating-point numbers (type double) with simple merging.                                 |
+----------------+----------------------------------------------------------------------------------------------------------+
| 21             | Radix sort for floating-point numbers (type double) with odd-even merging (Batcher's method).            |
+----------------+----------------------------------------------------------------------------------------------------------+
| 22             | Shortest path search from one vertex (Dijkstra's algorithm). With CRS graphs.                            |
+----------------+----------------------------------------------------------------------------------------------------------+
| 23             | Shortest path search from one vertex (Bellman-Ford algorithm). With CRS graphs.                          |
+----------------+----------------------------------------------------------------------------------------------------------+
| 24             | Convex hull construction – Graham's scan.                                                                |
+----------------+----------------------------------------------------------------------------------------------------------+
| 25             | Convex hull construction – Jarvis's march.                                                               |
+----------------+----------------------------------------------------------------------------------------------------------+
| 26             | Linear image filtering (horizontal partition). Gaussian kernel 3x3.                                      |
+----------------+----------------------------------------------------------------------------------------------------------+
| 27             | Linear image filtering (vertical partition). Gaussian kernel 3x3.                                        |
+----------------+----------------------------------------------------------------------------------------------------------+
| 28             | Linear image filtering (block partition). Gaussian kernel 3x3.                                           |
+----------------+----------------------------------------------------------------------------------------------------------+
| 29             | Edge detection in an image using the Sobel operator.                                                     |
+----------------+----------------------------------------------------------------------------------------------------------+
| 30             | Contrast enhancement of grayscale image using linear histogram stretching.                               |
+----------------+----------------------------------------------------------------------------------------------------------+
| 31             | Labeling components on a binary image (black areas correspond to objects, white to background).          |
+----------------+----------------------------------------------------------------------------------------------------------+
| 32             | Convex hull construction for components of a binary image.                                               |
+----------------+----------------------------------------------------------------------------------------------------------+

Comments for tasks 2 and 3:
~~~~~~~~~~~~~~~~~~~~~~~~~~~

- MESSAGE PASSING METHODS “You need to implement the specified methods
  using only the Send and Recv functions. The implemented function
  should have the same prototype as the corresponding MPI function. The
  test program should allow selecting the root process number and
  perform array transmission (broadcast, gather) for at least the
  following types: MPI_INT, MPI_FLOAT, MPI_DOUBLE. In all operations,
  the transmission should be carried out using the ‘tree’ of processes.”

  Comments relevant for:

  =================== ===================
  Variants for task 2 Variants for task 3
  =================== ===================
  1 - 5               x
  =================== ===================

- DATA COMMUNICATION NETWORK TOPOLOGIES “You need to implement the
  virtual topology specified in the task using MPI capabilities for
  working with communicators and topologies, and ensure the ability to
  transfer data from any selected process to any other process. (Do not
  use MPI_Cart_Create and MPI_Graph_Create)”

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  6 - 10              x
  =================== ===================

- MATRIX COMPUTATIONS “In the horizontal scheme, the matrix is divided
  among processes by rows. In the vertical scheme, it is divided by
  columns, and in this case, the vector is also divided among
  processes.”

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  11 - 14             1 - 5
  =================== ===================

- COMPUTER GRAPHICS AND IMAGE PROCESSING “It is assumed that the image
  is given in color or grayscale, with the input data being a
  one-dimensional array. Loading a real image is not required, but is
  allowed.”

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  26 - 27             24 - 32
  =================== ===================

- SOLUTION OF A SYSTEM OF LINEAR ALGEBRAIC EQUATIONS

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  15 - 20             6
  =================== ===================

- SORT ALGORITHMS

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  21                  14 - 21
  =================== ===================

- GRAPH PROCESSING ALGORITHMS

  Comments relevant for:

  =================== ===================
  Varinats for task 2 Varinats for task 3
  =================== ===================
  x                   22 - 23
  =================== ===================
