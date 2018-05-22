#include "common_func.h"

#define SHORT_BLINK 100

//int8 g_led_timmer = -1;
int8 LED_FLASH_UP =0 ;    //up modify for 20170420
led_duty_t duty;

void led0_timmer_cb(void)
{
    //discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
    discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
}
void led01_timmer_cb_flash(void)     //UP MODIFY FOR 20170420
{
    //discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
   
    if(LED_FLASH_UP == 0)
    {
    	discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
		discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
		LED_FLASH_UP++;
    }
	else
	{
	   discrete_led_set(LED_ID_BT_SYS,LED_TWINKLE,&duty);
        discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
		LED_FLASH_UP++;
		if(LED_FLASH_UP == 2)
			{
			  LED_FLASH_UP = 0;
			}
	}
}

void led1_timmer_cb(void)
{
    //discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_ON,NULL);
    discrete_led_set(LED_ID_BT_SYS,LED_TWINKLE,&duty);
}

void set_led_state(led_status_e led_state)
{
    uint16 i;
    if(g_app_info_state_all.g_hp_led_state == led_state)
    {
        return;
    }

	if(led_state == 7)
	{
    	if((g_customer_state&CUSTOMER_SLAVEDIS) != 0)               
		{  
	      	PRINT_INFO("up led 123");
          	//g_customer_state&= (~CUSTOMER_SLAVEDIS);         
		 	return;
		}
	}
    //如果当前状态为低电，则不作连接、播放相关指示
    if (((g_app_info_state_all.g_hp_led_state == LED_LOW_POWER)         \
        || (g_app_info_state_all.g_hp_led_state == LED_LOW_POWER_OFF)   \
        || (g_app_info_state_all.g_hp_led_state == LED_POWER_OFF))      \
        && (led_state > LED_LPS_MODE))
    {
        return;
    }
    
    g_app_info_state_all.g_hp_led_state = led_state;

    if (g_led_timmer != -1)
    {
        kill_app_timer(g_led_timmer);
        g_led_timmer = -1;
        sys_os_time_dly(10);
    }
    libc_print("set led:",led_state,2);
    
    discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
    discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);

    switch(led_state)
    {
        case LED_POWER_OFF:
         discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);  //UP MODIFY 20170420
        for(i = 0;i < 2;i++)
        {
            discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_ON,NULL);
            sys_os_time_dly(10);
            discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
            sys_os_time_dly(50);
        }
		
        break;

        case LED_CHARGING:
        //discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_ON,NULL);
       // discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
        break;
        
        case LED_CHARGE_FULL:
       // discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
       // discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
        break;
        
        case LED_LOW_POWER:
        duty.duty_on_time = 200;
        duty.duty_off_time = 200;
        duty.cycle_count = 2;
        discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
        g_led_timmer = set_app_timer(APP_TIMER_ATTRB_COM_CONTROL,5000,led1_timmer_cb);
        break;
        
        case LED_LOW_POWER_OFF:
        duty.duty_on_time = 200;
        duty.duty_off_time = 200;
        duty.cycle_count = 2;
        discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
        discrete_led_set(LED_ID_BT_SYS,LED_TWINKLE,&duty);
        break;
        
        case LED_LPS_MODE:
        discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
        break;

        case LED_CONNECTING:
        duty.duty_on_time = 200;  //up modify for 20170420
        duty.duty_off_time = 0;
		//discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
       // discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_ON,NULL);
        g_led_timmer = set_app_timer(APP_TIMER_ATTRB_COM_CONTROL,200,led01_timmer_cb_flash);
        break;
        
        case LED_PAIRED:
      
        break;
        
        case LED_CONNECTED:
        duty.duty_on_time = 200;
        duty.duty_off_time = 200;
        duty.cycle_count = 2;
        discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        g_led_timmer = set_app_timer(APP_TIMER_ATTRB_COM_CONTROL,5000,led0_timmer_cb);
        break;
        
        case LED_PLAY:
        duty.duty_on_time = SHORT_BLINK;
        duty.duty_off_time = 5000;
        duty.cycle_count = -1;
        discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
        break;
        
        case LED_PAUSE:
		if((g_bt_stack_cur_info.dev_role == TWS_MASTER)||(g_bt_stack_cur_info.dev_role == NORMAL_DEV))
		{
		 
		    duty.duty_on_time = 200;
        	duty.duty_off_time = 200;
        	duty.cycle_count = 2;
        	discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        	g_led_timmer = set_app_timer(APP_TIMER_ATTRB_COM_CONTROL,5000,led0_timmer_cb);
		   
		   
		}
        else if(g_bt_stack_cur_info.dev_role == TWS_SLAVE)
        {
            duty.duty_on_time = 200;
        	duty.duty_off_time = 200;
        	duty.cycle_count = 2;
        	discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        	g_led_timmer = set_app_timer(APP_TIMER_ATTRB_COM_CONTROL,5000,led0_timmer_cb);
        }
        break;
        
        case LED_CALLING:
        duty.duty_on_time = SHORT_BLINK;
        duty.duty_off_time = 900;
        duty.cycle_count = -1;
        discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
        break;
         case LED_POWERON:
         discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);  //UP MODIFY 20170420
        for(i = 0;i < 2;i++)
        {
            discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
            //sys_os_time_dly(10);
            //discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
            sys_os_time_dly(30);
        }
        break;
			case LED_CLEARPAIRED:
        for(i = 0;i < 2;i++)
        {
             discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_ON,NULL);
			 discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_ON,NULL);
             sys_os_time_dly(50);
			  discrete_led_set(LED_ID_BT_LINK,LED_ALWAYS_OFF,NULL);
			  discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
			  sys_os_time_dly(50);
        }	
         break;
	    case LED_CALLON:
	
       duty.duty_on_time = SHORT_BLINK;
        duty.duty_off_time = 5000;
        duty.cycle_count = -1;
        discrete_led_set(LED_ID_BT_SYS,LED_ALWAYS_OFF,NULL);
        discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
         break;
		   case LED_TWSPAIR:
	
       duty.duty_on_time = 200;
        duty.duty_off_time = 1000;
        duty.cycle_count = -1;
      
        discrete_led_set(LED_ID_BT_LINK,LED_TWINKLE,&duty);
		  discrete_led_set(LED_ID_BT_SYS,LED_TWINKLE,&duty);
         break;
        default:
        break;
    }
}

