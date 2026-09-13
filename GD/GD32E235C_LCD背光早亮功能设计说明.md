# LCD 背光早亮功能设计说明（GD MCU 端新增背光使能 GPIO）

> 记录时间：2026-09-05
> 适用工程：`az65mcu_gd32e235kbu6 / GD`
> MCU：GD32E235（SPI1 驱动单 SDA 串口 LCD）
> 状态：**硬件已确认背光脚为 PB5，MCU PB5 与平台 gpio111 并联驱动 AW9364DNR 的 EN 脚；代码尚未改动**（按需求先不改代码，本文档作为落地参考；平台侧不在本会话处理）

---

## 1. 目标

在 GD MCU 端新增一个 GPIO 作为 LCD 背光使能脚，把"点亮背光"的时机从 **系统（Unisoc 主芯片）上电约 8 秒后**，提前到 **GD MCU 完成图像显示后立即点亮**，实现开机画面秒级可见。

---

## 2. 现状分析

### 2.1 显示调用链

`src/drivers/lcd/lcd_display.c` 中 `lcd_display()` 的时序：

```c
lcd_init();                          // 1. LCD 初始化（含 0x29 Display ON）
lcd_clear(WHITE);                    // 2. 刷白屏
lcd_draw_image(..., logo_buffer+72); // 3. 绘制 logo —— 图像在此就绪
printf("exit %s", ...);
```

### 2.2 关键事实：0x29 ≠ 背光

- 两个屏的 init 末尾都**已发送 `0x29`(Display ON)**（GC9309 `@414`、ST7789 `@512`），说明 LCD 控制器内部已"显示开启"，GRAM 内容可被面板扫描输出。
- **但背光 LED 是物理灯，由单独的 BL_EN/使能脚供电驱动，与 `0x29` 命令完全无关。** 没有背光，屏就是"开着控制器却全黑看不见"。
- 当前工程里**没有任何 LCD 背光使能控制**——背光完全由系统（Unisoc）在按开机键约 8 秒后点亮。
- `main.c:82` 的 `// set led/backlight` → `MCU_Set_Led_Up()` 控制的是板载 **LED5 状态指示灯**（`GPIO_init.c:191`），**不是** LCD 背光，不要混淆。

### 2.3 改板前历史方案（本次要解决的问题根源）

- **改板前**：LCD 背光使能接在**平台的 RGB 三色灯 pin** 上，MCU 端**没有任何背光控制引脚**。
- 因此 MCU 上电并跑完 `lcd_display()` 显示图像后，屏处于"控制器已 ON、背光 OFF"的暗屏状态，必须等**平台（Unisoc）约 8 秒启动完成**后才会由平台点亮背光——整段黑屏等待 ≈ 8 秒。
- 本次改板的目标，正是用 MCU 端新增的 PB5 提前拉高背光使能，把"亮背光"时机从"平台 8 秒后"压缩到"MCU 显示完图像立即"，消除这约 8 秒的黑屏等待。

### 2.4 当前现象对应

```
开机键按下 → GD MCU 启动 → lcd_display() 写入图像（控制器已 ON，但背光 OFF → 肉眼全黑）
            …… 约 8 秒空白（改板前由平台 8 秒后亮背光）……
            系统(Unisoc) 亮背光 → 画面才可见
```

---

## 3. 方案设计

### 3.1 软件改动（改动量极小，只动 3 处）

1. 新增背光 GPIO 初始化：RCU 时钟 + 输出模式 + 初始 OFF。
2. 在 `lcd_display()` 的 `lcd_draw_image()` **之后**调用背光 ON（确保首帧已就绪再亮，避免亮屏瞬间看到花屏/残影）。
3. 用宏抽象电平极性，便于硬件确认后只改宏、不动逻辑。

> 为什么放在 `lcd_draw_image()` 之后、而不是 `lcd_init()` 末尾？
> `lcd_init()` 末尾只发了 `0x29`，此时 GRAM 还是空白/初始化态；若立刻亮背光，用户会先看到一帧无效画面再被 logo 覆盖。放在 `lcd_draw_image()` 之后，首帧 logo 已写入，亮背光即所见即所得。

### 3.2 时序对比

