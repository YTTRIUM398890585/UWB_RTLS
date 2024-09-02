#include <Arduino.h>

struct MyLink
{
	uint16_t anchor_addr;

	// 0: average of current, previous, previous previous readings
	// 1: previous,
	// 2: previous previous
	float range[3];

    // anchor coordinates [x, y, z]
    float anchor_coords[3];

	float dbm;
	struct MyLink* next;
};

struct MyLink* init_link();
void add_link(struct MyLink* p, uint16_t addr);
struct MyLink* find_link(struct MyLink* p, uint16_t addr);
void update_link(struct MyLink* p, uint16_t addr, float range, float dbm);
void print_link(struct MyLink* p);
void delete_link(struct MyLink* p, uint16_t addr);
