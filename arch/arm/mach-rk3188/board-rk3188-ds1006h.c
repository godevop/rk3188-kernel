/*
 *
 * Copyright (C) 2012 ROCKCHIP, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/skbuff.h>
#include <linux/spi/spi.h>
#include <linux/mmc/host.h>
#include <linux/ion.h>
#include <linux/cpufreq.h>
#include <linux/clk.h>
#include <mach/dvfs.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>
#include <asm/hardware/gic.h>

#include <mach/board.h>
#include <mach/hardware.h>
#include <mach/io.h>
#include <mach/gpio.h>
#include <mach/iomux.h>
#include <linux/rk_fb.h>
#include <linux/regulator/machine.h>
#include <linux/rfkill-rk.h>
#include <linux/sensor-dev.h>
#include <linux/mfd/tps65910.h>
#include <linux/regulator/act8846.h>
#include <plat/efuse.h>
#include <linux/regulator/rk29-pwm-regulator.h>
#include <mach/ddr.h>

#ifdef CONFIG_CW2015_BATTERY
	#include <linux/power/cw2015_battery.h>
#endif

#if defined(CONFIG_CT36X_TS)
#include <linux/ct36x.h>
#endif
#if defined(CONFIG_MFD_RK610)
#include <linux/mfd/rk610_core.h>
#endif
#if defined(CONFIG_MFD_RK616)
#include <linux/mfd/rk616.h>
#endif
#ifdef CONFIG_TOUCHSCREEN_MELFAS
#include <linux/melfas_ts.h>
#endif

#if defined(CONFIG_RK_HDMI)
	#include "../../../drivers/video/rockchip/hdmi/rk_hdmi.h"
#endif

#if defined(CONFIG_SPIM_RK29)
#include "../../../drivers/spi/rk29_spim.h"
#endif
#if defined(CONFIG_MT6229)
#include <linux/mt6229.h>
#endif
#if defined(CONFIG_GPS_RK)
#include "../../../drivers/misc/gps/rk_gps/rk_gps.h"
#endif

#if defined(CONFIG_MT6620)
#include <linux/gps.h>
#endif

#include "../mach-rk30/board-rk3168-ds1006h-camera.c"
#include <plat/key.h>

#if defined(CONFIG_ANDROID_TIMED_GPIO)
#include "../../../drivers/staging/android/timed_gpio.h"
#endif

#if defined (CONFIG_SND_SOC_WM8904)
#include <sound/wm8904.h>
#endif

#if defined(CONFIG_TCHIP_MACH_TR785) || defined(CONFIG_TCHIP_MACH_TR1088)
	#define GS2_ORIGENTATION_MC3230        { 1, 0, 0,0, -1, 0,  0, 0, -1}
       //#define GS2_ORIGENTATION_STK8312       { 0, 0, 1,1, 0, 0,  0, -1, 0} //z,x,y
	#define GS2_ORIGENTATION_STK8312      { 0, 0, -1,-1, 0, 0,  0, -1, 0} //z,x,y
	#define GS2_ORIGENTATION_MMA7660       { 1, 0, 0,0, -1, 0,  0, 0, -1}
	#define GS2_ORIGENTATION_MMA8452        {0, 1, 0, 1, 0, 0, 0, 0, -1}    
#elif defined(CONFIG_TCHIP_MACH_TRQ7_LJ)
     #define GS2_ORIGENTATION_MC3230         {-1, 0, 0, 0, 1, 0, 0, 0, -1}
     #define GS2_ORIGENTATION_STK8312        {0, 1, 0, -1, 0, 0, 0, 0, 1}
     #define GS2_ORIGENTATION_MMA7660        {0, 1, 0, 1, 0, 0, 0, 0, -1} 
	#define GS2_ORIGENTATION_MMA8452        {0, 1, 0, 1, 0, 0, 0, 0, -1}    
#elif defined (CONFIG_TCHIP_MACH_TR101Q)	
       #define GS2_ORIGENTATION_MC3230         {0, -1, 0, 1, 0, 0, 0, 0, 1}
       #define GS2_ORIGENTATION_STK8312        {0, 1, 0, 1, 0, 0, 0, 0, -1}
       #define GS2_ORIGENTATION_MMA7660        {0, 1, 0, 1, 0, 0, 0, 0, -1}
		#define GS2_ORIGENTATION_STK831x 		{0, 1, 0, 0, 1, 0, 0, 0, 1}    
	#define GS2_ORIGENTATION_MMA8452        {-1, 0, 0, 0, 1, 0, 0, 0, -1}    
#else
	#define GS2_ORIGENTATION_MC3230         {0, 1, 0, 1, 0, 0, 0, 0, -1}
	#define GS2_ORIGENTATION_STK8312        {0, 1, 0, 1, 0, 0, 0, 0, -1}
	#define GS2_ORIGENTATION_MMA7660        {0, 1, 0, 1, 0, 0, 0, 0, -1}    
	#define GS2_ORIGENTATION_STK831x 		{0, 1, 0, 0, 1, 0, 0, 0, 1}    
	#define GS2_ORIGENTATION_MMA8452        {0, 1, 0, 1, 0, 0, 0, 0, -1}    
#endif

#if defined(CONFIG_SWITCH_GPIO) 
static struct gpio_switch_platform_data headset_switch_data = {
    .name = "h2w",
    .gpio = RK30_PIN0_PC1, 
};
static struct platform_device device_headset_switch = {
    .name = "switch-gpio",
    .dev  = {
        .platform_data    = &headset_switch_data,          
    }
};
#endif

#if defined (CONFIG_SND_SOC_WM8904)
struct wm8904_drc_cfg wm8904_drc_cfg[] = {
	{
		.name	=  "DRC_SPK",
		.regs	= {0xc1af, 0x324b, 0x0, 0x0607}
	},
	{
		.name	=  "DRC_HP",
		.regs	= {0x01af, 0x3248, 0x0, 0x0}
	}
};
struct wm8904_pdata  wm8904_pdata = {
	.num_drc_cfgs 	= ARRAY_SIZE(wm8904_drc_cfg),
	.drc_cfgs	= &wm8904_drc_cfg,
};
#endif

#if defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
static struct rk29_keys_button key_button[] = {
	{
		.desc	= "play",
		.code	= KEY_POWER,
		.gpio	= RK30_PIN0_PA4, 
		.active_low = PRESS_LEV_LOW,
		.wakeup	= 1,
	},
	{
		.desc   = "vol+",
		.code   = KEY_VOLUMEUP,
		.adc_value      = 1,
		.gpio = INVALID_GPIO,
		.active_low = PRESS_LEV_LOW,
	},
    {
        .desc   = "vol-",
        .code   = KEY_VOLUMEDOWN,
        .adc_value      = 171,
        .gpio   = INVALID_GPIO,
        .active_low = PRESS_LEV_LOW,
    },
};
#else
static struct rk29_keys_button key_button[] = {
	{
		.desc	= "play",
		.code	= KEY_POWER,
		.gpio	= RK30_PIN0_PA4, 
		.active_low = PRESS_LEV_LOW,
		.wakeup	= 1,
	},
    /*
	{
		.desc	= "esc",
		.code	= KEY_BACK,
		.adc_value	= 1,
		.gpio = INVALID_GPIO,
		.active_low = PRESS_LEV_LOW,
	},*/
    {
        .desc   = "vol+",
        .code   = KEY_VOLUMEUP,
#if defined(CONFIG_TCHIP_MACH_TR785) || defined(CONFIG_TCHIP_MACH_XBT_3188)
        .adc_value      = 171,
#else
        .adc_value      = 1,
#endif
        .gpio = INVALID_GPIO,
        .active_low = PRESS_LEV_LOW,
    },
    {
        .desc   = "vol-",
        .code   = KEY_VOLUMEDOWN,
#if !defined(CONFIG_TCHIP_MACH_TR785) && !defined(CONFIG_TCHIP_MACH_XBT_3188)
        .adc_value      = 171,
#else
        .adc_value      = 1,
#endif
        .gpio   = INVALID_GPIO,
        .active_low = PRESS_LEV_LOW,
    },
};
#endif
struct rk29_keys_platform_data rk29_keys_pdata = {
	.buttons	= key_button,
	.nbuttons	= ARRAY_SIZE(key_button),
	.chn	= 1,  //chn: 0-7, if do not use ADC,set 'chn' -1
};

#if defined (CONFIG_RK_HEADSET_DET) || defined (CONFIG_RK_HEADSET_IRQ_HOOK_ADC_DET)
#include "../../../drivers/headset_observe/rk_headset.h"
static int rk_headset_io_init(int gpio)
{
		int ret;
		ret = gpio_request(gpio, "headset_input");
		if(ret) 
			return ret;

		//rk30_mux_api_set(GPIO0B4_I2S8CHSDO0_NAME, GPIO0B_GPIO0B4);
		gpio_pull_updown(gpio, PullDisable);
		gpio_direction_input(gpio);
		mdelay(50);
		return 0;
};

struct rk_headset_pdata rk_headset_info = {
		.Headset_gpio		= RK30_PIN0_PB4,
		.headset_in_type = HEADSET_IN_LOW,
		//.Hook_adc_chn = 2,
		//.hook_key_code = KEY_MEDIA,
		.headset_io_init = rk_headset_io_init, 
};

struct platform_device rk_device_headset = {
		.name	= "rk_headsetdet",
		.id 	= 0,
		.dev    = {
			    .platform_data = &rk_headset_info,
		}
};
#endif

/*
     v1.0 : 	ignore
     v1.1 :      rk610 lvds + rk610 codec + MT5931_MT6622 + light photoresistor + adc/cw2015
     v1.2 :      lvds       + rt5631      + M500          + us5151              + adc
*/
#define DS1006H_V1_2_SUPPORT  1
int get_harware_version()
{
    #if DS1006H_V1_2_SUPPORT
        return 2;
    #else
        return 1;
    #endif
}
EXPORT_SYMBOL_GPL(get_harware_version);

#if defined (CONFIG_TOUCHSCREEN_MELFAS)
#define TOUCH_ENABLE_PIN        RK30_PIN1_PB4
#define TOUCH_INT_PIN           RK30_PIN1_PB2
#define TOUCH_RESET_PIN         RK30_PIN0_PD4
#define TOUCH_PWR_PIN           INVALID_GPIO

#define GPIO_TOUCH_EN  TOUCH_ENABLE_PIN
#define GPIO_TOUCH_INT TOUCH_INT_PIN 
#define GPIO_TOUCH_RST TOUCH_RESET_PIN

int melfas_init_platform_hw(void)
{
	int ret;

	//rk30_mux_api_set(GPIO4D0_SMCDATA8_TRACEDATA8_NAME, GPIO4D_GPIO4D0);
	//rk30_mux_api_set(GPIO4C2_SMCDATA2_TRACEDATA2_NAME, GPIO4C_GPIO4C2);
	//rk30_mux_api_set(GPIO0C7_TRACECTL_SMCADDR3_NAME, GPIO0C_GPIO0C7);
	//printk("%s:0x%x,0x%x\n",__func__,rk30_mux_api_get(GPIO4D0_SMCDATA8_TRACEDATA8_NAME),rk30_mux_api_get(GPIO4C2_SMCDATA2_TRACEDATA2_NAME));
	
	printk("%s\r\n", __func__);
	if (TOUCH_PWR_PIN != INVALID_GPIO) {
			ret = gpio_request(TOUCH_PWR_PIN, "melfas power pin");
			if (ret != 0) {
					gpio_free(TOUCH_PWR_PIN);
					printk("melfas gpio_request power error\n");
					return -EIO;
			}
			gpio_direction_output(TOUCH_PWR_PIN, 0);
			gpio_set_value(TOUCH_PWR_PIN, GPIO_HIGH);
			msleep(100);
			gpio_set_value(TOUCH_PWR_PIN, GPIO_LOW);
	        printk("%s: power pin init ok!\r\n", __func__);
	}
	
	if (TOUCH_ENABLE_PIN != INVALID_GPIO) {
			ret = gpio_request(TOUCH_ENABLE_PIN, "melfas enable pin");
			if (ret != 0) {
					gpio_free(TOUCH_ENABLE_PIN);
					printk("melfas gpio_request enable error\n");
					return -EIO;
			}
			gpio_direction_output(TOUCH_ENABLE_PIN, 0);
			//gpio_set_value(TOUCH_ENABLE_PIN, GPIO_LOW);
			gpio_set_value(TOUCH_ENABLE_PIN, GPIO_HIGH);
			msleep(100);
	}

	if (TOUCH_RESET_PIN != INVALID_GPIO) {
			ret = gpio_request(TOUCH_RESET_PIN, "melfas reset pin");
			if (ret != 0) {
					gpio_free(TOUCH_RESET_PIN);
					printk("melfas gpio_request reset error\n");
					return -EIO;
			}
			gpio_direction_output(TOUCH_RESET_PIN, 0);
			gpio_set_value(TOUCH_RESET_PIN, GPIO_LOW);
			msleep(10);
			gpio_set_value(TOUCH_RESET_PIN, GPIO_HIGH);
			msleep(500);
	        printk("%s: reset pin init ok!\r\n", __func__);
	}
	
	if (GPIO_TOUCH_INT != INVALID_GPIO) {
			ret = gpio_request(GPIO_TOUCH_INT, "melfas irq pin");
			if (ret != 0) {
					gpio_free(GPIO_TOUCH_INT);
					printk("melfas gpio_request irq error\n");
					return -EIO;
			}
			gpio_direction_input(GPIO_TOUCH_INT);
			gpio_pull_updown(TOUCH_INT_PIN, GPIO_HIGH);
	        printk("%s: reset irq init ok!\r\n", __func__);
	}	
	
	return 0;
}

static void melfas_touch_power_enable(int en)
{
//`	gpio_direction_output(TOUCH_RESET_PIN, en);
}


static struct melfas_tsi_platform_data melfas_touch_platform_data = {
	.max_x = 600,
	.max_y = 1024,
	.max_pressure = 255,
	.max_area = 255, // ice cream "max_area"
	//.gpio_scl = GPIO_TSP_SCL,
	//.gpio_sda = GPIO_TSP_SDA,
	.power_enable = melfas_touch_power_enable,
	.init_platform_hw = melfas_init_platform_hw,
};



#endif
#if defined(CONFIG_TOUCHSCREEN_GSLX680) || defined(CONFIG_TOUCHSCREEN_GSLX68X) || defined(CONFIG_TOUCHSCREEN_GSL2682)
#define TOUCH_RESET_PIN RK30_PIN0_PB6
#define TOUCH_EN_PIN NULL
#ifdef CONFIG_TCHIP_MACH_TR1088
#define TOUCH_PWR_PIN RK30_PIN1_PB5
#else
#define TOUCH_PWR_PIN NULL
#endif
#define TOUCH_INT_PIN RK30_PIN1_PB7

int gslx680_init_platform_hw(void)
{

       if(gpio_request(TOUCH_RESET_PIN,NULL) != 0){
                gpio_free(TOUCH_RESET_PIN);
                printk("gslx680_init_platform_hw gpio_request error\n");
                return -EIO;
        }
        if(gpio_request(TOUCH_INT_PIN,NULL) != 0){
                gpio_free(TOUCH_INT_PIN);
                printk("gslx680_init_platform_hw  gpio_request error\n");
                return -EIO;
        }
        if(gpio_request(TOUCH_PWR_PIN,NULL) != 0){
            gpio_free(TOUCH_RESET_PIN);
            printk("gslx680_init_platform_hw gpio_request error\n");
            return -EIO;
        }
        gpio_direction_output(TOUCH_PWR_PIN, GPIO_LOW);
        gpio_direction_output(TOUCH_RESET_PIN, GPIO_HIGH);
        mdelay(10);
        gpio_set_value(TOUCH_RESET_PIN,GPIO_LOW);
        mdelay(10);
        gpio_set_value(TOUCH_RESET_PIN,GPIO_HIGH);
        msleep(300);
        return 0;

}

struct ts_hw_data     gslx680_info = {
    .reset_gpio = TOUCH_RESET_PIN,
    .touch_en_gpio = TOUCH_INT_PIN,
    .init_platform_hw = gslx680_init_platform_hw,
};
#endif

#if defined(CONFIG_TOUCHSCREEN_ICN83XX)
#define TOUCH_INT_PIN		RK30_PIN1_PB7
#define TOUCH_RST_PIN		RK30_PIN0_PB6  //INVALID_GPIO
//if no the pins set 0
struct icn83xx_ts_hw_data { //this struct is define in icn838x_iic_ts.c 
	int reset_gpio;
	int touch_en_gpio;
	int (*init_platform_hw)(void);
	int revert_x;
	int revert_y;
	int revert_xy;
	int screen_max_x;
	int screen_max_y;
};
int icn83xx_init_platform_hw(void)
{
	if(TOUCH_RST_PIN == INVALID_GPIO){
		printk("----------------------->icn830x_rst_pin no defined\n");
		return 0;
	}
	gpio_request(TOUCH_RST_PIN, "icn830x_rst_pin");
	gpio_direction_output(TOUCH_RST_PIN, GPIO_HIGH);
	printk("----------------------->icn830x_rst_pin = %d\n",gpio_get_value(TOUCH_RST_PIN));
 
        return 0;

}