```
改前：
  开机 → [GD 显示图像(暗)] ───────── 8s ────────→ [系统亮背光(可见)]

改后：
  开机 → [GD 显示图像] → [GD 立即亮背光(可见)] ──── 8s ────→ [系统侧处理]
        （开机画面秒显，无需等待系统）
```

> 平台侧也已改为 GPIO 控制背光（见 3.3），本会话只处理 MCU 端 PB5 的实现；平台端不在本会话范围。

### 3.3 硬件接法（已确认：双 GPIO 并联驱动 WhiteLED Driver）

- 背光 LED 由 **WhiteLED Driver `AW9364DNR`** 驱动，其 **EN 脚**为背光使能控制引脚：**拉高 = 亮，拉低 = 灭**，仅作 on/off 开关（无需 PWM）。
- **MCU 的 PB5 与平台（Unisoc）的 `gpio111` 并联接到 AW9364DNR 的 EN 脚**：任意一端把 EN 拉高即可点亮背光。
- 分工：MCU 在 `lcd_display()` 显示完图像后立即拉高 PB5 → 背光提前亮（开机画面秒显）；平台端不再依赖原来的 RGB 三色灯 pin，也改为 `gpio111` 控制背光（**平台侧实现不在本会话范围**）。
- 这与此前假设的"接法 A/B/C"不同：背光只有一路（AW9364DNR EN），但由 MCU 与平台**两端并联**驱动，因此风险点从"是否双控"转为"并联 IO 不能相反驱动"。
- **灭屏交接（平台接管后 MCU 须释放 PB5 为高阻）**：MCU 亮屏阶段把 PB5 推挽拉高点亮背光；平台接管后 MCU 不能继续保持输出高，否则平台把 `gpio111` 拉低也灭不了屏（实测现象：灭屏后背光常亮、不受控）。因此在 MCU 进深睡前，由 `Set_Lcd_High_Resistance()`（`deepsleep.c` 调用）把 PB5 一并置为 `GPIO_MODE_ANALOG + GPIO_PUPD_NONE`（高阻），EN 控制权完全交还平台。

---

## 4. 代码骨架（背光脚已确认为 PB5，极性待硬件确认）

> 以下为**落地参考骨架**，背光 GPIO 已按原理图确认为 **PB5（GPIOB / GPIO_PIN_5 / RCU_GPIOB）**，电平极性暂按高有效占位并保留 `TODO`，待硬件确认后只改一个宏即可，无需改逻辑。**当前工程代码尚未改动。**

### 4.1 头文件 `inc/gd32e235c_lcd_eval.h`

```c
/* LCD backlight enable, driven by GD MCU, wired to PB5 (confirmed by schematic) */
#define LCD_BL_GPIO          GPIOB
#define LCD_BL_PIN           GPIO_PIN_5
#define LCD_BL_RCU           RCU_GPIOB
/* 1 = active-high (set to turn on), 0 = active-low;
   TODO: confirm polarity with HW (schematic shows PB5 as backlight enable) */
#define LCD_BL_ACTIVE_HIGH   1   /* TODO: set 0 if the schematic is active-low */

#if LCD_BL_ACTIVE_HIGH
#define LCD_BL_ON()   gpio_bit_set(LCD_BL_GPIO, LCD_BL_PIN)
#define LCD_BL_OFF()  gpio_bit_reset(LCD_BL_GPIO, LCD_BL_PIN)
#else
#define LCD_BL_ON()   gpio_bit_reset(LCD_BL_GPIO, LCD_BL_PIN)
#define LCD_BL_OFF()  gpio_bit_set(LCD_BL_GPIO, LCD_BL_PIN)
#endif

void lcd_backlight_init(void);
void lcd_backlight_on(void);
```

### 4.2 实现 `src/drivers/lcd/gd32e235c_lcd_eval.c`

```c
/*!
    \brief      init the GD-controlled LCD backlight enable pin (kept OFF)
    \note       call once during lcd_init(); backlight stays off until the
                first frame is drawn, then lcd_backlight_on() turns it on
*/
void lcd_backlight_init(void)
{
    rcu_periph_clock_enable(LCD_BL_RCU);
    gpio_mode_set(LCD_BL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_BL_PIN);
    gpio_output_options_set(LCD_BL_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_BL_PIN);
    LCD_BL_OFF();   /* keep backlight off until image is ready */
}

void lcd_backlight_on(void)
{
    LCD_BL_ON();
}
```

