/*
 Module:        link2.h
 Creation date: 18-jan-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Utilities functions to handle double link list.
*/


// insert at begining of list
#define Insert2Link(/* EntryNode ** */listPP, /* EntryNode * */elemP, prev, next) \
	{														\
	if( NULL != ((elemP)->next = *(listPP)))		\
		(*(listPP))->prev = (elemP);					\
	(elemP)->prev = NULL;								\
	*(listPP) = elemP;									\
	}


// remove from any position
#define Remove2Link(/* EntryNode ** */listPP, /* EntryNode * */elemP, prev, next) \
	{														\
	if( (elemP)->next == NULL) {					\
		if( *(listPP) == (elemP))		/* -> E */	\
			*(listPP) = NULL;							\
		else												\
			((elemP)->prev)->next = NULL;	/* -> ? E */ \
		}													\
	else {												\
		if( *(listPP) == (elemP)) {				\
			*(listPP) = (elemP)->next;		/* -> E ? */ \
			(*(listPP))->prev = NULL;				\
			}												\
		else {											\
			((elemP)->prev)->next = (elemP)->next;	/* -> ? E ? */ \
			((elemP)->next)->prev = (elemP)->prev;	\
			}												\
		}													\
	(elemP)->prev = (elemP)->next = NULL; /* not required.. for cleanup only */ \
	}



// macro to remove an element from a single-link list.
// headP is pointer to list head
// tailP is pointer to list tail, NULL == none
#define RemoveFromSList( headP, tailP, elemP, nextF)	\
	do {																\
	void * p, * prev;												\
																		\
	for( p = *(headP); p; p = p->(nextF)) {				\
		if( (elemP) == p) {										\
			if( *(headP) == (elemP)								\
				*(headP) = (elemP)->(nextF);					\
			else														\
				prev->(nextF) = (elemP)->(nextF);			\
			if( (tailP) && (elemP) == *(tailP)				\
				*(tailP) = prev;									\
			break;													\
			}															\
		prev = p;													\
		}																\
	} while( 0)



