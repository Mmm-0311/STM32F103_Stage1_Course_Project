

# STM32F10x PlatformIO 项目模板

本项目是一个基于 PlatformIO 的 STM32F10x 系列微控制器开发模板，提供了完整的标准外设驱动库配置和代码格式化支持。

## 项目特点

- **PlatformIO 构建系统**：跨平台的嵌入式开发工具链
- **STM32F10x 标准外设库**：包含完整的外设驱动源代码
- **Clang-format 代码格式化**：统一的代码风格
- **常用工具函数**：提供精确的延时功能
- **中断处理框架**：预置常见中断服务函数

## 项目结构

```
├── .clang-format          # 代码格式化配置
├── .gitignore             # Git 忽略规则
├── .vscode/               # VS Code 配置
│   ├── extensions.json    # 推荐安装的插件
│   └── settings.json      # 工作区设置
├── include/               # 头文件目录
│   ├── stm32f10x.h        # 设备头文件
│   ├── stm32f10x_conf.h   # 外设配置
│   ├── stm32f10x_it.h     # 中断处理头文件
│   └── system_stm32f10x.h # 系统初始化头文件
├── lib/                   # 第三方库目录
├── src/                   # 源代码目录
│   ├── main.c             # 主程序入口
│   ├── stm32f10x_it.c     # 中断处理实现
│   ├── system_stm32f10x.c # 系统初始化
│   ├── startup_stm32f10x_md.s  # 启动文件
│   ├── utility/           # 工具函数
│   │   ├── Delay.c        # 延时函数实现
│   │   └── Delay.h        # 延时函数声明
│   └── STM32F10x_StdPeriph_Driver/  # 标准外设库
├── test/                  # 测试目录
├── platformio.ini        # PlatformIO 配置文件
└── README.md              # 本说明文档
```

## 快速开始

### 1. 获取项目

```bash
git clone https://gitee.com/hanesheng/stm32f10x_pio_template.git
cd stm32f10x_pio_template
```

### 2. 配置开发环境

#### 安装 VS Code 插件
1. 安装 **PlatformIO IDE** 扩展
2. 安装 **Clang-Format** 扩展（用于代码格式化）

#### 解决库冲突问题

安装 PlatformIO IDE 后，可能会出现库文件冲突。按照以下步骤解决：

1. 打开用户目录下的 PlatformIO 配置文件夹：
   ```
   C:\Users\{用户名}\.platformio
   ```

2. 删除以下路径中的文件（**保留 link 文件夹**）：
   ```
   .platformio\packages\framework-cmsis-stm32f1\Source\Templates\gcc\
   ```

3. 删除以下路径中的所有文件：
   ```
   .platformio\packages\framework-cmsis-stm32f1\Include\
   ```

### 3. 构建项目

1. 使用 VS Code 打开项目
2. 点击底部工具栏的 "Build" 按钮（√ 图标）
3. 等待编译完成

### 4. 烧录固件

1. 连接 STM32 开发板
2. 点击底部工具栏的 "Upload" 按钮（→ 图标）
3. 固件将自动烧录到目标设备

## 代码格式化

本项目已配置 Clang-format 支持。格式化整个项目：

- **Windows/Linux**: `Shift + Alt + F`
- **macOS**: `Shift + Option + F`

或在终端运行：
```bash
clang-format -i src/**/*.c include/**/*.h
```

## 延时函数

项目提供了精确的延时功能：

```c
#include "Delay.h"

void Delay_Us(uint32_t us);  // 微秒延时
void Delay_Ms(uint32_t ms);  // 毫秒延时
void Delay_S(uint32_t s);    // 秒延时
```

## 常用外设驱动

标准外设库包含以下外设驱动：

- **ADC** - 模数转换器
- **GPIO** - 通用输入输出
- **USART** - 串口通信
- **SPI** - 串行外设接口
- **I2C** - 集成电路总线
- **TIM** - 定时器
- **DMA** - 直接内存访问
- **RTC** - 实时时钟
- **PWR** - 电源管理
- **BKP** - 备份寄存器
- **WWDG/IWDG** - 看门狗定时器
- **CAN** - 控制器局域网
- **CRC** - 循环冗余校验
- **DAC** - 数模转换器
- **FSMC** - 静态存储器控制器
- **SDIO** - SD 卡接口

## 使用 CLion 开发

本项目同样支持使用 CLion 进行开发：

1. 安装 CLion
2. 在 CLion 设置中配置 PlatformIO 工具链
3. 打开项目目录即可使用

------

## printf 串口重定向（等效 Keil MicroLIB）

本项目支持在 PlatformIO + GCC 环境下实现等效 Keil "Use MicroLIB" 的轻量级标准库配置，并将 `printf()` 输出重定向到 USART1。

### 1. 启用轻量级标准库（newlib-nano）

在 `platformio.ini` 中添加：

```ini
build_flags =
    -specs=nano.specs
```

如需支持浮点格式输出：

```ini
    -u _printf_float
```

说明：

- `-specs=nano.specs` 使用 newlib-nano，减小 libc 占用空间
- 功能等效于 Keil 勾选 "Use MicroLIB"
- 不依赖 semihosting

------

### 2. 修改 unistd.h库中的系统调用函数_write

```c
#include <unistd.h>

int _write(int fd, const char *ptr, int len)
{
    (void)fd;

    for (int i = 0; i < len; i++)
    {
        while (!(USART1->SR & USART_FLAG_TXE));
        USART1->DR = (uint16_t)ptr[i];
    }

    return len;
}
```

实现原理：

```
printf()
   ↓
vfprintf()
   ↓
_write()
   ↓
USART1->DR
```

通过重写 `_write()`，接管标准输出，将数据逐字节发送至 USART1。



------

### 3. 与 Keil 配置对比

| Keil MDK      | PlatformIO        |
| ------------- | ----------------- |
| Use MicroLIB  | -specs=nano.specs |
| 重写 fputc()  | 重写 _write()     |
| printf → UART | printf → UART     |

至此，项目已支持轻量级标准库 + printf 串口调试输出功能。


## 许可证
