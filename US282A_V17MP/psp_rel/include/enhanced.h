/*******************************************************************************
 *                              US212A
 *                            Module: ENHANCED
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       liminxian  2011-9-15 15:37    1.0             build this file
 *******************************************************************************/

#ifndef  _ENHACED_H
#define  _ENHACED_H

#include <typeext.h>
#include <api.h>
#include <driver_manager.h>
#include <kernel_interface.h>
#include <vfs_interface.h>
#include <libc_interface.h>

//ɨ����Ƶ�ļ�ʱ�����˵�¼��Ŀ¼(���磺\RECORD)
#define FILTER_RECORD_DIR

#define ID3_SUPPORT

#define  ID3_ITEM_SIZE    28    //ID3��Ϣ����
#define  DIRLAYER_MAX     9     //Ŀ¼���,root + 8����Ŀ¼
#define  EH_SECTOR_SIZE   512   //��д������С
#define  USERPL_TITLE_MAX  (ID3_ITEM_SIZE * 2)  //�ղؼ���ʾ����

#ifdef ID3_SUPPORT

//id3�ļ��� buffer����
#define Id3BufSIZE 92

//id3ר��/����buffer����
#define ArtBufSIZE 52

//ע�⣺����ڴ汻������ʹ�á�
//���ԣ�enhanced��ʹ����ʱ��һ��Ҫ��֤��������STOP
#define PLIST_INFO_ADDR  0x9fc2e000

//ע�⣺����ڴ汻������ʹ�á�
//���ԣ�enhanced��ʹ����ʱ��һ��Ҫ��֤��������STOP
#define PLIST_INDEX_ADDR 0x9fc2f000
//end
//ID3��ȡ֧������
typedef enum
{
    ID3_TYPE_AA = 0,
    ID3_TYPE_AAC,
    ID3_TYPE_AAX,
    ID3_TYPE_APE,
    ID3_TYPE_FLAC,
    ID3_TYPE_MP3,
    ID3_TYPE_OGG,
    ID3_TYPE_WMA,
    ID3_TYPE_END
} id3_type_e;

//��ȡID3����Ϣ�ṹ
typedef struct
{
    uint8 *tit2_buffer;  //�����ļ�����buffer
    uint8 *tpe1_buffer;  //��������buffer
    uint8 *talb_buffer;  //����ר��buffer
    uint8 *genre_buffer; //��������buffer
    uint8 *drm_buffer;   //����drm��buffer
    uint8 *track_buffer; //��������buffer
    uint8 tit2_length;   //�ļ����ⳤ��
    uint8 tpe1_length;   //���߳���
    uint8 talb_length;   //ר������
    uint8 genre_length;  //���ɳ���
    uint8 drm_length;    //drm����
    uint8 track_length;  //���쳤��
    uint8 apic_type;     // 0-��ͼƬ��1-jpeg
    uint8 apic_flag;     //��ȡͼƬƫ�Ʊ��
    uint8 ldb_flag;      //��ȡldb���ƫ�Ʊ��
    uint8 reserve;       //��������
    uint16 track_num;    //�����
    uint32 ldb_offset;   //ldb������ļ��е��ֽ�ƫ��
    uint32 apic_offset;  //apicͼƬ���ļ��е��ֽ�ƫ��
} id3_info_t;

/**********************************
 (6)id3��Ϣ��ȡ
 Ŀ¼��\id3
 **********************************/
extern bool fsel_get_id3_info(id3_info_t *id3_info,const char *filename,id3_type_e file_type) __FAR__;

#endif //ID3_SUPPORT

//�ļ�ѡ��������
typedef enum
{
    FSEL_MODULE_DIR = 0, 
    FSEL_MODULE_PLIST, 
} fsel_module_e;

//�ļ�ѡ����ѡ������
typedef enum
{
    //������˳�򲥷�
    FSEL_TYPE_COMMONDIR = 0,
    //����Ŀ����˳�򲥷�
    FSEL_TYPE_LISTDIR,
    FSEL_TYPE_SDFILE,        //sd���ļ� 
    FSEL_TYPE_LINEIN,        //LINEIN 
    FSEL_TYPE_MAX
} fsel_type_e;

