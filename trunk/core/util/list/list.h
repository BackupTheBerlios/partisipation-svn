#ifndef LIST_H
#define LIST_H

#include <util/config/globals.h>

void add_node(struct account *);

void del_node(int);

struct node *get_node(int);

int get_length();

void print_list();

extern struct node *head;

extern struct node *tail;

#endif
