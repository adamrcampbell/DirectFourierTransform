# Direct Fourier Transform - CPU Implementation
###### Note: currently single threaded; multithreaded CPU implementation considered for future work.
Instructions for installation of this software (includes profiling and unit testing):

1. Install Valgrind (profiling, memory checks, memory leaks etc.)
   * sudo apt-get install valgrind
2. Install Cmake/Make (build tools)
   * sudo apt-get install cmake
3. Install Google Test (unit testing)
   * sudo apt-get install libgtest-dev
   * cd /usr/src/gtest
   * sudo cmake CMakeLists.txt
   * sudo make
   * sudo cp *.a /usr/lib
   * See [this tutorial](https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/) for usage demo