struct icn83xx_ts_hw_data     icn83xx_info = {
	.reset_gpio = TOUCH_RST_PIN,//0
	.touch_en_gpio = 0, //0
	.init_platform_hw = icn83xx_init_platform_hw,//0
	#if defined(CONFIG_TCHIP_MACH_TR726C) && defined(CONFIG_LCD_RK2926_V86)
	.revert_x = 0,
	.revert_y = 1,
	.revert_xy = 0,
	#else
	.revert_x = 1,
	.revert_y = 0,
	.revert_xy = 0,
	#endif
	#if defined(CONFIG_TCHIP_MACH_600P)
	.screen_max_x = 1024,
		#if defined(CONFIG_TCHIP_MACH_TP_SCREEN_DD)
	.screen_max_y = 626,
		#else
	.screen_max_y = 600,
		#endif
	#else //480P
	.screen_max_x = 800,
	.screen_max_y = 480,
	#endif
};
#endif

#if defined(CONFIG_CT36X_TS)

#define TOUCH_MODEL		363
#define TOUCH_MAX_X		1280
#define TOUCH_MAX_y		800
#define TOUCH_RESET_PIN		RK30_PIN0_PB6
#define TOUCH_INT_PIN		RK30_PIN1_PB7

static struct ct36x_platform_data ct36x_info = {
	.model   = TOUCH_MODEL,
	.x_max   = TOUCH_MAX_X,
	.y_max   = TOUCH_MAX_y,

	.rst_io = {
		.gpio = TOUCH_RESET_PIN,
		.active_low = 1,
	},
	.irq_io = {
		.gpio = TOUCH_INT_PIN,
		.active_low = 1,
	},
	.orientation = {1, 0, 0, 1},
};
#endif
static struct spi_board_info board_spi_devices[] = {
};

/***********************************************************
*	rk30  backlight
************************************************************/
#ifdef CONFIG_BACKLIGHT_RK29_BL
#define PWM_ID            3
#define PWM_MODE	  PWM3
#define PWM_EFFECT_VALUE  1

#define LCD_DISP_ON_PIN

#ifdef  LCD_DISP_ON_PIN
#if defined(CONFIG_TCHIP_MACH_TR7088)
#define BL_EN_PIN         RK30_PIN0_PA2//RK30_PIN0_PA2
#define BL_EN_VALUE       GPIO_HIGH
#else
#define BL_EN_PIN         RK30_PIN0_PA2
#define BL_EN_VALUE       GPIO_HIGH
#endif
#endif

int is_backligth_closed = 0;
static int rk29_backlight_pwm_suspend(void);
static int rk29_backlight_pwm_resume(void);

static int rk29_backlight_io_init(void)
{
          int ret =0;
#if defined(CONFIG_TCHIP_MACH_TR7088) && !defined(CONFIG_TCHIP_MACH_TR7088TN) && !defined(CONFIG_TCHIP_MACH_TR7088_CUBE)
	gpio_free(BL_EN_PIN);
    ret = gpio_request(BL_EN_PIN, NULL);
	if (ret != 0) {
		gpio_free(BL_EN_PIN);
	}
    rk29_backlight_pwm_suspend();
    rk29_backlight_pwm_resume();
#else
	iomux_set(PWM_MODE);

#ifdef  LCD_DISP_ON_PIN
	ret = gpio_request(BL_EN_PIN, NULL);
	if (ret != 0) {
		gpio_free(BL_EN_PIN);
	}

	gpio_direction_output(BL_EN_PIN, 0);
	gpio_set_value(BL_EN_PIN, BL_EN_VALUE);
#endif
#endif
            return ret;
}

static int rk29_backlight_io_deinit(void)
{
	int ret = 0, pwm_gpio;

#ifdef  LCD_DISP_ON_PIN
	gpio_free(BL_EN_PIN);
#endif
	is_backligth_closed = 1;
	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	gpio_request(pwm_gpio, NULL);
	gpio_direction_output(pwm_gpio, GPIO_LOW);
	return ret;
}

static int rk29_backlight_pwm_suspend(void)
{
	int ret = 0, pwm_gpio;

	pwm_gpio = iomux_mode_to_gpio(PWM_MODE);
	if (gpio_request(pwm_gpio, NULL)) {
		printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
		return -1;
	}
	is_backligth_closed = 1;
	gpio_direction_output(pwm_gpio, GPIO_LOW);
#ifdef  LCD_DISP_ON_PIN
	gpio_direction_output(BL_EN_PIN, 0);
	gpio_set_value(BL_EN_PIN, !BL_EN_VALUE);
#endif
	return ret;
}

static int rk29_backlight_pwm_resume(void)
{
	int pwm_gpio = iomux_mode_to_gpio(PWM_MODE);

#if defined(CONFIG_TCHIP_MACH_TR7088) && !defined(CONFIG_TCHIP_MACH_TR7088TN) && !defined(CONFIG_TCHIP_MACH_TR7088_CUBE)
	gpio_direction_output(BL_EN_PIN, 1);
	gpio_set_value(BL_EN_PIN, BL_EN_VALUE);
	msleep(200);
        gpio_free(pwm_gpio);
        iomux_set(PWM_MODE);
#else
    gpio_free(pwm_gpio);
	iomux_set(PWM_MODE);
#ifdef  LCD_DISP_ON_PIN
#if defined(CONFIG_TCHIP_MACH_TR838)
	msleep(100);
#else
    msleep(30);
#endif
	gpio_direction_output(BL_EN_PIN, 1);
	gpio_set_value(BL_EN_PIN, BL_EN_VALUE);
#endif
#endif
	is_backligth_closed = 0;
	return 0;
}

static struct rk29_bl_info rk29_bl_info = {
#if defined(CONFIG_TCHIP_MACH_TR785)
        .min_brightness = 65,
        .max_brightness = 165,//220,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 30 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR1088)
        .min_brightness = 50,
        .max_brightness = 240,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 10 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR7888)
        .min_brightness = 50,
        .max_brightness = 240,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 10 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR8088)
        .min_brightness = 50,
        .max_brightness = 240,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 10 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR7088TN)
        .min_brightness = 50,
        .max_brightness = 250,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 10 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR7088_CUBE)
        .min_brightness = 45,
        .max_brightness = 154,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 10 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR7088)
        .min_brightness = 30,
        .max_brightness = 145,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,//BRIGHTNESS_MODE_CONIC,
	.pre_div = 30 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TRQ7_LJ)
        .min_brightness = 40,
        .max_brightness = 250,
		.brightness_mode = BRIGHTNESS_MODE_ELONGATION,
		.pre_div = 30 * 1000,  // pwm output clk: 30k;
		.pre_div = 10 * 1000,  // pwm output clk: 30k;
		.pwm_id = PWM_ID,
		.bl_ref = !PWM_EFFECT_VALUE,
		.io_init = rk29_backlight_io_init,
		.io_deinit = rk29_backlight_io_deinit,
		.pwm_suspend = rk29_backlight_pwm_suspend,
		.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_TR838)
    .min_brightness = 40,
    .max_brightness = 220,
    .brightness_mode = BRIGHTNESS_MODE_ELONGATION,
	.pre_div = 30 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined(CONFIG_TCHIP_MACH_XBT_3188)
    .min_brightness = 10,
    .max_brightness = 220,
    .brightness_mode = BRIGHTNESS_MODE_ELONGATION,
	.pre_div = 20 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = !PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#elif defined (CONFIG_TCHIP_MACH_TR101Q)
        .min_brightness = 20,
        .max_brightness = 240,
        .brightness_mode =BRIGHTNESS_MODE_ELONGATION,
	.pre_div = 30 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#else
        .min_brightness = 65,
        .max_brightness = 150,
        .brightness_mode =BRIGHTNESS_MODE_CONIC,
	.pre_div = 30 * 1000,  // pwm output clk: 30k;
	.pwm_id = PWM_ID,
	.bl_ref = PWM_EFFECT_VALUE,
	.io_init = rk29_backlight_io_init,
	.io_deinit = rk29_backlight_io_deinit,
	.pwm_suspend = rk29_backlight_pwm_suspend,
	.pwm_resume = rk29_backlight_pwm_resume,
#endif
};

static struct platform_device rk29_device_backlight = {
	.name	= "rk29_backlight",
	.id 	= -1,
	.dev	= {
		.platform_data  = &rk29_bl_info,
	}
};

#endif

#if defined (CONFIG_SENSORS_STK8312)

#if defined(CONFIG_TCHIP_MACH_TRQ7_LJ)
#define STK8312_INT_PIN   RK30_PIN1_PB5
#else
#define STK8312_INT_PIN   RK30_PIN0_PB7
#endif

static struct gsensor_platform_data STK8312_info = {
    .model= 8312,
    .swap_xy = 0,
    .swap_xyz = 1,
    .orientation = GS2_ORIGENTATION_STK8312,
};
#endif


#if defined (CONFIG_GS_STK831X)
#define STK831X_INT_PIN RK30_PIN0_PB7
static int stk831x_init_platform_hw(void)
{
	//rk30_mux_api_set(GPIO1B2_SPI_RXD_UART1_SIN_NAME, GPIO1B_GPIO1B2);

	return 0;
}
static struct sensor_platform_data stk831x_info = {
	.type = SENSOR_TYPE_ACCEL,
	.irq_enable = 0,
	.poll_delay_ms = 30,
    .init_platform_hw = stk831x_init_platform_hw,
	.orientation = GS2_ORIGENTATION_STK831x,
};
#endif

#if defined (CONFIG_GS_MC3XXX)
#define MC3XXX_INT_PIN	RK30_PIN0_PB7
static int MC3XXX_init_platform_hw(void)
{
/*
	if(gpio_request(MC3XXX_INT_PIN,NULL) != 0){
		gpio_free(MC3XXX_INT_PIN);
		printk("MC3XXX_init_platform_hw gpio_request error\n");
		return -EIO;
	}
	gpio_pull_updown(MC3XXX_INT_PIN, 1);
*/
	return 0;
}

static struct sensor_platform_data MC3XXX_info = {
	.type = SENSOR_TYPE_ACCEL,
	.irq_enable = 0,
	.poll_delay_ms = 30,
        .init_platform_hw = MC3XXX_init_platform_hw,
        .orientation = GS2_ORIGENTATION_MC3230,
};
#endif


/*MMA8452 gsensor*/
#if defined (CONFIG_GS_MMA8452)
#define MMA8452_INT_PIN   RK30_PIN0_PB7

static int mma8452_init_platform_hw(void)
{
	return 0;
}

static struct sensor_platform_data mma8452_info = {
	.type = SENSOR_TYPE_ACCEL,
	.irq_enable = 1,
	.poll_delay_ms = 30,
        .init_platform_hw = mma8452_init_platform_hw,
#if defined(CONFIG_TCHIP_MACH_XBT_3188)
        .orientation = {0, 1, 0, 1, 0, 0, 0, 0, -1},
#else
        .orientation = GS2_ORIGENTATION_MMA8452,
#endif

};
#endif

/* mma7660 gsensor */
#if defined (CONFIG_GS_MMA7660)

#define MMA7660_INT_PIN   RK30_PIN0_PB7
static int mma7660_init_platform_hw(void)
{
	return 0;
}
static struct sensor_platform_data mma7660_info = {
	.type = SENSOR_TYPE_ACCEL,
	.irq_enable = 1,
	.poll_delay_ms = 30,
    .init_platform_hw = mma7660_init_platform_hw,
    .orientation = GS2_ORIGENTATION_MMA7660,
};
#endif
/* end of mma7660 */

#if defined (CONFIG_GS_LIS3DH)
#define LIS3DH_INT_PIN   RK30_PIN0_PB7

static int lis3dh_init_platform_hw(void)
{

        return 0;
}

static struct sensor_platform_data lis3dh_info = {
	.type = SENSOR_TYPE_ACCEL,
	.irq_enable = 1,
	.poll_delay_ms = 30,
        .init_platform_hw = lis3dh_init_platform_hw,
	.orientation = {-1, 0, 0, 0, 1, 0, 0, 0, -1},
};
#endif

#if defined(CONFIG_BATTERY_CW2015)
#if defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
static struct cw2015_platform_data cw2015_info =
{
    .dc_det_pin = RK30_PIN0_PB1,//INVALID_GPIO,
    //.batt_low_pin = RK30_PIN0_PB1,
    //.charge_ok_pin   = RK30_PIN0_PA6,
    .dc_det_level = GPIO_LOW,
    .batt_low_level = GPIO_LOW,
    //.charge_ok_level = GPIO_HIGH,
};
#elif defined(CONFIG_TCHIP_MACH_TR785)
static struct cw2015_platform_data cw2015_info =
{
    .dc_det_pin = INVALID_GPIO,
    //.batt_low_pin = RK30_PIN0_PB1,
    //.charge_ok_pin   = RK30_PIN0_PA6,
    .dc_det_level = GPIO_LOW,
    .batt_low_level = GPIO_LOW,
    //.charge_ok_level = GPIO_HIGH,
};
#else
static struct cw2015_platform_data cw2015_info =
{
    .dc_det_pin = RK30_PIN0_PB2,//INVALID_GPIO,
    //.batt_low_pin = RK30_PIN0_PB1,
    //.charge_ok_pin   = RK30_PIN0_PA6,
    .dc_det_level = GPIO_LOW,
    .batt_low_level = GPIO_LOW,
    //.charge_ok_level = GPIO_HIGH,
};
#endif
#endif

#if defined (CONFIG_COMPASS_AK8963)
static struct sensor_platform_data akm8963_info =
{
       .type = SENSOR_TYPE_COMPASS,
       .irq_enable = 1,
       .poll_delay_ms = 30,
       .m_layout = 
       {
               {
                       {0, 1, 0},
                       {1, 0, 0},
                       {0, 0, -1},
               },

               {
                       {1, 0, 0},
                       {0, 1, 0},
                       {0, 0, 1},
               },

               {
                       {0, -1, 0},
                       {-1, 0, 0},
                       {0, 0, -1},
               },

               {
                       {1, 0, 0},
                       {0, 1, 0},
                       {0, 0, 1},
               },
       }
};

#endif

#if defined(CONFIG_LS_STK2203)
static struct sensor_platform_data light_stk2203_info = {
    .type = SENSOR_TYPE_LIGHT,
    .irq_enable = 1,
    .poll_delay_ms = 200,
};
#endif

#if defined(CONFIG_LS_PHOTORESISTOR)
static struct sensor_platform_data light_photoresistor_info = {
	.type = SENSOR_TYPE_LIGHT,
	.irq_enable = 0,
        .address = 2   ,
	.poll_delay_ms = 200,
};
#endif

#if defined (CONFIG_COMPASS_AK8975)
static struct sensor_platform_data akm8975_info =
{
	.type = SENSOR_TYPE_COMPASS,
	.irq_enable = 1,
	.poll_delay_ms = 30,
	.m_layout = 
	{
		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1},
		},

		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1},
		},

		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1},
		},

		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1},
		},
	}
};

#endif

#if defined(CONFIG_MT6229)
static int mt6229_io_init(void)
{
      #if 0
	rk30_mux_api_set(GPIO2B6_LCDC1DATA14_SMCADDR18_TSSYNC_NAME, GPIO2B_GPIO2B6);
      k30_mux_api_set(GPIO4D2_SMCDATA10_TRACEDATA10_NAME, GPIO4D_GPIO4D2);
	rk30_mux_api_set(GPIO2B7_LCDC1DATA15_SMCADDR19_HSADCDATA7_NAME, GPIO2B_GPIO2B7);
	rk30_mux_api_set(GPIO2C0_LCDCDATA16_GPSCLK_HSADCCLKOUT_NAME, GPIO2C_GPIO2C0);
	rk30_mux_api_set(GPIO2C1_LCDC1DATA17_SMCBLSN0_HSADCDATA6_NAME, GPIO2C_GPIO2C1);
	rk30_mux_api_set(GPIO2C1_LCDC1DATA17_SMCBLSN0_HSADCDATA6_NAME, GPIO2C_GPIO2C1);
      #endif
	 return 0;
}

static int mt6229_io_deinit(void)
{
	
	return 0;
}
 
struct rk29_mt6229_data rk29_mt6229_info = {
	.io_init = mt6229_io_init,
  	.io_deinit = mt6229_io_deinit,
	.modem_power_en = RK30_PIN0_PC6,
	.bp_power = RK30_PIN2_PD5,
	.modem_usb_en = RK30_PIN0_PC7,
	.modem_uart_en = RK30_PIN2_PD4,
	.bp_wakeup_ap = RK30_PIN0_PC5,
	.ap_ready = RK30_PIN0_PC4,

};
struct platform_device rk29_device_mt6229 = {	
        .name = "mt6229",	
    	.id = -1,	
	.dev		= {
		.platform_data = &rk29_mt6229_info,
	}    	
    };
#endif

#if defined(CONFIG_GYRO_L3G4200D)

#include <linux/l3g4200d.h>
#define L3G4200D_INT_PIN  RK30_PIN0_PB4

