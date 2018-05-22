/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * �����������Ƹ�ǰ̨Ӧ��TTS��ؽӿ�ʵ�֡�
 * ���ߣ�cailizhen
 ********************************************************************************/

#include  "ap_btplay.h"
void btplay_tts_play(void)
{
#if (LINK_TTS_SEL == 0)
    if (g_standby_exit_flag == FALSE)
    {
        if (g_connected_flag == 0)
        {
            //���������ȴ�����
            if (g_last_connected_flag == 0)
            {
                if (g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                {
                    if (g_neednot_tts_play_reset == 1)
                    {
                        g_neednot_tts_play_reset = 0;
                    }
                    else
                    {
                        //PRINT_INFO("T1 ");
                        set_led_state(LED_CONNECTING);
                       com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
                    }
                }
            }
            else
            {
                //�������������Ͽ�&�ȴ�����
                if (g_neednot_tts_play_timeout == 1)
                {
                    //  libc_print("d4",0,0);
                    g_neednot_tts_play_timeout = 0;
                }
                else
                {
                    PRINT_INFO("dis1 ");
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                    flag_con_ok_one=0;
#endif
                    if((g_customer_state&CUSTOMER_SLAVEDIS) == 0)
					{
                    	com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_DISCON);
                    	com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
                    	set_led_state(LED_CONNECTING);
					}
                }
            }
        }
        else
        {
            if (g_last_connected_flag == 0)
            {
                //�������ӳɹ�
              //  PRINT_INFO("CON_OK1 ");
 #ifdef ENABLE_TRUE_WIRELESS_STEREO
                flag_con_ok_one=1;
 #endif
                com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_CON_OK);
                
                set_led_state(LED_CONNECTED);
            }
        }
    }
#else
    switch (g_btplay_cur_info.status)
    {
        case BTPLAY_IDLE:
        if (g_standby_exit_flag == FALSE)
        {
            if (g_last_btplay_status == BTPLAY_IDLE)
            {
                //���������ȴ�����
                if (g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                {
                    if (g_neednot_tts_play_reset == 1)
                    {
                        g_neednot_tts_play_reset = 0;
                    }
                    else
                    {
                       // PRINT_INFO("up pairing");
                        com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
                        set_led_state(LED_CONNECTING);
                    }
                }
            }
            else
            {
                //�������������Ͽ�&�ȴ�����
                if (g_neednot_tts_play_timeout == 1)
                {
                    g_neednot_tts_play_timeout = 0;
                }
                else
                {
                    com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_DISCON);
                    com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
                    set_led_state(LED_CONNECTING);
				
                }
            }
        }
        break;

        default:
        if (g_last_btplay_status == BTPLAY_IDLE)
        {
            //�������ӳɹ�
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_CON_OK);
            set_led_state(LED_CONNECTED);
        }
        break;
    }
#endif
}

#ifdef ENABLE_TRUE_WIRELESS_STEREO
//���ӵ����ɸ��������øøú������𲥱�tts
void btplay_tws_tts_play(void)
{
    //tws��绰��ʱ�������ϲ�������Գɹ�
    if((g_bt_stack_cur_info.pair==1)&&(flag2==1))
    {
       //  PRINT_INFO("dis2 ");
        PRINT_INFO_INT("play_timeout1#:", g_neednot_tts_play_timeout1);
        if(g_neednot_tts_play_timeout1==1)
        {
            g_neednot_tts_play_timeout1=0;
        }
        else
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAIR_SUCCEED); 
            if (g_app_info_state_all.g_hp_led_state != LED_CONNECTED)
            {
                set_led_state(LED_PAIRED);
            }
			if (g_bt_stack_cur_info.tws_master_phonecon == 0)
            {
               com_tts_state_play((uint16)TTS_MODE_ONLYONE,(void*) TTS_WAIT_BT_CON);
			   set_led_state(LED_CONNECTING);
            }
			else
			{
			     set_led_state(LED_CONNECTED);
			}
             
        }
        flag2=0;
                   
    }
    else if((tws_discon_flag==1)||(g_bt_stack_cur_info.tws_dev_discon==1))          //else if((g_bt_stack_cur_info.phonetomasterdis==1)||(g_bt_stack_cur_info.tws_dev_discon==1))
    {
        //�������������Ͽ�&�ȴ�����
         if(g_bt_stack_cur_info.dev_role != NORMAL_DEV)
        {
          //  PRINT_INFO("UP DISCONNECT");
        	com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_DISCON);
        	com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
        	set_led_state(LED_CONNECTING);
        }
		else if((g_bt_stack_cur_info.dev_role == NORMAL_DEV)
			&&(g_bt_stack_cur_info.num_connected == 1))
		{
		   
		     com_tts_state_play(TTS_MODE_ONLYONE, (void*) TTS_LAST_SONG);
		   
		}
		else if((g_bt_stack_cur_info.dev_role == NORMAL_DEV)
			&&(g_bt_stack_cur_info.num_connected == 0))
		{
		   
		     com_tts_state_play(TTS_MODE_ONLYONE, (void*) TTS_LAST_SONG);
			 com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_WAIT_BT_CON);
			 set_led_state(LED_CONNECTING);
		   
		}
        flag_con_ok_one=0;
        PRINT_INFO("dis2 ");
    }
    else
    {
        //do nothing
    }
}

void btplay_tws_tts_play1(void)
{
      if(tws_con_flag==1)
     {
        if(flag_con_ok_one==1)
        {
            flag_con_ok_one=0;
         }
        else
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_BT_CON_OK);
            set_led_state(LED_CONNECTED);
         }
                  
          PRINT_INFO("CON_OK2 ");
      }
}

#endif

