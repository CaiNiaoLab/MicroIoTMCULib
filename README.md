# MicroIoTMCULib

- [MicroIoT](#microiot)
  * [一、MicroIoT的配置](#--microiot---)
  * [二、用法](#----)



------

# MicroIoT

------

EngLish: CaiNiaoLab micro-IoT platform open source project, device-side code.中文：菜鸟实验室微型物联网平台开源项目，设备端代码。

------



## 一、MQTT的配置

**主题**：/cc3200/ToggleLEDCmdL1、/cc3200/ToggleLEDCmdL2、/cc3200/ToggleLEDCmdL3以分别控制LED9、GPIO12(PIN_03管脚)、GPIO04(PIN_59管脚)。按CC3200 LaunchPad上的SW2或SW3开关在/cc3200/ButtonPressEvtSw2或上发布消息/cc3200/ButtonPressEvtSw3，/cc3200/Uart1转发uart1串口数据（建议10s发一次，时间不能低于10s），/cc3200/TMP006读取TMP006传感器温度数据发送（60s发送一次）。

**心跳时间**：修改心跳包500s发送一次，保证连接时长为：2 Hour。

**ClientID**（usr_connect_config中ClientID 宏定义）：以避免干扰其他用户。在此示例中，这在struct中定义usr_connect_config为"WOOZhao-Dev1"。CC3200 MQTT库具有同时连接最多四个代理的功能。可以将新连接的配置作为新元素（在下一个索引上）添加到connect_config中。==注意==：客户端ID对于每个客户端都应该是唯一的。我们建议自行更改CC3200设备的客户端。

## 二、用法

1. 本示例可以使用FreeRTOS。请用USB-TTL连接PIN_55（UART0_TX）和PIN_57（UART0_RX）到PC。在主机PC上，打开带有以下参数的串行终端（[Tera Term](https://ttssh2.osdn.jp/index.html.en)）：
   	 

   |   端口   | 枚举的COM端口 |
   | :------: | :-----------: |
   |  波特率  |    500000     |
   |   数据   |      8位      |
   | 奇偶校验 |      无       |
   |   停止   |      1位      |
   | 流量控制 |      无       |

    

2. 运行参考应用程序。在CCS / IAR中打开项目。生成应用程序并进行调试以加载到设备，或者使用UniFlash刷新二进制文件。

3. 此示例要求AP具有Internet连接。如果与默认AP的连接失败，则用户将在串行终端上看到提示。
   ~ 绿色LED指示灯表示网络处理器已成功打开，只要未与AP建立连接，红色LED指示灯就会连续闪烁。
   ~ 建立后，红色LED会持续点亮一秒钟，然后所有LED都将关闭。

4. 在主机PC或移动设备上，打开MQTT客户端应用程序。连接到我们用于此示例的[实验室网站](www.probielab.com)。

5. 订阅主题/cc3200/ButtonPressEvtSw3和/cc3200/ButtonPressEvtSw2。

6. 在/cc3200/ToggleLEDCmdL1以分别控制LED9。

7. 按CC3200 LaunchPad上的SW2或SW3开关分别在/cc3200/ButtonPressEvtSw2或上发布消息/cc3200/ButtonPressEvtSw3。

8. 连接UART1，分别是管脚P58（TX）和P45（RX）、和GND，连接串口助手，基本设置如下：波特率：500000、数据： 8位、奇偶校验：无、停止： 1位、流量控制：无。设置每隔10s发送一次数据，SIZE>=512字节，格式为"xxxx"\n，订阅主题/cc3200/Uart1，可以收到发送数据。