static int l3g4200d_init_platform_hw(void)
{
	return 0;
}

static struct sensor_platform_data l3g4200d_info = {
	.type = SENSOR_TYPE_GYROSCOPE,
	.irq_enable = 1,
	.poll_delay_ms = 0,
	.orientation = {1, 0, 0 , 0 , -1, 0, 0, 0, -1},
	.init_platform_hw = l3g4200d_init_platform_hw,
	.x_min = 40,//x_min,y_min,z_min = (0-100) according to hardware
	.y_min = 40,
	.z_min = 20,
};

#endif

#ifdef CONFIG_LS_CM3217
static struct sensor_platform_data cm3217_info = {
	.type = SENSOR_TYPE_LIGHT,
	.irq_enable = 0,
	.poll_delay_ms = 500,
};

#endif

#ifdef CONFIG_LS_CM3232
static struct sensor_platform_data cm3232_info = {
	.type = SENSOR_TYPE_LIGHT,
	.irq_enable = 0,
	.poll_delay_ms = 500,
};
#endif


#ifdef CONFIG_FB_ROCKCHIP

#if DS1006H_V1_2_SUPPORT
#if defined(CONFIG_TCHIP_MACH_TR1088)
#define LCD_CS_PIN         INVALID_GPIO
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0//INVALID_GPIO
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_PWR_PIN        RK30_PIN1_PB2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined(CONFIG_TCHIP_MACH_TR7088)
#define LCD_CS_PIN         INVALID_GPIO
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0//INVALID_GPIO
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_PWR_PIN        RK30_PIN1_PB2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined(CONFIG_TCHIP_MACH_TR7888)
#define LCD_CS_PIN         INVALID_GPIO
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0//RK30_PIN0_PB0//INVALID_GPIO
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_PWR_PIN        INVALID_GPIO
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_TCON_EN			RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		RK30_PIN1_PB3
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined (CONFIG_TCHIP_MACH_TRQ7_LJ)
#define LCD_CS_PIN         RK30_PIN3_PD4        // ssd2828 RESET pin
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_CS_IOMUX       GPIO3_D4
#define LCD_PWR_PIN        INVALID_GPIO    // LCD_EN pin
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_EN_PIN         RK30_PIN0_PB0    // LCD_EN pin
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined (CONFIG_TCHIP_MACH_TR785)
#define LCD_CS_PIN         RK30_PIN1_PB5
#define LCD_CS_IOMUX       GPIO1_B5
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_PWR_PIN        INVALID_GPIO
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined (CONFIG_TCHIP_MACH_XBT_3188)
#define LCD_CS_PIN         RK30_PIN3_PD4
#define LCD_CS_IOMUX       GPIO3_D4
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_PWR_PIN        INVALID_GPIO
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#elif defined (CONFIG_TCHIP_MACH_TR101Q)
#define LCD_CS_PIN         RK30_PIN3_PD4
#define LCD_CS_IOMUX       GPIO3_D4
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_PWR_PIN        INVALID_GPIO
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#else
#define LCD_CS_PIN         RK30_PIN1_PB5
#define LCD_CS_IOMUX       GPIO1_B5
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_PWR_PIN        RK30_PIN1_PB2
#define LCD_PWR_IOMUX      GPIO1_B2
#define LCD_PWR_VALUE      GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			INVALID_GPIO//RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#endif
#else
#define LCD_CS_PIN         INVALID_GPIO
#define LCD_CS_VALUE       GPIO_HIGH
#define LCD_EN_PIN         RK30_PIN0_PB0
#define LCD_EN_VALUE       GPIO_LOW
#define LCD_TCON_EN			RK30_PIN0_PB4
#define LCD_TCON_EN_VALUE	GPIO_LOW
#define LCD_LVDS_STB		INVALID_GPIO//RK30_PIN1_PB3
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#define LCD_LVDS_STB		INVALID_GPIO
#define LCD_LVDS_STB_VALUE	GPIO_HIGH
#endif

static int rk_fb_io_init(struct rk29_fb_setting_info *fb_setting)
{
	int ret = 0;

	if(LCD_CS_PIN !=INVALID_GPIO)
	{
		ret = gpio_request(LCD_CS_PIN, NULL);
		if (ret != 0)
		{
			gpio_free(LCD_CS_PIN);
			printk(KERN_ERR "request lcd cs pin fail!\n");
			return -1;
		}
		else
		{
#ifdef LCD_CS_IOMUX
		iomux_set(LCD_CS_IOMUX);
#endif
			gpio_direction_output(LCD_CS_PIN, LCD_CS_VALUE);
		}
	}

	if(LCD_EN_PIN !=INVALID_GPIO)
	{
		ret = gpio_request(LCD_EN_PIN, NULL);
		if (ret != 0)
		{
			gpio_free(LCD_EN_PIN);
			printk(KERN_ERR "request lcd en pin fail!\n");
			return -1;
		}
		else
		{
			gpio_direction_output(LCD_EN_PIN, LCD_EN_VALUE);
#if defined(CONFIG_TCHIP_MACH_TR7088) && !defined(CONFIG_TCHIP_MACH_TR7088TN) && !defined(CONFIG_TCHIP_MACH_TR7088_CUBE)
			msleep(360);
#endif
		}
	}
	if(LCD_PWR_PIN !=INVALID_GPIO)
    	{
      		ret = gpio_request(LCD_PWR_PIN, NULL);
      		if (ret != 0)
      	{
        	gpio_free(LCD_PWR_PIN);
        	printk(KERN_ERR "request lcd en pin fail!\n");
        	return -1;
      	}
      	else
      	{
        	gpio_direction_output(LCD_PWR_PIN, LCD_PWR_VALUE);
      	}
    }
	if(LCD_TCON_EN !=INVALID_GPIO)
	{
  		ret = gpio_request(LCD_TCON_EN, NULL);
  		if (ret != 0)
      	{
        	gpio_free(LCD_TCON_EN);
        	printk(KERN_ERR "request lcd tcon en pin fail!\n");
        	return -1;
      	}
      	else
      	{
        	gpio_direction_output(LCD_TCON_EN, LCD_TCON_EN_VALUE);
      	}
    }
	if(LCD_LVDS_STB !=INVALID_GPIO)
	{
  		ret = gpio_request(LCD_LVDS_STB, NULL);
  		if (ret != 0)
      	{
        	gpio_free(LCD_LVDS_STB);
        	printk(KERN_ERR "request lcd tcon en pin fail!\n");
        	return -1;
      	}
      	else
      	{
        	gpio_direction_output(LCD_LVDS_STB, LCD_LVDS_STB_VALUE);
      	}
    }
	//msleep(300);
	return 0;
}
static int rk_fb_io_disable(void)
{
	if(LCD_CS_PIN !=INVALID_GPIO)
	{
		gpio_set_value(LCD_CS_PIN, !LCD_CS_VALUE);
	}
	if(LCD_EN_PIN !=INVALID_GPIO)
	{
		gpio_set_value(LCD_EN_PIN, !LCD_EN_VALUE);
	}
	if(LCD_PWR_PIN !=INVALID_GPIO)
      	{
        	gpio_set_value(LCD_PWR_PIN, !LCD_PWR_VALUE);
      	}
	if(LCD_TCON_EN !=INVALID_GPIO)
  	{
    	gpio_set_value(LCD_TCON_EN, !LCD_TCON_EN_VALUE);
  	}
	if(LCD_LVDS_STB !=INVALID_GPIO)
  	{
    	gpio_set_value(LCD_LVDS_STB, !LCD_LVDS_STB_VALUE);
  	}
	return 0;
}
static int rk_fb_io_enable(void)
{
	if(LCD_CS_PIN !=INVALID_GPIO)
	{
		gpio_set_value(LCD_CS_PIN, LCD_CS_VALUE);
	}
	if(LCD_EN_PIN !=INVALID_GPIO)
	{
		gpio_set_value(LCD_EN_PIN, LCD_EN_VALUE);
	}
	if(LCD_PWR_PIN !=INVALID_GPIO)
      	{
        	gpio_set_value(LCD_PWR_PIN, LCD_PWR_VALUE);
      	}
	if(LCD_TCON_EN !=INVALID_GPIO)
  	{
    	gpio_set_value(LCD_TCON_EN, LCD_TCON_EN_VALUE);
  	}
	if(LCD_LVDS_STB !=INVALID_GPIO)
  	{
    	gpio_set_value(LCD_LVDS_STB, LCD_LVDS_STB_VALUE);
  	}
	return 0;
}

#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TR7088) || defined (CONFIG_TCHIP_MACH_TRQ7) || defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_XBT_3188) || defined(CONFIG_TCHIP_MACH_TR101Q)


#if defined(CONFIG_LCDC1_RK3188)
struct rk29fb_info lcdc1_screen_info = {
	.prop           = EXTEND,       //extend display device
       .lcd_info  = NULL,
       .set_screen_info = hdmi_init_lcdc,

};
#endif

#if defined(CONFIG_LCDC0_RK3188)
struct rk29fb_info lcdc0_screen_info = {
	.prop	   = PRMRY,		//primary display device
	.io_init   = rk_fb_io_init,
	.io_disable = rk_fb_io_disable,
	.io_enable = rk_fb_io_enable,
	.set_screen_info = set_lcd_info,
	
};
#endif

#else
#if defined(CONFIG_LCDC0_RK3188)
struct rk29fb_info lcdc0_screen_info = {
	.prop           = EXTEND,       //extend display device
       .lcd_info  = NULL,
       .set_screen_info = hdmi_init_lcdc,

};
#endif

#if defined(CONFIG_LCDC1_RK3188)
struct rk29fb_info lcdc1_screen_info = {
	.prop	   = PRMRY,		//primary display device
	.io_init   = rk_fb_io_init,
	.io_disable = rk_fb_io_disable,
	.io_enable = rk_fb_io_enable,
	.set_screen_info = set_lcd_info,
	
};
#endif

#endif

