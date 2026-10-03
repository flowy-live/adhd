#ifndef COMMON_H
#define COMMON_H

typedef unsigned int U32;

struct date {
  U32 day;
  U32 month;
  U32 year;
};

struct date to_date(char *);
char *to_string(struct date);

#endif
