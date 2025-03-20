#ifndef COMPLEX_ERROR_MODEL_H
#define COMPLEX_ERROR_MODEL_H

#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/nstime.h"

using namespace ns3;

/**
 * 复合错误模型，可以模拟多种网络异常：
 * 1. 随机丢包
 * 2. 随机延迟抖动
 * 3. 周期性带宽变化
 * 4. 周期性丢包（模拟网络拥塞）
 */
class ComplexErrorModel : public ErrorModel {
public:
  static TypeId GetTypeId (void);
  ComplexErrorModel ();
  virtual ~ComplexErrorModel ();

  // 设置随机丢包率
  void SetDropRate (double rate);
  
  // 设置丢包的突发性（连续丢包的最大数量）
  void SetMaxDropBurst (uint32_t burst);
  
  // 设置周期性丢包
  void SetCyclicDropEnabled (bool enabled);
  void SetCyclicDropPeriod (Time period);
  void SetCyclicDropDuration (Time duration);
  
  // 设置延迟抖动
  void SetJitter (Ptr<RandomVariableStream> jitter);
  
  // 获取延迟抖动时间
  Time GetJitterDelay (void);
  
  // 检查是否要丢弃数据包
  virtual bool DoCorrupt (Ptr<Packet> p);
  
private:
  // 随机丢包相关参数
  double m_dropRate;             // 丢包率
  uint32_t m_dropBurstSize;      // 当前连续丢包的数量
  uint32_t m_maxDropBurstSize;   // 最大连续丢包数
  Ptr<UniformRandomVariable> m_dropRng;  // 丢包随机数生成器
  
  // 周期性丢包相关参数
  bool m_cyclicDropEnabled;      // 是否启用周期性丢包
  Time m_cyclicDropPeriod;       // 周期性丢包的周期
  Time m_cyclicDropDuration;     // 每次丢包持续时间
  Time m_nextCyclicDropStart;    // 下一次周期性丢包开始时间
  Time m_nextCyclicDropEnd;      // 下一次周期性丢包结束时间
  
  // 延迟抖动相关参数
  Ptr<RandomVariableStream> m_jitter;  // 抖动随机数生成器
};

#endif // COMPLEX_ERROR_MODEL_H 