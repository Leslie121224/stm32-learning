# STM32 Button Counter & OLED Display 專案

這是一個基於 STM32F401RE 的練習專案，旨在複習嵌入式 C 語言核心觀念，並實作中斷驅動的 UI 顯示。

## 🛠 硬體環境

* **開發板**: STM32F401RE (Nucleo-64)
* **顯示器**: SSD1306 OLED (128x32 I2C 介面)
* **輸入**: 板載藍色按鈕 (PC13)

## ✨ 主要功能

1. **GPIO 中斷計數**: 透過 EXTI (External Interrupt) 偵測 PC13 按鈕觸發，進行全域變數累加。
2. **即時數值顯示**: 將計數結果透過 I2C 顯示於 OLED 螢幕。
3. **條件觸發彩蛋**:
* 當計數值為 10 的倍數時（不含 0），螢幕會切換為特殊文字顯示：
```
Welcome to
Ave Mujica's World
```





## 🧠 核心 C 語言觀念應用

* **`volatile` 關鍵字**: 用於定義 `g_button_count`，確保主迴圈（Main Loop）能正確讀取被中斷服務程式（ISR）修改的變數數值，防止編譯器過度優化。
* **`__weak` 回呼函式**: 透過覆寫 HAL 庫中的 `HAL_GPIO_EXTI_Callback` 來實作自定義的中斷邏輯。
* **字串格式化**: 使用 `sprintf` 將整數型別轉換為字元陣列（Buffer），以便在 OLED 上顯示。
* **指標應用**: 透過 `&hi2c1` 結構體指標將硬體控制權傳遞給驅動程式。

## 🔧 技術筆記 (Troubleshooting)

### 1. 128x32 解析度修正

若使用 128x64 的驅動程式驅動 128x32 螢幕，會出現上方黑線或錯位。需在 `ssd1306.h` 或 `ssd1306.c` 修正：

* `SSD1306_HEIGHT` 設為 `32`。
* `SSD1306_COM_ALTERNATIVE_PIN_CONFIG` 設為 `0`。

### 2. 架構設計

* **中斷內**: 僅處理變數加總，保持執行速度。
* **主迴圈**: 負責耗時的 I2C 傳輸與 OLED 更新。使用「前次數值比較」機制，僅在數值變動時才刷洗螢幕，節省 CPU 資源。

### 3. 避免彩蛋文字與計數的畫面互相殘留導致干擾

* 更新OLED畫面前使用 'ssd1306_Fill(Black)' 將畫面清空。

## 📂 檔案架構

* `Core/Src/main.c`: 主要邏輯與中斷回呼實作。
* `Drivers/SSD1306_HAL_Driver`: 包含 `ssd1306.c` 驅動程式與字型檔。
