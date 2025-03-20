# 网络抖动 (Jitter) 场景

该场景构建了一个具有延迟抖动特性的网络链路，这种链路可以模拟真实网络中的延迟不稳定性。通过这个场景，您可以测试QUIC实现在不同程度网络抖动下的性能表现。网络抖动对于拥塞控制算法的性能评估尤为重要，因为它可以测试算法对延迟变化的适应能力。

此场景特别适合测试以下QUIC特性：
* 拥塞控制算法对延迟变化的适应性
* 在不稳定网络条件下的吞吐量和延迟表现
* 流量控制机制对网络抖动的响应
* 丢包检测和重传机制在延迟不稳定情况下的表现

该场景具有以下可配置属性：

* `--delay`: 网络的基本单向延迟。需要指定单位。这是必需参数。例如 `--delay=15ms`。

* `--bandwidth`: 链路的带宽。需要指定单位。这是必需参数。例如 `--bandwidth=10Mbps`。指定大于10Mbps的值可能导致模拟器CPU饱和。

* `--queue`: 链路上队列的大小。以数据包为单位。这是必需参数。例如 `--queue=25`。

* `--jitter_model`: 抖动模型，指定用于生成随机抖动值的概率分布。这是必需参数。可选值包括：
  - `ns3::NormalRandomVariable`: 正态分布
  - `ns3::UniformRandomVariable`: 均匀分布
  - `ns3::ExponentialRandomVariable`: 指数分布
  - `ns3::GammaRandomVariable`: 伽马分布

* `--jitter_params`: 抖动模型的参数，格式为 `参数名=值|参数名=值`。根据选择的模型不同，参数也不同：
  - 正态分布：`Mean=均值|Variance=方差`，例如 `Mean=5|Variance=2`
  - 均匀分布：`Min=最小值|Max=最大值`，例如 `Min=1|Max=10`
  - 指数分布：`Mean=均值`，例如 `Mean=5`
  - 伽马分布：`Alpha=α|Beta=β`，例如 `Alpha=2|Beta=3`

示例用法：
```bash
./run.sh "jitter --delay=15ms --bandwidth=10Mbps --queue=25 --jitter_model=ns3::NormalRandomVariable --jitter_params=Mean=5|Variance=2"
```

上述命令将创建一个基本延迟为15ms，带宽为10Mbps，队列大小为25个数据包的网络链路，并添加服从正态分布（均值为5ms，方差为2）的随机抖动。

此场景可以与其他网络异常（如丢包、带宽波动等）组合使用，以创建更复杂的测试环境，从而更全面地评估QUIC实现的性能和鲁棒性。 