#pragma once

#include <Arduino.h>
#include <string.h>

class AnchorLinkedList
{
public:
	AnchorLinkedList();
	~AnchorLinkedList();

    static const uint8_t MAX_BUFFER_SIZE = 10;

	struct AnchorNode
	{
		uint16_t anchor_addr;

		// array of last 10 distance raw measurements
		float distance[MAX_BUFFER_SIZE];
        bool buffer_full = false;

		// anchor coordinates [x, y, z]
		float anchor_coords[3];

		float dbm;

		struct AnchorNode* prev;
		struct AnchorNode* next;
	};

	void add_anchor(uint16_t anchor_addr, float* p_anchor_coords);
	void update_anchor(uint16_t anchor_addr, float distance, float dbm);
	void delete_anchor(uint16_t anchor_addr);

    bool all_buffer_full();

    void copyFrom(const AnchorLinkedList& other);

    void clearDistance();

    void clear();

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
