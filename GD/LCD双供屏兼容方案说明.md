# LCD 双供屏（GC9309 / ST7789M）MCU 端自动识别方案 — 过程记录

> 记录时间：2026-08-19
> 适用工程：`az65mcu_gd32e235kbu6 / GD`
> MCU：GD32E235（SPI1 驱动单 SDA 串口 LCD）
> 涉及文件：`src/drivers/lcd/gd32e235c_lcd_eval.c`、`inc/gd32e235c_lcd_eval.h`

---

## 1. 需求背景

项目需要在一颗 MCU 上兼容两块 LCD 模组：

| 供方 | 型号 | 识别依据 |
|---|---|---|
| 一供 | GC9309 | 硬件 LCD_ID 脚 **低电平（PB4=0）** |
| 二供 | ST7789M | 硬件 LCD_ID 脚 **高电平（PB4=1）** |

目标：MCU 上电、完成 LCD 硬件复位后，在执行具体 LCD 初始化命令表**之前**判断当前连接的是哪块屏，再选择对应的初始化流程，且**不改动两块屏已有的初始化参数**。

---

## 2. 方案演进（踩坑过程）

### 2.1 最初的设想：软件读 Display ID（0x04）

按两款屏的 Datasheet，二者都支持 DCS `Read Display ID`（命令 `0x04`），返回格式为：

```
RX[0] = Dummy
RX[1] = ID1
RX[2] = ID2
RX[3] = ID3
```

- GC9309 默认 ID：`00 93 09`
- ST7789M 默认 ID：`77 85 01`

计划是：硬件复位后、初始化前，先 `write_cmd(0x04)` 读 4 字节，按 ID1/ID2/ID3 三字节完整匹配判断类型，再分流到 `lcd_init_gc9309()` / `lcd_init_st7789()`。

### 2.2 关键硬件事实（导致软件读 ID 不可行）

排查代码时发现一个根本约束：

- 当前 `spi1_init()` 只配置了 **MOSI(PA14) / SCK(PB1)**，**没有 MISO 引脚**。
- LCD 是单 SDA 线串口，SDA 直接接在 SPI1_MOSI(PA14) 上；SPI1 的 MISO 缺省引脚 PA15 已被用作 LCD 的 CS。
- 因此硬件 SPI 全双工模式下回传数据走 MISO，而 MISO 没有布线，**读 ID 读不到**。

结论：**必须做半双工读取**——硬件 SPI 发出命令字节后，关闭 SPI、把 SCK 切 GPIO 翻时钟、把 SDA 当输入采样。于是第一版实现了 `lcd_rd_delay / lcd_bus_to_gpio / lcd_read_byte_gpio / lcd_read_cmd` 一套 GPIO 位翻转读时序，并用 `0x04`，失败再 fallback 到 `0xDA/0xDB/0xDC`。

### 2.3 第一版上板的两个问题

**问题 A（致命）：CS 电平被读 ID 改乱，导致整屏不亮。**

`lcd_read_cmd()` 每次读 ID 结束都会 `LCD_CS_SET`，而 `lcd_init_gc9309/st7789()` 内部**不会重新拉低 CS**（原代码假定进入时 CS 已为低，整段初始化期间 CS 一直保持低）。走完 detect，CS 被遗留在高电平 → LCD 被取消选通 → 后续所有写命令发不进去 → **两块屏都不亮**。

这也解释了用户现场现象：即使临时在 `default` 分支强制跑 `lcd_init_st7789()` 也点不亮——根因是 CS 电平，不是 ID 读对读错。

修复思路：检测完成后恢复 SPI 总线和引脚，并把自定义的 `lcd_bus_to_spi()` 改为**直接复用 `spi1_init()`**（用户明确要求不对 SPI 做多余自定义操作），同时让 `lcd_init()` 在每个分支进入前显式 `LCD_CS_CLR`。

**问题 B：恢复不彻底。** 仅 `spi_disable/spi_enable` 不足以让 SPI 与引脚回到已知良好状态，最终统一用 `spi1_init()` 完整重建。

### 2.4 方案转折点：原理图有专用 LCD_ID 脚

用户核对原理图后发现：板上有 **专门的 `LCD_ID` 脚，接在 MCU 的 PB4 上**：
- 接一供 GC9309 时，该脚被模组拉**低**；
- 接二供 ST7789M 时，该脚被模组拉**高**。

既然硬件已经把“我是谁”用 strap 电平告诉了 MCU，就**没必要再软件读 ID 了**——读 ID 的那套 SPI 翻转代码全部可以删掉，初始化前也不需要动 SPI 一根手指头。

### 2.5 最终方案：硬件 LCD_ID（PB4）判型（已上板验证通过）

- PB4 在 GD32E23x 上是 JTAG 的 JNTRST 脚，但本工程未启用 JTAG（PA13/PA14 已用作 LCD 的 DC/MOSI），且 `Set_Lcd_High_Resistance()` 本就在操作 PB4，可直接当普通 GPIO 输入使用。
- 配置 PB4 为**无上下拉输入**（`GPIO_PUPD_NONE`）：电平由模组拉死，MCU 内部不额外拉，避免干扰模组设定的 strap 电平。
- `lcd_detect()` 简化为 8 行：`gpio_input_bit_get(GPIOB, GPIO_PIN_4)` 低→GC9309、高→ST7789。
- 所有 SPI 代码（`spi1_init` / `spi_write_byte`）一字未动，CS/DC/RST 时序与最早“能点亮”的版本完全一致，自然规避了问题 A。

