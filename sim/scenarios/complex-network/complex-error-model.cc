#include "complex-error-model.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE("ComplexErrorModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ComplexErrorModel);

TypeId
ComplexErrorModel::GetTypeId(void)
{
  static TypeId tid = TypeId("ComplexErrorModel")
    .SetParent<ErrorModel> ()
    .AddConstructor<ComplexErrorModel> ()
    .AddAttribute("DropRate", 
                  "随机丢包的概率",
                  DoubleValue(0.0),
                  MakeDoubleAccessor(&ComplexErrorModel::m_dropRate),
                  MakeDoubleChecker<double>(0.0, 1.0))
    .AddAttribute("MaxDropBurst",
                  "最大连续丢包数",
                  UintegerValue(1),
                  MakeUintegerAccessor(&ComplexErrorModel::m_maxDropBurstSize),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("CyclicDropEnabled",
                  "是否启用周期性丢包",
                  BooleanValue(false),
                  MakeBooleanAccessor(&ComplexErrorModel::m_cyclicDropEnabled),
                  MakeBooleanChecker())
    .AddAttribute("CyclicDropPeriod",
                  "周期性丢包的周期",
                  TimeValue(Seconds(10)),
                  MakeTimeAccessor(&ComplexErrorModel::m_cyclicDropPeriod),
                  MakeTimeChecker())
    .AddAttribute("CyclicDropDuration",
                  "每次周期性丢包的持续时间",
                  TimeValue(Seconds(1)),
                  MakeTimeAccessor(&ComplexErrorModel::m_cyclicDropDuration),
                  MakeTimeChecker())
    .AddAttribute("Jitter",
                  "延迟抖动的随机变量",
                  PointerValue(),
                  MakePointerAccessor(&ComplexErrorModel::m_jitter),
                  MakePointerChecker<RandomVariableStream>())
  ;
  return tid;
}

ComplexErrorModel::ComplexErrorModel()
  : ErrorModel(),
    m_dropRate(0.0),
    m_dropBurstSize(0),
    m_maxDropBurstSize(1),
    m_cyclicDropEnabled(false),
    m_cyclicDropPeriod(Seconds(10)),
    m_cyclicDropDuration(Seconds(1)),
    m_nextCyclicDropStart(Seconds(0)),
    m_nextCyclicDropEnd(Seconds(0))
{
  NS_LOG_FUNCTION(this);
  // 初始化丢包随机数生成器
  m_dropRng = CreateObject<UniformRandomVariable>();
  
  // 设置第一次周期性丢包的时间
  if (m_cyclicDropEnabled) {
    m_nextCyclicDropStart = Simulator::Now() + m_cyclicDropPeriod;
    m_nextCyclicDropEnd = m_nextCyclicDropStart + m_cyclicDropDuration;
  }
}

ComplexErrorModel::~ComplexErrorModel()
{
  NS_LOG_FUNCTION(this);
}

void
ComplexErrorModel::SetDropRate(double rate)
{
  NS_LOG_FUNCTION(this << rate);
  m_dropRate = rate;
}

void
ComplexErrorModel::SetMaxDropBurst(uint32_t burst)
{
  NS_LOG_FUNCTION(this << burst);
  m_maxDropBurstSize = burst;
}

void
ComplexErrorModel::SetCyclicDropEnabled(bool enabled)
{
  NS_LOG_FUNCTION(this << enabled);
  m_cyclicDropEnabled = enabled;
  
  // 如果启用，设置第一次周期性丢包的时间
  if (m_cyclicDropEnabled) {
    m_nextCyclicDropStart = Simulator::Now() + m_cyclicDropPeriod;
    m_nextCyclicDropEnd = m_nextCyclicDropStart + m_cyclicDropDuration;
  }
}

void
ComplexErrorModel::SetCyclicDropPeriod(Time period)
{
  NS_LOG_FUNCTION(this << period);
  m_cyclicDropPeriod = period;
}

void
ComplexErrorModel::SetCyclicDropDuration(Time duration)
{
  NS_LOG_FUNCTION(this << duration);
  m_cyclicDropDuration = duration;
}

void
ComplexErrorModel::SetJitter(Ptr<RandomVariableStream> jitter)
{
  NS_LOG_FUNCTION(this << jitter);
  m_jitter = jitter;
}

Time
ComplexErrorModel::GetJitterDelay(void)
{
  // 如果没有设置抖动模型，返回零延迟
  if (!m_jitter) {
    return Seconds(0);
  }
  
  // 生成随机抖动值（毫秒）
  double jitterMs = m_jitter->GetValue();
  return MilliSeconds(jitterMs);
}

bool
ComplexErrorModel::DoCorrupt(Ptr<Packet> p)
{
  NS_LOG_FUNCTION(this << p);
  
  // 处理周期性丢包
  if (m_cyclicDropEnabled) {
    Time now = Simulator::Now();
    
    // 如果现在正处于丢包周期内
    if (now >= m_nextCyclicDropStart && now < m_nextCyclicDropEnd) {
      NS_LOG_INFO("周期性丢包: 在时间 " << now.GetSeconds() << "s");
      return true; // 丢弃数据包
    }
    
    // 如果当前周期已结束，计算下一个周期
    if (now >= m_nextCyclicDropEnd) {
      m_nextCyclicDropStart = m_nextCyclicDropEnd + (m_cyclicDropPeriod - m_cyclicDropDuration);
      m_nextCyclicDropEnd = m_nextCyclicDropStart + m_cyclicDropDuration;
    }
  }
  
  // 处理随机丢包
  if (m_dropRate > 0.0) {
    // 如果当前正在连续丢包中
    if (m_dropBurstSize > 0) {
      // 减少连续丢包计数
      m_dropBurstSize--;
      NS_LOG_INFO("随机丢包: 连续丢包还剩 " << m_dropBurstSize << " 个");
      return true; // 丢弃数据包
    }
    
    // 根据丢包率决定是否丢包
    double rng = m_dropRng->GetValue();
    if (rng < m_dropRate) {
      // 如果最大连续丢包数大于1，可能触发连续丢包
      if (m_maxDropBurstSize > 1) {
        // 随机确定这次连续丢包的数量
        m_dropBurstSize = m_dropRng->GetInteger(0, m_maxDropBurstSize - 1);
        NS_LOG_INFO("随机丢包: 触发连续丢包，将丢弃 " << (m_dropBurstSize + 1) << " 个包");
      }
      return true; // 丢弃数据包
    }
  }
  
  // 不丢弃数据包
  return false;
}

} // namespace ns3 