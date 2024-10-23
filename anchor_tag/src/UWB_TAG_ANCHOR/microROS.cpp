#ifdef IS_TAG

#include "UWB_TAG_ANCHOR/microROS.h"

rcl_publisher_t publisher;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

void errorHandler()
{
	while (1) {
	}
}

void setupMicroRos()
{
	// Configure transoport to be wifi
	IPAddress agent_ip(192, 168, 101, 96);
	size_t agent_port = 8888;

	char ssid[] = "Yttrium";
	char psk[] = "bananaonpizza";

	set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
	delay(2000);

	// Configure transoport to be serial
	// Serial.begin(115200);
	// set_microros_serial_transports(Serial);
	// delay(2000);

	allocator = rcl_get_default_allocator();

	rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));

	RCCHECK(rcl_init_options_set_domain_id(&init_options, size_t(ROS_DOMAIN_ID)));

	// create init_options
	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

	// create node
	RCCHECK(rclc_node_init_default(&node, "tag", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(&publisher,
	                                    &node,
	                                    ROSIDL_GET_MSG_TYPE_SUPPORT(micro_ros_interfaces, msg, TagPub),
	                                    "uwb_pub"));
}

void rosPublishLocation(AnchorLinkedList& uwb_data, Vector3f tagCoords)
{
	micro_ros_interfaces__msg__TagPub pub_msg;

    size_t size = uwb_data.getSize();

    // Allocate memory for the anchors array
    pub_msg.anchors.data = (micro_ros_interfaces__msg__AnchorData*)malloc(size * sizeof(micro_ros_interfaces__msg__AnchorData));
    pub_msg.anchors.size = size;
    pub_msg.anchors.capacity = size;

    AnchorLinkedList::AnchorNode* temp = uwb_data.getHead();

    for (size_t i = 0; i < size; i++) {
        micro_ros_interfaces__msg__AnchorData anchor_data;

        anchor_data.anchor_addr = temp->next->anchor_addr;
        anchor_data.anchor_coords.x = temp->next->anchor_coords[0];
        anchor_data.anchor_coords.y = temp->next->anchor_coords[1];
        anchor_data.anchor_coords.z = temp->next->anchor_coords[2];
        anchor_data.anchor_distance = temp->next->distance[0];
        anchor_data.dbm = temp->next->dbm;

        temp = temp->next;

        pub_msg.anchors.data[i] = anchor_data;

    }

    pub_msg.tag_coords.x = tagCoords.x();
    pub_msg.tag_coords.y = tagCoords.y();
    pub_msg.tag_coords.z = tagCoords.z();

    // publish message
    RCSOFTCHECK(rcl_publish(&publisher, &pub_msg, NULL));

    // Free the allocated memory
    free(pub_msg.anchors.data);
}

#endif