static struct resource resource_fb[] = {
	[0] = {
		.name  = "fb0 buf",
		.start = 0,
		.end   = 0,//RK30_FB0_MEM_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name  = "ipp buf",  //for rotate
		.start = 0,
		.end   = 0,//RK30_FB0_MEM_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.name  = "fb2 buf",
		.start = 0,
		.end   = 0,//RK30_FB0_MEM_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device device_fb = {
	.name		= "rk-fb",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(resource_fb),
	.resource	= resource_fb,
};
#endif

#if defined(CONFIG_ARCH_RK3188)
static struct resource resource_mali[] = {
	[0] = {
	.name  = "ump buf",
	.start = 0,
	.end   = 0,
	.flags = IORESOURCE_MEM,
	},

};

static struct platform_device device_mali= {
	.name		= "mali400_ump",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(resource_mali),
	.resource	= resource_mali,
};
#endif

#if defined(CONFIG_LCDC0_RK3188)
static struct resource resource_lcdc0[] = {
	[0] = {
		.name  = "lcdc0 reg",
		.start = RK30_LCDC0_PHYS,
		.end   = RK30_LCDC0_PHYS + RK30_LCDC0_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	
	[1] = {
		.name  = "lcdc0 irq",
		.start = IRQ_LCDC0,
		.end   = IRQ_LCDC0,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device device_lcdc0 = {
	.name		  = "rk30-lcdc",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(resource_lcdc0),
	.resource	  = resource_lcdc0,
	.dev 		= {
		.platform_data = &lcdc0_screen_info,
	},
};
#endif
#if defined(CONFIG_LCDC1_RK3188) 
static struct resource resource_lcdc1[] = {
	[0] = {
		.name  = "lcdc1 reg",
		.start = RK30_LCDC1_PHYS,
		.end   = RK30_LCDC1_PHYS + RK30_LCDC1_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name  = "lcdc1 irq",
		.start = IRQ_LCDC1,
		.end   = IRQ_LCDC1,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device device_lcdc1 = {
	.name		  = "rk30-lcdc",
	.id		  = 1,
	.num_resources	  = ARRAY_SIZE(resource_lcdc1),
	.resource	  = resource_lcdc1,
	.dev 		= {
		.platform_data = &lcdc1_screen_info,
	},
};
#endif

#if defined(CONFIG_MFD_RK610)
#define RK610_RST_PIN 			RK30_PIN3_PB2
static int rk610_power_on_init(void)
{
	int ret;
	if(RK610_RST_PIN != INVALID_GPIO)
	{
		ret = gpio_request(RK610_RST_PIN, "rk610 reset");
		if (ret)
		{
			printk(KERN_ERR "rk610_control_probe request gpio fail\n");
		}
		else 
		{
			gpio_direction_output(RK610_RST_PIN, GPIO_HIGH);
			msleep(100);
			gpio_direction_output(RK610_RST_PIN, GPIO_LOW);
			msleep(100);
	    		gpio_set_value(RK610_RST_PIN, GPIO_HIGH);
		}
	}

	return 0;
	
}


static struct rk610_ctl_platform_data rk610_ctl_pdata = {
	.rk610_power_on_init = rk610_power_on_init,
};
#endif



#ifdef CONFIG_SND_SOC_RK610
static int rk610_codec_io_init(void)
{
//if need iomux.
//Must not gpio_request
	return 0;
}

static struct rk610_codec_platform_data rk610_codec_pdata = {
	.spk_ctl_io = RK30_PIN2_PD7,
	.io_init = rk610_codec_io_init,
	.boot_depop = 1,
};
#endif

#if defined(CONFIG_MFD_RK616)

#if defined(CONFIG_TCHIP_MACH_BACK_MUSIC)
#define RK616_RST_PIN 			RK30_PIN1_PB5
#else
#define RK616_RST_PIN 			RK30_PIN3_PB2
#endif
#if defined(CONFIG_TCHIP_MACH_TR1088)  || defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
#define RK616_PWREN_PIN			RK30_PIN0_PA3
#else
#define RK616_PWREN_PIN			INVALID_GPIO//RK30_PIN0_PA3
#endif
#define RK616_SCL_RATE			(100*1000)   //i2c scl rate
static int rk616_power_on_init(void)
{
	int ret;

	if(RK616_PWREN_PIN != INVALID_GPIO)
	{
		ret = gpio_request(RK616_PWREN_PIN, "rk616 pwren");
		if (ret)
		{
			printk(KERN_ERR "rk616 pwren gpio request fail\n");
		}
		else 
		{
			gpio_direction_output(RK616_PWREN_PIN,GPIO_HIGH);
		}
	}
	
	if(RK616_RST_PIN != INVALID_GPIO)
	{
		ret = gpio_request(RK616_RST_PIN, "rk616 reset");
		if (ret)
		{
			printk(KERN_ERR "rk616 reset gpio request fail\n");
		}
		else 
		{
			gpio_direction_output(RK616_RST_PIN, GPIO_HIGH);
			msleep(2);
			gpio_direction_output(RK616_RST_PIN, GPIO_LOW);
			msleep(10);
	    		gpio_set_value(RK616_RST_PIN, GPIO_HIGH);
		}
	}

	return 0;
	
}


static int rk616_power_deinit(void)
{
	gpio_set_value(RK616_PWREN_PIN,GPIO_LOW);
	gpio_set_value(RK616_RST_PIN,GPIO_LOW);
	gpio_free(RK616_PWREN_PIN);
	gpio_free(RK616_RST_PIN);
	
	return 0;
}
#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TR7088) || defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
static struct rk616_platform_data rk616_pdata = {
	.power_init = rk616_power_on_init,
	.power_deinit = rk616_power_deinit,
	.scl_rate   = RK616_SCL_RATE,
	.lcd0_func = INPUT,             //port lcd0 as input
	.lcd1_func = INPUT,             //port lcd1 as input
 	.lvds_ch_nr = 1,		//the number of used lvds channel  
	.hdmi_irq = INVALID_GPIO,
	.spk_ctl_gpio = RK30_PIN2_PD7,
	.hp_ctl_gpio = RK30_PIN0_PC7,//RK30_PIN0_PC1,
    //.phone_ctl_gpio = RK30_PIN0_PC7,
};
#elif defined(CONFIG_TCHIP_MACH_TR838)
static struct rk616_platform_data rk616_pdata = {
	.power_init = rk616_power_on_init,
	.power_deinit = rk616_power_deinit,
	.scl_rate   = RK616_SCL_RATE,
	.lcd0_func = INPUT,             //port lcd0 as input
	.lcd1_func = INPUT,             //port lcd1 as input
 	.lvds_ch_nr = 1,		//the number of used lvds channel  
	.hdmi_irq = RK30_PIN2_PD6,//INVALID_GPIO,//RK30_PIN2_PD6,
	.spk_ctl_gpio = RK30_PIN2_PD7,
	.hp_ctl_gpio = INVALID_GPIO,//RK30_PIN0_PC7,
    //.phone_ctl_gpio = RK30_PIN0_PC7,
};
#else
static struct rk616_platform_data rk616_pdata = {
	.power_init = rk616_power_on_init,
	.power_deinit = rk616_power_deinit,
	.scl_rate   = RK616_SCL_RATE,
	.lcd0_func = INPUT,             //port lcd0 as input
	.lcd1_func = INPUT,             //port lcd1 as input
 	.lvds_ch_nr = 1,		//the number of used lvds channel  
	.hdmi_irq = RK30_PIN2_PD6,
	.spk_ctl_gpio = RK30_PIN2_PD7,
	.hp_ctl_gpio = RK30_PIN0_PC7,//RK30_PIN0_PC1,
    //.phone_ctl_gpio = RK30_PIN0_PC7,
};
#endif

#endif

#ifdef CONFIG_RK_HDMI
#if defined(CONFIG_TCHIP_MACH_TR838)
#define RK_HDMI_RST_PIN 			INVALID_GPIO//RK30_PIN3_PB2
#else
#define RK_HDMI_RST_PIN 			RK30_PIN3_PB2
#endif

#define RK_HDMI_EN_PIN              RK30_PIN3_PA0
#define RK_HDMI_EN_ENABLE           GPIO_LOW
#define RK_HDMI_EN_IOMUX            GPIO3_A0

static int rk_hdmi_power_init(void)
{
	int ret;

	if(RK_HDMI_EN_PIN != INVALID_GPIO)
	{
	    iomux_set(RK_HDMI_EN_IOMUX);
	    if (gpio_request(RK_HDMI_EN_PIN, NULL)) {
	        printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
	        return -1;
	    }
		gpio_direction_output(RK_HDMI_EN_PIN, RK_HDMI_EN_ENABLE);
		printk("func %s, line %d: set  RK_HDMI_EN_PIN low=%d\n", __FUNCTION__, __LINE__,gpio_get_value(RK_HDMI_EN_PIN) );
	}


	if(RK_HDMI_RST_PIN != INVALID_GPIO)
	{
		if (gpio_request(RK_HDMI_RST_PIN, NULL)) {
			printk("func %s, line %d: request gpio fail\n", __FUNCTION__, __LINE__);
			return -1;
		}
		gpio_direction_output(RK_HDMI_RST_PIN, GPIO_LOW);
		gpio_set_value(RK_HDMI_RST_PIN, GPIO_LOW);
		msleep(100);
		gpio_set_value(RK_HDMI_RST_PIN, GPIO_HIGH);
		msleep(50);
	}
	return 0;
}
static struct rk_hdmi_platform_data rk_hdmi_pdata = {
	.io_init = rk_hdmi_power_init,
};
#endif
#ifdef CONFIG_ION
#define ION_RESERVE_SIZE        (80 * SZ_1M)
#define ION_RESERVE_SIZE_120M   (120 * SZ_1M)
static struct ion_platform_data rk30_ion_pdata = {
	.nr = 1,
	.heaps = {
		{
			.type = ION_HEAP_TYPE_CARVEOUT,
			.id = ION_NOR_HEAP_ID,
			.name = "norheap",
//			.size = ION_RESERVE_SIZE,
		}
	},
};

static struct platform_device device_ion = {
	.name = "ion-rockchip",
	.id = 0,
	.dev = {
		.platform_data = &rk30_ion_pdata,
	},
};
#endif

/**************************************************************************************************
 * SDMMC devices,  include the module of SD,MMC,and sdio.noted by xbw at 2012-03-05
**************************************************************************************************/
#ifdef CONFIG_SDMMC_RK29
#include "board-rk3188-ds1006h-sdmmc-config.c"
#include "../plat-rk/rk-sdmmc-ops.c"
#include "../plat-rk/rk-sdmmc-wifi.c"
#endif //endif ---#ifdef CONFIG_SDMMC_RK29

#ifdef CONFIG_SDMMC0_RK29
static int rk29_sdmmc0_cfg_gpio(void)
{
#ifdef CONFIG_SDMMC_RK29_OLD
	iomux_set(MMC0_CMD);
	iomux_set(MMC0_CLKOUT);
	iomux_set(MMC0_D0);
	iomux_set(MMC0_D1);
	iomux_set(MMC0_D2);
	iomux_set(MMC0_D3);

	iomux_set_gpio_mode(iomux_mode_to_gpio(MMC0_DETN));

	gpio_request(RK30_PIN3_PA7, "sdmmc-power");
	gpio_direction_output(RK30_PIN3_PA7, GPIO_LOW);

#else
	rk29_sdmmc_set_iomux(0, 0xFFFF);

    #if defined(CONFIG_SDMMC0_RK29_SDCARD_DET_FROM_GPIO)
        #if SDMMC_USE_NEW_IOMUX_API
        iomux_set_gpio_mode(iomux_gpio_to_mode(RK29SDK_SD_CARD_DETECT_N));
        #else
        rk30_mux_api_set(RK29SDK_SD_CARD_DETECT_PIN_NAME, RK29SDK_SD_CARD_DETECT_IOMUX_FGPIO);
        #endif
    #else
        #if SDMMC_USE_NEW_IOMUX_API       
        iomux_set(MMC0_DETN);
        #else
        rk30_mux_api_set(RK29SDK_SD_CARD_DETECT_PIN_NAME, RK29SDK_SD_CARD_DETECT_IOMUX_FMUX);
        #endif
    #endif	

#if defined(CONFIG_SDMMC0_RK29_WRITE_PROTECT)
	gpio_request(SDMMC0_WRITE_PROTECT_PIN, "sdmmc-wp");
	gpio_direction_input(SDMMC0_WRITE_PROTECT_PIN);
#endif

#endif

	return 0;
}

#define CONFIG_SDMMC0_USE_DMA
struct rk29_sdmmc_platform_data default_sdmmc0_data = {
	.host_ocr_avail =
	    (MMC_VDD_25_26 | MMC_VDD_26_27 | MMC_VDD_27_28 | MMC_VDD_28_29 |
	     MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 |
	     MMC_VDD_33_34 | MMC_VDD_34_35 | MMC_VDD_35_36),
	.host_caps =
	    (MMC_CAP_4_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED),
	.io_init = rk29_sdmmc0_cfg_gpio,

#if !defined(CONFIG_SDMMC_RK29_OLD)
	.set_iomux = rk29_sdmmc_set_iomux,
#endif
#ifdef USE_SDMMC_DATA4_DATA7	
    .emmc_is_selected = NULL,
#endif
	.dma_name = "sd_mmc",
#ifdef CONFIG_SDMMC0_USE_DMA
	.use_dma = 1,
#else
	.use_dma = 0,
#endif

#if defined(CONFIG_WIFI_COMBO_MODULE_CONTROL_FUNC) && defined(CONFIG_USE_SDMMC0_FOR_WIFI_DEVELOP_BOARD)
    .status = rk29sdk_wifi_mmc0_status,
    .register_status_notify = rk29sdk_wifi_mmc0_status_register,
#endif

#if defined(RK29SDK_SD_CARD_PWR_EN) || (INVALID_GPIO != RK29SDK_SD_CARD_PWR_EN)
    .power_en = RK29SDK_SD_CARD_PWR_EN,
    .power_en_level = RK29SDK_SD_CARD_PWR_EN_LEVEL,
#else
    .power_en = INVALID_GPIO,
    .power_en_level = GPIO_LOW,
#endif    
	.enable_sd_wakeup = 0,

#if defined(CONFIG_SDMMC0_RK29_WRITE_PROTECT)
	.write_prt = SDMMC0_WRITE_PROTECT_PIN,
	.write_prt_enalbe_level = SDMMC0_WRITE_PROTECT_ENABLE_VALUE;
#else
	.write_prt = INVALID_GPIO,
#endif

    .det_pin_info = {    
    #if defined(RK29SDK_SD_CARD_DETECT_N) || (INVALID_GPIO != RK29SDK_SD_CARD_DETECT_N)  
        .io             = RK29SDK_SD_CARD_DETECT_N, //INVALID_GPIO,
        .enable         = RK29SDK_SD_CARD_INSERT_LEVEL,
        #ifdef RK29SDK_SD_CARD_DETECT_PIN_NAME
        .iomux          = {
            .name       = RK29SDK_SD_CARD_DETECT_PIN_NAME,
            #ifdef RK29SDK_SD_CARD_DETECT_IOMUX_FGPIO
            .fgpio      = RK29SDK_SD_CARD_DETECT_IOMUX_FGPIO,
            #endif
            #ifdef RK29SDK_SD_CARD_DETECT_IOMUX_FMUX
            .fmux       = RK29SDK_SD_CARD_DETECT_IOMUX_FMUX,
            #endif
        },
        #endif
    #else
        .io             = INVALID_GPIO,
        .enable         = GPIO_LOW,
    #endif    
    }, 

};
#endif // CONFIG_SDMMC0_RK29

#ifdef CONFIG_SDMMC1_RK29
#define CONFIG_SDMMC1_USE_DMA
static int rk29_sdmmc1_cfg_gpio(void)
{
#if defined(CONFIG_SDMMC_RK29_OLD)
	iomux_set(MMC1_CMD);
	iomux_set(MMC1_CLKOUT);
	iomux_set(MMC1_D0);
	iomux_set(MMC1_D1);
	iomux_set(MMC1_D2);
	iomux_set(MMC1_D3);
#else

#if defined(CONFIG_SDMMC1_RK29_WRITE_PROTECT)
	gpio_request(SDMMC1_WRITE_PROTECT_PIN, "sdio-wp");
	gpio_direction_input(SDMMC1_WRITE_PROTECT_PIN);
#endif

#endif

	return 0;
}

struct rk29_sdmmc_platform_data default_sdmmc1_data = {
	.host_ocr_avail =
	    (MMC_VDD_25_26 | MMC_VDD_26_27 | MMC_VDD_27_28 | MMC_VDD_28_29 |
	     MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 |
	     MMC_VDD_33_34),

#if !defined(CONFIG_USE_SDMMC1_FOR_WIFI_DEVELOP_BOARD)
	.host_caps = (MMC_CAP_4_BIT_DATA | MMC_CAP_SDIO_IRQ |
		      MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED),
#else
	.host_caps =
	    (MMC_CAP_4_BIT_DATA | MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED),
#endif

	.io_init = rk29_sdmmc1_cfg_gpio,

#if !defined(CONFIG_SDMMC_RK29_OLD)
	.set_iomux = rk29_sdmmc_set_iomux,
#endif
#ifdef USE_SDMMC_DATA4_DATA7	
	.emmc_is_selected = NULL,
#endif
	.dma_name = "sdio",
#ifdef CONFIG_SDMMC1_USE_DMA
	.use_dma = 1,
#else
	.use_dma = 0,
#endif

#if defined(CONFIG_WIFI_CONTROL_FUNC) || defined(CONFIG_WIFI_COMBO_MODULE_CONTROL_FUNC)
    .status = rk29sdk_wifi_status,
    .register_status_notify = rk29sdk_wifi_status_register,
#endif

#if defined(CONFIG_SDMMC1_RK29_WRITE_PROTECT)
	.write_prt = SDMMC1_WRITE_PROTECT_PIN,
	    .write_prt_enalbe_level = SDMMC1_WRITE_PROTECT_ENABLE_VALUE;
#else
	.write_prt = INVALID_GPIO,
#endif

    #if defined(CONFIG_RK29_SDIO_IRQ_FROM_GPIO)
        .sdio_INT_gpio = RK29SDK_WIFI_SDIO_CARD_INT,
    #endif

    .det_pin_info = {    
#if defined(CONFIG_USE_SDMMC1_FOR_WIFI_DEVELOP_BOARD)
     #if defined(RK29SDK_SD_CARD_DETECT_N) || (INVALID_GPIO != RK29SDK_SD_CARD_DETECT_N)  
        .io             = RK29SDK_SD_CARD_DETECT_N,
     #else
         .io             = INVALID_GPIO,
     #endif   

        .enable         = RK29SDK_SD_CARD_INSERT_LEVEL,
        #ifdef RK29SDK_SD_CARD_DETECT_PIN_NAME
        .iomux          = {
            .name       = RK29SDK_SD_CARD_DETECT_PIN_NAME,
            #ifdef RK29SDK_SD_CARD_DETECT_IOMUX_FGPIO
            .fgpio      = RK29SDK_SD_CARD_DETECT_IOMUX_FGPIO,
            #endif
            #ifdef RK29SDK_SD_CARD_DETECT_IOMUX_FMUX
            .fmux       = RK29SDK_SD_CARD_DETECT_IOMUX_FMUX,
            #endif
        },
        #endif
 #else
        .io             = INVALID_GPIO,
        .enable         = GPIO_LOW,
#endif
    },
   
	.enable_sd_wakeup = 0,
};
#endif //endif--#ifdef CONFIG_SDMMC1_RK29

#ifdef CONFIG_SDMMC2_RK29
static int rk29_sdmmc2_cfg_gpio(void)
{
    ;
}

struct rk29_sdmmc_platform_data default_sdmmc2_data = {
	.host_ocr_avail =
	    (MMC_VDD_165_195|MMC_VDD_25_26 | MMC_VDD_26_27 | MMC_VDD_27_28 | MMC_VDD_28_29 |
	     MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34),

	.host_caps = (MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA| MMC_CAP_NONREMOVABLE  |
	        MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED | MMC_CAP_UHS_SDR12 |MMC_CAP_UHS_SDR25 |MMC_CAP_UHS_SDR50),

	.io_init = rk29_sdmmc2_cfg_gpio,
	.set_iomux = rk29_sdmmc_set_iomux,
	.emmc_is_selected = sdmmc_is_selected_emmc,

	//.power_en = INVALID_GPIO,
   // .power_en_level = GPIO_LOW,

	.dma_name = "emmc",
	.use_dma = 1,

};
#endif//endif--#ifdef CONFIG_SDMMC2_RK29
/**************************************************************************************************
 * the end of setting for SDMMC devices
**************************************************************************************************/

#ifdef CONFIG_BATTERY_RK30_ADC_FAC

#if defined(CONFIG_TCHIP_MACH_XBT_3188)
static int batt_table[2*11+6] =
{
    0x4B434F52,0x7461625F,0x79726574,1,470,100,
    6863, 7287, 7394, 7479, 7544, 7644, 7731, 7867, 7989, 8129, 8304,//discharge
    7370, 7590, 7705, 7787, 7885, 8005, 8155, 8300, 8336, 8341 , 8344,//charge
};

static struct rk30_adc_battery_platform_data rk30_adc_battery_platdata = {
        .dc_det_pin      = RK30_PIN0_PB2,
        .batt_low_pin    = INVALID_GPIO, 
        .charge_set_pin  = INVALID_GPIO,
        .charge_ok_pin   = INVALID_GPIO,
        .usb_det_pin = INVALID_GPIO,
        .dc_det_level    = GPIO_LOW,
        .charge_ok_level = GPIO_HIGH,

        .reference_voltage = 1800, // the rK2928 is 3300;RK3066 and rk29 are 2500;rk3066B is 1800;
        .pull_up_res = 470,     //divider resistance ,  pull-up resistor
        .pull_down_res = 100, //divider resistance , pull-down resistor

        .is_reboot_charging = 1,
        .save_capacity   = 1 ,
        .use_board_table = 1,
        //.low_voltage_protection = 3600,
        .board_batt_table = batt_table,
};   

#elif  defined(CONFIG_TCHIP_MACH_TR101Q)
static int batt_table[2*11+6] =
{
    0x4B434F52,0x7461625F,0x79726574,1,470,100,
	6830, 7242, 7332, 7404, 7470, 7520, 7610, 7744, 7848, 8016, 8284,//discharge
	7630, 7754, 7852, 7908, 7956, 8024, 8112, 8220, 8306, 8318, 8458//charge
};

static struct rk30_adc_battery_platform_data rk30_adc_battery_platdata = {
        .dc_det_pin      = RK30_PIN0_PB2,
        .batt_low_pin    = INVALID_GPIO, 
        .charge_set_pin  = INVALID_GPIO,
        .charge_ok_pin   = RK30_PIN0_PA6,
        .usb_det_pin = INVALID_GPIO,
        .dc_det_level    = GPIO_LOW,
        .charge_ok_level = GPIO_HIGH,

        .reference_voltage = 1800, // the rK2928 is 3300;RK3066 and rk29 are 2500;rk3066B is 1800;
        .pull_up_res = 470,     //divider resistance ,  pull-up resistor
        .pull_down_res = 100, //divider resistance , pull-down resistor

        .is_reboot_charging = 1,
        .save_capacity   = 1 ,
        .use_board_table = 1,
        //.low_voltage_protection = 3600,
        .board_batt_table = batt_table,
};

#else    
static struct rk30_adc_battery_platform_data rk30_adc_battery_platdata = {
        .dc_det_pin      = RK30_PIN0_PB2,
        .batt_low_pin    = INVALID_GPIO, 
        .charge_set_pin  = INVALID_GPIO,
        .charge_ok_pin   = RK30_PIN0_PA6,
	 .usb_det_pin = INVALID_GPIO,
        .dc_det_level    = GPIO_LOW,
        .charge_ok_level = GPIO_HIGH,

	.reference_voltage = 1800, // the rK2928 is 3300;RK3066 and rk29 are 2500;rk3066B is 1800;
       .pull_up_res = 200,     //divider resistance ,  pull-up resistor
       .pull_down_res = 120, //divider resistance , pull-down resistor

	.is_reboot_charging = 1,
        .save_capacity   = 1 ,
        .low_voltage_protection = 3600,    
};
#endif

static struct platform_device rk30_device_adc_battery = {
        .name   = "rk30-battery",
        .id     = -1,
        .dev = {
                .platform_data = &rk30_adc_battery_platdata,
        },
};
#endif

#ifdef CONFIG_CW2015_BATTERY
/*
   note the follow array must set depend on the battery that you use
      you must send the battery to cellwise-semi the contact information:
         name: chen gan; tel:13416876079; E-mail: ben.chen@cellwise-semi.com
	  */
#if defined(CONFIG_TCHIP_MACH_TR1088)
static char config_info[SIZE_BATINFO] = { 
0x15,0x7E,0x5F,0x62,0x63,
0x61,0x5B,0x56,0x54,0x4F,
0x4A,0x45,0x40,0x35,0x2B,
0x22,0x1E,0x1B,0x23,0x2C,
0x3E,0x56,0x6E,0x6C,0x64,
0x59,0x0C,0x29,0x18,0x30,
0x41,0x44,0x42,0x3C,0x36,
0x50,0x3A,0x19,0x31,0x1C,
0x00,0x23,0x52,0x87,0x8F,
0x91,0x94,0x52,0x82,0x8C,
0x92,0x96,0x7F,0x5E,0xA3,
0xCB,0x2F,0x7D,0x72,0xA5,
0xB5,0xC1,0x7E,0x09,
};

#else
static u8 config_info[SIZE_BATINFO] = {
        0x15, 0x42, 0x60, 0x59, 0x52,
        0x58, 0x4D, 0x48, 0x48, 0x44,
       0x44, 0x46, 0x49, 0x48, 0x32,
        0x24, 0x20, 0x17, 0x13, 0x0F,
        0x19, 0x3E, 0x51, 0x45, 0x08,
        0x76, 0x0B, 0x85, 0x0E, 0x1C,
        0x2E, 0x3E, 0x4D, 0x52, 0x52,
        0x57, 0x3D, 0x1B, 0x6A, 0x2D,
        0x25, 0x43, 0x52, 0x87, 0x8F,
        0x91, 0x94, 0x52, 0x82, 0x8C,
        0x92, 0x96, 0xFF, 0x7B, 0xBB,
        0xCB, 0x2F, 0x7D, 0x72, 0xA5,
        0xB5, 0xC1, 0x46, 0xAE 
};
#endif

static struct cw_bat_platform_data cw_bat_platdata = {
        .dc_det_pin      = RK30_PIN0_PB2,
        .bat_low_pin    = INVALID_GPIO,//RK30_PIN0_PB1,
        .chg_ok_pin   = RK30_PIN0_PA6,
        .dc_det_level    = GPIO_LOW,
        .bat_low_level  = GPIO_LOW,
        .chg_ok_level = GPIO_HIGH,
		.is_usb_charge = 1,
        .chg_mode_sel_pin = INVALID_GPIO,
		.cw_bat_config_info     = config_info,

	};
#endif

#ifdef CONFIG_RK30_PWM_REGULATOR
static int pwm_voltage_map[] = {
	800000,825000,850000, 875000,900000, 925000 ,950000, 975000,1000000, 1025000, 1050000, 1075000, 1100000, 1125000, 1150000, 1175000, 1200000, 1225000, 1250000, 1275000, 1300000, 1325000, 1350000,1375000
};

static struct regulator_consumer_supply pwm_dcdc1_consumers[] = {
	{
		.supply = "vdd_core",
	}
};
static struct regulator_consumer_supply pwm_dcdc2_consumers[] = {
	{
		.supply = "vdd_cpu",
	}
};


struct regulator_init_data pwm_regulator_init_dcdc[2] =
{
	{
		.constraints = {
			.name = "PWM_DCDC1",
			.min_uV = 600000,
			.max_uV = 1800000,	//0.6-1.8V
			.apply_uV = true,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE,
		},
		.num_consumer_supplies = ARRAY_SIZE(pwm_dcdc1_consumers),
		.consumer_supplies = pwm_dcdc1_consumers,
	},
	{
		.constraints = {
			.name = "PWM_DCDC2",
			.min_uV = 600000,
			.max_uV = 1800000,      //0.6-1.8V
			.apply_uV = true,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE,
		},
		.num_consumer_supplies = ARRAY_SIZE(pwm_dcdc2_consumers),
		.consumer_supplies = pwm_dcdc2_consumers,
	},
};
#define	GPIO0A4_PWM1_NAME				"gpio0a4_pwm1_name"
#define	GPIO0A3_PWM0_NAME				"gpio0a3_pwm0_name"
static struct pwm_platform_data pwm_regulator_info[2] = {
	{ 	//PWM_LOG
		.pwm_id = 1,
		.pwm_gpio = RK30_PIN3_PD4,
		.pwm_iomux_name=GPIO0A4_PWM1_NAME,
		.pwm_iomux_pwm = PWM1,
		.pwm_iomux_gpio = GPIO3_D4,
		.pwm_voltage = 1100000,
		.suspend_voltage = 1050000,
		.min_uV=1000000,
		.max_uV	= 1400000,
		.coefficient = 700,	//57.5%
		.pwm_voltage_map = pwm_voltage_map,
		.init_data	= &pwm_regulator_init_dcdc[0],
	},
	{	//PWM_ARM
		.pwm_id = 0,
		.pwm_gpio = RK30_PIN3_PD3,
		.pwm_iomux_pwm = PWM0,
		.pwm_iomux_name=GPIO0A3_PWM0_NAME,
		.pwm_iomux_gpio = GPIO3_D3,
		.pwm_voltage = 1100000,
		.suspend_voltage = 1050000,
		.min_uV=1000000,
		.max_uV	= 1375000,
		.coefficient = 470,	//57.5%
		.pwm_voltage_map = pwm_voltage_map,
		.init_data	= &pwm_regulator_init_dcdc[1],
	},
	
};

struct platform_device pwm_regulator_device[2] = {
	{
		.name = "pwm-voltage-regulator",
		.id = 0,
		.dev		= {
			.platform_data = &pwm_regulator_info[0],
		}
	},
	{
		.name = "pwm-voltage-regulator",
		.id = 1,
		.dev		= {
			.platform_data = &pwm_regulator_info[1],
		}
	},
};
#endif

#ifdef CONFIG_RFKILL_RK
// bluetooth rfkill device, its driver in net/rfkill/rfkill-rk.c
#if defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
static struct rfkill_rk_platform_data rfkill_rk_platdata = {
    .type               = RFKILL_TYPE_BLUETOOTH,

    .poweron_gpio       = { // BT_REG_ON
        .io             = RK30_PIN3_PC6, //RK30_PIN3_PC7,
        .enable         = GPIO_HIGH,
        .iomux          = {
            .name       = "bt_poweron",
            .fgpio      = GPIO3_C6,
        },
    },

    .reset_gpio         = { // BT_RST
        .io             = INVALID_GPIO, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "bt_reset",
            .fgpio      = GPIO3_D1,
       },
   }, 
   
    .wake_gpio          = { // BT_WAKE, use to control bt's sleep and wakeup
        .io             = RK30_PIN1_PA1, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "gpio1a1_uart0sout_name",
            .fgpio      = GPIO1_A1,
            .fmux       = UART0_SOUT,
        },
    },

    .wake_host_irq      = {
        .gpio           = { 
            .io         = RK30_PIN1_PA0,
            .enable     = GPIO_LOW,
            .iomux      = {
                .name   = "uart0_rx",
                .fgpio  = GPIO1_A0,
                .fmux   = UART0_SIN,
            },
        },
    },

    .rts_gpio           = { // UART_RTS, enable or disable BT's data coming
        .io             = RK30_PIN1_PA3, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "bt_rts",
            .fgpio      = GPIO1_A3,
            .fmux       = UART0_RTSN,
        },
    },
};
#else
static struct rfkill_rk_platform_data rfkill_rk_platdata = {
    .type               = RFKILL_TYPE_BLUETOOTH,

    .poweron_gpio       = { // BT_REG_ON
#if defined(CONFIG_TCHIP_MACH_TR7088)
        .io             = RK30_PIN3_PC6, //RK30_PIN3_PC7,
#else
        .io             = INVALID_GPIO, //RK30_PIN3_PC7,
#endif
        .enable         = GPIO_HIGH,
        .iomux          = {
            .name       = "bt_poweron",
#if defined(CONFIG_TCHIP_MACH_TR7088)
            .fgpio      = GPIO3_C6,
#else
            .fgpio      = GPIO3_C7,
#endif
        },
    },

    .reset_gpio         = { // BT_RST
#if defined(CONFIG_TCHIP_MACH_TR7088)
        .io             = INVALID_GPIO, // set io to INVALID_GPIO for disable it
#else
        .io             = RK30_PIN3_PD1, // set io to INVALID_GPIO for disable it
#endif
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "bt_reset",
            .fgpio      = GPIO3_D1,
       },
   }, 
#if defined(CONFIG_TCHIP_MACH_TR7088)
    .wake_gpio          = { // BT_WAKE, use to control bt's sleep and wakeup
        .io             = RK30_PIN1_PA1, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "gpio1a1_uart0sout_name",
            .fgpio      = GPIO1_A1,
            .fmux       = UART0_SOUT,
        },
    },
#else
    .wake_gpio          = { // BT_WAKE, use to control bt's sleep and wakeup
        .io             = RK30_PIN3_PC6, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_HIGH,
        .iomux          = {
            .name       = "bt_wake",
            .fgpio      = GPIO3_C6,
        },
    },
#endif

#if defined(CONFIG_TCHIP_MACH_TR7088)
    .wake_host_irq      = {
        .gpio           = { 
            .io         = RK30_PIN1_PA0,
            .enable     = GPIO_LOW,
            .iomux      = {
                .name   = "uart0_rx",
                .fgpio  = GPIO1_A0,
                .fmux   = UART0_SIN,
            },
        },
    },
#else
    .wake_host_irq      = { // BT_HOST_WAKE, for bt wakeup host when it is in deep sleep
        .gpio           = {
#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TRQ7_LJ) || defined(CONFIG_TCHIP_MACH_XBT_3188)
            .io         = RK30_PIN3_PC7, // set io to INVALID_GPIO for disable it
#else
            .io         = RK30_PIN0_PA5, // set io to INVALID_GPIO for disable it
#endif
            .enable     = GPIO_LOW,      // set GPIO_LOW for falling, set 0 for rising
            .iomux      = {
#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TRQ7_LJ) || defined(CONFIG_TCHIP_MACH_XBT_3188)
            	.name       = "bt_wake_host",
           		.fgpio      = GPIO3_C7,
#else
                .name   = NULL,
#endif
            },
        },
    },
#endif

