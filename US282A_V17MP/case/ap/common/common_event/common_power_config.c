#include "psp_includes.h"
#include "config_id.h"
#include "common_bat_charge.h"
#include "vm_def.h"

#define ADAPTOR_OUT_NUM                   100
#define POWEROFF_AFTER_EXIT_CHARGE       0
#define LED_BL_CHARGE_FULL_LED_PIN       2
#define LED_BL_CHARGING_LED_PIN          3

#define CHARGE_END_VOLTAGE_4_16            0
#define CHARGE_END_VOLTAGE_4_18            1
#define CHARGE_END_CHECK_TIME_12MIN        0
#define CHARGE_END_CHECK_TIME_20S          1
#define CHARGE_STEADY_VOLTAGE_3_81         0
#define CHARGE_STEADY_VOLTAGE_4_00         1
#define CHARGE_STEADY_VOLTAGE_4_25         2
#define CHARGE_CURRENT_25                   0
#define CHARGE_CURRENT_50                   1
#define CHARGE_ENFASTCHG4_4_23             1
#define CHARGE_ENFASTCHG4_4_26             2
#define CHARGE_ENFASTCHG4_4_29             3
#define CHARGE_ENFASTCHG4_4_32             4
#define CHARGE_ENFASTCHG4_4_35             5

#define  STATUS_CHARGING                     0
#define  STATUS_CHARGE_FULL                  1

static void flush_rtc(void)
{
    uint32 i;
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }

    for (i = 0; i < 256; i++)
    {
        ;//nothing for QAC
    }
}

static void delay_ms_32k(int ms)
{
    int count = ms * 5;

    while (count != 0)
    {
        count--;
    }
}

static void set_charge_led(uint8 mode)
{
	switch (mode)
	{
		case STATUS_CHARGING:
			{
				act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << LED_BL_CHARGE_FULL_LED_PIN), SIO_OUTEN);
				act_writel(act_readl(SIO_DAT) & (~(uint32) (1 << LED_BL_CHARGE_FULL_LED_PIN)), SIO_DAT);
				act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << LED_BL_CHARGING_LED_PIN), SIO_OUTEN);
                act_writel(act_readl(SIO_DAT) | (uint32) (1 << LED_BL_CHARGING_LED_PIN), SIO_DAT);
			}
			break;
			
		case STATUS_CHARGE_FULL:
			{
				act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << LED_BL_CHARGING_LED_PIN), SIO_OUTEN);
				act_writel(act_readl(SIO_DAT) & (~(uint32) (1 << LED_BL_CHARGING_LED_PIN)), SIO_DAT);
				act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << LED_BL_CHARGE_FULL_LED_PIN), SIO_OUTEN);
                act_writel(act_readl(SIO_DAT) | (uint32) (1 << LED_BL_CHARGE_FULL_LED_PIN), SIO_DAT);
			}
			break;
			
		default:
			break;
	}
	
}

static void start_charge(void)
{
    uint32 charge_ctl_val = 0;
    charge_ctl_val = act_readl(CHG_CTL);
    
    //开充电
    charge_ctl_val |=  (1 << CHG_CTL_CHGEN); 
	// 充电结束电压4.18V
	charge_ctl_val &= ~CHG_CTL_STOPV_MASK;
	charge_ctl_val |=  (CHARGE_END_VOLTAGE_4_16 << CHG_CTL_STOPV_SHIFT); 
	// 充电终止检测时间20s
	charge_ctl_val |=  (CHARGE_END_CHECK_TIME_20S << CHG_CTL_DTSEL);
	// 使能涓流充电
	charge_ctl_val |=  (1 << CHG_CTL_ENTKLE);

	//charge_ctl_val &= ~CHG_CTL_CHG_CURRENT_MASK;
//	charge_ctl_val |= (CHARGE_CURRENT_50 << CHG_CTL_CHG_CURRENT_SHIFT);
	// 设置恒流截止电压，即3V到该电压间都是恒流
	charge_ctl_val &= ~CHG_CTL_STDY_SET_MASK;
	charge_ctl_val |=  (CHARGE_STEADY_VOLTAGE_4_00 << CHG_CTL_STDY_SET_SHIFT); 
	charge_ctl_val &= ~CHG_CTL_ENFASTCHG_MASK;
	charge_ctl_val |= (CHARGE_ENFASTCHG4_4_26 << CHG_CTL_ENFASTCHG_SHIFT);
	
	PRINT_INFO_INT("charge_ctl_val",charge_ctl_val);
    act_writel(charge_ctl_val, CHG_CTL);

	set_charge_led(STATUS_CHARGING);
}

