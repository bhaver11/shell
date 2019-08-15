# Sample Makefile (don't change the filename)
# Modify the below contents as needed

# CFLAGS?= <add flags required for GCC here>

# Considering 2 files in this sample
# file2.c has main(), and will create the executable that you want to run
# file1.c doesn't have main(), but file2.c uses file1.c, and so, file1.c
# needs to be compiled (hence -c flag added separately)

#Modify as per your need

all: rule1 #

# file2.c also includes header2.h
rule1: shell.c myshell.h # basically this tells that if any of these files are changed, next cmd should run
	$(CC) $(CFLAGS) shell.c -o shell

run: rule1
	./shell input

clean:
	$(RM) rule1 rule2
	$(RM) -r *.dSYM
	$(RM) file2_executable


# To compile, type this: make
# Then, execute by typing ./file2_executable <in_file>
# To compile and run: make run (this will also execute your program)
# Type: make clean to remove executable & other junk files
