# 硬件MCU调试流程 
本文介绍硬件MCU调试流程，主要帮助传统硬件厂商快速，简单实现智能化。主要涉及到三端：硬件MCU、WiFi模块、测试APP。硬件MCU端和WiFi模块端之间通过串口通信。WiFi模块端和测试APP端之间数据已打通。
 
## 硬件MCU

按照[串口协议模板](assist.dtston.com/sdk/Embedded/D+mcu_1.7.doc )，结合产品功能，完善该产品串口协议，再根据串口协议做相应的MCU程序开发。

## WiFi模块

硬件厂商选择适配的WiFi/GPRS模块，烧录相应的[模板固件](assist.dtston.com/sdk/Embedded/D+mcu_1.7.doc )。待硬件MCU软件修改完后，将WiFi模块和硬件MCU通过串口（**TX,RX,GND**）连接上即可。

## 测试APP 

用连接好的硬件和 [测试APP](assist.dtston.com/sdk/android/sdk_demo_1.3.2.apk )进行联调。

1.接好硬件，触发硬件MCU发送`WiFi配对`串口数据
<br /> ![Alt text](/assets/imgs/1498554513567.png)
	
2.打开测试APP，注册过云平台的用户请输入`账号`和`密码`后点击`登录`。未注册用户点击`注册新用户`，按流程注册即可。
<br /> ![Alt text](/assets/imgs/1498555135660.png)
	
3.登入后点击`添加`。
`本地配网添加`：把未连接路由器的WiFi设备配对到路由器上。
`本地搜索添加`：可直接搜索到连接到同一路由器下的WiFi设备，无需再次操作配对流程。（本操作适用于已连接上路由器的WiFi设备）
<br /> ![Alt text](/assets/imgs/1498555188519.png)
	
4.点击本地配网添加后，选择正确的`WiFi模块`，输入正确的`路由器密码`、`产品类型`、`产品名称`。**一定要等WiFi设备进入了配对状态后**，再点击`下一步`。
<br /> ![Alt text](/assets/imgs/1498555232140.png)
	
5.配对成功后，设备列表界面显示了一个智能插座。
<br /> ![Alt text](/assets/imgs/1498555270798.png)
	
6.点击`智能插座`后，进入到设备控制界面。输入正确的`指令功能`和`指令内容`，点击`发送`，硬件即可收到指令数据。APP收到的数据会显示在APP下方。
<br /> ![Alt text](/assets/imgs/1498555297335.png)
	

