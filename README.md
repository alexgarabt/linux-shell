# Basic linux shell.  
*This project is a simple linux shell implementation with parallel commands and redirections.*  
**Advise**: The code is a mess. This project was a quick proof of how linux shells works.


# Modes of the linux shell
The linux shell has 2 different modes: **Interactive** and **Batch** mode. 
*Like the normal bash shell of linux.*


# How to use it
```bash
# Step 1: Clone the repository to your local machine.
git clone https://github.com/alexgarabt/linux-shell.git  
cd linux-shell

# Step 2: Compile it.
make

# To clean the compiled and middle files.
make clean

# Step 3: Use Interactive or Batch mode.

# Interative mode.
./UVash

# Batch mode. (filename is the file with the commands to run).
./UVash filename
```
# Test it. 
There are some prepared test cases, to run it you must have the programm compiled.
```bash
./test-UVash.sh -c
```