    .rts_gpio           = { // UART_RTS, enable or disable BT's data coming
        .io             = RK30_PIN1_PA3, // set io to INVALID_GPIO for disable it
        .enable         = GPIO_LOW,
        .iomux          = {
            .name       = "bt_rts",
            .fgpio      = GPIO1_A3,
            .fmux       = UART0_RTSN,
        },
    },
};
#endif

static struct platform_device device_rfkill_rk = {
    .name   = "rfkill_rk",
    .id     = -1,
    .dev    = {
        .platform_data = &rfkill_rk_platdata,
    },
};
#endif

#if defined(CONFIG_GPS_RK)
int rk_gps_io_init(void)
{
	printk("%s \n", __FUNCTION__);
	
	rk30_mux_api_set(GPIO1B5_UART3RTSN_NAME, GPIO1B_GPIO1B5);//VCC_EN
	gpio_request(RK30_PIN1_PB5, NULL);
	gpio_direction_output(RK30_PIN1_PB5, GPIO_LOW);

	rk30_mux_api_set(GPIO1B4_UART3CTSN_GPSRFCLK_NAME, GPIO1B_GPSRFCLK);//GPS_CLK
	rk30_mux_api_set(GPIO1B2_UART3SIN_GPSMAG_NAME, GPIO1B_GPSMAG);//GPS_MAG
	rk30_mux_api_set(GPIO1B3_UART3SOUT_GPSSIG_NAME, GPIO1B_GPSSIG);//GPS_SIGN

	rk30_mux_api_set(GPIO1A6_UART1CTSN_SPI0CLK_NAME, GPIO1A_GPIO1A6);//SPI_CLK
	gpio_request(RK30_PIN1_PA6, NULL);
	gpio_direction_output(RK30_PIN1_PA6, GPIO_LOW);

	rk30_mux_api_set(GPIO1A5_UART1SOUT_SPI0TXD_NAME, GPIO1A_GPIO1A5);//SPI_MOSI
	gpio_request(RK30_PIN1_PA5, NULL);
	gpio_direction_output(RK30_PIN1_PA5, GPIO_LOW);	

	rk30_mux_api_set(GPIO1A7_UART1RTSN_SPI0CSN0_NAME, GPIO1A_GPIO1A7);//SPI_CS
	gpio_request(RK30_PIN1_PA7, NULL);
	gpio_direction_output(RK30_PIN1_PA7, GPIO_LOW);		
	return 0;
}
int rk_gps_power_up(void)
{
	printk("%s \n", __FUNCTION__);

	return 0;
}

int rk_gps_power_down(void)
{
	printk("%s \n", __FUNCTION__);

	return 0;
}

int rk_gps_reset_set(int level)
{
	return 0;
}
int rk_enable_hclk_gps(void)
{
	printk("%s \n", __FUNCTION__);
	clk_enable(clk_get(NULL, "hclk_gps"));
	return 0;
}
int rk_disable_hclk_gps(void)
{
	printk("%s \n", __FUNCTION__);
	clk_disable(clk_get(NULL, "hclk_gps"));
	return 0;
}
struct rk_gps_data rk_gps_info = {
	.io_init = rk_gps_io_init,
	.power_up = rk_gps_power_up,
	.power_down = rk_gps_power_down,
	.reset = rk_gps_reset_set,
	.enable_hclk_gps = rk_enable_hclk_gps,
	.disable_hclk_gps = rk_disable_hclk_gps,
	.GpsSign = RK30_PIN1_PB3,
	.GpsMag = RK30_PIN1_PB2,        //GPIO index
	.GpsClk = RK30_PIN1_PB4,        //GPIO index
	.GpsVCCEn = RK30_PIN1_PB5,     //GPIO index
	.GpsSpi_CSO = RK30_PIN1_PA4,    //GPIO index
	.GpsSpiClk = RK30_PIN1_PA5,     //GPIO index
	.GpsSpiMOSI = RK30_PIN1_PA7,	  //GPIO index
	.GpsIrq = IRQ_GPS,
	.GpsSpiEn = 0,
	.GpsAdcCh = 2,
	.u32GpsPhyAddr = RK30_GPS_PHYS,
	.u32GpsPhySize = RK30_GPS_SIZE,
};

struct platform_device rk_device_gps = {
	.name = "gps_hv5820b",
	.id = -1,
	.dev		= {
	.platform_data = &rk_gps_info,
		}
	};
#endif

#if defined(CONFIG_MT5931_MT6622) || defined(CONFIG_MTK_MT6622)
static struct mt6622_platform_data mt6622_platdata = {
		    .power_gpio         = { // BT_REG_ON
		      #if DS1006H_V1_2_SUPPORT
                    .io             = RK30_PIN3_PC7, // set io to INVALID_GPIO for disable it
		      #else
		    	.io             = RK30_PIN3_PD5, // set io to INVALID_GPIO for disable it
		    	#endif
			    .enable         = GPIO_HIGH,
			    .iomux          = {
				    .name       = NULL,
				},
		    },

		    .reset_gpio         = { // BT_RST
		        #if DS1006H_V1_2_SUPPORT
                    .io             = RK30_PIN3_PD1,
		        #else
		        .io             = RK30_PIN0_PD7,
		        #endif
		        .enable         = GPIO_HIGH,
		        .iomux          = {
		            .name       = NULL,
		        },
		    },

		    .irq_gpio           = {
		          #if DS1006H_V1_2_SUPPORT
                       .io             = RK30_PIN0_PA5,
		          #else
			    .io             = RK30_PIN3_PD2,
			    #endif
				#if defined(CONFIG_TCHIP_MACH_TR101Q)
			    .enable         = GPIO_LOW,
				#else
			    .enable         = GPIO_HIGH,
				#endif
			    .iomux          = {
				    .name       = NULL,
				},
		    }
};

static struct platform_device device_mt6622 = {
		    .name   = "mt6622",
			.id     = -1,
			.dev    = {
			       .platform_data = &mt6622_platdata,
			},
};	
#endif

#if defined CONFIG_TCC_BT_DEV
static struct tcc_bt_platform_data tcc_bt_platdata = {

    .power_gpio   = { // ldoon
        .io             =  RK2928_PIN3_PC0,
        .enable         = GPIO_HIGH,
        .iomux          = {
            .name       = NULL,
            },
        },

