Interface
=========

Interface即网络设备，拥有1个或多个协议地址，例如以太网卡同时有 **AF_INET** 和 **AF_INET6** 协议地址。

Interface拥有非常多的信息，以下分5个部分介绍。

Interface信息
-------------

.. list-table:: 实现信息
   :widths: 25 25 50
   :header-rows: 1

   * - 类型
     - 变量名
     - 注释
   * - char*
     - name
     - 网络接口名，例如lo、eth1
   * - int 
     - pcount
     - BPF listener数量
   * - int 
     - pcount
     - BPF listener数量
   * - 
     - BPF
     - 分组过滤器
   * - 
     - flag
     - | IFF_BROADCAST 接口用于广播网
       | IFF_LOOPBACK 环回网络
       | IFF_POINTOPOINT 点对点网络


.. list-table:: 硬件特性
   :widths: 25 25 50
   :header-rows: 1

   * - 类型
     - 变量名
     - 注释
   * - char
     - type
     - 接口类型，IFT_ETHER、IFT_LOOP
   * - char 
     - addlen
     - 接口链路层地址长度，例如以太网地址长度为6个字节
   * - char
     - hdrlen
     - 接口链路层首部长度，例如以太网首部长度为14个字节
   * - 
     - mtu
     - maximum transmission unit，即接口一次send最大长度
   * - 
     - metric
     - - 接口的统计信息
       | 接收包数
       | 发送包数
       | 接收字节数
       | 发送字节数
       | 接收丢弃包数
       | 发送丢弃包数
       | ...


.. list-table:: Interface函数指针
   :widths: 50 50
   :header-rows: 1

   * - 函数名
     - 注释
   * - if_init
     - interface初始化函数
   * - if_output
     - 输出分组排队
   * - if_start
     - 启动分组传输
   * - if_ioctl
     - 控制命令

.. list-table:: interface发送queue
   :widths: 25 25 50
   :header-rows: 1

   * - 类型
     - 变量名
     - 注释
   * - mbuf*
     - head
     - 分组数据头
   * - mbuf*
     - tail
     - 分组数据尾
   * - 
     - len
     - 当前长度
   * - 
     - maxlen
     - 最大长度