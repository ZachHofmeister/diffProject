#include <stdio.h> //for printf() etc...
#include <stdlib.h> //for exit(), atoi() etc...
#include <string.h> //for strcmp() etc...
#include <stdbool.h> //for type bool
#include <ctype.h>

#include "diff.h"
#include "text.h"

bool argVersion, argHelp, argBrief, argRIF, argSBS, argLeftCol, argSCL, argContext, argUnified;
int contextNum, unifiedNum;
FILE *file1, *file2;

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printHelp();
        exit(1);
    }
    processArgs(--argc, ++argv);
    if (!argHelp && !argVersion) diff();
    fclose(file1);
    fclose(file2);
    return 0;
}

void processArgs(int argc, const char * argv[]) {
    //First process all optional arguments
    while (argc-- > 0 && **argv == '-') {
        if (strcmp(*argv, "-v") == 0 || strcmp(*argv, "--version") == 0) {
            argVersion = true;
            printVersion();
            return;
        } else if (strcmp(*argv, "--help") == 0) {
            argHelp = true;
            printHelp();
            return;
        } else if (strcmp(*argv, "-q") == 0 || strcmp(*argv, "--brief") == 0) {
            argBrief = true;
        } else if (strcmp(*argv, "-s") == 0 || strcmp(*argv, "--report-identical-files") == 0) {
            argRIF = true;
        } else if (strcmp(*argv, "-y") == 0 || strcmp(*argv, "--side-by-side") == 0) {
            argSBS = true;
        } else if (strcmp(*argv, "--left-column") == 0) {
            argLeftCol = true;
        } else if (strcmp(*argv, "--suppress-common-lines") == 0) {
            argSCL = true;
        } else if (strcmp(*argv, "-c") == 0 || strncmp(*argv, "--context", 9) == 0) {
            argContext = true;
            if (*(*argv+1) == 'c') { //using "-c NUM" syntax
                if (!isdigit(**(argv+1))) contextNum = 3; //Default
                else contextNum = atoi(*(++argv));
            } else { //using "--context=NUM" syntax
                if (strlen(*argv) == 9) contextNum = 3; //Default
                else {
                    while (*(*argv)++ != '=') {} //advances *argv to begin at the character after the '='
                    contextNum = atoi(*argv);
                }
            }
        } else if (strcmp(*argv, "-u") == 0 || strncmp(*argv, "--unified", 9) == 0) {
            argUnified = true;
            if (*(*argv+1) == 'u') { //using "-u NUM" syntax
                if (!isdigit(**(argv+1))) unifiedNum = 3; //Default
                else unifiedNum = atoi(*(++argv));
            } else { //using "--unified=NUM" syntax
                if (strlen(*argv) == 9) unifiedNum = 3; //Default
                else {
                    while (*(*argv)++ != '=') {} //advances *argv to begin at the character after the '='
                    unifiedNum = atoi(*argv);
                }
            }
        }
        ++argv;
    }
    //Lastly process file name arguments
    if (!argVersion && (file1 = fopen(*argv, "r")) == NULL) { //Open first file, check that it opens successfully
        fprintf(stderr, "Error opening file 1 \"%s\".\n", *argv);
        exit(1);
    }
    if (!argVersion && (file2 = fopen(*++argv, "r")) == NULL) { //Open second file, check that it opens successfully
        fprintf(stderr, "Error opening file 2 \"%s\".\n", *argv);
        exit(1);
    }
}

