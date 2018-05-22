/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * ÃèÊö£ºCASE ÏµÍ³2HzÈí¶¨Ê±Æ÷HANDLE£¬¼à²âÏîÄ¿°üÀ¨£º¿¨ÈÈ°Î²å£¬AUXÈÈ°Î²å£¬Ê¡µç¹Ø»ú£¬
 *       LEDÊıÂë¹Ü¹ØÆÁ£¬³äµçºÍµçÁ¿£¬APK/APP×´Ì¬¡£
 * ×÷Õß£ºcailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */

static sys_status_e get_cur_sys_status(void);

void __section__(".bank") enter_lower_mode(void)
{
    msg_apps_t msg;

    //µ÷ÊÔÄ£Ê½²»ÏìÓ¦½øÈëS2,S3×´Ì¬
    if (g_app_info_state.stub_tools_type == 0)
    {
        //¹ã²¥½øÈëµÍ¹¦ºÄÏûÏ¢
        if (sys_comval->lowpower_mode == LOW_POWER_MODE_S2)
        {
            msg.type = MSG_SYSTEM_ENTER_S2;
        }
        else
        {
            msg.type = MSG_SYSTEM_ENTER_S3;
            act_writel(act_readl(WAKE_PD), WAKE_PD); //Çåpending
            sys_mdelay(5);
        }
        broadcast_msg(&msg, TRUE);    
    }
}
/******************************************************************************/
/*!
 * \par  Description:
 *    ÏµÍ³¼ÆÊ±Èí¶¨Ê±Æ÷·şÎñÀı³Ì£¬´¦ÀíÆÁÄ»±³¹â£¬ÆÁ±££¬Ê¡µç¹Ø»ú£¬Ë¯Ãß¹Ø»ú£¬»Øµ½ÕıÔÚ
 *    ²¥·ÅµÈ¡£
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 * \li  ÏµÍ³¶¨Ê±¹¦ÄÜ£¬°üÀ¨±³¹â£¬ÆÁ±££¬Ê¡µç¹Ø»ú£¬Ë¯Ãß¹Ø»ú£¬appĞİÃß£¬ÓÉÇ°Ì¨AP½øĞĞ¼ÆÊ±¡£
 *      ¼´ÔÚÇ°Ì¨AP´´½¨Ò»¸ö¶¨Ê±Æ÷£¬×¨ÓÃÓÚÉÏÃæÎå¸ö¹¦ÄÜ¼ÆÊ±Æ÷µÄ¼ÆÊ±£¬ÔÚ get_app_msg ÖĞ
 *      µ÷ÓÃ handle_timers È¥×ö¼ì²â£¬Ò»µ©¶¨Ê±ÒÑµ½£¬´¥·¢¶ÔÓ¦¶¨Ê±¹¦ÄÜ¡£
 * \li  ÆÁÄ»±³¹âÍêÈ«ÓÉ common Ä£¿é¿ØÖÆ£¬ÎŞĞëÓÃ»§¹ØĞÄ£¬ÓÃ»§¿ÉÒÔÍ¨¹ı get_backlight_state
 *      À´»ñÈ¡±³¹â×´Ì¬¡£
 * \li  ÆÁÄ»±£»¤Í¬ÑùÍêÈ«ÓÉ common Ä£¿é¿ØÖÆ£¬ÎŞĞëÓÃ»§¹ØĞÄ¡£µÈĞèÒª½øÈëÆÁÄ»±£»¤Ê±£¬ÏÈ·¢ËÍ
 *      Òì²½ÏûÏ¢ MSG_ENTER_SCREEN_SAVE ¸øµ±Ç°AP£¬ÔÚ com_message_box ÖĞ½øÈëÆÁ±£¡£
 * \li  Ê¡µç¹Ø»úºÍË¯Ãß£¨¶¨Ê±£©¹Ø»úÔòÊÇ¹ã²¥ MSG_POWER_OFF ÏûÏ¢£¬manager ap ÔÚÊÕµ½¸ÃÏûÏ¢
 *      ºó£¬·¢ËÍ MSG_APP_QUIT ÈÃÇ°Ì¨ºÍºóÌ¨AP×Ô¼ºÍË³ö¡£
 * \li  ·µ»ØÕıÔÚ²¥·Å½çÃæÍ¨¹ıÒÔ"·µ»ØÕıÔÚ²¥·ÅµÄÄ£Ê½"´´½¨¶ÔÓ¦µÄÇ°Ì¨AP»òÕß·¢ËÍÒì²½ÏûÏ¢ back_to_playing
 *      ÈÃÇ°Ì¨AP×Ô¼º»Øµ½ÕıÔÚ²¥·Å³¡¾°¡£·µ»ØÕıÔÚ²¥·Å½çÃæĞèÒªAPµÄÕıÔÚ²¥·Å³¡¾°Ğ­Öú£¬¼´ÔÚ½øÈë³¡¾°Ê±
 *      change_app_state(APP_STATE_PLAYING)£¬ÔÚÍË³ö³¡¾°Ê± change_app_state(APP_STATE_NO_PLAY)¡£
 *******************************************************************************/