//����ģʽ
//��ʱ����g_shuffle_flag���һ���ֽڣ�
//����Ҫ������Ϻ󣬻��ܲ���֪�������ͱ�����ֵ�Ƕ��ٵ����⡣
//���Ա����ͱ�����ֵ�Ƕ��٣����н����ģ��������������ӡ�
typedef enum
{
    FSEL_MODE_LOOPALL = 0,      //ѭ������
    FSEL_MODE_LOOPONE,      //ѭ���ŵ�ǰ��
    FSEL_MODE_LOOPONEDIR,   //ѭ���ŵ�ǰĿ¼
    FSEL_MODE_RANOM,        //�������(shuffle + loop all)   
    FSEL_MODE_NORMAL,       //��ͨ����
    FSEL_MODE_DIRNOR,       //˳�򲥷�Ŀ¼
    FSEL_MODE_INTRO,        //�������
} fsel_mode_e;

//�ӿڵ��Դ�������
typedef enum
{
    FSEL_NO_ERR = 0,        //û�б���
    FSEL_ERR_OUTOF,         //ѡ�����
    FSEL_ERR_EMPTY,         //Ŀ¼�ձ���
    FSEL_ERR_SETDIR,        //����Ŀ¼·������
    FSEL_ERR_SETFILE,       //�����ļ���Ϣ����
    FSEL_ERR_EXCEPTION,     //�쳣����
    FSEL_ERR_FAIL,          //�ӿڲ���ʧ��
    FSEL_ERR_DISK,          //�̷�����
    FSEL_ERR_EXIST,         //�ļ��Ѵ���
    FSEL_ERR_NO_EXIST,      //�ļ�������
    FSEL_ERR_USING,         //�ļ���ʹ����
    FSEL_ERR_STG,           //��װ����ʧ��
    FSEL_ERR_FS,            //�ļ�ϵͳ�ӿڱ���
    FSEL_ERR_LIST_NOEXIST,  //�ղؼл򲥷��б�������
    FSEL_ERR_SHUFFLE,       //shuffle���Ƴ���
    FSEL_ERR_INPUT          //�����������
} fsel_error_e;


//ENHANCED ��ʼ������
typedef struct
{
    uint8 fsel_type;            //ѡ������      fsel_type_e
    uint8 fsel_mode;            //����ģʽ      fsel_mode_e   
    uint8 disk;                 //DISK_C;DISK_H;DISK_U
    uint32 file_type_bit;       //�ļ���׺��bitmap
} fsel_param_t;

typedef struct
{
    uint8  file_ext[4];         //�ļ��ĺ�׺����ȫ��0�����ļ���ϢΪ��
    uint32 cluster_no;          //�ļ���Ŀ¼�����ڵĴغ�
    uint32 dir_entry;           //�ļ���Ŀ¼�������ڴغ��ڵ�ƫ��
} file_extend_info_t;

typedef struct
{
    uint8  file_name[12];       //�ļ���
} file_name_info_t;

typedef union
{
  file_extend_info_t  file_extend_info;  
  file_name_info_t    file_name_info; 
} file_info_u;

//Ŀ¼�µ��ļ���Ϣ
typedef struct
{
    uint8 disk;                 //DISK_C;DISK_H;DISK_U
    uint8 dir_layer;            //��ǰĿ¼��Σ���0��ʼ��0��ʾ��Ŀ¼
    uint16 resever;             //��������
    file_info_u file_info;      //�ļ���Ϣ
    pdir_layer_t dir_layer_info;//Ŀ¼�����Ϣ
    uint8 res_3byte[3];         //��������
    uint16 file_total;          //��ǰĿ¼�ļ�����
    uint16 file_num;            //��ǰ�ļ���Ŀ¼�е����
} file_location_t;

//�����б��µ��ļ���Ϣ
typedef struct
{
    uint8 disk;                 //DISK_C;DISK_H;DISK_U
    uint8 list_layer;           //�б�����
    uint16 list_type;           //�ӱ����� plist_type_e
    file_info_u file_info;      //�ļ���Ϣ
    pdir_layer_t dir_layer_info;//Ŀ¼�����Ϣ
    uint8 res_3byte[3];         //��������
    uint16 file_total;          //��ǰ�ļ����� (�ڵ�ǰ�б����ļ��������)
    uint16 file_num;            //��ǰ�ļ���� (�ڵ�ǰ�б����ļ�������)
    uint16 file_index;          //��ǰ�ļ������ţ�������������ţ�
    uint16 reserved;            //��������
    uint16 list_layer_no[4];    //��¼�ļ����ڵ�ÿ���б��е�λ�ã����ֱ��¼��ÿ��������λ�ã�
    uint16 layer_list_offset;   //listƫ��λ��
    uint16 reserve;             //��������

} plist_location_t;

