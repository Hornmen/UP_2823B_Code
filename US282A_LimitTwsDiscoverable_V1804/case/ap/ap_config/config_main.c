/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main.c
 * \brief    开关机的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_config.h"
#include "uhost.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     1

/*!
 *  \brief
 *  app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
 */
app_timer_t conf_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];
//系统变量
comval_t g_comval;

//wav录音掉电回写文件头结构体
record_writehead_t g_writehead;
//是否第一次上电标志
bool g_first_boot = FALSE;

/*main中要填充这两个值，有系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_FRONT_LOW_STK_POS;
INT8U prio = AP_FRONT_LOW_PRIO;

bool usb_in_flag = FALSE;
bool uhost_in_flag = FALSE;
bool adaptor_in_flag = FALSE;
bool sd_in_flag = FALSE;
bool linein_in_flag = FALSE;
bool rtcalarm_flag = FALSE;
bool g_config_standby_exit = FALSE;
bool g_config_bt_flag = FALSE;
bool g_config_esd_restart = FALSE;

//是否支持UHOST延迟
uint8 g_uhost_delay = 0;

/******************************************************************************/
/*!
 * \par  Description:
 *  读取应用VM全局变量,初始化全局变量
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
void __section__(".bank_var") _read_var(void)
{
    wp_zone_info_t wp_zone_info;
    
    //读取common VM公共变量信息
    com_setting_comval_init(&g_comval);

    sys_vm_read(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));
    //初始化config的VM变量
    //if ((g_config_var.magic != VRAM_MAGIC(VM_AP_CONFIG)) || ((uint8) ((act_readl(RTC_BAK0) \
   //     & (0xff << MY_RTC_FUNC_INDEX)) >> MY_RTC_FUNC_INDEX) == APP_FUNC_INVALID))
   if ((g_config_var.magic != VRAM_MAGIC(VM_AP_CONFIG))||
            (g_config_var.ap_id == APP_FUNC_INVALID))
    {
        //第一次上电
        g_first_boot = TRUE;
        g_config_var.magic = VRAM_MAGIC(VM_AP_CONFIG);

        g_config_var.esd_flag = FALSE;

        g_config_var.ap_id = APP_FUNC_INVALID;

        sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

        //清除S3BT标志
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_S3BT)), RTC_BAK0);
        config_flush_rtc_reg();
        
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_BT_FLAG)), RTC_BAK0);
        config_flush_rtc_reg();
        
        //清除ESD RESTART标志
        act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_ESD_FLAG)), RTC_BAK0);
        config_flush_rtc_reg();

        //第一次上电需要设置睡眠唤醒方式
        en_play_wake_up(FALSE);

        base_query_support_write_protect(&wp_zone_info);

        if(wp_zone_info.set_write_protect_size != 0)
        {
            PRINT_INFO("flash support write protect");

            PRINT_INFO_INT("fw size: ", wp_zone_info.fw_cap_size);

            PRINT_INFO_INT("spi flash size: ", wp_zone_info.spi_cap_size);

            PRINT_INFO_INT("need protect size: ", wp_zone_info.need_write_protect_size);

            PRINT_INFO_INT("real protect size: ", wp_zone_info.set_write_protect_size);
        }
        else
        {
            PRINT_INFO("flash not support write protect");
        }
    }
}
void HCL_CTL_init(void)
{
    act_writel(act_readl(HCL_CTL) & (~(1 << 0)), HCL_CTL);
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }
    act_writel(act_readl(HCL_CTL) | (1 << 0), HCL_CTL);
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }
    act_writel(act_readl(HCL_CTL) & (~(1 << 0)), HCL_CTL);
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }
}
/******************************************************************************/
/*!
 * \par  Description:
 *  applib初始化
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _app_init(void)
{
    //初始化applib库，前台AP
    applib_init(APP_ID_CONFIG, APP_TYPE_GUI);

    //初始化软定时器
    init_app_timers(conf_app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    //初始化 applib 消息模块
    applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前

    com_view_manager_init();
#ifdef SUPPORT_ASET_TEST
    aset_global_para_init();
#endif    

    return TRUE;
}

bool _app_exit(void)
{
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    com_view_manager_exit();

    //备份系统计时器
    sys_timer_exit();

    //执行applib库的注销操作
    applib_quit();

    return TRUE;
}
/******************************************************************************/
/*!
 * \par  Description:
 *    MainMenu 对耳左右声道通过GPIO口来硬件检测
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void Output_channel_IO_init (void)
{
   	act_writel(act_readl(MFP_CTL2) & (~(1<<30)), MFP_CTL2);
	act_writel(act_readl(GPIOBINEN) | (uint32) (1 << 9), GPIOBINEN);
    act_writel(act_readl(GPIOBOUTEN)  & (~(uint32) (1 << 9)), GPIOBOUTEN);
    act_writel(act_readl(GPIOBPUEN) | (uint32) (1 << 9), GPIOBPUEN);
    act_writel(act_readl(GPIOBINEN) | (uint32) (1 << 8), GPIOBINEN);
    act_writel(act_readl(GPIOBOUTEN)  & (~(uint32) (1 << 8)), GPIOBOUTEN);
    act_writel(act_readl(GPIOBPUEN) | (uint32) (1 << 8), GPIOBPUEN);
   
  
}

int check_s3bt_vram_valid(void)
{
    int start_index;

    int i;

    for(i = 0; i < 4; i++)
    {
        if(i == 2)
        {
            //VM_S3BT_APP_STATUS这个值在这里先不判断
            continue;
        }
        
        if(base_read_vram_index(VM_S3BT_APP_GLOBAL + (i << 16)) == 0)
        {
            break;
        }
    }

    if(i < 4)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}   

/******************************************************************************/
/*!
 * \par  Description:
 *    config应用的入口函数
 * \param[in]    开关机参数：0-开机；1-关机进S4；2-低电关机进S4；3-低功耗进S3
 * \param[out]   none
 * \return       the result
 * \retval
 * \retval
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note 关机说明，电池供电时，关机会进入stanby，开机时是重启系统
 电源供电时，关机只是挂起系统，再开机时不是重启
 *******************************************************************************/
