#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "../helper/quic-network-simulator-helper.h"
#include "../helper/quic-point-to-point-helper.h"
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ns3 simulator");

// 带宽变化模式枚举
enum BandwidthMode {
  STEP,      // 阶梯变化
  LINEAR,    // 线性变化
  PERIODIC   // 周期变化
};

// 存储当前带宽配置
struct BandwidthConfig {
  // 带宽变化模式
  BandwidthMode mode;
  // 阶梯变化参数
  std::string high_bandwidth;
  std::string low_bandwidth;
  Time duration_high;
  Time duration_low;
  // 线性变化参数
  double current_bandwidth;    
  double bandwidth_change_rate;
  // 周期变化参数
  double period;                 
  double amplitude;              
  double mean_bandwidth;     
};

// 全局配置对象
BandwidthConfig g_config;
Ptr<NetDevice> g_left_device;
Ptr<NetDevice> g_right_device;
Ptr<PointToPointNetDevice> g_p2p_left;
Ptr<PointToPointNetDevice> g_p2p_right;

// 将字符串带宽转换为数值（Mbps）
double BandwidthToMbps(const std::string& bandwidth) {
  double value;
  std::string unit;
  std::istringstream iss(bandwidth);
  iss >> value >> unit;
  
  if (unit == "Mbps") return value;
  if (unit == "Kbps") return value / 1000.0;
  if (unit == "Gbps") return value * 1000.0;
  
  return value;
}

// 将数值（Mbps）转换为带宽字符串
std::string MbpsToBandwidth(double mbps) {
  std::ostringstream oss;
  oss << mbps << "Mbps";
  return oss.str();
}

// 阶梯变化函数
void SwitchToLowBandwidth();
void SwitchToHighBandwidth();

void SwitchToHighBandwidth() {
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到高带宽: " << g_config.high_bandwidth << std::endl;
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(g_config.high_bandwidth));
  g_p2p_right->SetDataRate(DataRate(g_config.high_bandwidth));
  Simulator::Schedule(g_config.duration_high, &SwitchToLowBandwidth);
}

void SwitchToLowBandwidth() {
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到低带宽: " << g_config.low_bandwidth << std::endl;
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(g_config.low_bandwidth));
  g_p2p_right->SetDataRate(DataRate(g_config.low_bandwidth));
  Simulator::Schedule(g_config.duration_low, &SwitchToHighBandwidth);
}

// 线性变化函数
void LinearBandwidthChange() {
  // 计算新带宽
  double current_time = Simulator::Now().GetSeconds();
  g_config.current_bandwidth -= g_config.bandwidth_change_rate;
  
  // 确保带宽在高低带宽范围内
  double high_bw = BandwidthToMbps(g_config.high_bandwidth);
  double low_bw = BandwidthToMbps(g_config.low_bandwidth);
  if (g_config.current_bandwidth < low_bw) {
    g_config.current_bandwidth = low_bw;
    g_config.bandwidth_change_rate = -g_config.bandwidth_change_rate;
  } else if (g_config.current_bandwidth > high_bw) {
    g_config.current_bandwidth = high_bw;
    g_config.bandwidth_change_rate = -g_config.bandwidth_change_rate;
  }
  std::cout << current_time << "s: 线性变化带宽: " << MbpsToBandwidth(g_config.current_bandwidth) << std::endl;
  
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(MbpsToBandwidth(g_config.current_bandwidth)));
  g_p2p_right->SetDataRate(DataRate(MbpsToBandwidth(g_config.current_bandwidth)));
  Simulator::Schedule(Seconds(0.1), &LinearBandwidthChange);
}

// 周期变化函数
void PeriodicBandwidthChange() {
  // 计算新带宽
  double current_time = Simulator::Now().GetSeconds();
  double bandwidth = g_config.mean_bandwidth + 
                     g_config.amplitude * std::sin(2 * M_PI * current_time / g_config.period);
  std::cout << current_time << "s: 周期变化带宽: " << MbpsToBandwidth(bandwidth) << std::endl;
  
  // 更新设备的数据速率
  g_p2p_left->SetDataRate(DataRate(MbpsToBandwidth(bandwidth)));
  g_p2p_right->SetDataRate(DataRate(MbpsToBandwidth(bandwidth)));
  Simulator::Schedule(Seconds(0.1), &PeriodicBandwidthChange);
}

