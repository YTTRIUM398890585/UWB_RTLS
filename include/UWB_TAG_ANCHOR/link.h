#include <Arduino.h>

class AnchorLinkedList
{
private:
    struct AnchorNode
	{
		uint16_t anchor_addr;

		// TODO: maybe do a moving average instead of average of whole history?
		// 0: average of current, previous, previous previous readings
		// 1: previous,
		// 2: previous previous
		float range[3];

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
    void update_anchor(uint16_t anchor_addr, float range, float dbm);
	void delete_anchor(uint16_t anchor_addr);

	void print_list();	
};
