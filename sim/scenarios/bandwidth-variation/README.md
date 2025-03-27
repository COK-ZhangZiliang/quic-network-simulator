# 带宽变化 (Bandwidth Variation) 场景

该场景构建了一个带宽动态变化的网络链路，可以模拟真实网络中的带宽波动情况。通过这个场景，您可以测试QUIC实现在不同带宽变化模式下的性能表现，特别是拥塞控制算法对带宽变化的感知和适应能力。

此场景特别适合测试以下QUIC特性：
* 拥塞控制算法对带宽变化的适应性
* 在带宽波动条件下的吞吐量利用率
* BBR等基于带宽探测的拥塞控制算法的表现
* 在带宽突变场景下的缓冲区管理和流量控制

## 带宽变化模式

该场景支持三种带宽变化模式：

### 1. 阶梯变化模式 (step)
在高低带宽之间进行阶梯式切换。

必需参数：
* `--delay`: 网络的基本单向延迟。需要指定单位。例如 `--delay=15ms`
* `--high_bandwidth`: 高带宽阶段的带宽值。需要指定单位。例如 `--high_bandwidth=10Mbps`
* `--low_bandwidth`: 低带宽阶段的带宽值。需要指定单位。例如 `--low_bandwidth=1Mbps`
* `--queue`: 链路上队列的大小。以数据包为单位。例如 `--queue=25`
* `--duration_high`: 高带宽持续的时间。需要指定单位。例如 `--duration_high=10s`
* `--duration_low`: 低带宽持续的时间。需要指定单位。例如 `--duration_low=5s`

示例：
```bash
./run.sh "bandwidth-variation --mode=step --delay=15ms --high_bandwidth=10Mbps --low_bandwidth=1Mbps --queue=25 --duration_high=10s --duration_low=5s"
```

### 2. 线性变化模式 (linear)
带宽在高带宽和低带宽之间线性变化。

必需参数：
* `--delay`: 网络的基本单向延迟
* `--high_bandwidth`: 高带宽值
* `--low_bandwidth`: 低带宽值
* `--queue`: 队列大小
* `--bandwidth_change_rate`: 带宽变化速率（Mbps/s）

示例：
```bash
./run.sh "bandwidth-variation --mode=linear --delay=15ms --high_bandwidth=10Mbps --low_bandwidth=2Mbps --queue=25 --bandwidth_change_rate=0.5"
```

### 3. 周期变化模式 (periodic)
带宽按照正弦函数在高低带宽之间周期性变化。

必需参数：
* `--delay`: 网络的基本单向延迟
* `--queue`: 队列大小
* `--period`: 变化周期（秒）
* `--amplitude`: 变化振幅（Mbps）
* `--mean_bandwidth`: 平均带宽（Mbps）

示例：
```bash
./run.sh "bandwidth-variation --mode=periodic --delay=15ms --queue=25 --period=10 --amplitude=4 --mean_bandwidth=6"
```

## 注意事项

1. 所有模式都需要指定`delay`和`queue`参数
2. 阶梯变化和线性变化模式需要指定`high_bandwidth`和`low_bandwidth`
3. 周期变化模式下，振幅不能大于平均带宽
4. 带宽变化间隔为0.1秒

## 应用场景

此场景特别适合测试以下场景：
1. 移动网络环境下的QUIC性能
2. 网络拥塞周期性变化的环境
3. 不同网络覆盖区域切换的情况
4. 网络流量高峰期与低谷期交替出现的情况

通过这个场景，您可以全面评估QUIC实现在各种带宽变化条件下的性能表现，特别是其拥塞控制算法和流量控制机制的有效性。 