# booboox-xiaoche

基于 `xiaozhi-esp32` 改造的桌面小车机器人项目。

这个版本在小智语音交互能力的基础上，增加了双电机驱动控制，用于实现前进、后退、转向、跳舞等动作。当前仓库已经接入自定义板型 `booboox-xiaoche`，可以直接在 `ESP-IDF` 的 `menuconfig` 里选择并编译。

## 1. 功能简介

- 语音唤醒与语音对话
- 通过表情或动作指令驱动电机运动
- 支持 OLED 显示
- 支持独立待机页：全屏大号时间，下一行显示日期和星期
- 唤醒后自动切换到眼睛动画界面
- 支持 `TTP223` 触摸模块，轻触后触发电机震动反馈
- 支持本地编译、烧录和串口监视
- 基于 `ESP32-S3-DevKitC-1` 的 DIY 面包板方案

## 2. 硬件清单

- 开发板：`ESP32-S3-DevKitC-1`（推荐 `WROOM N16R8`）
- 数字麦克风：`INMP441` / `ICS43434`
- 功放：`MAX98357A`
- 喇叭：`8Ω 2~3W` 或 `4Ω 2~3W`
- 屏幕：`SSD1306 128x64` 或 `SSD1306 128x32`
- 电机驱动：迷你 `L298N` / `TC1508` 类双路驱动模块
- 电机：N20 减速电机或其他双轮电机
- 触摸模块：`TTP223`（可选）
- 面包板、杜邦线、Type-C 数据线等常规配件

## 3. 关键引脚

### 3.1 电机控制引脚

- `GPIO12`：左轮前进
- `GPIO13`：左轮后退
- `GPIO14`：右轮前进
- `GPIO21`：右轮后退

### 3.2 OLED 引脚

- `GPIO41`：SDA
- `GPIO42`：SCL

### 3.3 TTP223 触摸模块引脚

- `GPIO2`：TTP223 输出引脚 `OUT`
- `3.3V`：TTP223 `VCC`
- `GND`：TTP223 `GND`

轻触一次 `TTP223`，机器人会执行一小段前后抖动序列，用电机动作模拟“震动”反馈。

### 3.4 音频引脚

- 麦克风 WS：`GPIO4`
- 麦克风 SCK：`GPIO5`
- 麦克风 DIN：`GPIO6`
- 喇叭 DOUT：`GPIO7`
- 喇叭 BCLK：`GPIO15`
- 喇叭 LRCK：`GPIO16`

对应定义见 [config.h](main/boards/booboox-xiaoche/config.h)。

## 4. 显示逻辑

### 4.1 待机页

设备初始化完成并进入待机后，屏幕会显示独立待机页：

- 中间显示大号时间
- 下一行显示日期和星期
- 时间中的 `:` 会每秒闪烁一次

如果设备尚未联网校时，待机页会先显示：

- `--:--`
- `等待校时`

### 4.2 对话页

当设备被唤醒时：

- 语音唤醒词触发后进入对话
- 或者启动完成后按下 `BOOT` 按钮进入对话

屏幕会从待机页切换为眼睛动画界面：

- `Listening` 状态显示 listening 眼睛
- `Speaking` 状态显示 happy 眼睛
- 对话结束后返回待机时钟页

## 5. 编译方法

### 5.1 进入 ESP-IDF 环境

如果你的 Mac 上和当前环境一致，可以这样进入：

```bash
export PATH=/Library/Frameworks/Python.framework/Versions/3.11/bin:$PATH
source ~/.espressif/v5.5/esp-idf/export.sh
cd /path/to/xiaozhi-esp32
```

### 5.2 使用 menuconfig 编译

```bash
idf.py set-target esp32s3
idf.py fullclean
idf.py menuconfig
```

在菜单中选择：

- `Xiaozhi Assistant -> Board Type -> Booboox Xiaoche`
- `Xiaozhi Assistant -> OLED Type -> SSD1306 128*64`

如果你使用的是小屏，也可以选择：

- `Xiaozhi Assistant -> OLED Type -> SSD1306 128*32`

然后执行：

```bash
idf.py build
```

### 5.3 使用脚本快速编译

`128x64` OLED:

```bash
python3 scripts/release.py booboox-xiaoche --name booboox-xiaoche-128x64
```

`128x32` OLED:

```bash
python3 scripts/release.py booboox-xiaoche --name booboox-xiaoche
```

## 6. 烧录方法

先查看串口：

```bash
ls /dev/cu.*
```

然后烧录并打开串口监视：

```bash
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

如果只烧录不打开监视：

```bash
idf.py -p /dev/cu.usbmodemXXXX flash
```

## 7. 生成文件

编译完成后常用文件位置：

- 应用固件：`build/xiaozhi.bin`
- 合并固件：`build/merged-binary.bin`
- 打包固件：`releases/`

## 8. 板型说明

当前自定义板型目录：

- [main/boards/booboox-xiaoche](main/boards/booboox-xiaoche)

板型配置入口：

- [main/Kconfig.projbuild](main/Kconfig.projbuild)
- [main/CMakeLists.txt](main/CMakeLists.txt)

## 9. 说明

这个仓库基于小智项目衍生，当前重点是双轮小车形态的本地开发和实验。如果后续增加激光传感器、机械臂、网页控制面板或更多 MCP 控制能力，可以继续在 `booboox-xiaoche` 板型上扩展。
