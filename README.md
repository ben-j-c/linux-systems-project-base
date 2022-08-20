# Purpose
The purpose of this project is to provide the base set of useful tools when
create a new C project on Linux.

# Features
- No makefile modifications necessary for new .c/.h files and folders
  - Dependencies checked for incremental compilation
  - Project wide include directory available in `src/global`
- Error handling boiler-plate
  - `FWD` for pushing additional errors to the existing error chain
  - `NEW` for creating a new error chain
- Argument parsing boiler-plate
  - Powered by GNU Argp
  - Example given with easy to start structure
- Multiple error handling compatible datastructures
  - Hashtable
    - Simple access for string and int types
    - User controllable data ownership
  - Vector
- Clang format present
- Tested on GCC 9.4.0
- ANSI flag compatible
- Multi-threaded epoll

# Future Features
- Shared memory tools
- Memory backed files tools
- IPC tools
- More compatible datastructures
  - Linked lists
  - Trees

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
5. Run `make all` or `make debug` for release and debug images respectively