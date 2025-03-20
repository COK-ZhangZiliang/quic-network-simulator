#ifndef COMPLEX_CHANNEL_H
#define COMPLEX_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "complex-error-model.h"

using namespace ns3;

// ComplexChannel扩展了PointToPointChannel，添加了更多的网络异常模拟功能
class ComplexChannel : public PointToPointChannel {
public:
  static TypeId GetTypeId (void);
  ComplexChannel ();
  
  // 设置抖动模型
  void SetJitterModel(Ptr<RandomVariableStream> jitter);
  
  // 设置带宽变化
  void SetBandwidthVariation(DataRate lowRate, DataRate highRate, Time lowDuration, Time highDuration);
  
  // 获取当前带宽
  DataRate GetCurrentBandwidth (void);
  
  // 覆盖PointToPointChannel的TransmitStart方法以添加变化的延迟
  virtual bool TransmitStart (Ptr<const Packet> p, Ptr<PointToPointNetDevice> src, Time txTime);

private:
  // 切换到高带宽
  void SwitchToHighBandwidth (void);
  
  // 切换到低带宽
  void SwitchToLowBandwidth (void);

  Ptr<RandomVariableStream> m_jitter; // 抖动延迟的随机变量
  
  // 带宽变化相关参数
  bool m_bandwidthVariationEnabled;  // 是否启用带宽变化
  DataRate m_lowBandwidth;           // 低带宽值
  DataRate m_highBandwidth;          // 高带宽值
  Time m_lowDuration;                // 低带宽持续时间
  Time m_highDuration;               // 高带宽持续时间
  DataRate m_currentBandwidth;       // 当前带宽值
};

#endif // COMPLEX_CHANNEL_H 