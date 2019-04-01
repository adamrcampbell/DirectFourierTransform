# Direct Fourier Transform - CPU Implementation
###### Note: currently single threaded; multithreaded CPU implementation considered for future work.
---
##### Instructions for installation of this software (includes profiling and unit testing):
1. Install [Valgrind](http://valgrind.org/) (profiling, memory checks, memory leaks etc.)
   ```bash 
   $ sudo apt-get install valgrind 
   ```
2. Install [Cmake](https://cmake.org/)/[Makefile](https://www.gnu.org/software/make/) (build tools)
   ```bash
   $ sudo apt-get install cmake
   ```
3. Install [Google Test](https://github.com/google/googletest) (unit testing) - See [this tutorial](https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/) for tutorial on using Google Test library
   ```bash
   $ sudo apt-get install libgtest-dev
   $ cd /usr/src/gtest
   $ sudo cmake CMakeLists.txt
   $ sudo make
   $ sudo cp *.a /usr/lib
   ```
4. Configure the code for usage (**modify direct_fourier_transform.c config**)
5. Build direct fourier transform project (from project folder)
   ```bash
   $ mkdir build && cd build
   $ cmake .. && make
   ```

---
##### Instructions for usage of this software (includes executing, testing, and profiling):
To perform memory checking, memory leak analysis, and profiling using [Valgrind](http://valgrind.org/docs/manual/quick-start.html), execute the following (assumes you are in the appropriate *build* folder (see step 5 above):
```bash
$ valgrind --leak-check=yes dft
```
To execute unit testing, execute the following (also assumes appropriate *build* folder):
```bash
./tests
````
To execute the direct fourier transform (once configured and built), execute the following command (also assumes appropriate *build* folder):
```bash
./dft
````