static bool  get_dc5v_status(void)
{
    bool status = 0;
    
    status =(act_readl(CHG_DET) & (1<<CHG_DET_UVLO)) >> CHG_DET_UVLO;//dc5v插入 status为真	

    return status;
}

static bool check_exit_charge(void)
{
    uint8 charge_full_flag = 0;
    static uint8 adator_out_cnt = 0;
	bool  dc5v_status = 0;
	
	dc5v_status = get_dc5v_status();
	if (dc5v_status == FALSE)
		adator_out_cnt++;
	else
		adator_out_cnt = 0;

	if (adator_out_cnt > ADAPTOR_OUT_NUM)
	{
		return TRUE;
	}

	return FALSE;
}
static bool check_charge_full()
{
	uint8 result = FALSE;

	if (bat_charge_full() != 0)
	{
		result = TRUE;
	}

	return result;
}


static void charge_loop(uint8 para)
{
    uint8 exit_op = para;
	uint8 set_led_flag = 0;

    while(1)
    {   	
     /*   CLEAR_WATCH_DOG;	
        if ( TRUE == check_exit_charge() )
        {
            if(exit_op  == POWEROFF_AFTER_EXIT_CHARGE) // 进入S4模式
            {
          	act_writel(act_readl(RTC_CTL) | (1 << 2), RTC_CTL);
    		act_writel(act_readl(HCL_CTL) & (~(1 << 0)), HCL_CTL);
    		act_writel(0xA596, RTC_REGUPDATA);
    		while (0x5A69 != act_readl(RTC_REGUPDATA))
    		{
        ;		//delay for RTC update complete
   			}
                act_writel(act_readl(WAKE_PD), WAKE_PD); //清pending
                act_writel(0,POWER_CTL);
               while(1)
                {
                    ;//nothing for QAC
               }
            }
            else
            {
                 act_writel(act_readl(RTC_BAK0) | ((1 << MY_CHARGE_OUT_STATE)), RTC_BAK0);
   		         flush_rtc();
                ENABLE_WATCH_DOG(1);
                while(1)
                {
                    ;//nothing for QAC
                }
            }
        }
        if ( TRUE == check_charge_full() )
        {
        	if (set_led_flag == 0)
        	{
        	    bat_charge_close();
        		set_charge_led(STATUS_CHARGE_FULL);
                set_led_flag = 1;
			}
        }*/
        ENABLE_WATCH_DOG(1);
        if ( TRUE == check_exit_charge() )
        {
            if(exit_op  == POWEROFF_AFTER_EXIT_CHARGE) // ??S4??
            {
            	CLEAR_WATCH_DOG;
                act_writel(act_readl(WAKE_PD), WAKE_PD); //?pending
                act_writel(0,POWER_CTL);
                while(1)
                {
                    ;//nothing for QAC
                }
            }
			else
            {
           
               // ENABLE_WATCH_DOG(1);
				while(1);
              
            }
			
        }
        else
        {
        	CLEAR_WATCH_DOG;
            if ( TRUE == check_charge_full() )
            {
            	if (set_led_flag == 0)
            	{
            		set_charge_led(STATUS_CHARGE_FULL);
            		bat_charge_close();
            		//??DC5V SYSpower???
            		//act_writel(act_readl(SPD_CTL) & (0xfffe7fff), SPD_CTL);
                    set_led_flag = 1;
		    	}
            }
        }     
    }
}


