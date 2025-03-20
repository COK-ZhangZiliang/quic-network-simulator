#include "complex-channel.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"
#include "ns3/data-rate.h"

NS_LOG_COMPONENT_DEFINE("ComplexChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ComplexChannel);

TypeId
ComplexChannel::GetTypeId(void)
{
  static TypeId tid = TypeId("ComplexChannel")
    .SetParent<PointToPointChannel> ()
    .AddConstructor<ComplexChannel> ()
    .AddAttribute("Jitter",
                  "随机变量用于确定链路延迟的抖动",
                  PointerValue(),
                  MakePointerAccessor(&ComplexChannel::m_jitter),
                  MakePointerChecker<RandomVariableStream>())
  ;
  return tid;
}

ComplexChannel::ComplexChannel()
  : PointToPointChannel(),
    m_bandwidthVariationEnabled(false),
    m_lowBandwidth(DataRate("1Mbps")),
    m_highBandwidth(DataRate("10Mbps")),
    m_lowDuration(Seconds(5)),
    m_highDuration(Seconds(10)),
    m_currentBandwidth(DataRate("10Mbps"))
{
  NS_LOG_FUNCTION(this);
}

void
ComplexChannel::SetJitterModel(Ptr<RandomVariableStream> jitter)
{
  NS_LOG_FUNCTION(this << jitter);
  m_jitter = jitter;
}

void
ComplexChannel::SetBandwidthVariation(DataRate lowRate, DataRate highRate, Time lowDuration, Time highDuration)
{
  NS_LOG_FUNCTION(this << lowRate << highRate << lowDuration << highDuration);
  m_bandwidthVariationEnabled = true;
  m_lowBandwidth = lowRate;
  m_highBandwidth = highRate;
  m_lowDuration = lowDuration;
  m_highDuration = highDuration;
  m_currentBandwidth = highRate;
  
  // 调度第一次带宽变化
  Simulator::Schedule(m_highDuration, &ComplexChannel::SwitchToLowBandwidth, this);
}

DataRate
ComplexChannel::GetCurrentBandwidth(void)
{
  return m_currentBandwidth;
}

void
ComplexChannel::SwitchToHighBandwidth(void)
{
  NS_LOG_FUNCTION(this);
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到高带宽: " << m_highBandwidth << std::endl;
  m_currentBandwidth = m_highBandwidth;
  
  // 调度下一次带宽变化
  Simulator::Schedule(m_highDuration, &ComplexChannel::SwitchToLowBandwidth, this);
}

void
ComplexChannel::SwitchToLowBandwidth(void)
{
  NS_LOG_FUNCTION(this);
  std::cout << Simulator::Now().GetSeconds() << "s: 切换到低带宽: " << m_lowBandwidth << std::endl;
  m_currentBandwidth = m_lowBandwidth;
  
  // 调度下一次带宽变化
  Simulator::Schedule(m_lowDuration, &ComplexChannel::SwitchToHighBandwidth, this);
}

bool
ComplexChannel::TransmitStart(Ptr<const Packet> p, Ptr<PointToPointNetDevice> src, Time txTime)
{
  NS_LOG_FUNCTION(this << p << src);
  NS_LOG_LOGIC("UID is " << p->GetUid() << ")");

  Ptr<PointToPointNetDevice> dst = GetDestination(src);

  // 获取基本延迟
  Time delay = GetDelay();
  
  // 如果设置了抖动，添加随机抖动
  if (m_jitter)
  {
    // 生成随机抖动，单位为毫秒
    double jitterMs = m_jitter->GetValue();
    Time jitterDelay = MilliSeconds(jitterMs);
    delay += jitterDelay;
    NS_LOG_INFO("添加抖动: " << jitterMs << "ms，总延迟: " << delay.GetMilliSeconds() << "ms");
  }

  // 计算传输延迟（基于当前带宽）
  Time transmissionDelay = Seconds(p->GetSize() * 8.0 / m_currentBandwidth.GetBitRate());
  NS_LOG_INFO("传输延迟: " << transmissionDelay.GetMilliSeconds() << "ms（基于带宽" << m_currentBandwidth << "）");
  
  // 调度接收事件（传播延迟 + 传输延迟）
  Simulator::ScheduleWithContext(dst->GetNode()->GetId(),
                                delay + transmissionDelay + txTime, 
                                &PointToPointNetDevice::Receive,
                                dst, p->Copy());
  return true;
}

} // namespace ns3 