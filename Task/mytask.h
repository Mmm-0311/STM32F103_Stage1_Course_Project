#ifndef __MYTASK_H__
#define __MYTASK_H__

//接收与解析
void Task_Bluetooth(void);

//连接 WiFi / AT 指令
void Task_ESP(void);

//状态回复 + 可能的日志
void Task_Report(void);

//

#endif /*__TASK_H__*/