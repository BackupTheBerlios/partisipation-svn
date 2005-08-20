#ifndef LIST_H
#define LIST_H

typedef struct account {
	int id;
	char *name;
	char *username;
	char *domain;
	char *authusername;
	char *password;
	char *displayname;
	char *outboundproxy;
	char *registrar;
	int autoregister;
} typeaccount;

typedef struct node {
	struct account *acc;
	struct node *next;
	struct node *prev;
} typenode;

void add_node(struct account *);

void del_node(int);

struct node *get_node(int);

extern struct node *head;

extern struct node *tail;

#endif
