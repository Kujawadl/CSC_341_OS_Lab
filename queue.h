#ifndef _SYS_QUEUE_H_
#define	_SYS_QUEUE_H_

/*
 * A singly-linked tail queue is headed by a pair of pointers, one to the
 * head of the list and the other to the tail of the list. The elements are
 * singly linked for minimum space and pointer manipulation overhead at the
 * expense of O(n) removal for arbitrary elements. New elements can be added
 * to the list after an existing element, at the head of the list, or at the
 * end of the list. Elements being removed from the head of the tail queue
 * should use the explicit macro for this purpose for optimum efficiency.
 * A singly-linked tail queue may only be traversed in the forward direction.
 * Singly-linked tail queues are ideal for applications with large datasets
 * and few or no removals or for implementing a FIFO queue.
 *
 * _HEAD
 * _HEAD_INITIALIZER
 * _ENTRY
 * _INIT
 * _EMPTY
 * _FIRST
 * _NEXT
 * _LAST
 * _FOREACH
 * _FOREACH_SAFE
 * _INSERT_HEAD
 * _INSERT_AFTER
 * _INSERT_TAIL
 * _CONCAT
 * _REMOVE_AFTER
 * _REMOVE_HEAD
 * _REMOVE_HEAD_UNTIL
 * _REMOVE
 * _SWAP
 *
 */

 /*
  * Horrible macros to enable use of code that was meant to be C-specific
  *   (and which push struct onto type) in C++; without these, C++ code
  *   that uses these macros in the context of a class will blow up
  *   due to "struct" being preprended to "type" by the macros, causing
  *   inconsistent use of tags.
  *
  * This approach is necessary because these are macros; we have to use
  *   these on a per-macro basis (because the queues are implemented as
  *   macros, disabling this warning in the scope of the header file is
  *   insufficient), whuch means we can't use #pragma, and have to use
  *   _Pragma.  We only need to use these for the queue macros that
  *   prepend "struct" to "type" and will cause C++ to blow up.
  */
 #if defined(__clang__) && defined(__cplusplus)
 #define __MISMATCH_TAGS_PUSH						\
 	_Pragma("clang diagnostic push")				\
 	_Pragma("clang diagnostic ignored \"-Wmismatched-tags\"")
 #define __MISMATCH_TAGS_POP						\
 	_Pragma("clang diagnostic pop")
 #else
 #define __MISMATCH_TAGS_PUSH
 #define __MISMATCH_TAGS_POP
 #endif

/*
 * Singly-linked Tail queue declarations.
 */
#define	QUEUE_HEAD(name, type)						\
__MISMATCH_TAGS_PUSH							\
struct name {								\
	struct type *stqh_first;/* first element */			\
	struct type **stqh_last;/* addr of last next element */		\
}									\
__MISMATCH_TAGS_POP

#define	QUEUE_HEAD_INITIALIZER(head)					\
	{ NULL, &(head).stqh_first }

#define	QUEUE_ENTRY(type)						\
__MISMATCH_TAGS_PUSH							\
struct {								\
	struct type *stqe_next;	/* next element */			\
}									\
__MISMATCH_TAGS_POP

/*
 * Singly-linked Tail queue functions.
 */
#define	QUEUE_CONCAT(head1, head2) do {				\
	if (!QUEUE_EMPTY((head2))) {					\
		*(head1)->stqh_last = (head2)->stqh_first;		\
		(head1)->stqh_last = (head2)->stqh_last;		\
		QUEUE_INIT((head2));					\
	}								\
} while (0)

#define	QUEUE_EMPTY(head)	((head)->stqh_first == NULL)

#define	QUEUE_FIRST(head)	((head)->stqh_first)

#define	QUEUE_FOREACH(var, head, field)				\
	for((var) = QUEUE_FIRST((head));				\
	   (var);							\
	   (var) = QUEUE_NEXT((var), field))


#define	QUEUE_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = QUEUE_FIRST((head));				\
	    (var) && ((tvar) = QUEUE_NEXT((var), field), 1);		\
	    (var) = (tvar))

