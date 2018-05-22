/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * ������ǰ̨Ӧ�ÿ�ݼ�Ĭ�ϴ���ǰ̨Ӧ���Լ�����İ������������ڿ�ݼ�Ĭ�ϴ���
 * ���ߣ�cailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */
#define POWEROFF_TIME 5
uint8 key_hold_flag = 0;
uint8 key_clearpaired_flag = 0;

app_result_e com_key_deal_switch_app(void);
app_result_e com_key_deal_switch_mute(void);
app_result_e com_key_deal_volume_add(void);
app_result_e com_key_deal_volume_sub(void);
//app_result_e com_key_deal_enter_clock(void);
//app_result_e com_key_deal_enter_crecord(void);
//app_result_e com_key_deal_enter_urecord(void);
//app_result_e com_key_deal_enter_record_cplay(void);
//app_result_e com_key_deal_enter_record_uplay(void);
//app_result_e com_key_deal_switch_dae(void);
app_result_e com_key_deal_shutoff(void);
//reserve
app_result_e com_key_deal_switch_tts_language(void);
app_result_e com_key_deal_force_unlink(void);
app_result_e com_key_deal_call_last(void);
app_result_e com_key_deal_BQB_test(void);
app_result_e com_key_deal_FT_test(void);
app_result_e com_key_deal_siri(void);

//for externsion function
app_result_e com_hid_send_key(void);
app_result_e com_hid_disconnect(void);
app_result_e com_key_deal_enter_ota(void);
app_result_e com_key_deal_force_ble_unlink(void);
app_result_e com_key_deal_force_unlink_10s(void);
#if 0
app_result_e com_key_cancel_search(void);
app_result_e com_tws_disconnect(void);
#endif

