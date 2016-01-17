/*
 * hashtable.c
 *
 *  Created on: Jan 17, 2016
 *      Author: Jameson
 */

#include "hashtable.h"

/********************************************
 	 HASH TABLE FUNCTIONS
 ********************************************/

// listInsert: insert into a linked list
nodeType* listInsert (nodeType *head, long long int key, int value) {
	if (head == NULL) {
		head = malloc(sizeof(nodeType));
		if (head == NULL) {
			printf ("Memory allocation for head failed\n");
		}
		head->key = key;
		head->value = value;
		head->next = NULL;
	}
	else if (head->key != key) {
		head->next = listInsert (head->next, key, value);
	}

	return head;
}

// listPrint: prints out the contents of a list for debugging
void listPrint (nodeType *head) {
	if (head == NULL) {
		printf ("\n\n");
	}
	else {
		printf ("(%lli,%d) ", head->key, head->value);
		listPrint (head->next);
	}
}

// handToKey: computes the key for a given hand
long long int handToKey (handType *hand) {
	long long int key = 0;

	for (int i = 0; i < hand->handSize; i++) {
		if (hand->cards[i]->rank < 9) {
			key = key * 10 + hand->cards[i]->rank;
		}
		else {
			key = key * 10 + 9;	// ten, jack, queen, king have same value in blackjack, so we won't differentiate for the key
		}
	}

	return key;
}

// handToIndex: finds the index in the hash table for a given hand
int handToIndex (handType *hand) {
	int rawSum = 0;

	for (int i = 0; i < hand->handSize; i++) {
		if (hand->cards[i]->rank < 9) {
			rawSum += hand->cards[i]->rank + 1;
		}
		else {
			rawSum += 10;
		}
	}

	return rawSum;
}

void hashTableInsert (hashTableType *table, handType *hand, int response) {
	int index = handToIndex(hand);
	long long int key = handToKey(hand);
	table->heads[index] = listInsert(table->heads[index], key, response);

	printf ("hashTableInsert: index %d\n", index);
	listPrint (table->heads[index]);
}

void static hashTableInitAllKeys (hashTableType *table, handType *hand, int* ranks, int curRank) {
	printf ("hashTableInitAllKeys: ");
	for (int i = 0; i < hand->handSize; i++) {
		printf ("%d ", hand->cards[i]->rank);
	}
	printf ("\n");

	hashTableInsert (table, hand, randInt(0, 1));	// choose an initially random response for the AI

	if (ranks[curRank] >= NUM_SUITS) {
		curRank += 1;
	}

	for (int i = curRank; i < NUM_RANKS; i++) {
		hand->cards[hand->handSize]->rank = i;
		hand->handSize += 1;
		ranks[i] += 1;

		if (handFindSum (hand) > 21) {
			hand->handSize -= 1;
			ranks[i] -=1;
			break;
		}

		hashTableInitAllKeys (table, hand, ranks, i);
		hand->handSize -= 1;
		ranks[i] -= 1;
	}
}

void hashTableInit (hashTableType *table) {
	printf ("hashTableInit\n");

	handType hand;
	int ranks[NUM_RANKS] = {0};

	table = malloc (sizeof (hashTableType));
	if (table == NULL) {
		printf ("Memory allocation for hash table failed\n");
	}

	for (int i = 0; i < MAX_HAND_SIZE; i++) {
		hand.cards[i] = malloc (sizeof (cardType));
		if (hand.cards[i] == NULL) {
			printf ("Memory allocation for cards in hand failed\n");
		}
	}

	hand.handSize = 0;

	for (int i = 0; i < NUM_RANKS; i++) {
		hand.cards[hand.handSize]->rank = i;	// we don't care about suit here
		hand.handSize += 1;
		ranks[i] += 1;

		for (int j = i; j < NUM_RANKS; j++) {
			hand.cards[hand.handSize]->rank = j;
			hand.handSize += 1;
			ranks[j] += 1;

			hashTableInitAllKeys (table, &hand, ranks, j);	// guarantees that hand will have at least two cards

			hand.handSize -= 1;
			ranks[j] -= 1;
		}

		hand.handSize -=1;
		ranks[i] -= 1;
	}
}


