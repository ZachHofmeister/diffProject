#include "text.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

text * text_create(void) {
    text * t = (text*)malloc(sizeof(text));
    memset(t->links, -1, sizeof(t->links));
    t->size = 0;
    return t;
}

text * text_intersection(text * t1, text * t2) {
    text * inter = text_create();
    
    int lastIndex = 0;
    for (int i = 0; i < t1->size; ++i) {
        if (text_lineEmpty(t1, i)) continue; //Don't try to match empty lines, at least if they are by themselves
        for (int j = lastIndex; j < t2->size; ++j) {
            if (strcmp(text_getLine(t1, i), text_getLine(t2, j)) == 0) {
                text_addLine(inter, text_getLine(t1, i));
                while (i+1 < t1->size && j+1 < t2->size && strcmp(text_getLine(t1, i+1), text_getLine(t2, j+1)) == 0) { //compare if lines following each matching line also match
                    text_addLine(inter, text_getLine(t1, ++i));
                    ++j;
                }
                lastIndex = j + 1;
                break;
            }
        }
    }

    return inter;
}

void text_toGaps(text * t1, text * t2, text * inter, text * t1_gaps, text * t2_gaps) {
    int i_t1 = 0, i_t2 = 0, i_inter = 0;
    char *t1_line, *t2_line, *inter_line;
    while(i_t1 < t1->size || i_t2 < t2->size) { //TODO clean this up
        if (i_t1 >= t1->size) { //finish up remaining t2 that doesn't match t1
            t2_line = text_getLine(t2, i_t2);
            text_addLine(t1_gaps, "~~~");
            text_addLineLink(t2_gaps, t2, i_t2);
            ++i_t2;
            continue;
        } else if (i_t2 >= t2->size) { //finish up remaining t1 that doesn't match t2
            t1_line = text_getLine(t1, i_t1);
            text_addLineLink(t1_gaps, t1, i_t1);
            text_addLine(t2_gaps, "~~~");
            ++i_t1;
            continue;
        }

        t1_line = text_getLine(t1, i_t1);
        t2_line = text_getLine(t2, i_t2);
        if (i_inter < inter->size) inter_line = text_getLine(inter, i_inter);
        else inter_line = strdup("~~~");

        if (strcmp(t1_line, t2_line) == 0) {
            text_addLineLink(t1_gaps, t1, i_t1);
            text_addLineLink(t2_gaps, t2, i_t2);
            ++i_t1;
            ++i_t2;
            ++i_inter;
        } else if (strcmp(t1_line, inter_line) == 0) {
            while (strcmp(t2_line, inter_line) != 0) {
                text_addLine(t1_gaps, "~~~");
                text_addLineLink(t2_gaps, t2, i_t2);
                ++i_t2;
                t2_line = text_getLine(t2, i_t2);
            }
            text_addLineLink(t1_gaps, t1, i_t1);
            text_addLineLink(t2_gaps, t2, i_t2);
            ++i_t1;
            ++i_t2;
            ++i_inter;
        } else if (strcmp(t2_line, inter_line) == 0) {
            while (strcmp(t1_line, inter_line) != 0) {
                text_addLineLink(t1_gaps, t1, i_t1);
                text_addLine(t2_gaps, "~~~");
                ++i_t1;
                t1_line = text_getLine(t1, i_t1);
            }
            text_addLineLink(t1_gaps, t1, i_t1);
            text_addLineLink(t2_gaps, t2, i_t2);
            ++i_t1;
            ++i_t2;
            ++i_inter;
        } else { //text lines are not equal and neither matches an intersection
            if (strlen(t1_line) == 0) text_addLine(t1_gaps, "~~~");
            else text_addLineLink(t1_gaps, t1, i_t1);
            if (strlen(t2_line) == 0) text_addLine(t2_gaps, "~~~");
            else text_addLineLink(t2_gaps, t2, i_t2);
            ++i_t1;
            ++i_t2;
        }
    }
}

void text_readFile(text * t, FILE * fin) {
    int c = 0;
    char line[BUFSIZ], *linep = line;
    memset(line, '\0', sizeof(line));

    while (c != EOF) {
        c = fgetc(fin);
        if (c != '\n' && c != EOF) *linep++ = c;
        else {
            t->lines[t->size++] = strdup(line);
            memset(line, '\0', sizeof(line));
            linep = line;
        }
    }
}

void text_addLine(text * t, const char * str) {
    t->lines[t->size++] = strdup(str);
}

void text_addLineLink(text * t, text * src, int line) {
    t->lines[t->size] = text_getLine(src, line);
    t->links[t->size++] = line;
}


char * text_getLine(text * t, int line) {
    if (line >= t->size) { //check that line is in bounds
        printf("Overflow error from text_getLine(): line %d does not exist.", line);
        return NULL;
    } 
    return strdup(t->lines[line]);
}

bool text_lineEmpty(text * t, int line) {
    if (line >= t->size) { //check that line is in bounds
        printf("Overflow error from text_lineEmpty(): line %d does not exist.", line);
        return true;
    } 
    char * l = text_getLine(t, line);
    while (*l != '\0') {
        if (!isspace(*l++)) return false;
    }
    return true;
}

int text_longest(text * t) {
    int longest = 0, current = 0;
    for (int i = 0; i < t->size; ++i)
        if ((current = strlen(text_getLine(t, i))) > longest)
            longest = current;
    return longest;
}

void text_print(text * t) {
    for (int i = 0; i < t->size; ++i) printf("%4d - %s\n", i+1, t->lines[i]);
}

void text_printLinked(text * t) {
    for (int i = 0; i < t->size; ++i) printf("%4d - %s\n", t->links[i] + 1, t->lines[i]);
}

void text_printSBS(text * l, text * r) {
    if (l->size != r->size) {
        fprintf(stderr, "Overflow error from text_printSBS(): l and r have different sizes.\n");
        return;
    }
    int longestL = text_longest(l), longestR = text_longest(r);
    printf("%-*s%-*s\n", longestL + 10, "Left file:", longestR, "Right file:");
    for (int i = 0; i < l->size; ++i) printf("%-*s%-*s\n", longestL + 10, l->lines[i], longestR, r->lines[i]);
}