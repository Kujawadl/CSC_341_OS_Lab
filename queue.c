#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	int userNum;
	struct Node* next;
	registers userRegisters;
} Node;

typedef struct Queue {
	Node* head;
	Node* tail;

	void (*enqueue) (struct Queue*, Node*);
	Node (*dequeue) (struct Queue*);
	Node* (*peek) (struct Queue*);
	void (*display) (struct Queue*);
	int size;
} Queue;

void enqueue (Queue* queue, Node* n);
Node dequeue (Queue* queue);
Node* peek (Queue* queue);
void display (Queue* queue);

Queue createQueue ();

void enqueue (Queue* queue, Node* n) {
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

Node dequeue (Queue* queue) {
	Node* head = queue->head;
	Node* current = head;
	queue->head = head->next;
	queue->size--;
	return *current;
}


Node* peek (Queue* queue) {
	Node* current = queue->head;
	return current;
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
			printf("\n\trA: %d, r1: %d, r2: %d, r3: %d", current->userRegisters.rA, current->userRegisters.r1, current->userRegisters.r2, current->userRegisters.r3);
			printf("\n\tIR: %d, PC: %d, CR: %d\n", current->userRegisters.IR, current->userRegisters.PC, current->userRegisters.CR);
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
	queue.enqueue = &enqueue;
	queue.dequeue = &dequeue;
	queue.peek = &peek;
	queue.display = &display;
	return queue;
}