const key_event_map_t __section__(".rodata.ke_maplist") com_ke_maplist[] =
{
    /*! �л�Ӧ��ģʽ */
 //   { { KEY_MODE, 0, KEY_TYPE_SHORT_UP, 0 }, com_key_deal_switch_app },

    /*! �������� */
  //  { { KEY_MUTE, 0, KEY_TYPE_SHORT_UP, 0 }, com_key_deal_switch_mute },

    /*! ������ */
 //   { { KEY_VADD, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP }, com_key_deal_volume_add },
  //  { { KEY_NEXT_VOLADD, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP }, com_key_deal_volume_add },
//#ifdef OTA_VERSION
//#if 0
      //{ { KEY_VADD, 0, KEY_TYPE_DBL_CLICK, 0 }, com_key_deal_force_ble_unlink },
//#endif //shade ota ap hot key deal
//#endif //OTA_VERSION
    /*! ������ */
//    { { KEY_VSUB, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP }, com_key_deal_volume_sub },
 //   { { KEY_PREV_VOLSUB, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, KEY_DEAL_FILTER_UP }, com_key_deal_volume_sub },

    /*! ��ʾʱ�� */
    //{{ KEY_CLK, 0, KEY_TYPE_SHORT_UP, 0},                              com_key_deal_enter_clock},
    //{{ KEY_ABREC, 0, KEY_TYPE_SHORT_UP, 0},                            com_key_deal_enter_clock},
#ifdef __SUPPORT_HID_      
      /*! hid photo */
    //{ { KEY_EQ, 0, KEY_TYPE_SHORT_UP, 0 }, com_hid_send_key },
     /*! hid disconenct */
  //  { { KEY_EQ, 0, KEY_TYPE_DBL_CLICK, 0 }, com_hid_disconnect },
#endif //__SUPPORT_HID_

#if (__SUPPORT_SIRI_ == 1)
       // SIRI
   // { { KEY_EQ, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_siri },
#endif
    //{ { KEY_EQ, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_cancel_search},

    //{ { KEY_EQ, 0, KEY_TYPE_DBL_CLICK, 0 }, com_tws_disconnect },


    /*! ����test mode */
    //{ { KEY_EQ, 0, KEY_TYPE_LONG_10S, KEY_DEAL_FILTER }, com_key_deal_FT_test },
 //   { { KEY_EQ, 0, KEY_TYPE_LONG_10S, KEY_DEAL_FILTER }, com_key_deal_force_unlink_10s },
    //{ { KEY_EQ, 0, KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER }, com_key_deal_BQB_test },
 //   { { KEY_EQ, 0, KEY_TYPE_DBL_CLICK, KEY_DEAL_FILTER }, com_key_deal_force_ble_unlink },

    /*! ����¼�� */
#if (SUPPORT_RECORD == 1)
    { { KEY_ABREC, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_enter_crecord },
    { { KEY_NUM9, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_enter_urecord },
#endif

    /*! ¼���ط� */
 //   { { KEY_MUTE, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_enter_record_cplay },
  //  { { KEY_NUM0, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_enter_record_uplay },

    /*! �л�������Ч */
    //{{ KEY_EQ, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, 0},            com_key_deal_switch_dae},
    /*! �ػ� */
 //   { { KEY_POWER, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_shutoff },

#if (CASE_BOARD_TYPE != CASE_BOARD_DEMO)
    { { KEY_PLAY, 0, KEY_TYPE_HOLD, 0 }, com_key_deal_shutoff },
#endif
     { { KEY_PLAY, 0, KEY_TYPE_THREE_CLICK, 0 }, com_key_deal_force_unlink_10s },
    /*! �Ͽ����� */
  //  { { KEY_MODE, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, com_key_deal_force_unlink },
    
    { { KEY_PLAY, 0, KEY_TYPE_DBL_CLICK, 0 }, com_key_deal_force_unlink },
    { { KEY_PLAY, 0, KEY_TYPE_LONG_UP, 0}, com_key_deal_siri},

//{ { KEY_MODE, 0, KEY_TYPE_LONG_10S, KEY_DEAL_ITSELF }, com_key_deal_force_unlink_10s },

    /*! �ز����һ���绰��ֻ��������� */
#if (SUPPORT_PHONE_KEY == 1)
    { { KEY_PHONE, 0, KEY_TYPE_DBL_CLICK, 0 }, com_key_deal_call_last },
    { { KEY_PHONEPLAY, 0, KEY_TYPE_DBL_CLICK, 0 }, com_key_deal_call_last },
#else
   //{ { KEY_PLAY, 0, KEY_TYPE_DBL_CLICK, 0 }, com_key_deal_call_last },
#endif

    /*! ������־ */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0 }, NULL },
};

#ifdef OTA_VERSION
const key_event_map_t __section__(".rodata.ke_maplist") com_ke_maplist_null[] =
{
    /*! ������־ */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0 }, NULL },
};
#endif


#ifdef ENABLE_TRUE_WIRELESS_STEREO
void __section__(".text.BANK_UI") com_app_deal_play_prev_next(void)
{
    g_prev_next_ch_start = 0;
    if(prev_next_ch_timer_id != -1)
    {
         kill_app_timer(prev_next_ch_timer_id);
        prev_next_ch_timer_id = -1;
    }
    // libc_print("start 0",0,0);
}
//void __section__(".text.BANK_UI")com_creat_play_prev_next_timer(void)
void __section__(".text.BANK2")com_creat_play_prev_next_timer(void)

{
    g_prev_next_ch_start = 1;
    prev_next_ch_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_COM_CONTROL | (APP_TIMER_TAG_SYS << 8), 200,
              com_app_deal_play_prev_next);
}
#endif

app_result_e com_key_deal_switch_app(void)
{
    uint8 next_func_id;

    next_func_id = com_ap_switch_ask_next_func(RESULT_NEXT_FUNCTION);
    if (next_func_id == get_cur_func_id())
    {
        PRINT_WARNING("NO next APP to switch!!");
        return RESULT_NULL;
    }
    else
    {
        //������������Ӧ�Ķ̰�����
        com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_SHORT_UP);
        return RESULT_NEXT_FUNCTION;
    }
}