#define	QUEUE_INIT(head) do {						\
	QUEUE_FIRST((head)) = NULL;					\
	(head)->stqh_last = &QUEUE_FIRST((head));			\
} while (0)

#define	QUEUE_INSERT_AFTER(head, tqelm, elm, field) do {		\
	if ((QUEUE_NEXT((elm), field) = QUEUE_NEXT((tqelm), field)) == NULL)\
		(head)->stqh_last = &QUEUE_NEXT((elm), field);		\
	QUEUE_NEXT((tqelm), field) = (elm);				\
} while (0)

#define	QUEUE_INSERT_HEAD(head, elm, field) do {			\
	if ((QUEUE_NEXT((elm), field) = QUEUE_FIRST((head))) == NULL)	\
		(head)->stqh_last = &QUEUE_NEXT((elm), field);		\
	QUEUE_FIRST((head)) = (elm);					\
} while (0)

#define	QUEUE_INSERT_TAIL(head, elm, field) do {			\
	QUEUE_NEXT((elm), field) = NULL;				\
	*(head)->stqh_last = (elm);					\
	(head)->stqh_last = &QUEUE_NEXT((elm), field);			\
} while (0)

#define	QUEUE_LAST(head, type, field)					\
__MISMATCH_TAGS_PUSH							\
	(QUEUE_EMPTY((head)) ?						\
		NULL :							\
	        ((struct type *)(void *)				\
		((char *)((head)->stqh_last) - __offsetof(struct type, field))))\
__MISMATCH_TAGS_POP

#define	QUEUE_NEXT(elm, field)	((elm)->field.stqe_next)

#define	QUEUE_REMOVE(head, elm, type, field)				\
__MISMATCH_TAGS_PUSH							\
do {									\
	if (QUEUE_FIRST((head)) == (elm)) {				\
		QUEUE_REMOVE_HEAD((head), field);			\
	}								\
	else {								\
		struct type *curelm = QUEUE_FIRST((head));		\
		while (QUEUE_NEXT(curelm, field) != (elm))		\
			curelm = QUEUE_NEXT(curelm, field);		\
		QUEUE_REMOVE_AFTER(head, curelm, field);		\
	}								\
	TRASHIT((elm)->field.stqe_next);				\
} while (0)								\
__MISMATCH_TAGS_POP

#define	QUEUE_REMOVE_HEAD(head, field) do {				\
	if ((QUEUE_FIRST((head)) =					\
	     QUEUE_NEXT(QUEUE_FIRST((head)), field)) == NULL)		\
		(head)->stqh_last = &QUEUE_FIRST((head));		\
} while (0)

#define QUEUE_REMOVE_HEAD_UNTIL(head, elm, field) do {                 \
       if ((QUEUE_FIRST((head)) = QUEUE_NEXT((elm), field)) == NULL) \
               (head)->stqh_last = &QUEUE_FIRST((head));              \
} while (0)

#define QUEUE_REMOVE_AFTER(head, elm, field) do {			\
	if ((QUEUE_NEXT(elm, field) =					\
	     QUEUE_NEXT(QUEUE_NEXT(elm, field), field)) == NULL)	\
		(head)->stqh_last = &QUEUE_NEXT((elm), field);		\
} while (0)

#define QUEUE_SWAP(head1, head2, type)					\
__MISMATCH_TAGS_PUSH							\
do {									\
	struct type *swap_first = QUEUE_FIRST(head1);			\
	struct type **swap_last = (head1)->stqh_last;			\
	QUEUE_FIRST(head1) = QUEUE_FIRST(head2);			\
	(head1)->stqh_last = (head2)->stqh_last;			\
	QUEUE_FIRST(head2) = swap_first;				\
	(head2)->stqh_last = swap_last;					\
	if (QUEUE_EMPTY(head1))					\
		(head1)->stqh_last = &QUEUE_FIRST(head1);		\
	if (QUEUE_EMPTY(head2))					\
		(head2)->stqh_last = &QUEUE_FIRST(head2);		\
} while (0)								\
__MISMATCH_TAGS_POP

#endif /* !_SYS_QUEUE_H_ */
