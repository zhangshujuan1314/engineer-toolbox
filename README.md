# 工程师工具箱

嵌入式工业调试上位机 — 单文件 HTML，双击即用。

## 功能模块

| 模块 | 说明 |
|------|------|
| **PID 实时调参** | 质量-弹簧-阻尼系统仿真 + Web Serial 单片机连接，滑块/数值双向同步，实时曲线绘制 |
| **姿态校验** | 四元数 ↔ 欧拉角 (ZYX) 双向转换，Three.js 3D 可视化，万向节锁检测 |
| **CAN 解码** | Hex 报文解析，Intel / Motorola 位序提取，信号定义表增删改，8×8 位布局图 |
| **IoU / NMS** | 可拖拽检测框，实时 IoU 计算，NMS 逐步动画展示 |
| **机械臂** | 6 轴标准 DH 正运动学，关节滑块实时联动，3D 模型渲染 |

## 技术栈

- 纯原生 HTML / CSS / JavaScript，单文件内联
- Three.js r128 (CDN) — 3D 渲染
- Canvas 2D — 曲线图表 / 检测框绘制
- Web Serial API — 单片机串口通信
- 无框架、无构建工具、无后端依赖

## 快速开始

1. 浏览器打开 `engineer-toolbox.html`
2. 顶部导航切换 5 个工具模块
3. **串口连接**：波特率选 115200，点击「连接串口」→ 选择 MCU 端口

> ⚠️ Web Serial 需要 Chrome / Edge 桌面版 + HTTPS 或 localhost

## 串口协议

```
PC → MCU:  P Kp Ki Kd SP\n     (例: P 2.5 0.5 0.3 1.0)
MCU → PC:  y_val\n             (例: 0.523)
```

配套 Arduino 固件见 `pid_mcu_firmware.ino`。

## 待完善

- [ ] 机械臂 3D 模型几何尺寸与 DH 坐标系对齐
- [ ] 部分标签页内容渲染修复
- [ ] 触摸屏拖拽体验优化

## 许可证

MIT
