// Copyright 2023 Nesterov Alexander
#include <mpi.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <iostream>
#include <string>

// https://www.boost.org/doc/libs/1_68_0/doc/html/mpi/tutorial.html
int main(int argc, char** argv) {
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  world.barrier();
  std::cout << "Processor = " << boost::mpi::environment::processor_name() << std::endl;
  std::cout << "Rank = " << world.rank() << std::endl;
  std::cout << "Number of processors = " << world.size() << std::endl;

  return 0;
}