void diff() {
    //Read the files into text structures
    text * t1 = text_create();
    text * t2 = text_create();
    text_readFile(t1, file1);
    text_readFile(t2, file2);

    //Find the intersection of the two texts
    text * inter = text_intersection(t1, t2);

    //Enough information for brief / report identical files
    bool identical = t1->size == t2->size && t1->size == inter->size;
    if (argBrief && argRIF) {
        if (identical) printf("The files are identical.\n");
        else printf("The files are different.\n");
        return;
    } else if (argBrief && !identical) {
        printf("The files are different.\n");
        return;
    } else if (argRIF && identical) {
        printf("The files are identical.\n");
        return;
    }

    //Create new versions of the texts, which will have gaps where lines aren't present
    text * t1_gaps = text_create();
    text * t2_gaps = text_create();
    text_toGaps(t1, t2, inter, t1_gaps, t2_gaps);

    //Print the differences
    char *t1_line, *t2_line;
    int longestL = text_longest(t1), longestR = text_longest(t2);
    for (int i = 0; i < t1_gaps->size; ++i) {
        t1_line = text_getLine(t1_gaps, i);
        t2_line = text_getLine(t2_gaps, i);

        if (strcmp(t1_line, t2_line) == 0) { //SAME
            // printf("EQUAL\n");
            if (argSBS && !argSCL) {
                printf("%-*s%c %-*s\n", longestL + 10, text_getLine(t1_gaps, i), argLeftCol? '(' : ' ', longestR, argLeftCol? "" : text_getLine(t2_gaps, i));
            }
        } else if (strcmp(t1_line, "~~~") == 0) { //ADDED
            if (argSBS) { //SBS mode
                int longestL = text_longest(t1), longestR = text_longest(t2);
                printf("%-*s%c %-*s\n", longestL + 10, "", '>', longestR, text_getLine(t2_gaps, i));
            } else { //Normal mode
                int t1_first = t1_gaps->links[i == 0? 0 : i-1];
                int t2_first = t2_gaps->links[i], t2_last = t2_gaps->links[i];
                while(i+1 < t1_gaps->size && strcmp((t1_line = text_getLine(t1_gaps, i+1)), "~~~") == 0) {
                    t2_last = t2_gaps->links[++i];
                }
                //Print diff function
                if (t2_first == t2_last) {
                    printf("%da%d\n", t1_first+1, t2_first+1);
                } else {
                    printf("%da%d,%d\n", t1_first+1, t2_first+1, t2_last+1);
                }
                //Print lines
                for (int j = t2_first; j <= t2_last; ++j) {
                    printf("> %s\n", text_getLine(t2, j));
                }
            }
        } else if (strcmp(t2_line, "~~~") == 0) { //DELETED
            if (argSBS) { //SBS mode
                printf("%-*s%c %-*s\n", longestL + 10, text_getLine(t1_gaps, i), '<', longestR, "");
            } else { //Normal mode
                int t1_first = t1_gaps->links[i], t1_last = t1_gaps->links[i];
                int t2_first = t2_gaps->links[i == 0? 0 : i-1];
                while(i+1 < t1_gaps->size && strcmp((t2_line = text_getLine(t2_gaps, i+1)), "~~~") == 0) {
                    t1_last = t1_gaps->links[++i];
                }
                //Print diff function
                if (t1_first == t1_last) {
                    printf("%dd%d\n", t1_first+1, t2_first+1);
                } else {
                    printf("%d,%dd%d\n", t1_first+1, t1_last+1, t2_first+1);
                }
                //Print lines
                for (int j = t1_first; j <= t1_last; ++j) {
                    printf("< %s\n", text_getLine(t1, j));
                }
            }
        } else { //CHANGED
            if (argSBS) { //SBS mode
                printf("%-*s%c %-*s\n", longestL + 10, text_getLine(t1_gaps, i), '|', longestR, text_getLine(t2_gaps, i));
            } else { //Normal mode
                int t1_first = t1_gaps->links[i], t1_last = t1_gaps->links[i];
                int t2_first = t2_gaps->links[i], t2_last = t2_gaps->links[i];
                while(strcmp((t1_line = text_getLine(t1_gaps, i+1)), (t2_line = text_getLine(t2_gaps, i+1))) != 0) {
                    ++i;
                    if (strcmp(t1_line, "~~~") != 0) t1_last = t1_gaps->links[i];
                    if (strcmp(t2_line, "~~~") != 0) t2_last = t2_gaps->links[i];
                }
                //Print diff function
                if (t1_first == t1_last) {
                    printf("%dc", t1_first+1);
                } else {
                    printf("%d,%dc", t1_first+1, t1_last+1);
                }
                if (t2_first == t2_last) {
                    printf("%d\n", t2_first+1);
                } else {
                    printf("%d,%d\n", t2_first+1, t2_last+1);
                }
                //Print lines
                for (int j = t1_first; j <= t1_last; ++j) {
                    printf("< %s\n", text_getLine(t1, j));
                }
                printf("---\n");
                for (int j = t2_first; j <= t2_last; ++j) {
                    printf("> %s\n", text_getLine(t2, j));
                }
            }
        }
    }

}

void printVersion() {
    printf("diff_recreated 1.0.0\n");
    printf("Copyright (C) 2020 Zach Hofmeister\n");
    printf("\n");
    printf("Written by Zach Hofmeister.\n");
}

void printHelp() {
    printf("\t%s\n", "Usage: ./diff [OPTION]... FILE1 FILE2");
    printf("\t%s\n", "Compare files line by line.");
    printf("\t%s\n", "");
    printf("\t%s\n", "--help  Output this help.");
    printf("\t%s\n", "-v  --version  Output version info.");
    printf("\t%s\n", "--normal  Output a normal diff.");
    printf("\t%s\n", "-q  --brief  Output only whether files differ.");
    printf("\t%s\n", "-s  --report-identical-files  Report when two files are the same.");
    printf("\t%s\n", "-y  --side-by-side  Output in two columns.");
    printf("\t%s\n", "  --left-column  Output only the left column of common lines.");
    printf("\t%s\n", "  --suppress-common-lines  Do not output common lines.");
    printf("\t%s\n", "");
    printf("\t%s\n", "Not fully implimented yet:");
    printf("\t%s\n", "-c [NUM]  --context[=NUM]  Output NUM (default 3) lines of copied context.");
    printf("\t%s\n", "-u [NUM]  --unified[=NUM]  Output NUM (default 3) lines of unified context.");
}