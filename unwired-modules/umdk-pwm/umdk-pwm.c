/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 * @file	umdk-pwm.c
 * @brief       umdk-pwm module implementation
 * @author      Mikhail Perkov
 * @author		Eugene Ponomarev
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "board.h"
#include "periph/gpio.h"

#include "unwds-common.h"
#include "include/umdk-pwm.h"

static umdk_pwm_dev_t pwm_devs[UMDK_PWM_NUM_DEVS] = {
    { PWM_0, UMDK_PWM_0_NUM_CH_MAX, PWM_LEFT, UMDK_PWM_FREQ_DEFAULT, UMDK_PWM_RES_DEFAULT, false },
    { PWM_1, UMDK_PWM_1_NUM_CH_MAX, PWM_LEFT, UMDK_PWM_FREQ_DEFAULT, UMDK_PWM_RES_DEFAULT, false },
    { PWM_2, UMDK_PWM_2_NUM_CH_MAX, PWM_LEFT, UMDK_PWM_FREQ_DEFAULT, UMDK_PWM_RES_DEFAULT, false },
};

static umdk_pwm_ch_t pwm_chs[UMDK_PWM_NUM_CH] = {
    { PWM_0, UMDK_PWM_CH_0, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
    { PWM_0, UMDK_PWM_CH_1, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
    { PWM_0, UMDK_PWM_CH_2, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
    { PWM_0, UMDK_PWM_CH_3, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },

    { PWM_1, UMDK_PWM_CH_0, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
    { PWM_1, UMDK_PWM_CH_1, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },

    { PWM_2, UMDK_PWM_CH_0, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
    { PWM_2, UMDK_PWM_CH_1, UMDK_PWM_STATUS_DEFAULT, UMDK_PWM_DUTY_DEFAULT },
};

static uwnds_cb_t *callback;

void umdk_pwm_init(uint32_t *non_gpio_pin_map, uwnds_cb_t *event_callback)
{
    (void)non_gpio_pin_map;

    callback = event_callback;

    for (int i = 0; i < UMDK_PWM_NUM_DEVS; i++) {
        umdk_pwm_dev_t *dev = &pwm_devs[i];

        printf("[umdk-pwm] Initializing PWM#%d with frequency %d Hz and resolution up to %d\n", dev->dev, (int) dev->freq, dev->res);

        pwm_init(dev->dev, dev->mode, dev->freq, dev->res);
    }
}

static inline void update_pwm_freq(umdk_pwm_dev_t *dev, uint32_t freq)
{
    dev->freq = freq;

    pwm_stop(dev->dev);
    pwm_init(dev->dev, dev->mode, dev->freq, dev->res);
}

static void set_pwm_value(umdk_pwm_dev_t *dev, umdk_pwm_ch_t *ch, uint8_t value)
{
    value = (uint8_t)((UMDK_PWM_RES_DEFAULT * value) / UMDK_PWM_DUTY_MAX);

    ch->duty_cycle = value;

    /*
     * Check that device for corresponding channel is need to be started or can be stopped.
     * We could stop the PWM device if all it's channels are having default duty cycle
     */
    bool need_to_start = !dev->is_started;
    bool can_be_stopped = true;

    for (int i = 0; i < UMDK_PWM_NUM_CH; i++) {
        umdk_pwm_ch_t *ch = &pwm_chs[i];

        /* Check channels in current PWM device */
        if (ch->dev == dev->dev) {
            /* Device can't be stopped if it has channels with non-default duty cycle */
            if (ch->status == UMDK_PWM_CH_TURN_ON) {
                can_be_stopped = false;
            }
        }
    }

    /* Start or stop current PWM device */
    if (need_to_start) {
        pwm_start(dev->dev);
        dev->is_started = true;

        printf("[umdk-pwm] PWM device #%d is started\n", dev->dev);
    }
    else
    if (can_be_stopped) {
        pwm_stop(dev->dev);
        dev->is_started = false;

        printf("[umdk-pwm] PWM device #%d is stopped\n", dev->dev);
    }

    /* Set value for the current channel in current PWM device if it's running */
    if (dev->is_started) {
        printf("[umdk-pwm] Setting pwm #%d channel #%d to %d\n", dev->dev, ch->ch, ch->duty_cycle);
        pwm_set(dev->dev, ch->ch, ch->duty_cycle);
    }
}

static inline void umdk_pwm_turn_off_pin(gpio_t pin)
{
  GPIO_TypeDef *port = (GPIO_TypeDef *)( pin & ~(0x0f));
  int pin_num =  ( pin & 0x0f);

  /* disable pull-up on GPIO */
  port->PUPDR &= ~(0x3 << (2 * pin_num));
  /*  Set analog mode */
  port->MODER &= ~(0x3 << (2 * pin_num));
  port->MODER |= (0x3 << (2 * pin_num));
}

bool umdk_pwm_cmd(module_data_t *cmd, module_data_t *reply)
{
    /* Check minimum command length */
    if (cmd->length < 2) {
        return false;
    }

    umdk_pwm_cmd_t c = (cmd->data[0] >> 4) & 0x0F;

    switch (c) {
        case UMDK_PWM_CMD_SET: {

	    uint32_t freq = (((cmd->data[0] & 0x0F) << 16) + (cmd->data[1] << 8) + (cmd->data[2])) & 0x00FFFFFF;
	    if(freq > UMDK_PWM_FREQ_MAX) {
                printf("[umdk-pwm] Invalid frequency set: %d > %d Hz\n", (int)freq, UMDK_PWM_FREQ_MAX);
                return false;
	    }

	   uint8_t dev_id = cmd->data[3];
	   if (dev_id >= UMDK_PWM_NUM_DEVS) {
		printf("[umdk-pwm] Invalid PWM device selected: %d >= %d\n", dev_id, UMDK_PWM_NUM_DEVS);
		return false;
	    }


	    uint8_t status = (cmd->data[4] >> 7) & 0x01;
	    if(status > 1) {
		printf("[umdk-pwm] Invalid value of command turn on/off channel: %d (Allow 1 or 0 )\n", status);
		return false;
	    }

	    uint8_t ch_num = (cmd->data[4] & 0x03);
	    if(ch_num > pwm_devs[dev_id].num_chan) {
		printf("[umdk-pwm] Invalid number of channel of the PWM device#%d selected: %d. Max: %d\n", dev_id, ch_num, pwm_devs[dev_id].num_chan);
		return false;
	    }

	    uint8_t duty_value = cmd->data[5];
	    if(duty_value > UMDK_PWM_DUTY_MAX) {
		printf("[umdk-pwm] Invalid duty cycle selected: %d. Max: %d\n", duty_value, UMDK_PWM_DUTY_MAX);
		return false;
	    }

            /* Update corresponding PWM channel */
            umdk_pwm_ch_t *ch = &pwm_chs[ch_num + 2*dev_id + 2];
            umdk_pwm_dev_t *dev = &pwm_devs[dev_id];



	    if(status == UMDK_PWM_CH_TURN_OFF) {
		if(ch->status == UMDK_PWM_CH_TURN_OFF) {
		    printf("[umdk-pwm] Channel %d of the #%d PWM device turned off YET\n", ch_num, dev_id);
		}
		else {
		    printf("[umdk-pwm] Channel %d of the #%d PWM device turned off\n", ch_num, dev_id);
		    umdk_pwm_turn_off_pin(pwm_config[dev_id].pins[ch_num]);
		    ch->status = UMDK_PWM_CH_TURN_OFF;
		}
	    }

	    if(status == UMDK_PWM_CH_TURN_ON) {
		if(ch->status == UMDK_PWM_CH_TURN_OFF) {
		    gpio_init(pwm_config[dev_id].pins[ch_num], GPIO_OUT);
		    gpio_init_af(pwm_config[dev_id].pins[ch_num], pwm_config[dev_id].af);
		    ch->status = UMDK_PWM_CH_TURN_ON;

		}

	   if (dev->freq != freq) {
		printf("[umdk-pwm] Updating PWM device #%d frequency from %d to %d Hz\n", dev->dev, (int) dev->freq, (int) freq);
		update_pwm_freq(dev, freq);
	    }

	    printf("[umdk-pwm] Setting PWM#%d ch: %d to %d/%d with frequency %d Hz\n",
		   dev->dev, ch_num, duty_value, dev->res, (int) freq);
	    }

	    set_pwm_value(dev, ch, duty_value);

	    reply->length = 4;
	     reply->data[0] = UNWDS_PWM_MODULE_ID;
	     reply->data[1] = 'o';
	     reply->data[2] = 'k';
	     reply->data[3] = '\0';

	     return true; /* Allow reply */

        }

        break;
    }

    return false;
}


#ifdef __cplusplus
}
#endif