int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;
//	uint32 onoff_state = 0;
//	uint32 rtc_onoff_state = 0;
    bool power_on_flag = 0;
//	int16 dt = 180;    // 检测到onoff键长按2s才开机。因为从开机运行到次约200ms时间，所以这里检测1800ms
	g_customer_state &= (~CUSTOMER_POWERONOFF);
	g_customer_state&= (~CUSTOMER_SLAVEDIS) ;    //add by up 20171109
	
	g_customer_state&= (~CUSTOMER_TWSPAIR_FLAG);
	//libc_print("state1",g_customer_state,2);
	
    if(((uint32) argc & 0xff) == POWER_ON)
    {
        power_on_flag = 1;
        argc &= 0xff00;
    }
	
//	PRINT_INFO_INT("RTC_BAK0",act_readl(RTC_BAK0));
//	PRINT_INFO_INT("ONOFF_KEY",act_readl(ONOFF_KEY));
	
//	rtc_onoff_state = act_readl(RTC_BAK0) & (1<<MY_RTC_ONOFF_STATE);
	//onoff_state = act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0);

/*	if (rtc_onoff_state == (1 << MY_RTC_ONOFF_STATE))
	{
	   
	    act_writel(act_readl(RTC_BAK0)&(~(1<<MY_RTC_ONOFF_STATE)), RTC_BAK0);
		config_flush_rtc_reg();
		while (dt--)
		{
			sys_os_time_dly(1);   // 10ms检测一次，防止漏检测
			if ((act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0)) == 0) 
			{
				argc = SWITCH_OFF; //短按关机
				break;
			}
	    }
	}*/

	//如果在welcome检测到onoff按下：ap_config又检测到其按下就认为是onoff长按，否则是短按
	/*if ((rtc_onoff_state == (1 << MY_RTC_ONOFF_STATE)) && (onoff_state == (1<<ONOFF_KEY_ONOFF_PRESS_0)))
	{
		 PRINT_INFO("onoff long press");
		 act_writel(act_readl(RTC_BAK0)&(~(1<<MY_RTC_ONOFF_STATE)), RTC_BAK0);
		 config_flush_rtc_reg();
	}
	else if ((rtc_onoff_state == (1 << MY_RTC_ONOFF_STATE)) && (onoff_state == (0<<ONOFF_KEY_ONOFF_PRESS_0)))
	{
		 PRINT_INFO("onoff short press");
		 act_writel(act_readl(RTC_BAK0)&(~(1<<MY_RTC_ONOFF_STATE)), RTC_BAK0);
		 config_flush_rtc_reg(); 
		 argc = SWITCH_OFF; //短按关机
	}*/
	
    //开机提速
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL8, FREQ_NULL);
    HCL_CTL_init();
    _read_var(); //g_comval 的初始化必须在 config_globe_data_init 之前

	Output_channel_IO_init();
   
   // if (((uint32) argc & 0xff) == SWITCH_ON)
       if ((((uint32) argc & 0xff) == SWITCH_ON)||
        ((((uint32) argc & 0xff) == SWITCH_S3BT_TIMER_MODE)))
    {
        //DAE配置环境已初始化好的标志，确保不会在DAE配置环境初始化好之前进行DAE参数设置
        g_comval.dae_inited_flag = FALSE;

        //初始化所有前台应用共享的全局变量（这些全局变量必须且只能初始化一次）
        config_globe_data_init(); //必须在 _app_init 之前调用

        if ((act_readl(RTC_BAK0) & (1 << MY_RTC_S3BT)) != 0)
        {
            if ((act_readl(RTC_BAK0) & (1 << MY_RTC_BT_FLAG)) != 0)
            {
                g_config_bt_flag = TRUE;               
            }
          // g_customer_state&= (~CUSTOMER_POWERONOFF) ;    //add by up 20170912
            //清除S3BT标志
            act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_S3BT)), RTC_BAK0);
            config_flush_rtc_reg();
            act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_BT_FLAG)), RTC_BAK0);
            config_flush_rtc_reg();

            g_config_standby_exit = TRUE;
            
            if(power_on_flag)       //意外断电
            {
                libc_print("UNEXP SHUT DOWN",0,0);
                g_config_bt_flag = FALSE;
                g_config_standby_exit = FALSE;
            }

            //这里判断vram相关index是否存在，如果不存在，则不走s3bt流程
            if(check_s3bt_vram_valid() == FALSE)
            {
                libc_print("s3bt vram invalid",0,0);
                g_config_bt_flag = FALSE;
                g_config_standby_exit = FALSE;                
            }
        }
        else
        {
			g_customer_state &= (~S3WAKE_ONOFF_PLAY);
            if ((act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0)) != 0) //PLAY键 //add by up 20170912
	 		{
		    	g_customer_state |= CUSTOMER_POWERONOFF;

				act_writel(act_readl(ONOFF_KEY) & (~ (1<<ONOFF_KEY_ONOFF_RST_EN)), ONOFF_KEY);
				config_flush_rtc_reg();
				//tmp = act_readl(ONOFF_KEY);
	            //libc_print("ONOFF",tmp,2);
				
			}
            if(base_read_vram_index(VM_S3BT_BT_STACK) == (VM_S3BT_BT_STACK_LEN + 4))
            {        
                base_clear_vram_index(VM_S3BT_BT_STACK);            
            }    
            
            if (((act_readl(RTC_BAK0) & (1 << MY_RTC_ESD_FLAG)) != 0) || (g_config_var.esd_flag == TRUE))
            {
                //检测记录的ap func index是否合法，不合法的ap func index进行过滤
                //防止开机又关机
                if(com_check_ap_func_index_valid() == TRUE)
                {
                    g_customer_state |= CUSTOMER_ESD_WAKEUP;
                    //不清除ESD RESTART标志，由AP自己清除
                    g_config_esd_restart = TRUE;
                }
                
                if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)//电源接入
                {
                    g_config_esd_restart = FALSE;
                }
            }
        }
    }

    if(((uint32) argc & 0xff) == SWITCH_S3BT_TIMER_MODE)
    {
        enter_standby_S3S4(FALSE);
    }

    g_config_var.esd_flag = TRUE;
    
    sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

    //applib initial
    _app_init();

    if (((uint32) argc & 0xff) == SWITCH_ON)
    {
     
        config_sys_init_display();

        //尽量提前加载BT STACK
        if ((g_config_standby_exit == TRUE) && (g_config_bt_flag == TRUE))
        {
            sys_base_set_enter_s3bt_scene();
            
            com_btmanager_init(TRUE);//pathcode已加载，不用再加载
        }

        //卡驱动时钟，驱动能力等设置
        card_set_drv_power();

        config_key_module();

        //每次开机都重新某些DAE配置项及参数
        com_reset_dae_config(&g_comval);
        //初始化DAE配置参数共享内存
        com_init_dae_config(&g_comval);

        if ((g_comval.hard_support_uhost == 1) && (g_comval.support_uhostplay == 1))
        {
            UHOST_POWER_OPEN();
            UHOST_POWER_ENABLE();
            //set uhost cfg
            g_uhost_delay = (uint8) com_get_config_default(SETTING_UHOST_USE_DELAY);
            g_uhost_cfg = g_uhost_delay;
        }

        config_sys_init();

        if (g_config_standby_exit == TRUE)
        {
            PRINT_INFO("EXIT S3BT");

			if((g_customer_state&S3WAKE_ONOFF_PLAY) != 0)
			{
			    if ((act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0)) != 0) //PLAY键
			    {
					 
		     		g_customer_state&= (~S3WAKE_ONOFF_PLAY) ;
					 
			    }
			}

            if ((g_comval.hard_support_uhost == 1) && (g_comval.support_uhostplay == 1))
            {
                if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) == 0)
                {
                    //切换到UHOST并延时等待系统检测
                    key_peripheral_detect_handle(PER_DETECT_UHOST_FORCE);
                }
            }

            result = RESULT_SYSTEM_EXIT_S3;
        }
        else
        {
            result = config_poweron_option();

            if (g_config_esd_restart == TRUE)
            {
                PRINT_INFO("ESD RESTART");
                result = RESULT_ESD_RESTART; //ESD RESTART 不理会开机选择，执行开机选项仅仅是为了保证外设状态正确
            }

            if (result == RESULT_APP_QUIT)
            {
                goto config_exit;
                //自动测试，已自己加载应用，无条件退出
            }

            if ((result != RESULT_LOW_POWER) && (result != RESULT_ESD_RESTART)) //低电不做任何事情关机
            {
                config_record_writehead();
            }
        }

        //开启按键音
        keytone_set_on_off(TRUE);

        Reset_start:
