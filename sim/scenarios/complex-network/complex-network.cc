#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "../helper/quic-network-simulator-helper.h"
#include "complex-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ns3 simulator");

int main(int argc, char *argv[]) {
  std::string delay, bandwidth, queue;
  std::string jitter_model, jitter_params;
  double drop_rate;
  uint32_t max_burst;
  bool cyclic_drop_enabled;
  std::string cyclic_period, cyclic_duration;
  bool bandwidth_variation_enabled;
  std::string low_bandwidth, high_bandwidth, low_duration, high_duration;
  
  CommandLine cmd;
  cmd.AddValue("delay", "基本传播延迟", delay);
  cmd.AddValue("bandwidth", "基本带宽", bandwidth);
  cmd.AddValue("queue", "队列大小（单位：数据包）", queue);
  
  // 抖动相关参数
  cmd.AddValue("jitter_model", "抖动模型，如 'ns3::NormalRandomVariable'", jitter_model);
  cmd.AddValue("jitter_params", "抖动模型参数，如 'Mean=5|Variance=2'", jitter_params);
  
  // 随机丢包相关参数
  cmd.AddValue("drop_rate", "随机丢包率 (0.0-1.0)", drop_rate);
  cmd.AddValue("max_burst", "最大连续丢包数", max_burst);
  
  // 周期性丢包相关参数
  cmd.AddValue("cyclic_drop", "是否启用周期性丢包 (0:关闭, 1:开启)", cyclic_drop_enabled);
  cmd.AddValue("cyclic_period", "周期性丢包周期，如 '10s'", cyclic_period);
  cmd.AddValue("cyclic_duration", "每次周期性丢包持续时间，如 '1s'", cyclic_duration);
  
  // 带宽变化相关参数
  cmd.AddValue("bandwidth_variation", "是否启用带宽变化 (0:关闭, 1:开启)", bandwidth_variation_enabled);
  cmd.AddValue("low_bandwidth", "低带宽值，如 '1Mbps'", low_bandwidth);
  cmd.AddValue("high_bandwidth", "高带宽值，如 '10Mbps'", high_bandwidth);
  cmd.AddValue("low_duration", "低带宽持续时间，如 '5s'", low_duration);
  cmd.AddValue("high_duration", "高带宽持续时间，如 '10s'", high_duration);
  
  cmd.Parse(argc, argv);

  // 检查必要参数
  NS_ABORT_MSG_IF(delay.length() == 0, "Missing parameter: delay");
  NS_ABORT_MSG_IF(bandwidth.length() == 0, "Missing parameter: bandwidth");
  NS_ABORT_MSG_IF(queue.length() == 0, "Missing parameter: queue");
  
  // 初始化网络模拟器
  QuicNetworkSimulatorHelper sim;

  // 创建复合网络辅助类
  ComplexHelper complex;
  complex.SetDeviceAttribute("DataRate", StringValue(bandwidth));
  complex.SetChannelAttribute("Delay", StringValue(delay));
  complex.SetQueueSize(StringValue(queue + "p"));
  
  // 设置抖动
  if (jitter_model.length() > 0) {
    complex.SetJitter(jitter_model, jitter_params);
  }
  
  // 设置随机丢包
  if (drop_rate > 0.0) {
    complex.SetRandomDropRate(drop_rate, max_burst);
  }
  
  // 设置周期性丢包
  if (cyclic_drop_enabled && cyclic_period.length() > 0 && cyclic_duration.length() > 0) {
    complex.SetCyclicDrop(true, Time(cyclic_period), Time(cyclic_duration));
  }
  
  // 设置带宽变化
  if (bandwidth_variation_enabled && 
      low_bandwidth.length() > 0 && high_bandwidth.length() > 0 && 
      low_duration.length() > 0 && high_duration.length() > 0) {
    complex.SetBandwidthVariation(low_bandwidth, high_bandwidth, 
                                 Time(low_duration), Time(high_duration));
  }
  
  // 安装到节点
  NetDeviceContainer devices = complex.Install(sim.GetLeftNode(), sim.GetRightNode());

  sim.Run(Seconds(36000));
} 