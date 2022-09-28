# Purpose
The purpose of this project is to provide the base set of useful tools when
create a new C project on Linux.

# Features
- No makefile modifications necessary for new .c/.h files and folders
  - Dependencies checked for incremental compilation
  - Project wide include directory available in `src/global`
- Automated testing framework through makefile
  - Just add a new `.c` file in `tests`
  - Run `make tests` to run tests
- Error handling boiler-plate
  - `FWD` for pushing additional errors to the existing error chain
  - `NEW` for creating a new error chain
- Argument parsing boiler-plate
  - Powered by GNU Argp
  - Example given with easy to start structure
- Multiple datastructures (all compatible with error handling boiler-plate)
  - Hashtable
    - Existing macros for string and int typed keys
      - Predefined hashing functions
      - Easy to use
    - User controllable data ownership (hash table can allocate data, move data, free data, or do nothing depending on settings used)
  - Vector
    - Contiguous data segment
    - Support for `push_back`, `emplace_back`, and `take_data` (for freeing all but the underlying data array)
  - Doubly linked list
    - Infallible add/remove/init
    - Ergonomic iterator
  - AVL tree
    - Infallible add/remove/find
    - Ergonomic iterator
- Clang format present
- Tested on GCC 9.4.0
- ANSI flag compatible
- Multi-threaded epoll

# Future Features
- Shared memory tools
- Memory backed files tools
- IPC tools
- More tests

# How To Use

1. Clone the repo to your **`new-project`** directory
```
git clone git@github.com:ben-j-c/linux-systems-project-base ./new-project
```
2. Rename the project in the Makefile
```
EXE_NAME = PROJECT_NAME
```
3. Set the origin url to your repo
```
git remote set-url origin git@github.com:your-username/your-repo
git push
```
4. Install `libbsd-dev` for `bsd/string.h`
```
sudo apt install libbsd-dev
```
5. Run `make executable`, `make tests`, or `make all` to build the project executalbe, run the tests, or do both. To build with optimizations and without debug symbols use `make ... RELEASE=1`