app_result_e com_key_deal_switch_mute(void)
{
    if (g_ignore_switch_mute == FALSE)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        com_switch_mute(1,0,NULL);
#else
         com_switch_mute();
#endif
    }

    return RESULT_NULL;
}

app_result_e com_key_deal_volume_add(void)
{

    
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (g_prev_next_ch_start == 0)
#endif
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //com_kill_play_prev_next_timer();
        com_creat_play_prev_next_timer();
#endif
        //libc_print("add##",0,0);
        com_volume_add(FALSE);
    }

    return RESULT_NULL;
}

app_result_e com_key_deal_volume_sub(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (g_prev_next_ch_start == 0)
#endif
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //com_kill_play_prev_next_timer();
        com_creat_play_prev_next_timer();
#endif
        com_volume_sub(FALSE);
    }
    

    return RESULT_NULL;
}

/*app_result_e com_key_deal_enter_clock(void)
{
#if (SUPPORT_LED_DRIVER == 1)
    if (sys_comval->support_led_display == 1)
    {
        tm_alarm_create_clock_view();
    }
#endif

    return RESULT_NULL;
}

app_result_e com_key_deal_switch_dae(void)
{
    if (sys_comval->eq_enable == TRUE)
    {
        com_eq_next();
    }

    return RESULT_NULL;
}*/






app_result_e com_key_deal_shutoff(void)
{
    app_result_e result = RESULT_NULL;

   if((g_customer_state&CUSTOMER_POWERONOFF) != 0)
    {
        return result;
   }
#ifdef ENABLE_TRUE_WIRELESS_STEREO

    //���粻��Ӧ�ػ�����
    if(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status == HFP_STATUS_INCOMING)
    {
        return result;
    }
   if (g_bt_stack_cur_info.sim_a_en == 1)
   {
        libc_print("sim",0,0);
   }
   else
#endif           
    {    
    key_hold_flag++;
    if (POWEROFF_TIME < key_hold_flag)
    {
        key_hold_flag = 0;
        com_filter_key_hold();
#ifdef ENABLE_TRUE_WIRELESS_STEREO
       //if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
       if((g_bt_stack_cur_info.dev_role!=NORMAL_DEV)&&(g_app_info_vector[APP_TYPE_BTSTACK].used != 0))
        {   
             //libc_print("_POWER_",0,0);
             com_btmanager_power_off();        
        }  
#endif
    
        result = RESULT_POWER_OFF;
    }
    }
    
    return result;
}

/*app_result_e com_key_deal_enter_crecord(void)
{
    app_result_e result = RESULT_NULL;

    if (g_this_app_info->app_id != APP_ID_RECORD)
    {
        //���ݲ���,ȷ��¼��Դ
        if ((get_engine_type() == ENGINE_LINEIN) && (get_play_disk_type() == PLAY_DISK_LINEIN))
        {
            result = RESULT_RECORD_LINEIN_TOC;
        }
        else if (get_engine_type() == ENGINE_RADIO)
        {
            result = RESULT_RECORD_FM_TOC;
        }
        else
        {
            result = RESULT_RECORD_MIC_TOC;
        }
    }

    return result;
}

app_result_e com_key_deal_enter_urecord(void)
{
    app_result_e result = RESULT_NULL;

    if (g_this_app_info->app_id != APP_ID_RECORD)
    {
        //���ݲ���,ȷ��¼��Դ
        if ((get_engine_type() == ENGINE_LINEIN) && (get_play_disk_type() == PLAY_DISK_LINEIN))
        {
            result = RESULT_RECORD_LINEIN_TOU;
        }
        else if (get_engine_type() == ENGINE_RADIO)
        {
            result = RESULT_RECORD_FM_TOU;
        }
        else
        {
            result = RESULT_RECORD_MIC_TOU;
        }
    }

    return result;
}*/

