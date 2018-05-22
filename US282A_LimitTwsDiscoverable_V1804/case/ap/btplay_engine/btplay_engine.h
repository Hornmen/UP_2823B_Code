/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * �����������Ƹ�����Ӧ����ڣ�����Ӧ�ó�ʼ����Ӧ���˳��ȡ�
 * ���ߣ�cailizhen
 ********************************************************************************/

#ifndef _BTPLAY_ENGINE_H
#define _BTPLAY_ENGINE_H

#include "psp_includes.h"
#include "case_include.h"
#include "btplay_common.h"

// �Զ������������ͨ��GPIO ���
//#define ENABLE_OUTPUT_CH_DET_BY_IO

typedef struct
{
    uint8 reserve;
} btplay_config_t;

typedef struct
{
    bool codec_flag;
    bool tts_play_flag;
    uint8 media_type;
    uint8 aac_delay_start;
    uint8 aac_delay_lowest;
    uint8 sbc_delay_start;
    uint8 sbc_delay_lowest;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    uint8 tws_m_output_channel;
    uint8 tws_s_output_channel;
#endif
    uint16 filter_after_pause; //�û���С������ͣ�����һС��ʱ��AVTDP��Ƶ���ݰ�
} btplay_global_variable_struct;

typedef struct
{
    uint8 pc_tools:2;    //0x00:��֧��WAVES��Ч; 0x01:WAVES��Ч����ģʽ; 0x02:WAVES��Ч�̼�ģʽ
    uint8 w_input:1;     //0x00:WAVES��Ч�̼�ģʽ�²�֧��input����;0x01:WAVES��Ч�̼�ģʽ��֧��input����;
    uint8 reserve:5;        
} support_waves_bt_player_t;

extern app_result_e btplay_deal_app_msg(void);
extern app_result_e get_message_loop(void);

extern uint32 read_sbc_data(uint8 *middle_buffer, uint32 need_data_length, uint32 return_status);
extern void a2dp_decode_init(uint8 media_type);
extern void a2dp_decode_quit(void);

extern btplay_global_variable_struct btplay_gl_var;
extern btplay_config_t g_btplay_cfg;

extern uint16 pause_cnt;
extern uint16 play_cnt;
extern uint8 change_flag;

extern void *mp_handle;

extern bt_stack_pipe_t *g_p_play_pipe;
extern bt_stack_info_t *g_p_bt_stack_cur_info;
extern bt_stack_twsinfo_t *g_p_tws_info;

extern btplay_info_t g_btplay_info[2];
extern btplay_info_t *g_p_btplay_info;

#ifdef WAVES_ASET_TOOLS
extern support_waves_bt_player_t g_support_waves_bt_player;
#endif

//for dae adjust freq
extern dae_config_t *g_dae_cfg_shm;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern void send_mmm_exit_cmd(void);
extern void send_mmm_exit_cmd2(uint8 op);
extern void send_tts_stop_cmd(void);
extern void send_tts_start_cmd(void);
extern void Tws_clear_filter(void);


//for tws or no tws
extern uint8 tws_or_notws_flag;
extern uint8 send_once_cmd;
extern uint8 second_empty_flag;
extern uint8 second_empty_flag2;
extern int8 clear_filter_timer_id;
#endif
extern int8 tws_adjfreq_timer_id;

extern app_result_e app_message_deal(private_msg_t *pri_msg) __FAR__;
extern void btplay_engine_pipe_control(uint8 op) __FAR__;

#endif/* _BTPLAY_ENGINE_H */

