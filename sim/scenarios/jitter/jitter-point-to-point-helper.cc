#include "jitter-point-to-point-helper.h"
#include "jitter-channel.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/names.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("JitterPointToPointHelper");

namespace ns3 {

JitterPointToPointHelper::JitterPointToPointHelper() 
  : PointToPointHelper()
{
  NS_LOG_FUNCTION(this);
  m_jitter = 0;
}

void
JitterPointToPointHelper::SetJitter(std::string jitterModel, std::string jitterParams)
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

NetDeviceContainer
JitterPointToPointHelper::Install(Ptr<Node> a, Ptr<Node> b)
{
  NS_LOG_FUNCTION(this << a << b);
  
  NetDeviceContainer devices;
  Ptr<PointToPointNetDevice> devA = m_deviceFactory.Create<PointToPointNetDevice>();
  Ptr<PointToPointNetDevice> devB = m_deviceFactory.Create<PointToPointNetDevice>();
  devA->SetAddress(Mac48Address::Allocate());
  devB->SetAddress(Mac48Address::Allocate());
  a->AddDevice(devA);
  b->AddDevice(devB);
  
  // 创建JitterChannel而不是标准的PointToPointChannel
  Ptr<JitterChannel> channel = CreateObject<JitterChannel>();
  devA->Attach(channel);
  devB->Attach(channel);
  
  // 设置通道的基本属性（从PointToPointHelper复制）
  Ptr<PointToPointNetDevice> ttdevA = devA;
  Ptr<PointToPointNetDevice> ttdevB = devB;
  
  // 设置通道的延迟属性
  channel->SetAttribute("Delay", m_channelFactory.GetDelay());
  
  // 设置抖动
  if (m_jitter) {
    channel->SetJitterDistribution(m_jitter);
  }
  
  devices.Add(devA);
  devices.Add(devB);
  
  return devices;
}

} // namespace ns3 