#ifndef REQUEST_H
#define REQUEST_H

#include "chibiWebDefs.h"
#include "tsQueue.h"

#define REQUEST_NULL 0
#define REQUEST_GET  1
#define REQUEST_POST 2

typedef struct param {
  char *key;
  char *value;
  struct param *next;
} Param;

typedef struct request {
    char *buf;
    int type;
    char *version;
    char *path;
    char *root;
    char *file;
    char *paramStr;
    Param *param;
    TSQueue *headers;
    char *body;
} Request;
/* parse a char request and return a new filled request */
Request *request_parse(char *request);
Request *request_new();
void request_free(Request *r);
void request_addParam(Request *r, Param *p);

#endif /* REQUEST_H */
