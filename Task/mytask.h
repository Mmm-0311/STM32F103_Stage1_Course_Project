#ifndef __MYTASK_H__
#define __MYTASK_H__

//接收与解析
void Task_Bluetooth(void* pvParameters);

//连接 WiFi / AT 指令
void Task_ESP(void* pvParameters);

//状态回复 + 可能的日志
void Task_Report(void* pvParameters);

//

#endif /*__TASK_H__*/