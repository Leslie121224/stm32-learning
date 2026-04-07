# STM32 ADC 類比訊號遙測專案 (ADC Telemetry with Fixed-point Math)

本專案實作於 **STM32F401RE (Nucleo-64)** 開發板，透過 ADC1 讀取外部可變電阻的電壓變化，並利用 **定點數運算（Fixed-point arithmetic）** 在不消耗浮點數運算資源的情況下，實現精確至毫伏特（mV）的即時數據監控。

## 🚀 核心功能

1. **單次觸發取樣 (Single Conversion Mode)**：採用「手動啟動、輪詢等待」機制，有效避免連續模式下常見的 Overrun (溢位) 錯誤，確保每一筆數據皆為即時更新。
2. **高效能定點數換算**：避開耗時的 `float` 運算，直接在整數空間進行電壓換算，大幅節省 Flash 空間與運算時間。
3. **即時數據回報 (UART Telemetry)**：將 12-bit 原始數值與換算後的電壓值格式化輸出，支援補零顯示功能（如 `3.005V`），提升數據閱讀性。

---

## 📐 數學邏輯

### 1. ADC 原始值換算
STM32 ADC 解析度為 12-bit ($0 \sim 4095$)，參考電壓 ($V_{REF}$) 為 $3.3V$。

### 2. 定點數毫伏特換算公式
為了避免使用浮點數，我們將電壓放大 1000 倍進行運算：

$$Voltage(mV) = \frac{ADC_{Raw} \times 3300}{4095}$$

在輸出時，再透過商數（Quotient）與餘數（Remainder）拆解出整數與小數部分。

---

## 🛠 硬體配置

* **ADC**: ADC1 (Resolution: 12-bit)
    * **Mode**: Single Conversion (Manual Trigger)
    * **Scan/Continuous Mode**: Disabled
* **UART**: **USART2** (115200 bps, 8N1)
* **GPIO**:
    * **PA0 (A0)**: ADC 類比輸入腳位（接可變電阻中間腳）。
    * **PA2 / PA3**: `USART2_TX / RX` (連至電腦端 Console)。
    * **3.3V / GND**: 提供可變電阻基準電源。

---

## 📂 程式碼片段：核心取樣邏輯

### 1. 單次取樣與獲取
在 `while(1)` 迴圈中，採用「啟動 $\to$ 等待 $\to$ 讀取」的標準流程：
```c
HAL_ADC_Start(&hadc1); // 手動啟動轉換

if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
    adc_raw = HAL_ADC_GetValue(&hadc1); // 獲取 12-bit 數據
}
```

### 2. 數據封裝與輸出 (mV 補零招式)
利用 `%03lu` 確保小數點後三位的正確顯示，避免如 $3.005V$ 顯示為 $3.5V$ 的錯誤。
```c
uint32_t voltage_mv = (adc_raw * 3300) / 4095;
uint32_t v_part = voltage_mv / 1000;  // 伏特整數
uint32_t mv_part = voltage_mv % 1000; // 毫伏特餘數

char buf[64];
int len = sprintf(buf, "Raw: %4lu | Voltage: %lu.%03luV\r\n", adc_raw, v_part, mv_part);
HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 100);
```

---

## 📖 使用說明
1. 將可變電阻兩側接至 **3.3V** 與 **GND**，中間滑動端接至 **A0 (PA0)**。
2. 開啟序列埠監控工具（Baud Rate: **115200**）。
3. 旋轉可變電阻，觀察 `Raw` 數值從 $0 \sim 4095$ 變化，並確認 `Voltage` 數值與電壓表量測一致。

---

### 🚀 專業小筆記
* **為何不開連續模式？** 在沒有配合 DMA 的情況下，手動單次觸發能提供最高的系統穩定度，是偵錯階段的最佳選擇。
* **為什麼 `voltage_mv` 計算要先乘 3300？** 這是為了保留計算精準度。若先除 4095，整數運算會直接導致數值變為 0。