#if (SUPPORT_OUTER_CHARGE == 1)
        if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            if(result == RESULT_USB_TRANS)
            {
                if (IS_EXTERN_BATTERY_IN () == 1)
                {
                    result = RESULT_NULL;
                }
            }
        }
#endif
        config_power_on_dispatch(result);
    }
    else //以下是关机操作
    {
        //解除静音模式
        com_set_mute(FALSE);
        
		
        if (((uint32) argc & 0xff) == SWITCH_LOWPOWER)
        {
            //电量不足
            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NORECV_ALL_EV_YES, (void *) TTS_BATTERY_LOW);
            sys_os_time_dly(50);
        }

        /*播报关机*/
        if ((((uint32) argc & 0xff) == SWITCH_OFF) || (((uint32) argc & 0xff) == SWITCH_LOWPOWER))
        {
            if (g_ap_switch_var.rtcalarm_poweroff == FALSE) //闹钟完毕后关机不播报TTS
            {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                //tws下只有主箱才可以播报关机
               // if(g_bt_stack_cur_info.dev_role!=TWS_SLAVE)
#endif
               // {
                    if (detect_power_on() == TRUE) //电源插入，进入充电关机不播报电量低
                    {
                        if(DC5V_MANAGER_MODE==1)
                        {
                            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NORECV_ALL_EV_YES, (void *) TTS_POWEROFF);
                        }
                    }
                    else
                    {
                         com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NORECV_ALL_EV_YES, (void *) TTS_POWEROFF);
                         set_led_state(LED_POWER_OFF);
                    }
                //}
                
            }
			
        }

        //进入S3BT
        if (((uint32) argc & 0xff) == SWITCH_STANDBY)
        {
            //如果这个时候收到PD标志位，结束退出流程
            if((act_readl(INTC_PD) & 0x01) != 0)
            {
                PRINT_INFO("exit config");
                com_ap_switch_deal(RESULT_SYSTEM_EXIT_S3);  
                goto config_exit;
            }
            else
            {
            	//BTSTACK 退出
            	g_config_var.esd_flag = FALSE;          //清除ESD标记
                act_writel(act_readl(RTC_BAK0) | (1 << MY_RTC_BT_FLAG), RTC_BAK0);
                config_flush_rtc_reg();
                com_btmanager_exit(TRUE, FALSE);
                result = sys_power_off(TRUE);
                goto config_exit;
            }
        }
        else
        {
            g_config_var.esd_flag = FALSE;          //清除ESD标记
            sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));

            //BTSTACK 退出
            com_btmanager_exit(FALSE, TRUE);
            sys_os_time_dly(50);

            //进入空闲模式
            if (((uint32) argc & 0xff) == SWITCH_IDLE_MODE)
            {
                result = config_idle_mode_loop();
            }
            //断电关机
            else
            {
                result = sys_power_off(FALSE);
            }
        }

        goto Reset_start;
    }

    config_exit: _app_exit();

    return RESULT_NULL;
}

