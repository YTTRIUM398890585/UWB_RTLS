#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include "micro_ros_interfaces/msg/two_int_test.h"


#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only avaliable for Arduino framework with serial transport.
#endif

rcl_publisher_t publisher;
rcl_subscription_t subscriber;
micro_ros_interfaces__msg__TwoIntTest send_msg;
std_msgs__msg__Int32 recv_msg;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

String debug_str = "0";

bool onLed = true;

#define RCCHECK(fn)                  \
    {                                \
        rcl_ret_t temp_rc = fn;      \
        if ((temp_rc != RCL_RET_OK)) \
        {                            \
            error_loop();            \
        }                            \
    }
#define RCSOFTCHECK(fn)              \
    {                                \
        rcl_ret_t temp_rc = fn;      \
        if ((temp_rc != RCL_RET_OK)) \
        {                            \
        }                            \
    }

// Error handle loop
void error_loop()
{
    while (1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        Serial.println(debug_str);
    }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    RCLC_UNUSED(last_call_time);
    if (timer != NULL)
    {
        RCSOFTCHECK(rcl_publish(&publisher, &send_msg, NULL));
        send_msg.data1++;
        send_msg.data2++;
    }
}

void subscription_callback(const void *msgin)
{
    const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msgin;

    if (msg->data > 0)
    {
        onLed = true;
    }
    else
    {
        onLed = false;
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Configure transoport to be wifi
    IPAddress agent_ip(192, 168, 1, 113);
    size_t agent_port = 8888;

    char ssid[] = "WIFI_SSID";
    char psk[]= "WIFI_PSK";

    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
    delay(2000);

    allocator = rcl_get_default_allocator();

    debug_str = "1";

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_options, allocator));

    debug_str = "2";

    size_t domain_id = 57;
    RCCHECK(rcl_init_options_set_domain_id(&init_options, domain_id));

    debug_str = "3";

    // create init_options
    RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

    debug_str = "4";

    // create node
    RCCHECK(rclc_node_init_default(&node, "uROS", "", &support));

    debug_str = "5";

    // create publisher
    RCCHECK(rclc_publisher_init_default(
        &publisher,
        &node,
        // ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        ROSIDL_GET_MSG_TYPE_SUPPORT(micro_ros_interfaces, msg, TwoIntTest),
        "uROS_pub"));

    debug_str = "6";

    // create timer
    const unsigned int timer_timeout = 1000;
    RCCHECK(rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(timer_timeout),
        timer_callback));

    debug_str = "7";

    // create subscriber
    RCCHECK(rclc_subscription_init_default(
        &subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "uROS_sub"));

    // create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &recv_msg, &subscription_callback, ON_NEW_DATA));

    debug_str = "8";

    RCCHECK(rclc_executor_add_timer(&executor, &timer));

    send_msg.data1 = 0;
    send_msg.data2 = 1;

    debug_str = "9";
}

void loop()
{
    delay(200);
    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

    if (onLed)
    {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}