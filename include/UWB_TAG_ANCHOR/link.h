#pragma once

#include <Arduino.h>
#include <string.h>

class AnchorLinkedList
{
public:
	AnchorLinkedList();
	~AnchorLinkedList();

	struct AnchorNode
	{
		uint16_t anchor_addr;

		// array of last 10 distance raw measurements
		float distance[10];

		// anchor coordinates [x, y, z]
		float anchor_coords[3];

		float dbm;

		struct AnchorNode* prev;
		struct AnchorNode* next;
	};

	void add_anchor(uint16_t anchor_addr, float* p_anchor_coords);
	void update_anchor(uint16_t anchor_addr, float distance, float dbm);
	void delete_anchor(uint16_t anchor_addr);

	void print_list();

	AnchorNode* getHead()
	{
		return head;
	}

    size_t getSize()
    {
        return size;
    }

private:
	AnchorNode* head;
	AnchorNode* tail;

	size_t size;

	AnchorNode* find_anchor(uint16_t anchor_addr);
};