> 建议在 `lcd_init()` 中 `spi1_init()` 之后调用 `lcd_backlight_init()`，与现有 LCD 硬件初始化统一收口。

### 4.3 调用点 `src/drivers/lcd/lcd_display.c`

```c
    /* 3.draw logo image */
    lcd_draw_image(0, (Y_MAX_PIXEL - 80) / 2, 240, 80, logo_buffer + 72);

    /* turn on LCD backlight as soon as the first frame is ready */
    lcd_backlight_on();

    printf("exit %s\n", __func__);
```

---

## 5. 待硬件确认清单（建议拿到主板后逐项核对）

1. ~~**MCU 背光 GPIO pin**~~：**已确认 = PB5（GPIOB / GPIO_PIN_5 / RCU_GPIOB）**；平台侧 = `gpio111`（并联，不在本会话处理）。
2. **AW9364DNR EN 极性**：高有效还是低有效（填 `LCD_BL_ACTIVE_HIGH`；当前骨架默认高有效，需查芯片手册/原理图确认）。
3. **并联 IO 配置（避免相反驱动）**：PB5 与平台 `gpio111` 都是 GPIO 输出并联到同一 EN，需确认两端 IO 模式（推挽 / 开漏+上拉）与默认电平，确保不会一端拉高、另一端拉低造成对拉短路。平台侧不在此会话，但风险需知会平台端。
4. **是否需 PWM 调光**：AW9364DNR 若支持 PWM 调光，本期先按 on/off 使能脚处理；如需亮度等级后续可扩展。

---

## 6. 风险与注意

- **并联双 GPIO 驱动冲突（最高优先级）**：PB5（MCU）与平台 `gpio111` 并联到 AW9364DNR EN。若两端在某一时刻驱动相反电平（一端拉高、一端拉低），推挽对拉会产生大电流、甚至损坏 IO。需与平台端对齐：两端 IO 模式（推荐开漏+外部上拉，形成线或）与默认态（默认都释放为高阻/拉低），确保 MCU 提前拉高亮背光时平台侧不会反向拉低。平台侧不在本会话处理，但集成时必须对齐。
- **灭屏后背光常亮（已修改待上板验证，2026-09-10）**：根因是 MCU 亮屏后 PB5 一直保持推挽输出高，进深睡前未释放，平台接管后拉低 `gpio111` 仍压不过 MCU 侧高电平 → 灭屏失效、背光常亮。修复：在 `Set_Lcd_High_Resistance()`（`src/drivers/lcd/gd32e235c_lcd_eval.c`，由 `deepsleep.c` 在进深睡前调用）的 GPIOB 列表中加入 `GPIO_PIN_5`，与 LCD 其余引脚（PB1~PB4）一起释放为 `GPIO_MODE_ANALOG + GPIO_PUPD_NONE` 高阻，背光使能交接给平台。若后续新增背光相关改动，记得保持该释放动作不被遗漏。
- **亮背光时机的花屏**：务必放在 `lcd_draw_image()` 之后；如观察到亮屏瞬间有残影，可在 `lcd_backlight_on()` 前加 `delay_1ms(20~50)` 等待首帧稳定。
- **引脚冲突**：PB5 为 GD32E23x 普通 GPIO（非 SWD/JTAG 脚，JTAG/SWD 为 PA13/PA14/PA15/PB3/PB4），不与现有 LCD 引脚（PA14/PB1/PA15/PA13/PB2/PB4）冲突，可安全使用。
- **保持常亮**：`lcd_backlight_on()` 调用后维持电平即可，无需重复操作；除非后续需要息屏，否则不主动 OFF。

---

*— 本文档由 WorkBuddy 基于现有 LCD 驱动代码、显示调用链、最新原理图与硬件接法（AW9364DNR EN 由 PB5 与 gpio111 并联驱动）整理；背光脚 PB5 已确认，代码骨架待后续集成。平台侧 gpio111 实现不在本会话范围。*
