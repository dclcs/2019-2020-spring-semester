# OS Review

## Network

### 网络驱动模型

应用层 / 传输层 / 网络层 / 数据链路层 / 物理层。

分别对应「网络请求进程 / TCP、UDP 协议、端口 / IP 协议、主机地址 / 网络设备驱动 / 网络设备」。

Linux 里的函数分别对应了「高级网络函数 / `dev_queue_xmit()` 和 `netif_rx()` / `struct net_device` / `ndo_start_xmit()`、RX 中断处理 / 网络硬件设备」。

### 中断合并

网卡收到数据包的频率是非常高的。因此网卡内部含有缓冲区；在累积到一定数量後，再打包发给 CPU。

吞吐量更高了，只是延迟会比较高。

### Linux 吃包过程

* 网卡收到数据包
* DMA 将数据帧传送到内核内存中的 `rx_ring` 中
* 网卡触发一个硬件中断
* CPU 收到中断，先进行 Top Half 操作
  * 先分配好 `sk_buff` 数据结构，管理 `rx_ring` 中的数据包
    * skb 本身不包含数据，只是指向 `rx_ring` 内的数据而已
  * 再将处理请求入队
* 之後是推迟过後的 Bottom Half
  * 根据报文类型递交给对应协议进行处理
  * 调用 handler 处理 skb 包
* 进入 IP 层，调用 `ip_rcv()` 函数
  * 调用 `ip_router_input()`，进行路由处理
* 进入传输层，`tcp_v4_rcv()`，检查 TCP Header
  * 调用 `__tcp_v4_lookup()` 查找数据包对应的 Open Socket
  * 调用 `tcp_prequeue()` 把 TCP Payload 丢到用户空间，放入 Socket 接收队列
* 进入应用层，`socket` 被唤醒之後通过系统调用，通过 `tcp_recvmsg()` 从 Socket 接受队列中获取数据
  * 用户态调用 `read` 或 `recvfrom`，拿到数据