    .wake_host_gpio  = { // BT_HOST_WAKE, for bt wakeup host when it is in deep sleep
        .io         = RK2928_PIN0_PC5, // set io to INVALID_GPIO for disable it
        .enable     = IRQF_TRIGGER_RISING,// set IRQF_TRIGGER_FALLING for falling, set IRQF_TRIGGER_RISING for rising
        .iomux      = {
            .name       = NULL,
        },
    },
};

static struct platform_device device_tcc_bt = {
    .name   = "tcc_bt_dev",
    .id     = -1,
    .dev    = {
        .platform_data = &tcc_bt_platdata,
        },
};
#endif


#ifdef CONFIG_ANDROID_TIMED_GPIO
#if defined (CONFIG_TCHIP_MACH_TR101Q)  
static struct timed_gpio timed_gpios[] = {
	{
		.name = "vibrator",
		.gpio = RK30_PIN0_PC7,
		.max_timeout = 1000,
		.active_low = 0,
		.adjust_time =20,      //adjust for diff product
	},
	/*
	{
		.name = "vibrator1",
		.gpio = RK30_PIN0_PC5,
		.max_timeout = 1000,
		.active_low = 0,
		.adjust_time =20,      //adjust for diff product
	},
	*/

};

static struct timed_gpio_platform_data rk29_vibrator_info = {
	.num_gpios = ARRAY_SIZE(timed_gpios),
	.gpios = timed_gpios,
};

static struct platform_device rk29_device_vibrator = {
	.name = "timed-gpio",
	.id = -1,
	.dev = {
		.platform_data = &rk29_vibrator_info,
	},

};
#endif
#endif





static struct platform_device *devices[] __initdata = {
#if defined(CONFIG_SWITCH_GPIO) 
    &device_headset_switch,
#endif
#ifdef CONFIG_ION
	&device_ion,
#endif
#ifdef CONFIG_WIFI_CONTROL_FUNC
	&rk29sdk_wifi_device,
#endif

#if defined(CONFIG_MT6620)
	    &mt3326_device_gps,
#endif

#ifdef CONFIG_BATTERY_RK30_ADC_FAC
 	&rk30_device_adc_battery,
#endif
#ifdef CONFIG_RFKILL_RK
	&device_rfkill_rk,
#endif
#ifdef CONFIG_GPS_RK
	&rk_device_gps,
#endif
#if defined(CONFIG_MT5931_MT6622) || defined(CONFIG_MTK_MT6622)
	&device_mt6622,
#endif
#if defined(CONFIG_MT6229)
	&rk29_device_mt6229,
#endif
#if defined(CONFIG_ARCH_RK3188)
	&device_mali,
#endif

#ifdef CONFIG_TCC_BT_DEV
        &device_tcc_bt,
#endif

#ifdef CONFIG_RK30_PWM_REGULATOR
        &pwm_regulator_device[0],
        &pwm_regulator_device[1],
#endif

#ifdef CONFIG_ANDROID_TIMED_GPIO
#if defined (CONFIG_TCHIP_MACH_TR101Q)  
	&rk29_device_vibrator,
#endif
#endif

#if defined (CONFIG_RK_HEADSET_DET) || defined (CONFIG_RK_HEADSET_IRQ_HOOK_ADC_DET)
	&rk_device_headset,
#endif
};


static int rk_platform_add_display_devices(void)
{
	struct platform_device *fb = NULL;  //fb
	struct platform_device *lcdc0 = NULL; //lcdc0
	struct platform_device *lcdc1 = NULL; //lcdc1
	struct platform_device *bl = NULL; //backlight
#ifdef CONFIG_FB_ROCKCHIP
	fb = &device_fb;
#endif

#if defined(CONFIG_LCDC0_RK3188)
	lcdc0 = &device_lcdc0,
#endif

#if defined(CONFIG_LCDC1_RK3188)
	lcdc1 = &device_lcdc1,
#endif

#ifdef CONFIG_BACKLIGHT_RK29_BL
	bl = &rk29_device_backlight,
#endif
	__rk_platform_add_display_devices(fb,lcdc0,lcdc1,bl);

	return 0;
	
}

//@ tchip  atx8 device 
#define ATX8_RST_PIN    RK30_PIN3_PD7
#define ATX8_RST_PIN_NAME       0
#define ATX8_RST_PIN_FGPIO      0

#if defined (CONFIG_ENCRYPTION_DEVICE)
#include "../../../drivers/input/netupdate/at18.h"
static void tchip_iomux_set(struct atx8_gpio *gpio, int iomux)
{
	if ( gpio->iomux.name != 0 )
	{
		if ( iomux ) // 1 -- iomux i2c  mode 
			iomux_set(gpio->iomux.name);
		else
			iomux_set_gpio_mode(iomux_mode_to_gpio(gpio->iomux.name));
	}
}
static struct atx8_platform_data tchip_atx8_platdata = {
    .name               = "atx8",
	.iomux_set			= tchip_iomux_set,
    .cs_gpio       = {
        .name       = "atx8_cs",
        .io             = ATX8_RST_PIN,
        .enable         = GPIO_HIGH,    // atx8 enable level
        .iomux          = {
			.name		= ATX8_RST_PIN_NAME,
            .fgpio      = ATX8_RST_PIN_FGPIO,
        },
    },

