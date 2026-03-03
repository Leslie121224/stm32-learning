# STM32 PWM 呼吸燈專案 (Structured Breathing LED)

本專案實作於 **STM32F401RE (Nucleo-64)** 開發板，主要目的在於練習使用 C 語言結構體（Structure）管理硬體參數，並透過 Timer PWM 與 EXTI 中斷達成互動式燈光效果。

## 🚀 核心功能

1. **結構體封裝 (Struct Practice)**：使用自定義結構體 `Breath_LED_t` 統一管理 LED 的週期、亮度及啟用狀態，提升程式碼維護性。
2. **雙邊沿中斷控制 (EXTI Dual Edge)**：偵測 PC13 藍色按鈕。
    * **按下按鈕**：觸發 Falling Edge，停止呼吸（`is_enabled = 0`），燈光固定於當前亮度。
    * **放開按鈕**：觸發 Rising Edge，恢復呼吸（`is_enabled = 1`）。


3. **精準 PWM 控制**：
    * 使用 **TIM2 Channel 1 (PA5)** 輸出 PWM。
    * 解析度設定為 `0 ~ 999`（ARR = 999），透過修改 `CCR` 暫存器達成動態亮度調節。


4. **動態速率運算**：
    * 支援透過 `#define` 調整主迴圈延遲（`LOOP_DELAY_MS`）。
    * 支援透過結構體內的 `period` 成員動態改變呼吸速度。



## 📐 數學邏輯

為了讓呼吸燈在指定的 `period`（毫秒）內完成一次「暗 $\to$ 亮 $\to$ 暗」的循環，我們計算每一次迴圈應變化的步進值（Step）：

$$step = \frac{MaxBrightness \times 2}{\frac{Period}{LoopInterval}}$$

在程式實作中，為了避免整數除法造成的誤差，並加入**「最小值保護」**（防止 `step` 為 0 導致呼吸停止）：

```c
uint16_t step = (1000 * 2 * LOOP_DELAY_MS) / LED2.period;
if (step == 0) step = 1; // 確保即便週期很長，至少也要動

```

## 🛠 硬體配置

* **Timer**: TIM2 (Internal Clock: 84MHz)
* **Prescaler**: 83 (得到 1MHz 計數頻率)
* **Counter Period (ARR)**: 999 (得到 1kHz PWM 頻率)


* **GPIO**:
* **PA5**: 設定為 `TIM2_CH1` (Alternate Function)。
* **PC13**: 設定為 `GPIO_EXTI13`，觸發模式為 `Rising/Falling edge`。
* **NVIC**: 必須開啟 `EXTI line[15:10] interrupts` 使能。



## 📂 程式碼片段：結構體與中斷邏輯

```c
/* Structure Define */
typedef struct {
    uint32_t period;         // 呼吸總週期 (ms)
    uint16_t cur_brightness; // 當前亮度
    uint8_t is_enabled;      // 1: 呼吸中, 0: 暫停呼吸
} Breath_LED_t;

/* ISR */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_13) {
        // 按下為 RESET，放開為 SET
        LED2.is_enabled = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET);
    }
}

```
