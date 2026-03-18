# STM32 PWM 呼吸燈專案 (Structured Breathing LED with UART)

本專案實作於 **STM32F401RE (Nucleo-64)** 開發板。除了基本的結構體管理與中斷控制，進一步整合了 **UART 全雙工通訊**，達成即時狀態回報與遠端指令控制。

## 🚀 核心功能

1.  **結構體封裝 (Struct Practice)**：使用自定義結構體 `Breath_LED_t` 統一管理 LED 的週期、亮度及啟用狀態。
2.  **雙重控制模式 (Hybrid Control)**：
    * **實體控制 (EXTI)**：透過 PC13 藍色按鈕達成「按住停止、放開恢復」。
    * **遠端控制 (UART RX)**：透過序列埠傳送指令：
        * 鍵盤輸入 `'s'` (Stop)：停止呼吸。
        * 鍵盤輸入 `'r'` (Resume)：恢復呼吸。
3.  **即時遙測回報 (UART Telemetry)**：每 500ms 透過 UART 回報當前 PWM 佔空比（Duty Cycle），方便開發者監控亮度曲線。
4.  **精準 PWM 控制**：使用 TIM2 Channel 1 (PA5) 達成 $0 \sim 999$ 階亮度調節。

---

## 📐 數學邏輯

$$step = \frac{MaxBrightness \times 2}{\frac{Period}{LoopInterval}}$$

在程式實作中，我們使用 `sprintf` 將運算結果格式化為 ASCII 字串，並透過 UART 傳送至 PC 端的終端機顯示。

---

## 🛠 硬體配置

* **Timer**: TIM2 (Prescaler: 83, ARR: 999) -> 1kHz PWM
* **UART**: **USART2** (VCP via USB)
    * **Baud Rate**: 115200
    * **Word Length**: 8 bits (including parity: None)
    * **Stop Bits**: 1
* **GPIO**:
    * **PA5**: `TIM2_CH1` (PWM Output)
    * **PA2 / PA3**: `USART2_TX / RX`
    * **PC13**: `GPIO_EXTI13` (User Button)
* **NVIC**: 
    * 開啟 `EXTI line[15:10] interrupts`
    * 開啟 `USART2 global interrupt` (必要，用於非阻塞式指令接收)

---

## 📂 程式碼片段：通訊與控制邏輯

### 1. UART 狀態回報 (TX)
使用 `sprintf` 進行字串打包，確保資料傳輸的靈活性。
```c
char msg_buf[64];
int len = sprintf(msg_buf, "Duty Cycle: %d%%\r\n", LED2.cur_brightness);
HAL_UART_Transmit(&huart2, (uint8_t*)msg_buf, len, 100);
```

### 2. UART 指令接收中斷 (RX Callback)
利用中斷機制達成非阻塞式控制。注意：由於 HAL 接收中斷為一次性任務，處理完畢後需重新啟動監聽。
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart->Instance == USART2) {
        // 指令判斷
        if(rx_data == 's')      LED2.is_enabled = 0; // Stop
        else if(rx_data == 'r') LED2.is_enabled = 1; // Resume

        // 重新開啟中斷監聽
        HAL_UART_Receive_IT(&huart2, &rx_data, 1);
    }
}
```

---

## 📖 使用說明
1.  將開發板連接至電腦。
2.  開啟序列埠調試工具（如 STM32CubeIDE Terminal 或 VOFA+）。
3.  設定鮑率為 **115200**。
4.  觀察螢幕顯示的亮度數值，並嘗試輸入 `'s'` 或 `'r'` 來控制 LED 狀態。

---
