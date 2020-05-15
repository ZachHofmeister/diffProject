#ifndef TEXT_H
#define TEXT_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_LINES 10000

typedef struct text text;
struct text {
    char * lines[MAX_LINES];
    int links[MAX_LINES];
    size_t size;
};

text * text_create(void);
text * text_intersection(text * t1, text * t2);
void text_toGaps(text * t1, text * t2, text * inter, text * t1_gaps, text * t2_gaps);

void text_readFile(text * t, FILE * fin);

void text_addLine(text * t, const char * str);
void text_addLineLink(text * t, text * src, int line);

char * text_getLine(text * t, int line);
bool text_lineEmpty(text * t, int line);
int text_longest(text * t);

void text_print(text * t);
void text_printLinked(text * t);
void text_printSBS(text * left, text * right);

#endif //TEXT_H