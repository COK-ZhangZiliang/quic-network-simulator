#include "complex-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("ComplexHelper");

namespace ns3 {

ComplexHelper::ComplexHelper() 
  : PointToPointHelper(),
    m_dropRate(0.0),
    m_maxDropBurst(1),
    m_cyclicDropEnabled(false),
    m_cyclicDropPeriod(Seconds(10)),
    m_cyclicDropDuration(Seconds(1)),
    m_bandwidthVariationEnabled(false),
    m_lowBandwidth("1Mbps"),
    m_highBandwidth("10Mbps"),
    m_lowDuration(Seconds(5)),
    m_highDuration(Seconds(10))
{
  NS_LOG_FUNCTION(this);
}

void
ComplexHelper::SetJitter(std::string jitterModel, std::string jitterParams)
{
  NS_LOG_FUNCTION(this << jitterModel << jitterParams);
  
  // 创建随机变量
  ObjectFactory factory;
  factory.SetTypeId(jitterModel);
  
  // 解析参数，格式为"参数1=值1|参数2=值2|..."
  if (jitterParams.length() > 0) {
    std::string::size_type start = 0;
    std::string::size_type delimiter;
    
    while ((delimiter = jitterParams.find('|', start)) != std::string::npos) {
      std::string param = jitterParams.substr(start, delimiter - start);
      std::string::size_type equal = param.find('=');
      
      if (equal != std::string::npos) {
        std::string paramName = param.substr(0, equal);
        std::string paramValue = param.substr(equal + 1);
        factory.Set(paramName, StringValue(paramValue));
      }
      
      start = delimiter + 1;
    }
    
    // 处理最后一个参数
    std::string param = jitterParams.substr(start);
    std::string::size_type equal = param.find('=');
    
    if (equal != std::string::npos) {
      std::string paramName = param.substr(0, equal);
      std::string paramValue = param.substr(equal + 1);
      factory.Set(paramName, StringValue(paramValue));
    }
  }
  
  // 创建随机变量
  m_jitter = factory.Create<RandomVariableStream>();
}

void
ComplexHelper::SetRandomDropRate(double rate, uint32_t maxBurst)
{
  NS_LOG_FUNCTION(this << rate << maxBurst);
  m_dropRate = rate;
  m_maxDropBurst = maxBurst;
}

void
ComplexHelper::SetCyclicDrop(bool enabled, Time period, Time duration)
{
  NS_LOG_FUNCTION(this << enabled << period << duration);
  m_cyclicDropEnabled = enabled;
  m_cyclicDropPeriod = period;
  m_cyclicDropDuration = duration;
}

void
ComplexHelper::SetBandwidthVariation(std::string lowBandwidth, std::string highBandwidth,
                                    Time lowDuration, Time highDuration)
{
  NS_LOG_FUNCTION(this << lowBandwidth << highBandwidth << lowDuration << highDuration);
  m_bandwidthVariationEnabled = true;
  m_lowBandwidth = lowBandwidth;
  m_highBandwidth = highBandwidth;
  m_lowDuration = lowDuration;
  m_highDuration = highDuration;
}

NetDeviceContainer
ComplexHelper::Install(Ptr<Node> a, Ptr<Node> b)
{
  NS_LOG_FUNCTION(this << a << b);
  
  NetDeviceContainer devices;
  Ptr<PointToPointNetDevice> devA = m_deviceFactory.Create<PointToPointNetDevice>();
  Ptr<PointToPointNetDevice> devB = m_deviceFactory.Create<PointToPointNetDevice>();
  devA->SetAddress(Mac48Address::Allocate());
  devB->SetAddress(Mac48Address::Allocate());
  a->AddDevice(devA);
  b->AddDevice(devB);
  
  // 创建复合错误模型（处理丢包）
  Ptr<ComplexErrorModel> errorA = CreateObject<ComplexErrorModel>();
  Ptr<ComplexErrorModel> errorB = CreateObject<ComplexErrorModel>();
  
  // 设置随机丢包
  if (m_dropRate > 0.0) {
    errorA->SetDropRate(m_dropRate);
    errorA->SetMaxDropBurst(m_maxDropBurst);
    errorB->SetDropRate(m_dropRate);
    errorB->SetMaxDropBurst(m_maxDropBurst);
  }
  
  // 设置周期性丢包
  if (m_cyclicDropEnabled) {
    errorA->SetCyclicDropEnabled(true);
    errorA->SetCyclicDropPeriod(m_cyclicDropPeriod);
    errorA->SetCyclicDropDuration(m_cyclicDropDuration);
    errorB->SetCyclicDropEnabled(true);
    errorB->SetCyclicDropPeriod(m_cyclicDropPeriod);
    errorB->SetCyclicDropDuration(m_cyclicDropDuration);
  }
  
  // 创建复合通道（处理延迟和带宽）
  Ptr<ComplexChannel> channel = CreateObject<ComplexChannel>();
  
  // 设置基本延迟
  channel->SetAttribute("Delay", StringValue(m_channelFactory.GetDelay().GetTimeStep().GetSeconds() + "s"));
  
  // 设置抖动
  if (m_jitter) {
    channel->SetJitterModel(m_jitter);
    errorA->SetJitter(m_jitter);
    errorB->SetJitter(m_jitter);
  }
  
  // 设置带宽变化
  if (m_bandwidthVariationEnabled) {
    DataRate lowRate(m_lowBandwidth);
    DataRate highRate(m_highBandwidth);
    channel->SetBandwidthVariation(lowRate, highRate, m_lowDuration, m_highDuration);
  }
  
  // 将设备连接到通道
  devA->Attach(channel);
  devB->Attach(channel);
  
  // 设置错误模型
  devA->SetReceiveErrorModel(errorB);
  devB->SetReceiveErrorModel(errorA);
  
  devices.Add(devA);
  devices.Add(devB);
  
  return devices;
}

} // namespace ns3 