void sys_counter_handle(void)
{

    bool usb_cable_adapter = FALSE;
    sys_status_e cur_sys_status;
    bool enter_lp_flag = FALSE;
    uint16 cur_poff_timer; //E!µç1O»úE±1ä(0.5sµYÎ»)
    msg_apps_t msg;

    if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0) //USBµçAÂ»o3äµçEEAäIß2aEë
    {
        usb_cable_adapter = TRUE;
    }

#if (SUPPORT_OUTER_CHARGE == 1)
    if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_CAHREGE_IN() == 1)
        {
            usb_cable_adapter = TRUE;
        }
    }
#endif

    //µç3O1°Aä3äµç1i2â´¦Aí
    com_battery_charge_deal();

    if ((g_app_last_state.last_ui_status != UI_STATUS_IDLE)
            && (g_app_info_state_all.ui_status == UI_STATUS_IDLE))
    {
        g_sys_counter.ui_idle_counter = 0;
    }

    /* 1O±O±31â1°Ç°I¨1oEë1¦oÄÓA»—ÄLE1 */
    if (usb_cable_adapter == FALSE)
    {
        if ((sys_comval->light_timer > 0) && (get_backlight_state() == BACKLIGHT_STATE_NORMAL))
        {
            g_sys_counter.light_counter++;
            if (g_sys_counter.light_counter >= (2u * sys_comval->light_timer)) //light_timer OÔ1SÎaµYÎ»
            {
                com_set_display_onoff(FALSE);
            }
        }

        /* ?OI?O»¶ÎE±1ä3Í1µµÍAµÂE */
        if (g_app_info_state_all.ui_status == UI_STATUS_IDLE)
        {
            g_sys_counter.ui_idle_counter++;
            if (g_sys_counter.ui_idle_counter >= UI_IDLE_TIMER)
            {
                adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
                g_app_info_state_all.ui_status = UI_STATUS_IDLE_LOWFREQ;
            }
        }
    }

    /* »nE!µ±Ç°IµÍ31¤×÷×´I¬ */
    cur_sys_status = get_cur_sys_status();

    /* ?aAôE!µç1O»ú1AE± */
    if (((g_app_last_state.last_sys_status != SYS_STATUS_IDLE) && (cur_sys_status == SYS_STATUS_IDLE))
            || ((g_app_last_state.last_sys_status != SYS_STATUS_WAIT_LINK) && (cur_sys_status == SYS_STATUS_WAIT_LINK)))
    {
        g_sys_counter.poweroff_counter = 0;
    }

    /* E!µç1O»ú1AE±A÷L»ÔÚUSBµçAÂA»Ó?2aEëE±Ó??§ */
    if ((usb_cable_adapter == FALSE) && (cur_sys_status != SYS_STATUS_WORKING))
    {
       if ((g_bt_stack_cur_info.tws_master_phonecon == 1)
           || (g_bt_stack_cur_info.phonetomastercon == 1)
           ||((g_bt_stack_cur_info.dev_role == NORMAL_DEV)
                   && (g_bt_stack_cur_info.num_connected == 1)))  //???????????,??S3  UP-20170615
                    	
       	{
            if (sys_comval->lowpower_timer == 0)
            {
                enter_lp_flag = TRUE;
            }
            else
            {
                cur_poff_timer = 120u * sys_comval->lowpower_timer;
				libc_print("333",g_sys_counter.poweroff_counter,2);

                g_sys_counter.poweroff_counter++;
                if (g_sys_counter.poweroff_counter >= cur_poff_timer)
                {
                    enter_lp_flag = TRUE;
                }
            }

            if (enter_lp_flag == TRUE)
            {
/*#ifdef ENABLE_TRUE_WIRELESS_STEREO
                if(g_bt_stack_cur_info.dev_role==NORMAL_DEV)
                {
                    enter_lower_mode();
                }
#else*/
                enter_lower_mode();                
//#endif      

            }
        }
        else          //?????,???? up-20170615
        {
            //poweroff_timer µEÓÚ02»E!µç1O»ú
              if (cur_sys_status == SYS_STATUS_WAIT_LINK)
              {
                    //A¶NAE!µçE±1äL¬O»°a3¤ÓÚE!µç1O»úE±1äL¬Ä¬EIÎa5·ÖÖÓ
                    cur_poff_timer = 120u * sys_comval->poweroff_bt_waitpair_timer;
                }
                else
                {
                    cur_poff_timer = 120u * sys_comval->poweroff_timer;
                }

                g_sys_counter.poweroff_counter++;
			//  libc_print("222",g_sys_counter.poweroff_counter,2);
                if (g_sys_counter.poweroff_counter >= cur_poff_timer)
                {
                  if((g_bt_stack_cur_info.dev_role!=NORMAL_DEV)&&(g_app_info_vector[APP_TYPE_BTSTACK].used != 0))
        		  {   
             		  com_btmanager_power_off();        
        		  }  
                    //1a2Y1O»úIuIc
                    msg.type = MSG_POWER_OFF;
                    broadcast_msg(&msg, TRUE);
                }
            }
        }
  

    /* 3AëÖÓA»»nE!µ1APKAüÁîL¬±íE3APK1oEëoóI¨ÄLE1 */
    g_sys_counter.apk_counter++;
    if (g_sys_counter.apk_counter >= APK_BACKGROND_OVERTIME)
    {
        set_apk_status(APK_BACKGROUND);
    }

    /* ¸üĞÂ×î½ü×´Ì¬ */
    g_app_last_state.last_sys_status = cur_sys_status;
    g_app_last_state.last_ui_status = g_app_info_state_all.ui_status;
}