/*app_result_e com_key_deal_enter_record_cplay(void)
{
    return RESULT_RECORD_CPLAY;
}

app_result_e com_key_deal_enter_record_uplay(void)
{
    return RESULT_RECORD_UPLAY;
}*/

app_result_e com_key_deal_switch_tts_language(void)
{
  
    if((g_bt_stack_cur_info.dev_role==0)&&(g_bt_stack_cur_info.conn_status==CONN_STATUS_WAIT_PAIR))
	 {
		   if (sys_comval->tts_language < (TTS_LANGUAGE_TOTAL - 1))
		   {
			   sys_comval->tts_language++;
		   }
		   else
		   {
			   sys_comval->tts_language = 0;
		   }
		   com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_LANGUAGE);
		   
	 }
	 
    return RESULT_NULL;
}



#if 0
/*demo*/
app_result_e com_key_cancel_search(void)
{

    if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
    {        
        msg_apps_t msg;

        msg.type = MSG_BTSTACK_TWS_CANCEL_SEARCHING;
        //����ͬ����Ϣ
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);

    }
    return RESULT_NULL;
}

app_result_e com_tws_disconnect(void)
{
    uint8 id;
    //disconnect 
    for(id=0;id<RMT_DEV_NUM;id++)
    {
        //connct to phone
        if(g_bt_stack_cur_info.rmt_dev[id].dev_type == TWS_DEV)
        {
        	//disconnect
        	msg_apps_t msg;
            msg.content.data[0] = id;
            //��Ϣ����(����Ϣ����)
            msg.type = MSG_BTSTACK_FORCE_DISCONN; 
            //����ͬ����Ϣ
            send_sync_msg_btmanager(NULL, &msg, NULL, 0); 
            break;
        }
    }
    return RESULT_NULL;
}
#endif

app_result_e com_key_deal_force_unlink(void)
{

    if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
    {   
		
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //����mode��ͬʱ����Ժͽ���԰���ʱ��Ĳ�������
        if((g_bt_stack_cur_info.dev_role==0)&&(g_bt_stack_cur_info.conn_status==CONN_STATUS_WAIT_PAIR))
        	
        {
             com_btmanager_force_unlink(1);
            //���⵼������Գɹ��Ĳ���
            g_neednot_tts_play_timeout1=0;
            com_tts_state_play(TTS_MODE_ONLYONE,(void *) TTS_WAIT_BT_PAIR);
        }
        else
        {
             com_start_key_tone(KEYTONE_NOBLOCK); 
             com_btmanager_hfp_call_last();
        }
        //com_tts_state_play(TTS_MODE_ONLYONE,(void *) TTS_WAIT_BT_PAIR);
#endif
    }
    return RESULT_NULL;
}


app_result_e com_key_deal_force_unlink_10s(void)
{

    if((g_bt_stack_cur_info.dev_role==0)&&(g_bt_stack_cur_info.conn_status==CONN_STATUS_WAIT_PAIR))
   	{
		 #ifdef ENABLE_TRUE_WIRELESS_STEREO    
    	if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
   	    {
    		com_btmanager_clearpairlist();
    	}	
		#endif 
    	keytone_play(KEYTONE_KEYTONE, KEYTONE_NOBLOCK);
    	set_led_state(LED_CLEARPAIRED);
		set_led_state(LED_CONNECTING);
		
   	}
     return RESULT_NULL;
}

#if 1
app_result_e com_key_deal_call_last(void)
{
    if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
    {
        com_btmanager_hfp_call_last();
    }

    return RESULT_NULL;
}

app_result_e com_key_deal_BQB_test(void)
{
    if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
    {
        com_btmanager_BQB_test();
    }

    return RESULT_NULL;
}

