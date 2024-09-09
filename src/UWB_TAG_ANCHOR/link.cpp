#include "UWB_TAG_ANCHOR/link.h"

struct MyLink* init_link()
{
	struct MyLink* p = (struct MyLink*)malloc(sizeof(struct MyLink));

	p->anchor_addr = 0;

	p->range[0] = 0.0;
	p->range[1] = 0.0;
	p->range[2] = 0.0;

	p->anchor_coords[0] = 0.0;
	p->anchor_coords[1] = 0.0;
	p->anchor_coords[2] = 0.0;

	p->dbm = 0.0;

	p->next = NULL;

	return p;
}

void add_link(struct MyLink* p, uint16_t addr)
{
	struct MyLink* temp = p;
	// Find struct MyLink end
	while (temp->next != NULL) {
		temp = temp->next;
	}
	// Create an anchor
	struct MyLink* a = (struct MyLink*)malloc(sizeof(struct MyLink));
	a->anchor_addr = addr;
	a->range[0] = 0.0;
	a->range[1] = 0.0;
	a->range[2] = 0.0;
	a->dbm = 0.0;
	a->next = NULL;

	// Add the anchor to end of struct MyLink
	temp->next = a;

	return;
}

struct MyLink* find_link(struct MyLink* p, uint16_t addr)
{
	// Check inputs are valid
	if (addr == 0) {
		return NULL;
	}
	if (p->next == NULL) {
		return NULL;
	}

	struct MyLink* temp = p;
	// Find target struct MyLink or struct MyLink end
	while (temp->next != NULL) {
		temp = temp->next;
		if (temp->anchor_addr == addr) {
			return temp;
		}
	}

	Serial.println("find_link:Can't find addr");
	return NULL;
}

void update_link(struct MyLink* p, uint16_t addr, float range, float dbm)
{
	struct MyLink* temp = find_link(p, addr);
	if (temp != NULL) {
		temp->range[2] = temp->range[1];
		temp->range[1] = temp->range[0];
		temp->range[0] = (range + temp->range[1] + temp->range[2]) / 3;
		temp->dbm = dbm;
		return;
	}
	else {
		return;
	}
}

void print_link(struct MyLink* p)
{
	// in this implementation, the first struct MyLink is a dummy struct MyLink
	// all the real anchors is after it
	struct MyLink* temp = p;
	while (temp->next != NULL) {
		Serial.print("Dev ");
		Serial.print(temp->next->anchor_addr, HEX);
		Serial.print("\t");
		Serial.print(temp->next->range[0]);
		Serial.print(" m\t");
		Serial.print(temp->next->dbm);
		Serial.println(" dBm");

		temp = temp->next;
	}
	return;
}

void delete_link(struct MyLink* p, uint16_t addr)
{
	if (addr == 0) {
		return;
	}

	struct MyLink* temp = p;
	// Loop over all known links
	while (temp->next != NULL) {
		// Once we've found the link before the link to be removed
		if (temp->next->anchor_addr == addr) {
			// Miss out the next link
			struct MyLink* del = temp->next;
			temp->next = del->next;
			// Remove the next link
			free(del);
			return;
		}
		temp = temp->next;
	}
	return;
}