static sys_status_e get_cur_sys_status(void)
{
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
    {
        if (g_btmanager_gl_var.btmanager_status != BTMANAGER_STATUS_IDLE)
        {
            return SYS_STATUS_WORKING;
        }
    }

    if ((g_app_info_state_all.engine_state == ENGINE_STATE_PLAYING)
            || (g_app_info_state_all.app_state != APP_STATE_NO_PLAY)
            || (g_app_info_state_all.ui_status != UI_STATUS_IDLE_LOWFREQ))
    {
        return SYS_STATUS_WORKING;
    }

    //ºóÌ¨ÒıÇæÓëÇ°Ì¨Ó¦ÓÃ±£³Ö¿ÕÏĞ×´Ì¬£»À¶ÑÀÃ»ÓĞÍ¨ĞÅ£»Ç°Ì¨Ã»ÓĞ²Ù×÷³ÖĞø7ÃëÖÓÒÔÉÏ
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
    {
        if ((g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                && ((get_cur_func_id() == APP_FUNC_BTPLAY) || (get_cur_func_id() == APP_FUNC_BTCALL)))
        {
            return SYS_STATUS_WAIT_LINK;
        }
    }

    return SYS_STATUS_IDLE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    Íâ²¿Éè±¸¼ì²â·şÎñº¯Êı,200msÖ´ĞĞÒ»´Î
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
void peripheral_detect_handle(void)
{
    uint32 detect_mode = 0;
    
#if (SUPPORT_CARD_DETECT != 0)
    if (sys_comval->hard_support_card == 1)
    {
        detect_mode |= PER_DETECT_CARD;
        if (get_card_state() == CARD_STATE_CARD_NULL)
        {
            detect_mode |= PRE_DETECT_CARD_FOR_IN;
        }
        if (g_detect_card_force_flag == 1)
        {
            g_detect_card_force_flag = 0;
            detect_mode |= PER_DETECT_CARD_FORCE;
        }
    }
#endif

    if (sys_comval->support_linein == 1)
    {
        detect_mode |= PER_DETECT_AUX;
        if (get_linein_state() == LINEIN_STATE_LINEIN_NULL)
        {
            detect_mode |= PER_DETECT_AUX_FOR_IN;
        }
    }

#if (SUPPORT_HP_DETECT != 0)
    if (sys_comval->hard_support_headphone == 1)
    {
        detect_mode |= PER_DETECT_HP;
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            detect_mode |= PER_DETECT_HP_FOR_IN;
        }
    }
#endif

#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    //Ö»ÓĞÖ§³ÖUÅÌ²¥·Å¹¦ÄÜ²Å¶¯Ì¬¼ì²âusbºÍuÅÌ²åÈë£»·ñÔòÖ»¼ì²âusb²åÈë
    if ((sys_comval->hard_support_uhost == 1) && (sys_comval->support_uhostplay == 1))
    {
        detect_mode |= PER_DETECT_USBUHOST_SWITCH;
    }
#endif

    if (detect_mode != 0)
    {
        key_peripheral_detect_handle(detect_mode);
    }
}

//LEDÊıÂë¹Ü¹ØÆÁ¼°»Ö¸´
void __section__(".bank") com_set_display_onoff(bool onoff)
{
    if (onoff == TRUE)
    {
        led_wake_up_display();
        g_app_info_state.backlight_state = BACKLIGHT_STATE_NORMAL;
    }
    else
    {
        led_sleep_display();
        g_app_info_state.backlight_state = BACKLIGHT_STATE_OFF;
    }
}

/*! \endcond */
