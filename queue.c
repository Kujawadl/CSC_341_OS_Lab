#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	int userNum;
	struct Node* next;
	registers userRegister;
} Node;

typedef struct Queue {
	Node* head;
	Node* tail;

	void (*push) (struct Queue*, Node*);
	Node* (*pop) (struct Queue*);
	Node (*peek) (struct Queue*);
	void (*display) (struct Queue*);
	int size;
} Queue;

void push (Queue* queue, Node* n);
Node* pop (Queue* queue);
Node peek (Queue* queue);
void display (Queue* queue);

Queue createQueue ();


void push (Queue* queue, Node* n) {

	n->next = NULL;

	/* If queue is empty, set head to n */
	if (queue->head == NULL) {
		queue->head = n;
	} else {
		queue->tail->next = n;
	}
	queue->tail = n;
	queue->size++;
}

Node* pop (Queue* queue) {

	Node* head = queue->head;
	Node* current = head;

	queue->head = head->next;
	queue->size--;

	return current;
}


Node peek (Queue* queue) {
	Node* current = queue->head;
	return *current;
}


void display (Queue* queue) {
	printf("\nDisplay: ");
	// no item
	if (queue->size == 0)
		printf("No item in queue.\n");
	else {
		// has item(s)
		Node* current = queue->head;
		int i;
		int size = queue->size;
		printf("%d item(s):\n", queue->size);
		for (i = 0; i < size; i++) {
			if (i > 0)
				printf("\n");

			printf("UserNum: %d, Next: %p", current->userNum, current->next);
			current = current->next;
		}
	}
	printf("\n\n");
}

Queue createQueue () {
	Queue queue;
	queue.size = 0;
	queue.head = NULL;
	queue.tail = NULL;
	queue.push = &push;
	queue.pop = &pop;
	queue.peek = &peek;
	queue.display = &display;
	return queue;
}
