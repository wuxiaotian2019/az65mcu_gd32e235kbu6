#include "GPIO_init.h"
#include "gd32e23x.h"
#include "gd32e235c_eval.h"
#include <stdio.h>
#include <sprd_config.h>
#include "Process_Packet.h"

static volatile uint8_t key_pressed = 0;
static volatile uint32_t key_press_start_time = 0;
static volatile uint8_t key_long_press_detected = 0;
static volatile uint8_t key_shot_press_detected = 0;
static volatile uint8_t detection_window_active = 0;
static volatile uint32_t system_boot_start_time = 0;

/*!
    \brief      initilize the com GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_gpio_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USART0_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USART0_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* for mcu control soc reset */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    /* reset GPIO PA0 pin */
    gpio_bit_reset(GPIOA, GPIO_PIN_0);

    /* for mcu tell soc which spl is used 0:default spl, 1:spl_bak */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

    /* reset GPIO PA1 pin */
    gpio_bit_reset(GPIOA, GPIO_PIN_1);

    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* PB0, for soc tell mcu status, 0:not ready, 1:bootup to spl */
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);

    /* for mcu check pbint status, 0:no powerkey pressed, 1:powerkey pressed */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
}

/*!
    \brief      initilize the com GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com1_gpio_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
}

void init_key_detection(void)
{
    key_pressed = 0;
    key_long_press_detected = 0;
    detection_window_active = 1;
    system_boot_start_time = timer_handle.Timer_Count;
}
void check_key_press_during_boot(void)
{
    uint32_t current_time = timer_handle.Timer_Count;
    if (detection_window_active) {
        if (SET == gpio_input_bit_get(GPIOA, GPIO_PIN_13)) {
            if (!key_pressed) {
                key_press_start_time = current_time;
                key_pressed++;
            } else {
                if ((current_time - key_press_start_time) >= KEYPRESS_TIME) {
                    key_long_press_detected = 1;
                }
            }
        } else {
            if (key_pressed)
                key_shot_press_detected = 1;
            key_pressed = 0;
        }
    } else { 
        return;
    }
    if ((current_time - system_boot_start_time) >= KEYPRESS_CHECKTIME) {
        detection_window_active = 0;
    }
}
uint8_t is_long_key_or_nokey_press_power(void)
{
    if (!detection_window_active) {
        printf("key_long_press_detected:%d, key_shot_press_detected:%d\n", key_long_press_detected, key_shot_press_detected);
        return key_long_press_detected || !key_shot_press_detected;
    } else {
        return 0;
    }
}

void Set_SPI_High_Resistance(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_4);
}

void MCU_DeepSleepMode(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure LED1 GPIO port */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_4);

    pmu_to_deepsleepmode(PMU_LDO_NORMAL,WFI_CMD);  //deep Sleep
}

void MCU_FMC_program(uint32_t value)
{
    uint32_t *ptr = (uint32_t *)ERASE_PAGE_START_ADDR;
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* erase target page */
    fmc_page_erase(ERASE_PAGE_START_ADDR);
    /* check the erase result, light the LED2 if the result is failed */
    if(MCU_FLASH_CLEAR != (*ptr)){
        return;
    }
    /* program target address */
    fmc_word_program(ERASE_PAGE_START_ADDR, value);
    printf("SL8563MCU_W19.39.1 fmc_erase_and_program %d\n",value);/* check the program result, light the LED2 if the result is failed */
    if(ERASE_PAGE_START_ADDR != (*ptr)){
        return;
    }

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* lock the main FMC after the operation */
    fmc_lock();
}

uint32_t MCU_FMC_Read(void)
{
    return option_byte_value_get(ERASE_PAGE_START_ADDR);
}

void MCU_Set_Soc_Pbint(void)
{
    gpio_bit_set(GPIOA, GPIO_PIN_8);
}

void MCU_Reset_Soc_Pbint(void)
{
    gpio_bit_reset(GPIOA, GPIO_PIN_8);
}

/* set led/backlight */
void MCU_Set_Led_Up(void)
{
    gd_eval_led_on(LED5);
}

void MCU_Set_Led_Down(void)
{
    gd_eval_led_off(LED5);
}

void MCU_Detect_Key_Pressed(void)
{
    gpio_bit_set(GPIOA, GPIO_PIN_1);
}

bool MCU_Detect_Soc_bootup(void)
{
   return (SET != gpio_input_bit_get(GPIOB, GPIO_PIN_0));
}
