#ifndef JITTER_CHANNEL_H
#define JITTER_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

// JitterChannel扩展了PointToPointChannel，添加了可变延迟
class JitterChannel : public PointToPointChannel {
public:
  static TypeId GetTypeId (void);
  JitterChannel ();
  
  // 设置抖动参数
  void SetJitterDistribution(Ptr<RandomVariableStream> jitter);
  
  // 覆盖PointToPointChannel的Transmit方法以添加抖动
  virtual bool TransmitStart (Ptr<const Packet> p, Ptr<PointToPointNetDevice> src,
                             Time txTime);

private:
  Ptr<RandomVariableStream> m_jitter; // 抖动延迟的随机变量
};

#endif // JITTER_CHANNEL_H 