---

## 3. 最终代码实现（摘录）

### 3.1 头文件 `inc/gd32e235c_lcd_eval.h`

```c
/* LCD supplier detected by lcd_detect() */
typedef enum {
    LCD_TYPE_UNKNOWN = 0,   /* not detected yet / reserved */
    LCD_TYPE_GC9309,        /* 1st source panel, LCD_ID pin = 0 */
    LCD_TYPE_ST7789         /* 2nd source panel, LCD_ID pin = 1 */
} lcd_type_t;

/* lcd init with supplier auto-detection */
void lcd_init(void);
/* detect LCD supplier from hardware LCD_ID pin (PB4): 0=GC9309, 1=ST7789 */
lcd_type_t lcd_detect(void);
/* get the LCD type resolved during lcd_init() */
lcd_type_t lcd_get_type(void);
```

### 3.2 驱动文件 `src/drivers/lcd/gd32e235c_lcd_eval.c`

宏定义：

```c
#define LCD_ID_GPIO     GPIOB
#define LCD_ID_PIN      GPIO_PIN_4
```

ID 脚初始化与判型：

```c
static void lcd_id_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_mode_set(LCD_ID_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LCD_ID_PIN);
}

lcd_type_t lcd_detect(void)
{
    if(RESET == gpio_input_bit_get(LCD_ID_GPIO, LCD_ID_PIN)) {
        g_lcd_type = LCD_TYPE_GC9309;
    } else {
        g_lcd_type = LCD_TYPE_ST7789;
    }
    return g_lcd_type;
}
```

初始化入口（detect 不碰 CS，CS 从开头保持低，整段初始化期间不变）：

```c
void lcd_init(void)
{
    spi1_init();
    lcd_id_gpio_init();

    LCD_CS_CLR;
    lcd_reset();

    /* supplier selected by the hardware LCD_ID pin (PB4), then run the
       matching init sequence; CS stays low for the whole init as before */
    switch(lcd_detect()) {
    case LCD_TYPE_GC9309:
        lcd_init_gc9309();
        break;

    case LCD_TYPE_ST7789:
        lcd_init_st7789();
        break;

    default:
        /* not reachable with a 2-state strap pin, kept for safety */
        LCD_CS_SET;
        break;
    }
}
```

---

## 4. 修改文件清单

| 文件 | 改动内容 |
|---|---|
| `src/drivers/lcd/gd32e235c_lcd_eval.c` | 新增 `LCD_ID_GPIO/PIN` 宏、`lcd_id_gpio_init()`、`lcd_detect()`（硬件判型）、`lcd_get_type()`；重构 `lcd_init()` 分流；**删除**所有软件读 ID 代码（`lcd_rd_delay` / `lcd_bus_to_gpio` / `lcd_read_byte_gpio` / `lcd_read_cmd` 及 0x04/0xDA-DC 相关宏与 fallback 逻辑）；`lcd_init_gc9309/st7789` 命令表未改 |
| `inc/gd32e235c_lcd_eval.h` | 新增 `lcd_type_t` 枚举与 `lcd_detect()` / `lcd_get_type()` 声明，更新注释语义为硬件 LCD_ID 脚 |
| `lcd_init_gc9309()` / `lcd_init_st7789()` 命令表 | **未改动**（按要求只做识别和分流框架） |
| `spi1_init()` / `spi_write_byte()` | **未改动** |

---

## 5. 上板验证结果

- 编译下载后，MCU 能**正确区分两块屏**：GC9309 板走 `lcd_init_gc9309()` 点亮，ST7789M 板走 `lcd_init_st7789()` 点亮。
- 两块屏均正常显示，开机上屏内容与改造前一致。

---

## 6. 经验与扩展讨论

### 6.1 关键教训

1. **先查硬件再写时序**：单 SDA 串口、无 MISO 布线的情况下，软件读 ID 需要半双工 GPIO 翻转，复杂且易踩 CS/总线恢复坑；而原理图若有专用 strap 脚，应优先用硬件判型。
2. **CS 电平是 LCD 驱动的“生死线”**：任何在初始化前插入的读操作，都要保证退出后 CS 回到初始化所需的电平，否则整屏不亮——且症状会“伪装”成 ID 识别失败。
3. **恢复 SPI 总线优先复用 `spi1_init()`**，不要自己拼 `spi_disable/enable`。
4. **PB4 是 JTAG JNTRST 脚**：确认工程未启用 JTAG 后才能当普通 GPIO；本工程 PA13/14 已用于 LCD，故安全。

### 6.2 如果将来要兼容第三、第四块屏

- **1 个 GPIO 只有 2 种电平，最多区分 2 块屏**；第三块屏靠当前 PB4 区分不开。
- 可选扩展：
  - **方案 A（改板）**：再加 1 根 strap 脚（如 PB5），2-bit 编码最多 4 种组合，软件把 `lcd_detect()` 从读 1 脚改成读 2 脚拼 code，switch 增加 case 即可。
  - **方案 B（不动板）**：PB4 做第一层粗分（两组），组内再用 `0x04` 软件读 ID 细分新屏；现有两块屏完全不受影响。
  - **方案 C**：全部改软件读 ID，最通用但所有屏启动都变慢、最复杂，不推荐丢出现有的 strap 优势。
- **约束**：strap 方案成立前提是“模组把 ID 脚电平拉死”，新增屏时务必把 ID 脚电平写进采购/规格书约束。

---

*— 本文档由 WorkBuddy 根据实际代码改动与调试过程整理。*