app_result_e com_key_deal_FT_test(void)
{
    testval_t test_flag;
    test_flag.magic = 0x55AA;
    test_flag.api_flag = 0x01;
    sys_vm_write(&test_flag, VM_API_FLAG, sizeof(testval_t));
    sys_os_time_dly(300);   
    act_writel((0 << WD_CTL_CLKSEL_SHIFT) | (1 << WD_CTL_WDEN) | (1 << WD_CTL_CLR), WD_CTL);//352ms
    while(1)
    {
        ;//wait for reset
    }
}
#endif


app_result_e com_key_deal_siri(void)
{
    key_hold_flag = 0;
#if (__SUPPORT_SIRI_ == 1)    
    if (g_btmanager_gl_var.enable_siri)
    {
        //PRINT_INFO("com_key_deal_siri ");
        if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
        {
            com_btmanager_hfp_siri_handle();
        }
    }
#endif //#if (__SUPPORT_SIRI_ == 1)
    return RESULT_NULL;
}

app_result_e com_hid_send_key(void)
{
#ifdef __SUPPORT_HID_        
    msg_apps_t msg;
    app_result_e result = RESULT_NULL;
    //PRINT_INFO("com_hid_send_key\n");
    if (com_get_config_default(BTMANAGER_ENABLE_HID) !=0)
    {
        if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & HID_CONNECTED) == 0)
        {
            //PRINT_INFO("MSG_BTSTACK_HID_CONNECT_SYNC\n");
            //��Ϣ����(����Ϣ����)
            msg.type = MSG_BTSTACK_HID_CONNECT_SYNC;
        }
        else
        {
            //PRINT_INFO("MSG_BTSTACK_HID_SEND_SYNC\n");
            msg.type = MSG_BTSTACK_HID_SEND_SYNC;
            result = RESULT_HID_SEND_KEY_EVENT;
        }
        //����ͬ����Ϣ
        // send_sync_msg(APP_ID_BTSTACK, &msg, NULL, 0);
        send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].addr.bytes, &msg, NULL, 0);

    }
#endif //#ifdef __SUPPORT_HID_        
    return RESULT_NULL;
}

app_result_e com_hid_disconnect(void)
{
#ifdef __SUPPORT_HID_        
    if (com_get_config_default(BTMANAGER_ENABLE_HID)!=0)
    {
        msg_apps_t msg;
        //PRINT_INFO("com_hid_disconnect\n");
        
        // PRINT_INFO("MSG_BTSTACK_HID_DISCONNECT_SYNC\n");
        //��Ϣ����(����Ϣ����)
        msg.type = MSG_BTSTACK_HID_DISCONNECT_SYNC;
        //����ͬ����Ϣ
        // send_sync_msg(APP_ID_BTSTACK, &msg, NULL, 0);
        send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].addr.bytes, &msg, NULL, 0);

    }
#endif //#ifdef __SUPPORT_HID_            
    return RESULT_NULL;
}

/*app_result_e com_key_deal_enter_ota(void)
{
    otaval_t g_otaval;
    uint8 ota_mode;
    //if (com_get_config_default(SETTING_APP_SUPPORT_OTA)!=0)
    sys_get_fw_info(&ota_mode, FW_INFO_OTA_MODE_ADDR, FW_INFO_OTA_MODE_LEN);
    if(ota_mode!=0)
    {
        PRINT_INFO("com_key_deal_enter_ota\n");

        g_otaval.magic = VRAM_MAGIC(VM_OTA_UPGRADE);
        g_otaval.ota_upgrade_flag = OTA_UPGRADE_ENABLE;
        
        sys_vm_write(&g_otaval, VM_OTA_UPGRADE, sizeof(otaval_t));

        sys_local_irq_save();
        ENABLE_WATCH_DOG(1);
        while(1)
        {
            ;//wait for reset
        }
    }
    return RESULT_NULL;
}*/

app_result_e com_key_deal_force_ble_unlink(void)
{
	  
	  if(g_bt_stack_cur_info.ble_con_flag !=0)
	  {
	      com_btmanager_unlink_spp_ble_profile(1);
	  }	  
	  return RESULT_NULL;
}


/*! \endcond */
