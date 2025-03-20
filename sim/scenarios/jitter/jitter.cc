#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "../helper/quic-network-simulator-helper.h"
#include "jitter-point-to-point-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ns3 simulator");

int main(int argc, char *argv[]) {
  std::string delay, bandwidth, queue, jitter_model, jitter_params;
  CommandLine cmd;
  cmd.AddValue("delay", "delay of the p2p link", delay);
  cmd.AddValue("bandwidth", "bandwidth of the p2p link", bandwidth);
  cmd.AddValue("queue", "queue size of the p2p link (in packets)", queue);
  cmd.AddValue("jitter_model", "抖动模型 (如 'ns3::NormalRandomVariable')", jitter_model);
  cmd.AddValue("jitter_params", "抖动模型参数 (如 'Mean=5|Variance=2')", jitter_params);
  cmd.Parse(argc, argv);

  NS_ABORT_MSG_IF(delay.length() == 0, "Missing parameter: delay");
  NS_ABORT_MSG_IF(bandwidth.length() == 0, "Missing parameter: bandwidth");
  NS_ABORT_MSG_IF(queue.length() == 0, "Missing parameter: queue");
  NS_ABORT_MSG_IF(jitter_model.length() == 0, "Missing parameter: jitter_model");

  QuicNetworkSimulatorHelper sim;

  // 创建具有抖动特性的点对点链路
  JitterPointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue(bandwidth));
  p2p.SetChannelAttribute("Delay", StringValue(delay));
  p2p.SetQueueSize(StringValue(queue + "p"));
  p2p.SetJitter(jitter_model, jitter_params);

  NetDeviceContainer devices = p2p.Install(sim.GetLeftNode(), sim.GetRightNode());

  sim.Run(Seconds(36000));
} 