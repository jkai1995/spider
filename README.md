## ESP32控制器

局域网web服务通信\
pd供电时可选择5V/20V工作模式-供不同电机使用\
oled使用u8g2 UI库驱动\
sd卡支持 fat32文件系统


# 硬件框图
单刀双掷开关CH442E ：\
1.选择USB-typeC接入PD诱骗IC或者串口IC\
2.SDIO和BOOT引脚冲突，上电时选择BOOT引脚，启动后切换SDIO

![微信图片_20250330122232](https://github.com/user-attachments/assets/cebb075d-6863-4ae6-b456-c184091fe2c1)

# 软件框图
![image](https://github.com/user-attachments/assets/a4b17c74-3a50-4f4c-9b6f-38a6b64a30a2)

# PCB
![image](https://github.com/user-attachments/assets/83b61acf-f992-4279-8985-dcbdc084062c)

![image](https://github.com/user-attachments/assets/93aa3fe8-3a07-435d-8f37-c2e70d0d0885)