    .sda_gpio           = {
        .name       = "atx8_sda",
        .io             = RK30_PIN1_PD0,
        .enable         = GPIO_HIGH,    // sda high level
        .iomux          = {
			.name		= I2C0_SDA,
            .fgpio      = 0,
            .fi2c       = 1,
        },
    },
    .scl_gpio           = {
        .name       = "atx8_scl",
        .io             = RK30_PIN1_PD1,
        .enable         = GPIO_HIGH,    // scl high level
        .iomux          = {
			.name		= I2C0_SCL,
            .fgpio      = 0,
            .fi2c       = 1,
        },
    },
};
    
#endif //@ tchip atx8 end

// i2c
#ifdef CONFIG_I2C0_RK30
static struct i2c_board_info __initdata i2c0_info[] = {
#if defined (CONFIG_BATTERY_CW2015)
        {
                .type                   = "cw2015",
                .addr                   = 0x62,
                .flags                  = 0,
                .platform_data = &cw2015_info,
        },
#endif

#if defined (CONFIG_CW2015_BATTERY)
        {
                .type           = "cw201x",
                .addr           = 0x62,
                .flags          = 0,
                .platform_data  = &cw_bat_platdata,
	},
#endif
										

	
#if defined (CONFIG_GS_MMA8452)
	{
		.type	        = "gs_mma8452",
		.addr	        = 0x1d,
		.flags	        = 0,
		.irq	        = MMA8452_INT_PIN,
		.platform_data = &mma8452_info,
	},
#endif

#if defined (CONFIG_GS_LIS3DH)
	{
		.type	        = "gs_lis3dh",
		.addr	        = 0x19,   //0x19(SA0-->VCC), 0x18(SA0-->GND)
		.flags	        = 0,
		.irq	        = LIS3DH_INT_PIN,
		.platform_data = &lis3dh_info,
	},
#endif
#if defined (CONFIG_COMPASS_AK8963)
	{
		.type          = "ak8963",
		.addr          = 0x0d,
		.flags         = 0,
		.irq           = RK30_PIN3_PD7,	
		.platform_data = &akm8963_info,
	},
#endif

#if defined (CONFIG_LS_PHOTORESISTOR)
	{
		.type           = "ls_photoresistor",
		.addr           = 0x5e,            
		.flags          = 0,
		.irq            = INVALID_GPIO,	
		.platform_data = &light_photoresistor_info,
	},
#endif

#if defined (CONFIG_COMPASS_AK8975)
	{
		.type          = "ak8975",
		.addr          = 0x0d,
		.flags         = 0,
		.irq           = RK30_PIN3_PD7,	
		.platform_data = &akm8975_info,
		.irq           = RK30_PIN3_PD7,	
		.platform_data = &akm8975_info,
	},
#endif
#if defined (CONFIG_GYRO_L3G4200D)
	{
		.type          = "l3g4200d_gryo",
		.addr          = 0x69,
		.flags         = 0,
		.irq           = L3G4200D_INT_PIN,
		.platform_data = &l3g4200d_info,
	},
#endif
#if defined (CONFIG_SND_SOC_RK1000)
	{
		.type          = "rk1000_i2c_codec",
		.addr          = 0x60,
		.flags         = 0,
	},
	{
		.type          = "rk1000_control",
		.addr          = 0x40,
		.flags         = 0,
	},
#endif
#if defined (CONFIG_SENSORS_STK8312)
       {
         .type                   = "stk831x",
         .addr                   = 0x3d,
         .flags                  = 0,
         .irq                    = STK8312_INT_PIN,
         .platform_data          = &STK8312_info,
       },
#endif
#if defined (CONFIG_GS_STK831X)
	{
	  .type 		  = "gs_stk831x",
	  .addr 		  = 0x3d,
	  .flags		  = 0,
	  .irq			  = STK831X_INT_PIN,
	  .platform_data  = &stk831x_info,
	},
#endif
#if defined (CONFIG_GS_MMA7660)
        {
                .type           = "gs_mma7660",
                .addr           = 0x4c,
                .flags          = 0,
                .irq            = MMA7660_INT_PIN,
                .platform_data = &mma7660_info,
        },
#endif
#if defined (CONFIG_GS_MC3XXX)
	{
		.type	        = "gs_mc3XXX",
		.addr	        = 0x4c,
		.flags	        = 0,
		.irq	        = MC3XXX_INT_PIN,
		.platform_data = &MC3XXX_info,
	},
#endif

#if defined (CONFIG_ENCRYPTION_DEVICE)
       {
               .type                   = "netupdate",
               .addr                   = 0x2f,
               .flags                  = 0,
               .platform_data = &tchip_atx8_platdata,
       },
#endif

};
#endif

int __sramdata g_pmic_type =  0;
#ifdef CONFIG_I2C1_RK30
#ifdef CONFIG_REGULATOR_ACT8846
#define PMU_POWER_SLEEP RK30_PIN0_PA1
#define PMU_VSEL RK30_PIN3_PD3
#define ACT8846_HOST_IRQ                RK30_PIN0_PB3

static struct pmu_info  act8846_dcdc_info[] = {
	{
		.name          = "act_dcdc1",   //ddr
		.min_uv          = 1200000,
		.max_uv         = 1200000,
		.suspend_vol  =   1200000,
	},
	{
		.name          = "vdd_core",    //logic
		.min_uv          = 1000000,
		.max_uv         = 1000000,
		#ifdef CONFIG_ACT8846_SUPPORT_RESET
		.suspend_vol  =  1200000,
		#else
		.suspend_vol  =  900000,
		#endif
	},
	{
		.name          = "vdd_cpu",   //arm
		.min_uv          = 1000000,
		.max_uv         = 1000000,
		#ifdef CONFIG_ACT8846_SUPPORT_RESET
		.suspend_vol  =  1200000,
		#else
		.suspend_vol  =  900000,
		#endif
	},
#if defined( CONFIG_TCHIP_MACH_TR101Q )
	{
		.name          = "act_dcdc4",   //vccio
		.min_uv          = 3300000,
		.max_uv         = 3300000,
		#ifdef CONFIG_ACT8846_SUPPORT_RESET
		.suspend_vol  =  3300000,
		#else
		.suspend_vol  =  3300000,
		#endif
	},
#else
	{
		.name          = "act_dcdc4",   //vccio
		.min_uv          = 3000000,
		.max_uv         = 3000000,
		#ifdef CONFIG_ACT8846_SUPPORT_RESET
		.suspend_vol  =  3000000,
		#else
		.suspend_vol  =  2800000,
		#endif
	},
#endif
	
};
static  struct pmu_info  act8846_ldo_info[] = {
	{
		.name          = "act_ldo1",   //vdd10
		.min_uv          = 1000000,
		.max_uv         = 1000000,
	},
	{
		.name          = "act_ldo2",    //vdd12
		.min_uv          = 1200000,
		.max_uv         = 1200000,
	},
	{
		.name          = "act_ldo3",   //vcc18_cif
		.min_uv          = 1800000,
		.max_uv         = 1800000,
	},
	{
		.name          = "act_ldo4",   //vcca33
		.min_uv          = 3300000,
		.max_uv         = 3300000,
	},
	{
		.name          = "act_ldo5",   //vcctp
		.min_uv          = 3300000,
		.max_uv         = 3300000,
	},
	{
		.name          = "act_ldo6",   //vcc_jetta
		.min_uv          = 3300000,
		.max_uv         = 3300000,
	},
	{
		.name          = "act_ldo7",   //vcc18
		.min_uv          = 1800000,
		.max_uv         = 1800000,
	},
	{
		.name          = "act_ldo8",   //vcc28_cif
		.min_uv          = 2800000,
		.max_uv         = 2800000,
	},
 };

#include "../mach-rk30/board-pmu-act8846.c"
#endif

#ifdef CONFIG_MFD_WM831X_I2C
#define PMU_POWER_SLEEP 		RK30_PIN0_PA1 

static struct pmu_info  wm8326_dcdc_info[] = {
	{
		.name          = "vdd_core",   //logic
		.min_uv          = 1000000,
		.max_uv         = 1000000,
		.suspend_vol  =  950000,
	},
	{
		.name          = "vdd_cpu",    //arm
		.min_uv          = 1000000,
		.max_uv         = 1000000,
		.suspend_vol  =  950000,
	},
	{
		.name          = "dcdc3",   //ddr
		.min_uv          = 1150000,
		.max_uv         = 1150000,
		.suspend_vol  =  1150000,
	},
	#ifdef CONFIG_MACH_RK3066_SDK
	{
		.name          = "dcdc4",   //vcc_io
		.min_uv          = 3300000,
		.max_uv         = 3300000,
		.suspend_vol  =  3000000,
	},
	#else
	{
		.name          = "dcdc4",   //vcc_io
		.min_uv          = 3000000,
		.max_uv         = 3000000,
		.suspend_vol  =  2800000,
	},
	#endif
};

static struct pmu_info  wm8326_ldo_info[] = {
	{
		.name          = "ldo1",   //vcc18_cif
		.min_uv          = 1800000,
		.max_uv         = 1800000,
		.suspend_vol  =  1800000,
	},
	{
		.name          = "ldo2",    //vccio_wl
		.min_uv          = 1800000,
		.max_uv         = 1800000,
		.suspend_vol  =  1800000,
	},
	{
		.name          = "ldo3",   //
		.min_uv          = 1100000,
		.max_uv         = 1100000,
		.suspend_vol  =  1100000,
	},
	{
		.name          = "ldo4",   //vdd11
		.min_uv          = 1000000,
		.max_uv         = 1000000,
		.suspend_vol  =  1000000,
	},
	{
		.name          = "ldo5",   //vcc25
		.min_uv          = 1800000,
		.max_uv         = 1800000,
		.suspend_vol  =  1800000,
	},
	{
		.name          = "ldo6",   //vcc33
		.min_uv          = 3300000,
		.max_uv         = 3300000,
		.suspend_vol  =  3300000,
	},
	{
		.name          = "ldo7",   //vcc28_cif
		.min_uv          = 2800000,
		.max_uv         = 2800000,
		.suspend_vol  =  2800000,
	},
	{
		.name          = "ldo8",   //vcca33
		.min_uv          = 3300000,
		.max_uv         = 3300000,
		.suspend_vol  =  3300000,
	},
	{
		.name          = "ldo9",   //vcc_tp
		.min_uv          = 3300000,
		.max_uv         = 3300000,
		.suspend_vol  =  3300000,
	},
	{
		.name          = "ldo10",   //flash_io
		.min_uv          = 1800000,
		.max_uv         = 1800000,
		.suspend_vol  =  1800000,
	},
};

#include "../mach-rk30/board-pmu-wm8326.c"
#endif

#ifdef CONFIG_MFD_TPS65910
#ifdef CONFIG_ARCH_RK3188
#define TPS65910_HOST_IRQ        RK30_PIN0_PB3
#else
#define TPS65910_HOST_IRQ        RK30_PIN6_PA4
#endif

#define PMU_POWER_SLEEP RK30_PIN0_PA1

static struct pmu_info  tps65910_dcdc_info[] = {
	{
		.name          = "vdd_core",   //logic
		.min_uv          = 1100000,
		.max_uv         = 1100000,
	},
	{
		.name          = "vdd2",    //ddr
		.min_uv          = 1200000,
		.max_uv         = 1200000,
	},
	{
		.name          = "vio",   //vcc_io
		.min_uv          = 2500000,
		.max_uv         = 2500000,
	},
	
};
static  struct pmu_info  tps65910_ldo_info[] = {
	{
		.name          = "vpll",   //vdd10
		.min_uv          = 1000000,
		.max_uv         = 1000000,
	},
	{
		.name          = "vdig1",    //vcc18_cif
		.min_uv          = 1800000,
		.max_uv         = 1800000,
	},
	{
		.name          = "vdig2",   //vdd_jetta
		.min_uv          = 1200000,
		.max_uv         = 1200000,
	},
	{
		.name          = "vaux1",   //vcc28_cif
		.min_uv          = 2800000,
		.max_uv         = 2800000,
	},
	{
		.name          = "vaux2",   //vcca33
		.min_uv          = 3300000,
		.max_uv         = 3300000,
	},
	{
		.name          = "vaux33",   //vcc_tp
		.min_uv          = 3300000,
		.max_uv         = 3300000,
	},
	{
		.name          = "vmmc",   //vcc30
		.min_uv          = 3000000,
		.max_uv         = 3000000,
	},
	{
		.name          = "vdac",   //vcc18
		.min_uv          = 1800000,
		.max_uv         = 1800000,
	},
 };

#include "../mach-rk30/board-pmu-tps65910.c"
#endif

static struct i2c_board_info __initdata i2c1_info[] = {
#if defined (CONFIG_REGULATOR_ACT8846)
	{
		.type    		= "act8846",
		.addr           = 0x5a, 
		.flags			= 0,
		.irq            = ACT8846_HOST_IRQ,
		.platform_data=&act8846_data,
	},
#endif
#if defined (CONFIG_RTC_HYM8563)
	{
		.type                   = "rtc_hym8563",
		.addr           = 0x51,
		.flags                  = 0,
		.irq            = RK30_PIN0_PB5,
	},
#endif
#if defined (CONFIG_MFD_WM831X_I2C)
	{
		.type          = "wm8326",
		.addr          = 0x34,
		.flags         = 0,
		.irq           = RK30_PIN0_PB3,
		.platform_data = &wm831x_platdata,
	},
#endif
#if defined (CONFIG_MFD_TPS65910)
	{
        .type           = "tps65910",
        .addr           = TPS65910_I2C_ID0,
        .flags          = 0,
        .irq            = TPS65910_HOST_IRQ,
    	.platform_data = &tps65910_data,
	},
#endif
};
#endif

void __sramfunc board_pmu_suspend(void)
{      
        #if defined (CONFIG_REGULATOR_ACT8846)
       if(pmic_is_act8846())
       board_pmu_act8846_suspend(); 
       #endif
	#if defined (CONFIG_MFD_WM831X_I2C)
       if(pmic_is_wm8326())
       board_pmu_wm8326_suspend();
	#endif
	#if defined (CONFIG_MFD_TPS65910)
       if(pmic_is_tps65910())
       board_pmu_tps65910_suspend(); 
    #endif   
}

void __sramfunc board_pmu_resume(void)
{      
        #if defined (CONFIG_REGULATOR_ACT8846)
       if(pmic_is_act8846())
       board_pmu_act8846_resume(); 
       #endif
	#if defined (CONFIG_MFD_WM831X_I2C)
       if(pmic_is_wm8326())
       board_pmu_wm8326_resume();
	#endif
	#if defined (CONFIG_MFD_TPS65910)
       if(pmic_is_tps65910())
       board_pmu_tps65910_resume(); 
	#endif
}

#if CONFIG_RK30_PWM_REGULATOR
int __sramdata gpio3d3_iomux,gpio3d3_do,gpio3d3_dir,gpio3d3_en;
#else
int __sramdata gpio3d6_iomux,gpio3d6_do,gpio3d6_dir,gpio3d6_en;
#endif

#define grf_readl(offset)	readl_relaxed(RK30_GRF_BASE + offset)
#define grf_writel(v, offset)	do { writel_relaxed(v, RK30_GRF_BASE + offset); dsb(); } while (0)
 
void __sramfunc rk30_pwm_logic_suspend_voltage(void)
{
#ifdef CONFIG_RK30_PWM_REGULATOR

//	int gpio0d7_iomux,gpio0d7_do,gpio0d7_dir,gpio0d7_en;
	sram_udelay(10000);
    gpio3d3_iomux = grf_readl(0x009c);
    gpio3d3_do = grf_readl(GRF_GPIO3H_DO);
    gpio3d3_dir = grf_readl(GRF_GPIO3H_DIR);
    gpio3d3_en = grf_readl(GRF_GPIO3H_EN);
    
    grf_writel((1<<22)|(3<<24), 0x009c);
    grf_writel((3<<27)|(3<<11), GRF_GPIO3H_DIR);
    grf_writel((3<<27)|(3<<11), GRF_GPIO3H_DO);
    grf_writel((3<<27)|(3<<11), GRF_GPIO3H_EN);
#endif 
}
void __sramfunc rk30_pwm_logic_resume_voltage(void)
{
#ifdef CONFIG_RK30_PWM_REGULATOR
	grf_writel((1<<22)|(3<<24)|gpio3d3_iomux, 0x009c);
    grf_writel((3<<27)|gpio3d3_en, GRF_GPIO3H_EN);
    grf_writel((3<<27)|gpio3d3_dir, GRF_GPIO3H_DIR);
    grf_writel((3<<27)|gpio3d3_do, GRF_GPIO3H_DO);
    sram_udelay(10000);

#endif

}
extern void pwm_suspend_voltage(void);
extern void pwm_resume_voltage(void);
void  rk30_pwm_suspend_voltage_set(void)
{
#ifdef CONFIG_RK30_PWM_REGULATOR
	pwm_suspend_voltage();
#endif
}
void  rk30_pwm_resume_voltage_set(void)
{
#ifdef CONFIG_RK30_PWM_REGULATOR
	pwm_resume_voltage();
#endif
}


#ifdef CONFIG_I2C2_RK30
static struct i2c_board_info __initdata i2c2_info[] = {
#if defined (CONFIG_TOUCHSCREEN_GSLX680) || defined (CONFIG_TOUCHSCREEN_GSLX68X) || defined (CONFIG_TOUCHSCREEN_GSL2682)
    {
        .type           = "gslX680",
        .addr           = 0x40,
        .flags          = 0,
        .platform_data =&gslx680_info,
    },
#endif
#if defined(CONFIG_TOUCHSCREEN_GT928_IIC)
    {
        .type          = "Goodix-TS",
        .addr          = 0x5d,
        .flags         = 0,
        .irq           = RK30_PIN1_PB7,
    }, 
#endif
#if defined (CONFIG_TOUCHSCREEN_ICN83XX)
	{
		.type          = "chipone-ts",
		.addr          = 0x30,
		.flags         = 0,
		//.irq           = RK30_PIN0_PB4,
		.platform_data = &icn83xx_info,
	},
#endif
#if defined (CONFIG_TOUCHSCREEN_ICN850X)
    {
        .type          = "chipone-ts",
        .addr          = 0x48,
        .flags         = 0,
        .irq           = RK30_PIN1_PB7,
    },
#endif
#if defined (CONFIG_TOUCHSCREEN_ICN8503)
    {
        .type          = "chipone-ts",
        .addr          = 0x48,
        .flags         = 0,
        .irq           = RK30_PIN1_PB7,
    },
#endif
#if defined (CONFIG_CT36X_TS)
	{
		.type	       = CT36X_NAME,
		.addr          = 0x01,
		.flags         = 0,
		.platform_data = &ct36x_info,
	},
#endif

#if defined (CONFIG_TOUCHSCREEN_MELFAS)
        {
                I2C_BOARD_INFO("melfas_mms100_MIP", 0x48),
                .irq            = GPIO_TOUCH_INT , 
                .platform_data = &melfas_touch_platform_data,
        },
#endif

#if defined (CONFIG_LS_CM3217)
	{
		.type          = "lightsensor",
		.addr          = 0x10,
		.flags         = 0,
		.platform_data = &cm3217_info,
	},
#endif
#if defined (CONFIG_LS_CM3232)
	{
		.type          = "light_cm3232",
		.addr          = 0x10,
		.flags         = 0,
		.platform_data = &cm3232_info,
	},
#endif
#if defined (CONFIG_LS_US5151)
        {    
                .type           = "us5151",
                .addr           = 0x10,
                .flags          = 0, 
        },   
#endif

#if defined(CONFIG_HDMI_CAT66121)
	{
		.type		= "cat66121_hdmi",
		.addr		= 0x4c,
		.flags		= 0,
		.irq		= RK30_PIN2_PD6,
		.platform_data 	= &rk_hdmi_pdata,
	},
#endif
};
#endif

#ifdef CONFIG_I2C3_RK30
static struct i2c_board_info __initdata i2c3_info[] = {
};
#endif

#ifdef CONFIG_I2C4_RK30
static struct i2c_board_info __initdata i2c4_info[] = {
#if defined (CONFIG_MFD_RK616)
	{
		.type	       = "rk616",
		.addr	       = 0x50,
		.flags	       = 0,
		.platform_data = &rk616_pdata,
	},
#endif
#ifdef CONFIG_MFD_RK610
		{
			.type			= "rk610_ctl",
			.addr			= 0x40,
			.flags			= 0,
			.platform_data		= &rk610_ctl_pdata,
		},
#ifdef CONFIG_RK610_TVOUT
		{
			.type			= "rk610_tvout",
			.addr			= 0x42,
			.flags			= 0,
		},
#endif
#ifdef CONFIG_HDMI_RK610
		{
			.type			= "rk610_hdmi",
			.addr			= 0x46,
			.flags			= 0,
			.irq			= INVALID_GPIO,
		},
#endif
#ifdef CONFIG_SND_SOC_RK610
		{//RK610_CODEC addr  from 0x60 to 0x80 (0x60~0x80)
			.type			= "rk610_i2c_codec",
			.addr			= 0x60,
			.flags			= 0,
			.platform_data		= &rk610_codec_pdata,			
		},
#endif
#endif

#if defined (CONFIG_SND_SOC_RT5631)
        {
                .type                   = "rt5631",
                .addr                   = 0x1a,
                .flags                  = 0,
        },
#endif

#if defined (CONFIG_SND_SOC_ES8323) 
#if defined (CONFIG_TCHIP_MACH_TRQ7_LJ) || defined (CONFIG_TCHIP_MACH_TR101Q)
        {
                .type                   = "es8323",
                .addr                   = 0x10,
                .flags                  = 0,
        },
#endif
#endif
#if   defined (CONFIG_SND_SOC_WM8904) 
        {
                .type                   = "wm8904",
                .addr                   = 0x1a,
                .flags                  = 0,
                .platform_data		= &wm8904_pdata,
        }
#endif

};
#endif

#ifdef CONFIG_I2C_GPIO_RK30
#define I2C_SDA_PIN     INVALID_GPIO// RK30_PIN2_PD6   //set sda_pin here
#define I2C_SCL_PIN     INVALID_GPIO//RK30_PIN2_PD7   //set scl_pin here
static int rk30_i2c_io_init(void)
{
        //set iomux (gpio) here
        //rk30_mux_api_set(GPIO2D7_I2C1SCL_NAME, GPIO2D_GPIO2D7);
        //rk30_mux_api_set(GPIO2D6_I2C1SDA_NAME, GPIO2D_GPIO2D6);

        return 0;
}
struct i2c_gpio_platform_data default_i2c_gpio_data = {
       .sda_pin = I2C_SDA_PIN,
       .scl_pin = I2C_SCL_PIN,
       .udelay = 5, // clk = 500/udelay = 100Khz
       .timeout = 100,//msecs_to_jiffies(100),
       .bus_num    = 5,
       .io_init = rk30_i2c_io_init,
};
static struct i2c_board_info __initdata i2c_gpio_info[] = {
};
#endif

static void __init rk30_i2c_register_board_info(void)
{
#ifdef CONFIG_I2C0_RK30
	i2c_register_board_info(0, i2c0_info, ARRAY_SIZE(i2c0_info));
#endif
#ifdef CONFIG_I2C1_RK30
	i2c_register_board_info(1, i2c1_info, ARRAY_SIZE(i2c1_info));
#endif
#ifdef CONFIG_I2C2_RK30
	i2c_register_board_info(2, i2c2_info, ARRAY_SIZE(i2c2_info));
#endif
#ifdef CONFIG_I2C3_RK30
	i2c_register_board_info(3, i2c3_info, ARRAY_SIZE(i2c3_info));
#endif
#ifdef CONFIG_I2C4_RK30
	i2c_register_board_info(4, i2c4_info, ARRAY_SIZE(i2c4_info));
#endif
#ifdef CONFIG_I2C_GPIO_RK30
	i2c_register_board_info(5, i2c_gpio_info, ARRAY_SIZE(i2c_gpio_info));
#endif
}
//end of i2c

#define POWER_ON_PIN RK30_PIN0_PA0   //power_hold
extern int dwc_vbus_status();

static void rk30_tr838_shutdown(void)
{
    int power_press_cnt = 0;
    extern int power_supply_is_system_supplied(void);
    
    printk("%s-%d: Enter\n", __FUNCTION__, __LINE__);
    while(1)
    {
        // if have not power supply, just return(shutdown)
#ifdef CONFIG_BATTERY_RK30_USB_CHARGE
        if((power_supply_is_system_supplied() == 0)&&(0== dwc_vbus_status() ))
        {
            printk("%s-%d: no power supply, shutdown\n", __FUNCTION__, __LINE__);
            break;
        }
        else
        {
            //if have power supply and press power key, restart
            if(gpio_get_value(RK30_PIN0_PA4) == GPIO_LOW)
            {
                power_press_cnt++;
                printk("%s-%d: press pwr_ker cnt=%d\n", __FUNCTION__, __LINE__, power_press_cnt);
                msleep(100);
                if(power_press_cnt > 4)
                {
                    printk("%s-%d: restart\n", __FUNCTION__, __LINE__);
                    arm_pm_restart(0, NULL);
                    break;
                }
            }
            else if (0 != dwc_vbus_status())
			{
	    		printk("%s-%d: usb charge restart\n", __FUNCTION__, __LINE__);
                arm_pm_restart(0, "charge");
			}
	    else
                power_press_cnt=0;
        }
#endif  
    }
}

#if defined(CONFIG_TCHIP_MACH_TR101Q) || defined(CONFIG_TCHIP_MACH_XBT_3188)
int tchip_board_dc_det()
{
	return (gpio_get_value(RK30_PIN0_PB2)==GPIO_LOW) ? 1:0;
}
#endif

static void rk30_pm_power_off(void)
{
	printk(KERN_ERR "rk30_pm_power_off start...\n");
#if defined(CONFIG_MFD_WM831X)
	wm831x_set_bits(Wm831x,WM831X_GPIO_LEVEL,0x0001,0x0000);  //set sys_pwr 0
	wm831x_device_shutdown(Wm831x);//wm8326 shutdown
#endif
#if defined(CONFIG_REGULATOR_ACT8846)
//#ifdef CONFIG_TCHIP_MACH_TR785
//    act8846_device_shutdown();

#if defined(CONFIG_TCHIP_MACH_TR101Q) || defined(CONFIG_TCHIP_MACH_XBT_3188)
    if (pmic_is_act8846()) {
    	if( 1 == tchip_board_dc_det() || 1 == dwc_vbus_status() || 2 == dwc_vbus_status())
        	arm_pm_restart(0, "charge");
		else
    		act8846_device_shutdown();
	}
#else
       if (pmic_is_act8846()) {
#if defined(CONFIG_TCHIP_MACH_TR838) || defined(CONFIG_TCHIP_MACH_TR785)
       rk30_tr838_shutdown();
#else
               printk("enter dcdet===========\n");
               if(gpio_get_value (RK30_PIN0_PB2) == GPIO_LOW)
               {
                       printk("enter restart===========\n");
                       arm_pm_restart(0, "charge");
               }
#endif
		/** code here may cause tablet cannot boot when shutdown without charger pluged in
		  * and then plug in charger. -- Cody Xie
               else
		{
			act8846_device_shutdown();
		}
		  */
       }
#endif
#endif
#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TR7088) || defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088)
    if(gpio_get_value (RK30_PIN0_PB2) == GPIO_LOW || 1 == dwc_vbus_status() || 2 == dwc_vbus_status())
        arm_pm_restart(0, "charge");
#endif
	gpio_direction_output(POWER_ON_PIN, GPIO_LOW);
	while (1);
}

#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TR7088)
#define WL_PWR       RK30_PIN1_PB4 
#define WL_PWR_VALUE GPIO_LOW
#else
#define WL_PWR       INVALID_GPIO
#define WL_PWR_VALUE GPIO_LOW
#endif
void wifi_bt_io_init()
{
    if(WL_PWR != INVALID_GPIO)
    {
        gpio_free(RK30_PIN1_PB4);
        gpio_request(RK30_PIN1_PB4, "wifi");//RK30_PIN3_PD0
    }
}

void wifi_bt_power_ctl(bool on)
{
    if(WL_PWR != INVALID_GPIO)
    {
        if(on)
        {
            gpio_direction_output(WL_PWR, WL_PWR_VALUE);
        }
        else
        {
            gpio_direction_output(WL_PWR, !WL_PWR_VALUE);
        }
    }
}
#if defined (CONFIG_TCHIP_MACH_XBT_3188)  
#define HP_CON_PIN  RK30_PIN0_PC6
#define FS88X6_RST_PIN  RK30_PIN0_PC3
#endif

static void __init machine_rk30_board_init(void)
{
	int ret;
#if defined(CONFIG_TCHIP_MACH_TR7088) && !defined(CONFIG_TCHIP_MACH_TR7088TN) && !defined(CONFIG_TCHIP_MACH_TR7088_CUBE)
	gpio_request(RK30_PIN3_PD6,NULL);
    gpio_pull_updown(RK30_PIN3_PD6, GPIOPullDown);
#endif
#if defined(CONFIG_TCHIP_MACH_TR1088) || defined(CONFIG_TCHIP_MACH_TR7088)
    wifi_bt_io_init();
    wifi_bt_power_ctl(false);
#endif

#if defined (CONFIG_TCHIP_MACH_XBT_3188)  
	#define WIFI_PWN  RK30_PIN0_PC5
#else
	#define WIFI_PWN  INVALID_GPIO
#endif
    if(WIFI_PWN != INVALID_GPIO)
	{
      gpio_request(WIFI_PWN, "wifi_pwr");
      gpio_direction_output(WIFI_PWN, GPIO_HIGH);
	}
	
#if defined (CONFIG_TCHIP_MACH_TR101Q)
	//wifi
		iomux_set(GPIO1_B3);
		gpio_request(RK30_PIN1_PB3, "wifi_pwr");
		gpio_direction_output(RK30_PIN1_PB3, GPIO_HIGH);
	//
#endif

	//avs_init();
	gpio_request(POWER_ON_PIN, "poweronpin");
	gpio_direction_output(POWER_ON_PIN, GPIO_HIGH);
	
	pm_power_off = rk30_pm_power_off;
	
        gpio_direction_output(POWER_ON_PIN, GPIO_HIGH);

	gpio_request(ATX8_RST_PIN, "ATX8_RST_PIN");
	gpio_direction_output(ATX8_RST_PIN, GPIO_LOW);
	gpio_free(ATX8_RST_PIN);

	rk30_i2c_register_board_info();
	spi_register_board_info(board_spi_devices, ARRAY_SIZE(board_spi_devices));
	platform_add_devices(devices, ARRAY_SIZE(devices));
	rk_platform_add_display_devices();
	//board_usb_detect_init(RK30_PIN0_PA7);

#ifdef CONFIG_WIFI_CONTROL_FUNC
	rk29sdk_wifi_bt_gpio_control_init();
#endif

#if defined(CONFIG_MT6620)
	    clk_set_rate(clk_get_sys("rk_serial.1", "uart"), 48*1000000);
#endif

#if defined(CONFIG_MT5931_MT6622) || defined(CONFIG_MTK_MT6622)
		clk_set_rate(clk_get_sys("rk_serial.0", "uart"), 24*1000000);
#endif	
#if defined(CONFIG_BK3515A_COMBO)
		clk_set_rate(clk_get_sys("rk_serial.0", "uart"), 16*1000000);
#endif

#if defined (CONFIG_TCHIP_MACH_XBT_3188)
	ret = gpio_request(HP_CON_PIN, "headset_control");
	if(ret) 
		printk ("request headset_control pin fail !\n");
	else
	   	gpio_direction_output(HP_CON_PIN, GPIO_HIGH);
#endif
#if defined (CONFIG_TCHIP_MACH_XBT_3188)
	ret = gpio_request(FS88X6_RST_PIN, "fs88x6_rst");
	if(ret) 
		printk ("request fs88x6 rst pin fail !\n");
	else
	   	gpio_direction_output(FS88X6_RST_PIN, GPIO_HIGH);
#endif
}

#define HD_SCREEN_SIZE 1920UL*1200UL*4*3
static void __init rk30_reserve(void)
{
	int size, ion_reserve_size;
#if defined(CONFIG_ARCH_RK3188)
	/*if lcd resolution great than or equal to 1920*1200,reserve the ump memory */
	if(!(get_fb_size() < ALIGN(HD_SCREEN_SIZE,SZ_1M)))
	{
		int ump_mem_phy_size=512UL*1024UL*1024UL; 
		resource_mali[0].start = board_mem_reserve_add("ump buf", ump_mem_phy_size); 
		resource_mali[0].end = resource_mali[0].start + ump_mem_phy_size -1;
	}
#endif
#ifdef CONFIG_ION
	size = ddr_get_cap() >> 20;
	if(size >= 1024) { // DDR >= 1G, set ion to 120M
		rk30_ion_pdata.heaps[0].size = ION_RESERVE_SIZE_120M;
		ion_reserve_size = ION_RESERVE_SIZE_120M;
	}
	else {
		rk30_ion_pdata.heaps[0].size = ION_RESERVE_SIZE;
		ion_reserve_size = ION_RESERVE_SIZE;
	}
	printk("ddr size = %d M, set ion_reserve_size size to %d\n", size, ion_reserve_size);
	//rk30_ion_pdata.heaps[0].base = board_mem_reserve_add("ion", ION_RESERVE_SIZE);
	rk30_ion_pdata.heaps[0].base = board_mem_reserve_add("ion", ion_reserve_size);
#endif

#ifdef CONFIG_FB_ROCKCHIP
	resource_fb[0].start = board_mem_reserve_add("fb0 buf", get_fb_size());
	resource_fb[0].end = resource_fb[0].start + get_fb_size()- 1;
#if 0
	resource_fb[1].start = board_mem_reserve_add("ipp buf", RK30_FB0_MEM_SIZE);
	resource_fb[1].end = resource_fb[1].start + RK30_FB0_MEM_SIZE - 1;
#endif

#if defined(CONFIG_FB_ROTATE) || !defined(CONFIG_THREE_FB_BUFFER)
	resource_fb[2].start = board_mem_reserve_add("fb2 buf",get_fb_size());
	resource_fb[2].end = resource_fb[2].start + get_fb_size() - 1;
#endif
#endif


#ifdef CONFIG_VIDEO_RK29
	rk30_camera_request_reserve_mem();
#endif
	
#ifdef CONFIG_GPS_RK
	//it must be more than 8MB
	rk_gps_info.u32MemoryPhyAddr = board_mem_reserve_add("gps", SZ_8M);
#endif
	board_mem_reserved();
}
/******************************** arm dvfs frequency volt table **********************************/
/**
 * dvfs_cpu_logic_table: table for arm and logic dvfs 
 * @frequency	: arm frequency
 * @cpu_volt	: arm voltage depend on frequency
 */

//sdk
static struct cpufreq_frequency_table dvfs_arm_table_volt_level0[] = {
        {.frequency = 312 * 1000,       .index = 850 * 1000},
        {.frequency = 504 * 1000,       .index = 900 * 1000},
        {.frequency = 816 * 1000,       .index = 950 * 1000},
        {.frequency = 1008 * 1000,      .index = 1025 * 1000},
        {.frequency = 1200 * 1000,      .index = 1100 * 1000},
        {.frequency = 1416 * 1000,      .index = 1200 * 1000},
        {.frequency = 1608 * 1000,      .index = 1300 * 1000},
        {.frequency = CPUFREQ_TABLE_END},
};
//default
static struct cpufreq_frequency_table dvfs_arm_table_volt_level1[] = {
        {.frequency = 312 * 1000,       .index = 875 * 1000},
        {.frequency = 504 * 1000,       .index = 925 * 1000},
        {.frequency = 816 * 1000,       .index = 975 * 1000},
        {.frequency = 1008 * 1000,      .index = 1075 * 1000},
        {.frequency = 1200 * 1000,      .index = 1150 * 1000},
        {.frequency = 1416 * 1000,      .index = 1250 * 1000},
        {.frequency = 1608 * 1000,      .index = 1350 * 1000},
        {.frequency = CPUFREQ_TABLE_END},
};
// ds1006h 10'
#if defined(CONFIG_TCHIP_MACH_TR1088) ||  defined(CONFIG_TCHIP_MACH_TR7088) ||  defined(CONFIG_TCHIP_MACH_TR7888) || defined(CONFIG_TCHIP_MACH_TR8088) || defined(CONFIG_TCHIP_MACH_TR838)
static struct cpufreq_frequency_table dvfs_arm_table_volt_level2[] = {
        {.frequency = 312 * 1000,       .index = 950 * 1000},
        {.frequency = 504 * 1000,       .index = 975 * 1000},
        {.frequency = 816 * 1000,       .index = 1050 * 1000},
        {.frequency = 1008 * 1000,      .index = 1100 * 1000},
        {.frequency = 1200 * 1000,      .index = 1200 * 1000},
        {.frequency = 1416 * 1000,      .index = 1250 * 1000},
        #ifndef CONFIG_TCHIP_MACH_TR838
        {.frequency = 1608 * 1000,      .index = 1350 * 1000},
        #endif
        {.frequency = CPUFREQ_TABLE_END},
};

static struct cpufreq_frequency_table dvfs_arm_table_volt_level3[] = {
        {.frequency = 312 * 1000,       .index = 975  * 1000},
        {.frequency = 504 * 1000,       .index = 1000 * 1000},
        {.frequency = 816 * 1000,       .index = 1075 * 1000},
        {.frequency = 1008 * 1000,      .index = 1100 * 1000},
        {.frequency = 1200 * 1000,      .index = 1250 * 1000},
        {.frequency = 1416 * 1000,      .index = 1300 * 1000},
        {.frequency = 1608 * 1000,      .index = 1350 * 1000},
        {.frequency = CPUFREQ_TABLE_END},
};

#elif defined(CONFIG_TCHIP_MACH_TR101Q)
static struct cpufreq_frequency_table dvfs_arm_table_volt_level2[] = {
        {.frequency = 312 * 1000,       .index = 950 * 1000},
        {.frequency = 504 * 1000,       .index = 950 * 1000},
        {.frequency = 816 * 1000,       .index = 1025 * 1000},
        {.frequency = 1008 * 1000,      .index = 1100 * 1000},
        {.frequency = 1200 * 1000,      .index = 1225 * 1000},
        {.frequency = 1416 * 1000,      .index = 1300 * 1000},
		#ifndef CONFIG_ARCH_RK3188T
        {.frequency = 1608 * 1000,      .index = 1350 * 1000},
		#endif
        {.frequency = CPUFREQ_TABLE_END},
};

#else
static struct cpufreq_frequency_table dvfs_arm_table_volt_level2[] = {
        {.frequency = 312 * 1000,       .index = 950 * 1000},
        {.frequency = 504 * 1000,       .index = 950 * 1000},
        {.frequency = 816 * 1000,       .index = 1025 * 1000},
        {.frequency = 1008 * 1000,      .index = 1100 * 1000},
        {.frequency = 1200 * 1000,      .index = 1225 * 1000},
        {.frequency = 1416 * 1000,      .index = 1300 * 1000},
        {.frequency = 1608 * 1000,      .index = 1350 * 1000},
        {.frequency = CPUFREQ_TABLE_END},
};
#endif
//if you board is good for volt quality,select dvfs_arm_table_volt_level0
#define dvfs_arm_table dvfs_arm_table_volt_level2

/******************************** gpu dvfs frequency volt table **********************************/
//sdk
static struct cpufreq_frequency_table dvfs_gpu_table_volt_level0[] = {	
    {.frequency = 133 * 1000,       .index = 975 * 1000},//the mininum rate is limited 133M for rk3188
    {.frequency = 200 * 1000,       .index = 975 * 1000},
    {.frequency = 266 * 1000,       .index = 1000 * 1000},
    {.frequency = 300 * 1000,       .index = 1050 * 1000},
    {.frequency = 400 * 1000,       .index = 1100 * 1000},
    {.frequency = 600 * 1000,       .index = 1200 * 1000}, 
    {.frequency = CPUFREQ_TABLE_END},
};
//ds1006h 10'
static struct cpufreq_frequency_table dvfs_gpu_table_volt_level1[] = {	
    {.frequency = 133 * 1000,       .index = 1000 * 1000},
    {.frequency = 200 * 1000,       .index = 1025 * 1000},
    {.frequency = 266 * 1000,       .index = 1050 * 1000},
    {.frequency = 300 * 1000,       .index = 1075 * 1000},
    {.frequency = 400 * 1000,       .index = 1125 * 1000},
    {.frequency = 600 * 1000,       .index = 1250 * 1000},
    {.frequency = CPUFREQ_TABLE_END},
};

#define dvfs_gpu_table dvfs_gpu_table_volt_level1

/******************************** ddr dvfs frequency volt table **********************************/
static struct cpufreq_frequency_table dvfs_ddr_table_volt_level0[] = {
	{.frequency = 200 * 1000 + DDR_FREQ_SUSPEND,    .index = 1025 * 1000},
	{.frequency = 300 * 1000 + DDR_FREQ_VIDEO,      .index = 1075 * 1000},
	{.frequency = 396 * 1000 + DDR_FREQ_NORMAL,     .index = 1150 * 1000},
    {.frequency = 460 * 1000 + DDR_FREQ_DUALVIEW,   .index = 1200 * 1000},
	//{.frequency = 528 * 1000 + DDR_FREQ_NORMAL,     .index = 1200 * 1000},
	{.frequency = CPUFREQ_TABLE_END},
};

static struct cpufreq_frequency_table dvfs_ddr_table_t[] = {
	{.frequency = 200 * 1000 + DDR_FREQ_SUSPEND,    .index = 1025 * 1000},
	{.frequency = 460 * 1000 + DDR_FREQ_NORMAL,     .index = 1200 * 1000},
	{.frequency = CPUFREQ_TABLE_END},
};

static struct cpufreq_frequency_table dvfs_ddr_table_lpddr2[] = {
	{.frequency = 200 * 1000 + DDR_FREQ_SUSPEND,    .index = 1025 * 1000},
	{.frequency = 384 * 1000 + DDR_FREQ_NORMAL,     .index = 1200 * 1000},
	{.frequency = CPUFREQ_TABLE_END},
};
/*
 * Path:rk3188T-lpddr2\B2\B9\B6\A1_V1.1_20140409
 * Data:2014 4 14  wbj
 */
#if defined(USE_LPDDR2) && defined(CONFIG_ARCH_RK3188)
    #define dvfs_ddr_table dvfs_ddr_table_lpddr2
#else
    #define dvfs_ddr_table dvfs_ddr_table_volt_level0
#endif
/******************************** arm dvfs frequency volt table end **********************************/



//#define DVFS_CPU_TABLE_SIZE	(ARRAY_SIZE(dvfs_cpu_logic_table))
//static struct cpufreq_frequency_table cpu_dvfs_table[DVFS_CPU_TABLE_SIZE];
//static struct cpufreq_frequency_table dep_cpu2core_table[DVFS_CPU_TABLE_SIZE];
int get_max_freq(struct cpufreq_frequency_table *table)
{
	int i,temp=0;
	
	for(i=0;table[i].frequency!= CPUFREQ_TABLE_END;i++)
	{
		if(temp<table[i].frequency)
			temp=table[i].frequency;
	}	
	printk("get_max_freq=%d\n",temp);
	return temp;
}

void __init board_clock_init(void)
{
	u32 flags=RK30_CLOCKS_DEFAULT_FLAGS;
    int leakage_val=0;
#if !defined(CONFIG_ARCH_RK3188)
	if(get_max_freq(dvfs_gpu_table)<=(400*1000))
	{	
		flags=RK30_CLOCKS_DEFAULT_FLAGS|CLK_GPU_GPLL;
	}
	else
		flags=RK30_CLOCKS_DEFAULT_FLAGS|CLK_GPU_CPLL;
#endif	
	rk30_clock_data_init(periph_pll_default, codec_pll_default, flags);
	//dvfs_set_arm_logic_volt(dvfs_cpu_logic_table, cpu_dvfs_table, dep_cpu2core_table);	
#if defined(USE_LPDDR2) && defined(CONFIG_ARCH_RK3188)
     //add by yyz@rock-chips.com
     leakage_val=rk_leakage_val();
     printk("board init leakage_val is %d\n",leakage_val);
     if(leakage_val<=15000)
        dvfs_set_freq_volt_table(clk_get(NULL, "cpu"),dvfs_arm_table_volt_level3);
     else
        dvfs_set_freq_volt_table(clk_get(NULL, "cpu"),dvfs_arm_table);
#else
	dvfs_set_freq_volt_table(clk_get(NULL, "cpu"), dvfs_arm_table);
#endif
	dvfs_set_freq_volt_table(clk_get(NULL, "gpu"), dvfs_gpu_table);
/*
 * Path:rk3188T-lpddr2\B2\B9\B6\A1_V1.1_20140409
 * Data:2014 4 14  wbj
 */
#if defined(USE_LPDDR2) && defined(CONFIG_ARCH_RK3188)
       dvfs_set_freq_volt_table(clk_get(NULL, "ddr"), dvfs_ddr_table);
#elif defined(CONFIG_ARCH_RK3188)
	if (rk_pll_flag() == 0)
		dvfs_set_freq_volt_table(clk_get(NULL, "ddr"), dvfs_ddr_table);
	else
		dvfs_set_freq_volt_table(clk_get(NULL, "ddr"), dvfs_ddr_table_t);
#else
	dvfs_set_freq_volt_table(clk_get(NULL, "ddr"), dvfs_ddr_table);
#endif
}

MACHINE_START(RK30, "RK30board")
	.boot_params	= PLAT_PHYS_OFFSET + 0x800,
	.fixup		= rk30_fixup,
	.reserve	= &rk30_reserve,
	.map_io		= rk30_map_io,
	.init_irq	= rk30_init_irq,
	.timer		= &rk30_timer,
	.init_machine	= machine_rk30_board_init,
MACHINE_END
