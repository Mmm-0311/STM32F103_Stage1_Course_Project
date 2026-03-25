/*
    任务：实现 STM32 基于 HC-05 蓝牙模块完成 WiFi 配网，再通过 ESP-01S 模块接入 WiFi 网络，最终向手机APP反馈联网状态
    1.串口初始化：
        - USART1（PA9/PA10）：调试串口，波特率 115200，用于向 PC 打印解析结果、AT指令交互日志、联网状态等调试信息。
        - USART2（PA2/PA3）：ESP-01S 串口，波特率 115200（ESP-01S 默认波特率），用于向 ESP-01S 发送 AT
   指令、接收模块响应。
        - USART3（PB10/PB11）：蓝牙串口，波特率 9600（HC-05 默认波特率，需与模块实际配置一致），用于与
   HC-05 双向通信（接收配网信息、回复联网状态）。
    2.数据接收：
        - USART2 接收中断：接收 ESP-01S 对 AT 指令的响应数据，以换行/回车作为响应完成标志。
        - USART3 接收中断：接收 HC-05 转发的手机 WiFi 配置字符串（格式为!SSID=PASSWORD!），以末尾的!作为接收完成标志。
    3.数据解析：
        - Parse_WiFi_Info函数：严格按照!SSID=PASSWORD!格式解析蓝牙配网数据，提取 SSID
   和密码，同时校验数据格式（首尾!、等号存在性），格式错误时通过 USART1 打印提示。
    4.ESP-01S 联网：
        - ESP01S_Send_AT_Cmd函数：通用 AT
   指令发送函数，发送指令后等待指定响应（支持超时机制），返回指令执行结果（成功/失败/超时）。
        - ESP01S_Connect_WiFi函数：封装 WiFi 连接流程，依次执行“测试通信（AT）→ 设置 STA 模式（AT+CWMODE=1）→ 连接
   WiFi（AT+CWJAP）”，返回联网状态。
   5.回复消息：
        - Send_Bluetooth_Response函数：根据 ESP-01S 联网结果（成功/失败），拼接 JSON 格式的回复消息（{"status":0/1,
   "wifi_name": "SSID"}），通过 USART3 发送给 HC-05，再转发给手机 APP。
   6.异常处理：
        - 解析层：校验蓝牙配网数据格式，异常时打印错误信息，不执行联网流程。
        - 指令层：AT 指令发送超时/响应失败时，通过 USART1 打印日志，返回失败状态。
        - 缓冲区：所有串口接收缓冲区做溢出保护，避免数组越界。
    7.模块化设计：
        - usart：封装所有串口的初始化、发送、中断处理，与业务逻辑解耦。
        - bluetooth：封装蓝牙数据解析、回复消息拼接，依赖 USART3 外设。
        - esp01s：封装 ESP-01S AT 指令交互、WiFi 连接逻辑，依赖 USART2 外设。
        - main.c：串联核心业务流程（蓝牙接收→解析→ESP联网→蓝牙回复），仅调用各模块接口，无底层硬件操作。
*/
#include <string.h>
#include "Delay.h"
#include "FreeRTOS.h"
#include "bluetooth.h"
#include "esp01s.h"
#include "mytask.h"
#include "queue.h"
#include "stm32f10x.h"
#include "task.h"
#include "usart.h"

#define WIFI_TASK_STACK_DEPTH 64
#define WIFI_TASK_PRIORITY    1
#define ESP_TASK_STACK_DEPTH  64
#define ESP_TASK_PRIORITY     1
#define RPT_TASK_STACK_DEPTH  64
#define RPT_TASK_PRIORITY     1

xQueueHandle queue_wifi_req; //request, 请求
xQueueHandle queue_wifi_rsp; //response，响应

int main(void) {
    // 初始化串口
    USART1_Config(115200); // 调试串口
    USART2_Config(115200); // ESP01S串口
    USART3_Config(9600);   // 蓝牙串口

    // 开启全局中断
    // __enable_irq();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    USART_SendString(USART1, "\r\nSTM32 Bluetooth distribution network+ESP01S networking program has been started\r\n");

    xTaskCreate(Task_Bluetooth, "BT", WIFI_TASK_STACK_DEPTH, NULL, WIFI_TASK_PRIORITY, NULL);
    xTaskCreate(Task_ESP, "ESP", ESP_TASK_STACK_DEPTH, NULL, ESP_TASK_PRIORITY, NULL);
    //xTaskCreate(Task_Report, "RPT", RPT_TASK_STACK_DEPTH, NULL, RPT_TASK_PRIORITY, NULL);
    vTaskStartScheduler();
    //USART_SendString(USART2, "AT+CWJAP?\r\n");

    while (1) {
        //USART_SendString(USART1, "error exit!\n");
    }
}