#ifndef JITTER_POINT_TO_POINT_HELPER_H
#define JITTER_POINT_TO_POINT_HELPER_H

#include "ns3/point-to-point-helper.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

class JitterPointToPointHelper : public PointToPointHelper {
public:
  JitterPointToPointHelper();

  // 设置抖动参数
  void SetJitter(std::string jitterModel, std::string jitterParams);

  // 重写Install方法以使用JitterChannel
  virtual NetDeviceContainer Install(Ptr<Node> a, Ptr<Node> b);

private:
  Ptr<RandomVariableStream> m_jitter;
};

} // namespace ns3

#endif // JITTER_POINT_TO_POINT_HELPER_H 