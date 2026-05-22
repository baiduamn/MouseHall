# 基于 STM32F1 的手柄鼠标复合 HID 设备

本项目是一款集**高精度鼠标**与**线性手柄油门**功能于一体的 USB 复合输入设备。以 STM32F103C8T6 芯片为核心，通过 SPI 驱动原相 **PAW3395** 旗舰级光学传感器，并利用 **ADC 采集与标定算法**将霍尔传感器输出转化为游戏中的线性油门输入。软件设计上采用**时间片轮询任务调度机制**，在极低资源开销下实现了 **900Hz** 的超高轮询率。

---

##  项目亮点与核心功能

- **旗舰级 PAW3395 SPI 驱动**：编写了针对原相 PAW3395 传感器的完整驱动逻辑，包括上电初始化序列（Power-up sequence）、DPI 动态配置、以及高效的 Motion Burst 数据读取。
- **霍尔传感器线性标定**：利用 ADC1 实时采集霍尔传感器电压，通过软件设立偏置消抖并进行量程重映射，实现 12-bit (0-4095) 级的高精度油门控制输入。
- **自定义复合 USB HID 协议**：通过修改 HID 报文描述符，将鼠标与游戏手柄（Gamepad）两类设备融合于单一 USB 端点中。
  - **Report ID 1 (Mouse)**：上报按键、X/Y 相对位移和滚轮值。
  - **Report ID 2 (Gamepad)**：上报线性油门值（左右扳机键映射）。
- **极简时间片轮询任务调度器**：通过 1ms 定时器中断进行时钟计数，在主循环中实现非阻塞的任务调度，完美承载了 5 个核心周期性任务，避免了 RTOS 的庞大开销。
- **900Hz 轮询回报率**：系统稳定运行，USB 上报延时极低，充分发挥了高回报率电竞级鼠标的响应优势。

---

##  项目关键代码目录

```text
├── Core/               # CubeMX 自动生成的系统初始化代码 (时钟、外设等)
├── Drivers/            # STM32 HAL 固件库及 CMSIS 头文件
├── Middlewares/        # ST 官方 USB Device 协议栈
├── USB_DEVICE/         # USB 设备配置与描述符修改
├── MyCode/             #核心驱动与业务逻辑
│   ├── PAW3395.c/.h    # 原相 PAW3395 底层驱动（SROM 下载、寄存器读写、Motion Burst 运动数据解析）
│   ├── Hall.c/.h       # 霍尔传感器 ADC 读取与线性映射算法
│   ├── myMouse.c/.h    # 自定义复合 HID 报文格式及上报逻辑 (Report ID 1 和 Report ID 2)
│   ├── Key.c/.h        # 按键扫描与消抖逻辑
│   ├── Wheel.c/.h      # 鼠标滚轮逻辑及编码器输入处理
│   └── Delay.c/.h      # 基于系统滴答定时器的高精度延时函数
└── MDK-ARM/            # Keil 工程目录
    └── mouse_paw3395.uvprojx # Keil5 工程入口
```

---



## 主要修改与实现细节

### 1. 复合 HID 设备报文设计 (`myMouse.c`)
我们在软件层面上设计了一个组合上报结构体 `composite_hid_report_t`，通过 `Report ID` 区分鼠标和手柄数据：
- **鼠标模式 (Report ID = 1)**：
  通过 SPI 读取 PAW3395 寄存器数据并解析出 `X_Speed` 和 `Y_Speed` 的 16 位运动向量，连同按键和滚轮值打包发送，数据包大小为 7 字节。
- **手柄模式 (Report ID = 2)**：
  将霍尔传感器读取到的 ADC 信号映射到左/右扳机值，数据包大小为 4 字节。

### 2. 霍尔传感器标定算法 (`Hall.c`)
由于硬件组装存在物理误差，设计了基于基准偏置 `ZERO_OFFSET` 的校准逻辑：
```c
// 零点偏置电压对应 ADC 值 (2175)
const uint16_t ZERO_OFFSET = 2175U;

uint16_t Hall_Output(void) {
    uint16_t ADValue = My_ADC_Read();
    if (ADValue <= ZERO_OFFSET) {
        return 0; // 零区过滤，保证零点稳定
    } else {
        uint16_t diff = ADValue - ZERO_OFFSET;
        uint32_t Output = (diff * 4095U) / 25U; // 线性增益放大映射至 0-4095
        if (Output > 4095U) Output = 4095U;    // 限幅滤波
        return (uint16_t)Output;
    }
}
```

### 3. 时间片轮询任务调度器 (`main.c`)
为了实现高频轮询又不影响系统稳定性，设计了结构体数组调度模型。通过 1ms 的 TIM3 中断回调函数对任务周期进行递减，在主循环中完成任务调度：
```c
typedef struct {
    void (*pTask)(void);    // 任务函数指针
    uint16_t TaskPeriod;    // 任务调度周期 (ms)
} TaskStruct;

TaskStruct Task[] = {
    {Key_Task, 20},            // 按键扫描任务 (20ms 周期)
    {Mouse_XY_Updata, 1},      // 鼠标位移更新 (1ms 周期, 900Hz 左右回报率)
    {Mouse_wheel_Updata, 5},   // 滚轮扫描任务 (5ms 周期)
    {trigger, 1},              // 霍尔传感器 ADC 采集 (1ms 周期)
    {LED_Task, 100}            // 状态指示灯闪烁 (100ms 周期)
};
```



## 资源占用 

| 资源类别        | 字节数 (Bytes) | 占用大小 (KB) | 芯片总体占比 (STM32F103C8T6) |
| :-------------- | :------------: | :-----------: | :--------------------------: |
| **Flash (ROM)** |  18,960 Bytes  | **18.52 KB**  |     **28.9%** (总 64 KB)     |
| **SRAM (RAM)**  |  3,776 Bytes   |  **3.69 KB**  |     **18.5%** (总 20 KB)     |



