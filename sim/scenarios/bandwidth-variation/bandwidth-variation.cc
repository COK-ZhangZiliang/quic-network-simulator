#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "../helper/quic-network-simulator-helper.h"
#include "../helper/quic-point-to-point-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ns3 simulator");

// 存储当前带宽配置
struct BandwidthConfig {
  std::string high_bandwidth;
  std::string low_bandwidth;
  Time duration_high;
  Time duration_low;
};

// 全局配置对象
BandwidthConfig g_config;
Ptr<NetDevice> g_left_device;
Ptr<NetDevice> g_right_device;
Ptr<PointToPointNetDevice> g_p2p_left;
Ptr<PointToPointNetDevice> g_p2p_right;

// 切换到高带宽
void SwitchToHighBandwidth() {
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到高带宽: " << g_config.high_bandwidth << std::endl;
  
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(g_config.high_bandwidth));
  g_p2p_right->SetDataRate(DataRate(g_config.high_bandwidth));
  
  // 在低带宽持续时间后再次切换
  Simulator::Schedule(g_config.duration_high, &SwitchToLowBandwidth);
}

// 切换到低带宽
void SwitchToLowBandwidth() {
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到低带宽: " << g_config.low_bandwidth << std::endl;
  
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(g_config.low_bandwidth));
  g_p2p_right->SetDataRate(DataRate(g_config.low_bandwidth));
  
  // 在高带宽持续时间后再次切换
  Simulator::Schedule(g_config.duration_low, &SwitchToHighBandwidth);
}

int main(int argc, char *argv[]) {
  std::string delay, high_bandwidth, low_bandwidth, queue;
  std::string duration_high_str, duration_low_str;
  
  CommandLine cmd;
  cmd.AddValue("delay", "delay of the p2p link", delay);
  cmd.AddValue("high_bandwidth", "高带宽阶段的带宽值", high_bandwidth);
  cmd.AddValue("low_bandwidth", "低带宽阶段的带宽值", low_bandwidth);
  cmd.AddValue("queue", "queue size of the p2p link (in packets)", queue);
  cmd.AddValue("duration_high", "高带宽持续时间 (如 '10s')", duration_high_str);
  cmd.AddValue("duration_low", "低带宽持续时间 (如 '5s')", duration_low_str);
  cmd.Parse(argc, argv);

  NS_ABORT_MSG_IF(delay.length() == 0, "Missing parameter: delay");
  NS_ABORT_MSG_IF(high_bandwidth.length() == 0, "Missing parameter: high_bandwidth");
  NS_ABORT_MSG_IF(low_bandwidth.length() == 0, "Missing parameter: low_bandwidth");
  NS_ABORT_MSG_IF(queue.length() == 0, "Missing parameter: queue");
  NS_ABORT_MSG_IF(duration_high_str.length() == 0, "Missing parameter: duration_high");
  NS_ABORT_MSG_IF(duration_low_str.length() == 0, "Missing parameter: duration_low");

  // 保存配置
  g_config.high_bandwidth = high_bandwidth;
  g_config.low_bandwidth = low_bandwidth;
  g_config.duration_high = Time(duration_high_str);
  g_config.duration_low = Time(duration_low_str);

  QuicNetworkSimulatorHelper sim;

  // 设置点对点链路
  QuicPointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue(high_bandwidth)); // 初始为高带宽
  p2p.SetChannelAttribute("Delay", StringValue(delay));
  p2p.SetQueueSize(StringValue(queue + "p"));

  NetDeviceContainer devices = p2p.Install(sim.GetLeftNode(), sim.GetRightNode());
  
  // 保存设备指针以便后续动态修改
  g_left_device = devices.Get(0);
  g_right_device = devices.Get(1);
  g_p2p_left = DynamicCast<PointToPointNetDevice>(g_left_device);
  g_p2p_right = DynamicCast<PointToPointNetDevice>(g_right_device);

  // 调度第一次带宽切换
  Simulator::Schedule(g_config.duration_high, &SwitchToLowBandwidth);

  sim.Run(Seconds(36000));
} 