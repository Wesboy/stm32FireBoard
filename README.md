
# 介紹
這是基於野火指南者103開發板的一個開源項目，目的增加對Cortex-m3和STM32CubeX工具的學習了解
並增加編程能力。
需要開發上位機服務器，及多個終端app
需求：
實現上位機控制及監控板子狀態
# 功能
## 板級功能
- 全彩RGB LED燈
- 蜂鳴器
- 串口通信
- WIFI接入網絡
- ISP一鍵下載
- Flash
- EEPROM
- 支持32G TF卡(需外接)
- 攝像頭OV7725(需外接)
- NRF24L01(需外接)
- 可以做USB從機
## 控制單元
# 協議
## 協議描述
描述|協議
:--|:--
協議頭|0x55 
協議頭|0xaa
長度(包括校驗位)|len
數據| xx
...| xx
校驗位| check
整體數據幀| 0x55+0xaa+len+data+...+check
## LED控制
功能|描述|協議
:--|:--|:--
Red ON|紅燈亮|0x55 0xaa 0x03 0xB1 0x01 check
Red OFF|紅燈滅|0x55 0xaa 0x03 0xB1 0x00 check
Green ON|綠燈亮|0x55 0xaa 0x03 0xB2 0x01 check
Green OFF|綠燈滅|0x55 0xaa 0x03 0xB2 0x00 check
Blue ON|紅燈亮|0x55 0xaa 0x03 0xB3 0x01 check
Blue OFF|紅燈滅|0x55 0xaa 0x03 0xB3 0x00 check
Red PWM|紅燈PWM值(0-1024)|0x55 0xaa 0x04 0xB4 <font color=red>0x00 0x00</font>  check
Green PWM|綠燈PWM值(0-1024)|0x55 0xaa 0x04 0xB5 <font color=red>0x00 0x00</font>  check
blue PWM|藍燈PWM值(0-1024)|0x55 0xaa 0x04 0xB6 <font color=red>0x00 0x00</font>  check
LED flash frq|LED閃爍頻率(0-255)|0x55 0xaa  0x04 0xB7 <font color=red>0x00</font> <font color=green>0x00</font> <font color=blue>0x00</font> check

## LED狀態
功能|描述|協議
:--|:--|:--
Status Red ON|紅燈狀態亮|0x55 0xaa 0x03 0xC1 0x01  check
Status Red OFF|紅燈狀態滅|0x55 0xaa 0x03 0xC1 0x00  check
Status Green ON|綠燈狀態亮|0x55 0xaa 0x03 0xC2 0x01 check
Status Green OFF|綠燈狀態滅|0x55 0xaa 0x03 0xC2 0x00  check
Status Blue ON|紅燈狀態亮|0x55 0xaa 0xC3 0x03 0x01  check
Status Blue OFF|紅燈狀態滅|0x55 0xaa 0xC3 0x03 0x00  check
Status Red PWM|紅燈PWM值(0-1024)|0x55 0xaa 0x04 0xC4 <font color=red>0x00 0x00</font>  check
Status Green PWM|綠燈PWM值(0-1024)|0x55 0xaa 0x04 0xC5 <font color=red>0x00 0x00</font>  check
Status blue PWM|藍燈PWM值(0-1024)|0x55 0xaa 0x04 0xC6 <font color=red>0x00 0x00</font>  check
Status LED flash frq|LED閃爍頻率(0-255)|0x55 0xaa 0x04 0xC7 <font color=red>0x00</font> <font color=green>0x00</font> <font color=blue>0x00</font>  check


# 參考資料
- stm32F10x-reference manual
- Cortex-M3 權威指南