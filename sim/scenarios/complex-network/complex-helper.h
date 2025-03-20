#ifndef COMPLEX_HELPER_H
#define COMPLEX_HELPER_H

#include "ns3/point-to-point-helper.h"
#include "ns3/random-variable-stream.h"
#include "complex-error-model.h"
#include "complex-channel.h"

namespace ns3 {

class ComplexHelper : public PointToPointHelper {
public:
  ComplexHelper();

  // 设置抖动参数
  void SetJitter(std::string jitterModel, std::string jitterParams);
  
  // 设置随机丢包率
  void SetRandomDropRate(double rate, uint32_t maxBurst = 1);
  
  // 设置周期性丢包
  void SetCyclicDrop(bool enabled, Time period, Time duration);
  
  // 设置带宽变化
  void SetBandwidthVariation(std::string lowBandwidth, std::string highBandwidth,
                             Time lowDuration, Time highDuration);
  
  // 重写Install方法以使用ComplexChannel和ComplexErrorModel
  virtual NetDeviceContainer Install(Ptr<Node> a, Ptr<Node> b);

private:
  // 抖动相关参数
  Ptr<RandomVariableStream> m_jitter;
  
  // 随机丢包相关参数
  double m_dropRate;
  uint32_t m_maxDropBurst;
  
  // 周期性丢包相关参数
  bool m_cyclicDropEnabled;
  Time m_cyclicDropPeriod;
  Time m_cyclicDropDuration;
  
  // 带宽变化相关参数
  bool m_bandwidthVariationEnabled;
  std::string m_lowBandwidth;
  std::string m_highBandwidth;
  Time m_lowDuration;
  Time m_highDuration;
};

} // namespace ns3

#endif // COMPLEX_HELPER_H 