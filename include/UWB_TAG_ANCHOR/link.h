#include <Arduino.h>
#include <string.h>

class AnchorLinkedList
{
private:
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

    AnchorNode* head;
    AnchorNode* tail;

    size_t size;

	AnchorNode* find_anchor(uint16_t anchor_addr);
public:
    AnchorLinkedList();
    ~AnchorLinkedList();

	void add_anchor(uint16_t anchor_addr, float* p_anchor_coords);
    void update_anchor(uint16_t anchor_addr, float distance, float dbm);
	void delete_anchor(uint16_t anchor_addr);

	void print_list();	
};
