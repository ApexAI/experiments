// Copyright 2019 Apex.AI, Inc.
// All rights reserved.

#include <iostream>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/executors.hpp>
#include <std_msgs/msg/int32.hpp>

constexpr std::chrono::milliseconds dds_participant_discovery_time{200};

rmw_qos_profile_t qos() {
  auto profile = rmw_qos_profile_default;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  profile.depth = 1000;
  profile.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  return profile;
}


class pub_sub
{
public:
  pub_sub()
  : node(std::make_shared<rclcpp::Node>("pub_sub_test_node")),
    node1(std::make_shared<rclcpp::Node>("pub_sub_test_node1")),
    sub_ptr(node->create_subscription<std_msgs::msg::Int32>(
      "pub_sub_test_topic",
      std::bind(&pub_sub::cb, this, std::placeholders::_1),
      qos())),
    pub_ptr(node1->create_publisher<std_msgs::msg::Int32>("pub_sub_test_topic", qos())),
    vect()
  {
    vect.reserve(100);
  }

  void cb(const std_msgs::msg::Int32::SharedPtr msg)
  {
    vect.push_back(*msg);
  }

  void publish(int i)
  {
    msg.data = i;
    pub_ptr->publish(msg);
  }

  rclcpp::Node::SharedPtr node;
  rclcpp::Node::SharedPtr node1;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub_ptr;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub_ptr;
  std::vector<std_msgs::msg::Int32> vect;
  std_msgs::msg::Int32 msg;
}; // class pub_sub


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  pub_sub ps;
  std::this_thread::sleep_for(dds_participant_discovery_time);

  while(true) {
    ps.vect.clear();

    for(int i = 0; i < 100; ++i) {
      ps.publish(i);
    }
    std::cout << "Done publishing" << std::endl;

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(ps.node);
    while(ps.vect.size() < 100) {
      exec.spin_once();
      std::cout << ps.vect.size() << std::endl;
    }
    std::cout << "Done receiving" << std::endl;
  }
}
