#include "UWB_TAG_ANCHOR/link.h"

/**
 * @brief Construct for Linked List object, will initialise a dummy node
 *
 */
AnchorLinkedList::AnchorLinkedList()
{
	// allocate memory for a dummy node and make it the head
    // the first node in the linked list is a dummy node all the real anchors is after it
	this->head = (struct AnchorNode*)malloc(sizeof(struct AnchorNode));

	// set values for dummy node
	this->head->anchor_addr = 0;

    memset(this->head->distance, 0, sizeof(this->head->distance));

	this->head->anchor_coords[0] = 0.0;
	this->head->anchor_coords[1] = 0.0;
	this->head->anchor_coords[2] = 0.0;

	this->head->dbm = 0.0;

	this->head->prev = NULL;
    this->head->next = NULL;

	// set tail to be same as head since its only one node
	this->tail = this->head;

	// set size of list to be 0, dummy node not counted
	this->size = 0;
}

/**
 * @brief Destroy the Anchor Linked List object
 * 
 */
AnchorLinkedList::~AnchorLinkedList() {
    // start from the tail and delete each node to free malloc until list is empty
    while(this->size > 0){
        this->delete_anchor(this->tail->anchor_addr);
    }

    // free the dummy head node
    free(this->head);
}

/**
 * @brief find the node in the linked list with the wanted anchor_addr
 *
 * @param anchor_addr wanted anchor_addr
 * @return pointer to the node with the wanted anchor_addr
 */
AnchorLinkedList::AnchorNode* AnchorLinkedList::find_anchor(uint16_t anchor_addr)
{
	// Check input are valid
	// address cannot be 0, since thats the default, avoid nonsense edge case
	if (anchor_addr == 0) {
		return NULL;
	}

	// search will start from the head, if the next of head is null, means list is empty
	// equivalent to if size of list == 0
	if (this->head->next == NULL) {
#ifdef DEBUG
		Serial.print("find_anchor: list is empty");
#endif
		return NULL;
	}

	// temp pointer starting from head to go throuhg the list to find the node with the right address
	AnchorNode* temp = this->head;

	// dummy node is ignored in the search
	while (temp->next != NULL) {
		temp = temp->next;
		if (temp->anchor_addr == anchor_addr) {
			return temp;
		}
	}

#ifdef DEBUG
	Serial.print("find_anchor: can't find anchor with address = ");
	Serial.println(anchor_addr);
#endif
	return NULL;
}

/**
 * @brief Add a node with anchor_addr and anchor_coords to the list
 *
 * @param anchor_addr unique address of the anchor
 * @param p_anchor_coords pointer to float array of x, y, z
 */
void AnchorLinkedList::add_anchor(uint16_t anchor_addr, float* p_anchor_coords)
{
	// allocate memory for a new node
	AnchorNode* new_node = (struct AnchorNode*)malloc(sizeof(struct AnchorNode));

	// set the anchor adress
	new_node->anchor_addr = anchor_addr;

	// set all distance to be 0, distance will be updated in the update anchor function
    memset(new_node->distance, 0, sizeof(new_node->distance));

	// copy the x, y, z, coordinate
	memcpy(new_node->anchor_coords, p_anchor_coords, 3);

	// set dbm to be 0, dbm will be updated in the update anchor function
	new_node->dbm = 0.0;

	// set the prev node to be old tail and next node to be null
	new_node->prev = this->tail;
	new_node->next = NULL;

	// set the new node to be the next of the old tail
	this->tail->next = new_node;

	// set the new node to be the new tail
	this->tail = new_node;

	// increment the size of list
	this->size++;
}

/**
 * @brief update the anchor node with the wanted anchor address with the values from DW1000
 * 
 * @param anchor_addr wanted anchor address
 * @param distance measured distance
 * @param dbm measured dBm
 */
void AnchorLinkedList::update_anchor(uint16_t anchor_addr, float distance, float dbm)
{
	// temp pointer pointing to the node with the wanted anchor address
	AnchorNode* temp = this->find_anchor(anchor_addr);

	if (temp != NULL) {
        // shift the distance array to the right and update the first element with the new distance
        for (auto i = sizeof(temp->distance)/sizeof(*temp->distance) - 1; i > 0; --i) {
            temp->distance[i] = temp->distance[i - 1];
        }

        temp->distance[0] = distance;

        // update dbm of the node
		temp->dbm = dbm;
	}
	else {
#ifdef DEBUG
		Serial.print("update_anchor: can't find anchor with address = ");
		Serial.println(anchor_addr);
#endif
	}
}

/**
 * @brief delete a node with anchor_addr in the linked list
 * 
 * @param anchor_addr anchor address to be deleted
 */
void AnchorLinkedList::delete_anchor(uint16_t anchor_addr)
{
    // temp pointer pointing to the node with the wanted anchor address, which is to be deleted
	AnchorNode* temp = this->find_anchor(anchor_addr);

	if (temp != NULL) {
        // not be possible to deleted the head but possible to delete the tail
        // deleting tail is an edge case to be handled differently
        if (temp == this->tail) {
            // update tail to be the new tail
            this->tail = temp->prev;
        } else {
            // set the prev of the node after temp to be the node before temp
            temp->next->prev = temp->prev;
        }

        // set the next of the node before temp to be the node after temp
        // this handles tail edge case by default
        temp->prev->next = temp->next;

        // free malloc for temp
		free(temp);

        // decrement the size of list
	    this->size--;
	}
	else {
#ifdef DEBUG
		Serial.print("delete_anchor: can't find anchor with address = ");
		Serial.println(anchor_addr);
#endif
	}
}

/**
 * @brief print out the whole linked list
 * 
 */
void AnchorLinkedList::print_list()
{
    // temp pointer starting from head
	AnchorNode* temp = this->head;

	// print from head with dummy node ignored
    if (this->size == 0) {
        Serial.println(" ========= No anchors =========");
    }

	while (temp->next != NULL) {
		Serial.print("Dev ");
		Serial.print(temp->next->anchor_addr, HEX);
		Serial.print(" ");
        Serial.print(temp->next->dbm);
		Serial.println(" dBm");

        Serial.print("Distance (m): ");

        for (auto i = 0; i < sizeof(this->head->distance)/sizeof(*this->head->distance); i++) {
            Serial.print(temp->next->distance[i]);
            Serial.print(" ");
        }
		Serial.println("");

		temp = temp->next;
	}
}
