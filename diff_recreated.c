#include <stdio.h> //for printf() etc...
#include <stdlib.h> //for exit() etc...
#include <string.h> //for strcmp() etc...
#include <stdbool.h> //for type bool

const char * compiledName; //If not used again, don't need this variable to track it


void processArgs(int argc, const char * argv[]);

int main(int argc, const char * argv[]) {
    compiledName = *argv;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s (options) file1 file2\n", compiledName);
        exit(1);
    }
    processArgs(--argc, ++argv); 

    return 0;
}

void processArgs(int argc, const char * argv[]) {
    //First process all optional arguments
    while (argc-- > 2) {
        // printf("%s\n", *argv);
        if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {

        } else if (strcmp(*argv, "-q") == 0 || strcmp(*argv, "--brief") == 0) {

        } else if (strcmp(*argv, "-s") == 0 || strcmp(*argv, "--report-identical-files") == 0) {

        } else if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {

        } else if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {

        } else if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {

        } else if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {

        }
        ++argv;
    }
    //Lastly process file name arguments
    printf("%s\n", *argv++);
    printf("%s\n", *argv);
}