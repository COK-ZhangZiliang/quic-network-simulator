# 复合网络 (Complex Network) 场景

该场景构建了一个具有多种网络异常特性的复合网络环境，可以同时模拟网络抖动、带宽变化、随机丢包和周期性丢包等多种网络异常情况。通过这个场景，您可以测试QUIC实现在复杂多变的网络条件下的性能表现，这对于评估QUIC协议在真实互联网环境中的鲁棒性尤为重要。

此场景特别适合测试以下QUIC特性：
* 在复杂网络条件下拥塞控制算法的综合表现
* 多种网络异常同时存在时的错误恢复能力
* 在不可预测的网络环境中维持连接的能力
* 在极端网络条件下的性能降级策略

该场景具有以下可配置属性，您可以根据需要选择性地启用或禁用某些特性：

## 基本网络参数

* `--delay`: 网络的基本单向延迟。需要指定单位。这是必需参数。例如 `--delay=15ms`。

* `--bandwidth`: 链路的基本带宽。需要指定单位。这是必需参数。例如 `--bandwidth=10Mbps`。

* `--queue`: 链路上队列的大小。以数据包为单位。这是必需参数。例如 `--queue=25`。

## 抖动相关参数

* `--jitter_model`: 抖动模型，指定用于生成随机抖动值的概率分布。例如 `--jitter_model=ns3::NormalRandomVariable`。

* `--jitter_params`: 抖动模型的参数，格式为 `参数名=值|参数名=值`。例如 `--jitter_params=Mean=5|Variance=2`。

## 随机丢包相关参数

* `--drop_rate`: 随机丢包率，取值范围0.0-1.0。例如 `--drop_rate=0.01` 表示1%的丢包率。

* `--max_burst`: 最大连续丢包数。例如 `--max_burst=3` 表示最多连续丢弃3个数据包。

## 周期性丢包相关参数

* `--cyclic_drop`: 是否启用周期性丢包，0表示禁用，1表示启用。例如 `--cyclic_drop=1`。

* `--cyclic_period`: 周期性丢包的周期。需要指定单位。例如 `--cyclic_period=30s`。

* `--cyclic_duration`: 每次丢包持续的时间。需要指定单位。例如 `--cyclic_duration=2s`。

## 带宽变化相关参数

* `--bandwidth_variation`: 是否启用带宽变化，0表示禁用，1表示启用。例如 `--bandwidth_variation=1`。

* `--low_bandwidth`: 低带宽阶段的带宽值。需要指定单位。例如 `--low_bandwidth=1Mbps`。

* `--high_bandwidth`: 高带宽阶段的带宽值。需要指定单位。例如 `--high_bandwidth=10Mbps`。

* `--low_duration`: 低带宽持续的时间。需要指定单位。例如 `--low_duration=5s`。

* `--high_duration`: 高带宽持续的时间。需要指定单位。例如 `--high_duration=10s`。

## 示例用法

以下示例展示了如何创建一个具有多种网络异常特性的复合网络环境：

```bash
./run.sh "complex-network --delay=15ms --bandwidth=10Mbps --queue=25 --jitter_model=ns3::NormalRandomVariable --jitter_params=Mean=5|Variance=2 --drop_rate=0.01 --max_burst=3 --cyclic_drop=1 --cyclic_period=30s --cyclic_duration=2s --bandwidth_variation=1 --low_bandwidth=1Mbps --high_bandwidth=10Mbps --low_duration=5s --high_duration=10s"
```

上述命令将创建一个具有以下特性的复合网络环境：
- 基本延迟为15ms，基本带宽为10Mbps，队列大小为25个数据包
- 添加服从正态分布的随机抖动（均值为5ms，方差为2）
- 1%的随机丢包率，最多连续丢弃3个包
- 每30秒触发一次持续2秒的周期性丢包
- 带宽在10Mbps和1Mbps之间周期性变化，高带宽持续10秒，低带宽持续5秒

此场景可以帮助您全面评估QUIC实现在复杂多变的网络环境中的性能和鲁棒性，特别适合用于模拟极端网络条件下的性能测试和压力测试。 