//�ղؼ��µ��ļ���Ϣ
typedef struct
{
    uint8 disk;                     //'DISK_C;DISK_H;DISK_U
    uint8 reserved;                 //��������
    uint16 file_index;              //no used
    file_info_u file_info;          //�ļ���Ϣ
    pdir_layer_t dir_layer_info;    //Ŀ¼�����Ϣ
    uint8 res_3byte[3];             //��������
    uint16 file_total;              //��ǰ�ղؼ��ļ�����
    uint16 file_num;                //��ǰ�ļ����ղؼ��е����
    uint8 title[USERPL_TITLE_MAX];  // �ղؼ���ʾ���ļ�����
} flist_location_t;

//�ļ���Ϣ������
typedef union
{
    file_location_t  dirlocation;       //Ŀ¼ģʽ�µ��ļ���Ϣ
    plist_location_t plist_location;    //�����б�ģʽ�µ��ļ���Ϣ
    flist_location_t flist_location;    //�ղؼ���ģʽ�µ��ļ���Ϣ
} musfile_location_u;

//�ļ���Ϣ
typedef struct
{
    uint8 file_source;              //�ļ���Ϣ���ͣ�Ŀ¼�������б����ղؼ�
    uint8 reserved[3];              //��������
    musfile_location_u file_path;   //�ļ���Ϣ�ṹ
} file_path_info_t;


/********************************************************************************
 *            �����ṩ�Ľӿ��б�
 *********************************************************************************/

/**********************************
 (1)ģ��ϵͳ�ӿ�
 Ŀ¼��\COMMON
 **********************************/
extern fsel_error_e fsel_get_error(void) __FAR__;
extern int32 fsel_init(fsel_param_t *init_param,uint8 drv_mode) __FAR__;
extern bool fsel_exit(void) __FAR__;

/**********************************
 (3)����/�����б�/�ղؼ� �ļ���λ���Žӿ�
 Ŀ¼��\fsel
 **********************************/
extern bool fsel_set_mode(uint8 mode) __FAR__;

extern bool fsel_set_seltype(fsel_type_e type,uint16 cur_num) __FAR__;

extern bool fsel_set_typebit(uint32 file_type_bit,bool reset_dir_flag) __FAR__;

extern bool fsel_get_location(void *location) __FAR__;

extern bool fsel_set_location(void *location) __FAR__;

extern bool fsel_get_nextfile (char *strfile) __FAR__;

extern bool fsel_get_prevfile(char *strfile) __FAR__;

extern bool fsel_get_byno(char *strfile, uint16 num) __FAR__;

extern uint16 fsel_get_total(void) __FAR__;

extern uint16 fsel_get_fileno(void) __FAR__;

extern void fsel_set_fileno(uint16 set_num) __FAR__;

extern uint16 fsel_total_file_cur(uint8* dir_info, uint8 check_flag) __FAR__;

//����ΪĿ¼����ʱ����λĿ¼���Žӿ�(Ԥ��)
extern bool fsel_get_nextdir (uint8 *strfile) __FAR__;

extern bool fsel_get_prevdir(uint8 *strfile) __FAR__;

extern bool fsel_enter_dir(uint8 *name_short, uint8* name_long, uint8 dir_direct) __FAR__;

extern HANDLE lyric_open(char * music_filename,uint32* file_len) __FAR__;

//��ȡĿ¼������Ӧ���У���λ�����ļ�֮�����Ҫ��ȡ���ļ����ڵ�Ŀ¼��Ŀ¼����
//����Ŀ¼��locationָ�롢���Ŀ¼����bufferָ�롢buffer������Ϊ��������
extern bool fsel_get_dir_name(uint8 *strdir,uint8 name_len,file_location_t *location) __FAR__;

#endif//#ifndef  _ENHACED_H