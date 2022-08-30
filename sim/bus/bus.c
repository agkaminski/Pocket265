#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/error.h"
#include "bus.h"

typedef struct list {
	struct list *prev;
	struct list *next;
	busentry_t entry;
} list_t;

list_t *buslist;

static int list_add(list_t **list, list_t *node)
{
	list_t *curr;

	if (node->entry.begin > node->entry.end)
		FATAL("Invalid entry b: 0x%04x e: 0x%04x", node->entry.begin, node->entry.end);

	curr = *list;

	if (*list == NULL) {
		DEBUG("Inserting b: 0x%04x e: 0x%04x in empty list", node->entry.begin, node->entry.end);
		*list = node;
		node->next = NULL;
		node->prev = NULL;
		return 0;
	}

	while (1) {
		if (curr->entry.begin > node->entry.end) {
			if (curr->prev == NULL) {
				DEBUG("Inserting b: 0x%04x e: 0x%04x in prev", node->entry.begin, node->entry.end);
				curr->prev = node;
				node->next = curr;
				node->prev = NULL;
				return 0;
			}

			if (curr->prev->entry.end < node->entry.begin) {
				DEBUG("Inserting b: 0x%04x e: 0x%04x between prev and curr", node->entry.begin, node->entry.end);
				curr->prev->next = node;
				node->prev = curr->prev;
				node->next = curr;
				curr->prev = node;
				return 0;
			}

			if (curr->prev->entry.begin > node->entry.end) {
				DEBUG("Iterating prev");
				curr = curr->prev;
				continue;
			}
		}

		if (curr->entry.end < node->entry.begin) {
			if (curr->next == NULL) {
				DEBUG("Inserting b: 0x%04x e: 0x%04x in next", node->entry.begin, node->entry.end);
				curr->next = node;
				node->next = NULL;
				node->prev = curr;
				return 0;
			}

			if (curr->next->entry.begin > node->entry.end) {
				DEBUG("Inserting b: 0x%04x e: 0x%04x between next and curr", node->entry.begin, node->entry.end);
				curr->next->prev = node;
				node->next = curr->next;
				node->prev = curr;
				curr->next = node;
				return 0;
			}

			if (curr->next->entry.end < node->entry.begin) {
				DEBUG("Iterating next");
				curr = curr->next;
				continue;
			}
		}
	}
}

static list_t *list_find(list_t *list, u16 addr)
{
	list_t *curr = list;

	while (curr != NULL) {
		if (curr->entry.begin <= addr && curr->entry.end >= addr) {
			DEBUG("Found addr 0x%04x in b: 0x%04x e: 0x%04x", addr, curr->entry.begin, curr->entry.end);
			return curr;
		}

		curr = curr->prev;
	}

	curr = list->next;

	while (curr != NULL) {
		if (curr->entry.begin <= addr && curr->entry.end >= addr) {
			DEBUG("Found addr 0x%04x in b: 0x%04x e: 0x%04x", addr, curr->entry.begin, curr->entry.end);
			return curr;
		}

		curr = curr->next;
	}

	DEBUG("Not found");

	return NULL;
}

void bus_write(u16 addr, u8 data)
{
	list_t *node;

	node = list_find(buslist, addr);

	if (node == NULL) {
		WARN("Invalid bus access (address 0x%04x)", addr);
	}
	else {
		node->entry.write(addr - node->entry.begin, data);
		DEBUG("Wrote 0x%02x to 0x%04x", data, addr);
	}
}

u8 bus_read(u16 addr)
{
	list_t *node;
	u8 data;

	node = list_find(buslist, addr);

	if (node == NULL) {
		WARN("Invalid bus access (address 0x%04x)", addr);
		data = 0xff;
	}
	else {
		data = node->entry.read(addr - node->entry.begin);
		DEBUG("Read 0x%02x from 0x%04x", data, addr);
	}
	return data;
}

void bus_register(busentry_t entry)
{
	list_t *node;

	if ((node = malloc(sizeof(list_t))) == NULL)
		FATAL("Out of memory!");

	node->entry = entry;

	list_add(&buslist, node);
}
