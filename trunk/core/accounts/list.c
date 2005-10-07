#include <stdio.h>
#include <stdlib.h>
#include <accounts/list.h>

// first element of the list
struct node *head = NULL;

// last element of the list
struct node *tail = NULL;

void add_node(struct account *a) {
	struct node *new_node = (struct node *) malloc(sizeof(struct node));

	new_node->acc = a;
	new_node->next = NULL;
	new_node->prev = tail;

	if (head == NULL)
		head = new_node;
	if (tail != NULL)
		tail->next = new_node;
	tail = new_node;
}

struct node *get_node(int n) {

	struct node *tmp = head;

	while (tmp) {
		struct account *a = tmp->acc;

		if (a->id == n) {
			return tmp;
		} else {
			tmp = tmp->next;
		}
	}
	return NULL;
}

void free_memory(struct account *a) {
	free(a->name);
	free(a->username);
	free(a->domain);
	free(a->authusername);
	free(a->password);
	free(a->displayname);
	free(a->outboundproxy);
	free(a->registrar);
	free(a);
}

void del_node(int n) {

	struct node *dn = get_node(n);
	int num = get_length();

	if (dn != NULL) {
		if (num == 1) {
			// last remaining element is deleted, head and tail will be set to NULL, list is empty
			head = NULL;
			tail = NULL;
		} else {
			if (dn == head) {
				head = dn->next;
				head->prev = NULL;
			} else if (dn == tail) {
				tail = dn->prev;
				tail->next = NULL;
			} else {
				dn->prev->next = dn->next;
				dn->next->prev = dn->prev;
			}
		}

		free_memory(dn->acc);
		free(dn);
	}
}

int get_length() {
	int len = 0;

	struct node *tmp = head;

	while (tmp != NULL) {
		len++;
		tmp = tmp->next;
	}

	return len;
}

void print_list() {
	int i = 0;
	struct node *tmp = head;

	while (tmp) {
		i++;
		printf("#%d: [id = %d] -> ", i, tmp->acc->id);
		tmp = tmp->next;
	}
	printf("X\n");
}
