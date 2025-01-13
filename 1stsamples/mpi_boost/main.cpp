#include <mpi.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <iostream>
#include <string>

// https://www.boost.org/doc/libs/1_68_0/doc/html/mpi/tutorial.html
int main(int argc, char** argv) {
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world_;

  world_.barrier();
  std::cout << "Processor = " << boost::mpi::environment::processor_name() << '\n';
  std::cout << "Rank = " << world_.rank() << '\n';
  std::cout << "Number of processors = " << world_.size() << '\n';

  return 0;
}
