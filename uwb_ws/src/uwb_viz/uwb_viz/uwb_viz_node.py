import rclpy
from rclpy.node import Node

from micro_ros_interfaces.msg import TagPub

from geometry_msgs.msg import Point

from std_msgs.msg import ColorRGBA
from visualization_msgs.msg import Marker

class UwbVizNode(Node):
    def __init__(self):
        super().__init__('uwbVizNode')

        self.tagPubSubscription = self.create_subscription(
            TagPub,
            'uwb_pub',
            self.tagListenerCallback,
            10)
        
        self.markerPublisher = self.create_publisher(Marker, 'visualization_marker', 10)
        
    def tagListenerCallback(self, msg):
        # Get the anchors coordinates
        anchorsCoords = []  # List to store anchorCoords
        
        for anchor in msg.anchors:
            point = Point()
            point.x = anchor.anchor_coords.x
            point.y = anchor.anchor_coords.y
            point.z = anchor.anchor_coords.z
            anchorsCoords.append(point)
            
        # Get the tag coordinates
        tagCoords = Point()
        tagCoords.x = msg.tag_coords.x
        tagCoords.y = msg.tag_coords.y
        tagCoords.z = msg.tag_coords.z
        
        # Plot out the markers
        marker = Marker()
        marker.header.frame_id = "map"
        marker.header.stamp = self.get_clock().now().to_msg()
        marker.ns = "uwb_viz"
        marker.id = 0
        marker.type = Marker.POINTS
        marker.action = Marker.ADD
        marker.pose.orientation.w = 1.0
        marker.scale.x = 0.2
        marker.scale.y = 0.2
        
        anchorColor = ColorRGBA()
        anchorColor.r = 1.0
        anchorColor.g = 0.0
        anchorColor.b = 0.0
        anchorColor.a = 1.0
        
        tagColor = ColorRGBA()
        tagColor.r = 0.0
        tagColor.g = 1.0
        tagColor.b = 0.0
        tagColor.a = 1.0
        
        for anchorCoord in anchorsCoords:
            marker.points.append(anchorCoord)
            marker.colors.append(anchorColor)
            
        marker.points.append(tagCoords)
        marker.colors.append(tagColor)

        self.markerPublisher.publish(marker)

def main(args=None):
    rclpy.init(args=args)
    
    uwbVizNode = UwbVizNode()

    rclpy.spin(uwbVizNode)

    uwbVizNode.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()