/**
 * PID MCU Firmware — 配合 engineer-toolbox.html 使用
 * ======================================================
 * 通信协议:
 *   PC → MCU:  "P <Kp> <Ki> <Kd> <SP>\n"   例: "P 2.5 0.5 0.3 1.0\n"
 *   MCU → PC:  "<y>\n"                       例: "0.523\n"
 *
 * 接线 (以 Arduino Uno + 电位器 + LED 为例):
 *   电位器 → A0  (反馈信号，模拟传感器)
 *   LED    → D9  (PWM 输出，模拟执行器)
 *   串口   → USB  (与 PC 通信)
 *
 * 适配你自己的硬件:
 *   1. 替换 readSensor() — 读取实际传感器值
 *   2. 替换 applyOutput() — 驱动实际执行器
 *   3. 调整 DT_S (控制周期) 匹配你的系统
 *   4. 调整 OUTPUT_MIN / OUTPUT_MAX 匹配执行器范围
 */

// ===================== 用户可调参数 =====================
const float DT_S = 0.01;        // 控制周期 (秒)，推荐 0.001 ~ 0.05
const float OUTPUT_MIN = 0.0;   // 执行器输出下限
const float OUTPUT_MAX = 255.0; // 执行器输出上限
const bool  ANTI_WINDUP = true; // 抗积分饱和
const float INTEGRAL_MAX = 500; // 积分限幅

// ===================== PID 变量 =====================
float Kp = 2.5, Ki = 0.5, Kd = 0.3;
float setpoint = 1.0;

float integral = 0;
float prevError = 0;
float prevY = 0;
unsigned long lastMicros = 0;

// ===================== 串口命令解析 =====================
char serialBuf[64];
uint8_t bufIdx = 0;

void parseCommand(const char* cmd) {
  // 格式: "P Kp Ki Kd SP"
  if (cmd[0] == 'P' || cmd[0] == 'p') {
    float pkp, pki, pkd, psp;
    if (sscanf(cmd + 1, "%f %f %f %f", &pkp, &pki, &pkd, &psp) >= 4) {
      Kp = pkp; Ki = pki; Kd = pkd; setpoint = psp;
      // 可选: 参数变化时复位积分，避免突变冲击
      // integral = 0;
    }
  }
}

// ===================== 传感器读取 — 按你的硬件修改这里! =====================
float readSensor() {
  // 示例: 电位器分压 → 归一化到 0~setpoint_max
  int raw = analogRead(A0);                 // 0~1023
  return (float)raw / 1023.0 * 3.0;         // 映射到 0~3 (覆盖 setpoint 范围)
}

// ===================== 执行器输出 — 按你的硬件修改这里! =====================
void applyOutput(float u) {
  // 示例: PWM 输出
  float constrained = constrain(u, OUTPUT_MIN, OUTPUT_MAX);
  analogWrite(9, (int)constrained);
}

// ===================== PID 核心 =====================
float pidStep(float y, float dt) {
  float error = setpoint - y;

  // 积分
  integral += error * dt;
  if (ANTI_WINDUP) {
    integral = constrain(integral, -INTEGRAL_MAX, INTEGRAL_MAX);
  }

  // 微分 (对 PV 微分可避免 derivative kick)
  float derivative = (y - prevY) / dt;
  // 备选: 对 error 微分 = (error - prevError) / dt

  float output = Kp * error + Ki * integral - Kd * derivative;
  prevError = error;
  prevY = y;

  return output;
}

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);     // 波特率与工具箱中选择的一致
  while (!Serial);          // 等待串口就绪 (仅原生 USB 的板子需要)
  // 如果 MCU 一直卡在这里，注释掉上一行

  pinMode(9, OUTPUT);
  pinMode(A0, INPUT);

  lastMicros = micros();
}

// ===================== 主循环 =====================
void loop() {
  // ---- 1. 处理串口命令 ----
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (bufIdx > 0) {
        serialBuf[bufIdx] = '\0';
        parseCommand(serialBuf);
        bufIdx = 0;
      }
    } else if (bufIdx < sizeof(serialBuf) - 1) {
      serialBuf[bufIdx++] = c;
    }
  }

  // ---- 2. 定时控制循环 ----
  unsigned long now = micros();
  float dt = (now - lastMicros) / 1000000.0f;

  if (dt >= DT_S) {
    lastMicros = now;

    // 读传感器
    float y = readSensor();

    // PID 计算
    float u = pidStep(y, dt);

    // 驱动执行器
    applyOutput(u);

    // 回传测量值给 PC
    Serial.println(y, 4);   // 4 位小数精度
  }
  // 如果循环太快，可以加 delay(1) 降低 CPU 占用
}
