float this_anchor_target_distance = 10.1; // measured distance to anchor in m

uint16_t this_anchor_Adelay = 33120; // starting value
uint16_t Adelay_delta = 10;         // initial binary search step size

// We'll use a "short address" to make it easier to reference devices
char shortAddress[6];

bool calibrated = false;

void newRange();
void newDevice(DW1000Device* device);
void inactiveDevice(DW1000Device* device);