int main(int argc, char *argv[]) {
  std::string delay, high_bandwidth, low_bandwidth, queue;
  std::string duration_high_str, duration_low_str;
  std::string mode_str;
  double bandwidth_change_rate, period, amplitude, mean_bandwidth;
  
  CommandLine cmd;
  cmd.AddValue("delay", "延迟", delay);
  cmd.AddValue("queue", "队列大小（单位：包）", queue);
  cmd.AddValue("mode", "带宽变化模式 (step/linear/periodic)", mode_str);
  cmd.AddValue("high_bandwidth", "高带宽值", high_bandwidth);
  cmd.AddValue("low_bandwidth", "低带宽值", low_bandwidth);
  cmd.AddValue("duration_high", "高带宽持续时间", duration_high_str);
  cmd.AddValue("duration_low", "低带宽持续时间", duration_low_str);
  cmd.AddValue("bandwidth_change_rate", "线性变化速率 (Mbps/s)", bandwidth_change_rate);
  cmd.AddValue("period", "周期变化周期 (s)", period);
  cmd.AddValue("amplitude", "周期变化振幅 (Mbps)", amplitude);
  cmd.AddValue("mean_bandwidth", "周期变化平均带宽 (Mbps)", mean_bandwidth);
  cmd.Parse(argc, argv);

  NS_ABORT_MSG_IF(delay.length() == 0, "缺少延迟参数");
  NS_ABORT_MSG_IF(queue.length() == 0, "缺少队列大小参数");

  // 设置带宽变化模式并保存相应参数
  if (mode_str == "step" || mode_str == "linear") {
    NS_ABORT_MSG_IF(high_bandwidth.length() == 0, "缺少高带宽参数");
    NS_ABORT_MSG_IF(low_bandwidth.length() == 0, "缺少低带宽参数");
    g_config.high_bandwidth = high_bandwidth;
    g_config.low_bandwidth = low_bandwidth;
    if (mode_str == "step") {
      NS_ABORT_MSG_IF(duration_high_str.length() == 0, "缺少高带宽持续时间参数");
      NS_ABORT_MSG_IF(duration_low_str.length() == 0, "缺少低带宽持续时间参数");
      g_config.duration_high = Time(duration_high_str);
      g_config.duration_low = Time(duration_low_str);
    } else if (mode_str == "linear") {
      NS_ABORT_MSG_IF(bandwidth_change_rate <= 0, "线性变化速率必须大于0");
      g_config.bandwidth_change_rate = bandwidth_change_rate;
      g_config.current_bandwidth = BandwidthToMbps(high_bandwidth);
    }
  } else {
    NS_ABORT_MSG_IF(mean_bandwidth <= 0, "周期变化平均带宽必须大于0");
    NS_ABORT_MSG_IF(amplitude <= 0, "周期变化振幅必须大于0");
    NS_ABORT_MSG_IF(amplitude > mean_bandwidth, "周期变化振幅不能大于平均带宽");
    NS_ABORT_MSG_IF(period <= 0, "周期变化周期必须大于0");
    g_config.mean_bandwidth = mean_bandwidth;
    g_config.amplitude = amplitude;
    g_config.period = period;
  }

  QuicNetworkSimulatorHelper sim;

  // 设置点对点链路
  QuicPointToPointHelper p2p;
  if (mode_str == "step" || mode_str == "linear") {
    p2p.SetDeviceAttribute("DataRate", StringValue(high_bandwidth));
  } else {
    p2p.SetDeviceAttribute("DataRate", StringValue(MbpsToBandwidth(mean_bandwidth)));
  }
  p2p.SetChannelAttribute("Delay", StringValue(delay));
  p2p.SetQueueSize(StringValue(queue + "p"));

  NetDeviceContainer devices = p2p.Install(sim.GetLeftNode(), sim.GetRightNode());
  
  // 保存设备指针以便后续动态修改
  g_left_device = devices.Get(0);
  g_right_device = devices.Get(1);
  g_p2p_left = DynamicCast<PointToPointNetDevice>(g_left_device);
  g_p2p_right = DynamicCast<PointToPointNetDevice>(g_right_device);

  // 根据模式启动相应的带宽变化
  switch (g_config.mode) {
    case LINEAR:
      Simulator::Schedule(Seconds(0), &LinearBandwidthChange);
      break;
    case PERIODIC:
      Simulator::Schedule(Seconds(0), &PeriodicBandwidthChange);
      break;
    case STEP:
      Simulator::Schedule(g_config.duration_high, &SwitchToLowBandwidth);
      break;
  }

  sim.Run(Seconds(36000));
} 