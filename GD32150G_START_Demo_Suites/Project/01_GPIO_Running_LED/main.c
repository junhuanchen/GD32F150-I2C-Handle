/*!
    \file  main.c
    \brief slave receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0.h"
#include "gd32f1x0_eval.h"

#ifdef GD32F130_150
#define BOARD_I2C I2C0
#define I2C_SCL_GPIO_PIN GPIO_PIN_6
#define I2C_SDA_GPIO_PIN GPIO_PIN_7
#define I2C_RCC RCU_I2C0
#elif defined GD32F170_190
#define BOARD_I2C I2C1
#define I2C_SCL_GPIO_PIN GPIO_PIN_10
#define I2C_SDA_GPIO_PIN GPIO_PIN_11
#define I2C_RCC RCU_I2C1
#endif

#define I2C_OWN_ADDRESS7 0x84

// #define KEY_U // PA0
// #define KEY_D // PA2
// #define KEY_L // PA1
// #define KEY_R // PA3
// #define KEY_A // PB5
// #define KEY_B // PB4
// #define KEY_Y // PA9
// #define KEY_X // PA10
// #define KEY_START // PA4
// #define KEY_SELECT // PA5

#define KEY_U gpio_input_bit_get(GPIOA, GPIO_PIN_0)
#define KEY_D gpio_input_bit_get(GPIOA, GPIO_PIN_2)
#define KEY_L gpio_input_bit_get(GPIOA, GPIO_PIN_1)
#define KEY_R gpio_input_bit_get(GPIOA, GPIO_PIN_3)
#define KEY_A gpio_input_bit_get(GPIOB, GPIO_PIN_5)
#define KEY_B gpio_input_bit_get(GPIOB, GPIO_PIN_4)
#define KEY_Y gpio_input_bit_get(GPIOA, GPIO_PIN_9)
#define KEY_X gpio_input_bit_get(GPIOA, GPIO_PIN_10)
#define KEY_T gpio_input_bit_get(GPIOA, GPIO_PIN_4)
#define KEY_E gpio_input_bit_get(GPIOA, GPIO_PIN_5)

uint8_t i2c_key_state = 0;

void rcu_config(void);
void gpio_config(void);
void i2c_config(void);

void i2c_key_init(rcu_periph_enum periph, uint32_t gpio_periph, uint32_t pin)
{
    /* enable the key clock */
    rcu_periph_clock_enable(periph);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(gpio_periph, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin);

    // gpio_input_bit_get(gpio_periph, pin);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t count = 0, backup = 0;
    /* RCU config */
    rcu_config();
    /* GPIO config */
    gpio_config();
    /* I2C config */
    i2c_config();

    gd_eval_led_init(LED1);

    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_0);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_2);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_1);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_3);
    i2c_key_init(RCU_GPIOB, GPIOB, GPIO_PIN_5);
    i2c_key_init(RCU_GPIOB, GPIOB, GPIO_PIN_4);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_9);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_10);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_4);
    i2c_key_init(RCU_GPIOA, GPIOA, GPIO_PIN_5);

    while (1)
    {
        // i2c_key_state = (KEY_U) | (KEY_D << 1) | (KEY_L << 2) | (KEY_R << 3) | (KEY_A << 4) | (KEY_B << 5) | (KEY_Y << 6) | (KEY_X << 7) | (KEY_T << 8) | (KEY_E << 9);
        if (!count++) {
            // backup = i2c_key_state;
            i2c_key_state = 0;
            i2c_key_state |= KEY_R, i2c_key_state <<= 1;
            i2c_key_state |= KEY_L, i2c_key_state <<= 1;
            i2c_key_state |= KEY_D, i2c_key_state <<= 1;
            i2c_key_state |= 1, i2c_key_state <<= 1; // KEY_U
            i2c_key_state |= 1, i2c_key_state <<= 1; // KEY_T
            i2c_key_state |= 1, i2c_key_state <<= 1; // KEY_E
            i2c_key_state |= KEY_A, i2c_key_state <<= 1;
            i2c_key_state |= KEY_B;
            // if (i2c_key_state == UINT8_MAX) {
            //     i2c_key_state = backup;
            // }
        }

        /* wait until ADDSEND bit is set */
        if (i2c_flag_get(BOARD_I2C, I2C_FLAG_ADDSEND))
        {
            /* clear ADDSEND bit */
            i2c_flag_clear(BOARD_I2C, I2C_FLAG_ADDSEND);
        }
        /* wait until the RBNE bit is set */
        if (i2c_flag_get(BOARD_I2C, I2C_FLAG_RBNE))
        {
            /* read a data byte from I2C_DATA */
            // uint8_t tmp = i2c_data_receive(BOARD_I2C);

            // allow recv data

            gd_eval_led_toggle(LED1);
        }
        /* wait until the transmission data register is empty */
        if (i2c_flag_get(BOARD_I2C, I2C_FLAG_TBE))
        {
            /* send a data byte */
            // i2c_data_transmit(BOARD_I2C, ~(uint8_t)(i2c_key_state >> 8));
            i2c_data_transmit(BOARD_I2C, ~i2c_key_state);
        }
        /* the master doesn't acknowledge for the last byte */
        if (i2c_flag_get(BOARD_I2C, I2C_FLAG_AERR))
        {
            /* clear the bit of AERR */
            i2c_flag_clear(BOARD_I2C, I2C_FLAG_AERR);
        }
        /* wait until the STPDET bit is set */
        if (i2c_flag_get(BOARD_I2C, I2C_FLAG_STPDET))
        {
            /* clear the STPDET bit */
            i2c_enable(BOARD_I2C);
        }
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C clock */
    rcu_periph_clock_enable(I2C_RCC);
}

/*!
    \brief      cofigure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* I2C GPIO ports */
    /* connect I2C_SCL_GPIO_PIN to I2C_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SCL_GPIO_PIN);
    /* connect I2C_SDA_GPIO_PIN to I2C_SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, I2C_SDA_GPIO_PIN);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, I2C_SDA_GPIO_PIN);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);
}

/*!
    \brief      cofigure the I2C interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* I2C clock configure */
    i2c_clock_config(BOARD_I2C, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(BOARD_I2C, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_OWN_ADDRESS7);
    /* enable I2C */
    i2c_enable(BOARD_I2C);
    /* enable acknowledge */
    i2c_ack_config(BOARD_I2C, I2C_ACK_ENABLE);
}
