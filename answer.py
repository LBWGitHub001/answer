from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
 
def generate_launch_description():
    return LaunchDescription([
   	 Node(
            package='homework',
            executable='homework_node',
            name='homework'
        ),
        Node(
            package='answer',
            executable='answer_node',
            name='answer'),
        
  ])
   
