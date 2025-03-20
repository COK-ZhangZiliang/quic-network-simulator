#include "jitter-channel.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"

NS_LOG_COMPONENT_DEFINE("JitterChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(JitterChannel);

TypeId
JitterChannel::GetTypeId(void)
{
  static TypeId tid = TypeId("JitterChannel")
    .SetParent<PointToPointChannel> ()
    .AddConstructor<JitterChannel> ()
    .AddAttribute("Jitter",
                  "随机变量用于确定链路延迟的抖动",
                  PointerValue(),
                  MakePointerAccessor(&JitterChannel::m_jitter),
                  MakePointerChecker<RandomVariableStream>())
  ;
  return tid;
}

JitterChannel::JitterChannel()
  : PointToPointChannel()
{
  NS_LOG_FUNCTION(this);
}

void
JitterChannel::SetJitterDistribution(Ptr<RandomVariableStream> jitter)
{
  NS_LOG_FUNCTION(this << jitter);
  m_jitter = jitter;
}

bool
JitterChannel::TransmitStart(Ptr<const Packet> p, Ptr<PointToPointNetDevice> src, Time txTime)
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

  // 调度接收事件
  Simulator::ScheduleWithContext(dst->GetNode()->GetId(),
                                delay + txTime, &PointToPointNetDevice::Receive,
                                dst, p->Copy());
  return true;
}

} // namespace ns3 