/*降频再降压，升压再升频*/
static void s2_low_power_configuration(void)
{
    uint32 t1 = 0;
    uint32 t2 = 0;
    uint32 t3 = 0;
    uint32 k1 = 0;
    uint32 _24MPLL_CTL_temp = 0;
    uint32 CORE_PLL_CTL_temp = 0;
    uint32 CMU_SYSCLK_temp = 0;
       
    _24MPLL_CTL_temp = act_readl(_24MPLL_CTL);
    CORE_PLL_CTL_temp = act_readl(CORE_PLL_CTL);
    CMU_SYSCLK_temp = act_readl(CMU_SYSCLK);
        
	  PRINT_INFO("-- enter s2 ---");

    act_writel(0x00000000,BT_CFG);			//0.01ma
    flush_rtc();

    //Module CLK disable  
    act_writel(0x00000000,CMU_DEVCLKEN);	//0.03ma

    act_writel(0x00000004,MRCR);
    act_writeb(0,USB_PHYCTRL);

    /*****************************************PMU************************************/
    //USB standby
    act_writel(0,MULTI_USED);

    // disable dcdc
    act_writel(0,VD15_DCDC_CTL);

    //close bangap pulldown
    t1 = 0x0000001F;
    t2 = act_readl(BDG_CTL);
    t2 = t2&t1;
    act_writel(t2,BDG_CTL);

    //close over voltage det
    //act_writel(0x0010013a,CHG_CTL);

    //close ADC
    act_writel(0,PMUADC_CTL);
    //close S3 timer
    act_writel(0,TIMER_CTL);

    //close ldo pd 
    act_writel(0x000a8088,LDO_CTL);

    //耐压偏置切为小电流
    act_writel(0x000002af,SYSTEM_SET);

    //关闭S4下拉
    act_writel(0x00000100,SPD_CTL);

    //close wake source
    //act_writel(0,WKEN_CTL);
    //flush_rtc();

    //GPIO 2823B用的nor ss是GPIOA29 需要在spi_nor控制器复位后，将该IO口配置成输出模式并拉高
    act_writel(0,GPIOBOUTEN);

    act_writel(0xf0000000,GPIOAOUTEN); //1ma  	 
    act_writel(0xf0000000,GPIOADAT);

    act_writel(0,GPIOBDAT);
    act_writel(0xffffffff,SIO_OUTEN);

    act_writel(0,SIO_DAT);
    act_writel(0x14,GPIOB0_CTL); 
    flush_rtc();
    act_writel(0,PADDRV0);
    act_writel(0,PADDRV1);
    act_writel(0,PADDRV2);
    act_writel(0,PADDRV3);

    //close HCL
    act_writel(0,HCL_CTL);
    flush_rtc();
    // OPEN HCL    
    act_writel(1,HCL_CTL);
    flush_rtc();
    //close HCL
    act_writel(0,HCL_CTL);
    flush_rtc(); 

    //选择内部Losc   
    act_writel(0,CMU_SYSCLK); 				//0.1ma

    // close HOSC & 24MPLL
    act_writel(0,_24MPLL_CTL);				//1.9ma

    // close HOSC GMAIN 
    act_writel(0,HOSC_CTL);

    act_writel(0x00000010,CORE_PLL_CTL);	//0.2ma
    act_writel(0x00000046,AUDIO_PLL_CTL);   //0.2ma

    act_writel(0x2022,VOUT_CTL);			//0.2ma 
    delay_ms_32k(2);
} 

void power_processor(void)
{
	uint8 sys_enters4_or_reboot = 0;
	sys_enters4_or_reboot = (uint8) com_get_config_default(SETTING_CHARGE_FULL_DC5V_UNSTICK_SYS_MODE);
    if(sys_enters4_or_reboot == 1)
    {
    	act_writel(act_readl(RTC_BAK0) | ((1 << MY_CHARGE_OUT_STATE)), RTC_BAK0);
    	flush_rtc();
    }
	sys_local_irq_save();
    sys_os_sched_lock();
    
    s2_low_power_configuration();
    
    start_charge();
    charge_loop(sys_enters4_or_reboot);
}