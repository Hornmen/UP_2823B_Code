/*******************************************************************************
 *                         
 * WAVES PC工具和算法数据接口代码
 *
 *******************************************************************************/
#include <psp_includes.h>

const uint8 waves_dae_para_bin[] ={ "wseparam.bin" };

//input参数表，通过按键音量来控制

const int16 w_input_table[32] ={-600, -525, -468, -425, -380, -368, -350, -324, \
                                -302, -287, -267, -252, -240, -230, -222, -211, \ 
                                -204, -199, -192, -185, -181, -173, -166, -158,  \
                                -154, -147, -140, -132, -124, -117, -113, -77   }; 

static int dsp_to_mips_address(int addr)
{
    //only for addr <= 0x30000
    return (((addr & 0x1ffff) << 1) + 0x9fc20000);
}

static int mips_to_dsp_address(int addr)
{
    //only for addr >= 0x9fc20000
    return ((((addr & 0x3ffff) - 0x20000) >> 1) + 0x20000);
}

static void read_waves_dae_para(void)
{
    sd_handle p_waves_dae_para_bin_handle;
    
    int addr_mips = 0;
    p_waves_dae_para_bin_handle = sys_sd_fopen(waves_dae_para_bin);

    if (p_waves_dae_para_bin_handle == NULL)
    {       
        PRINT_INFO("open waves dae bin fail!!!");
        
        return;   
    }

    sys_sd_fread(p_waves_dae_para_bin_handle, (void*)WAVES_DAE_PARA_START_ADDR, WAVES_DAE_PARA_LENGTH);
    
    if (p_waves_dae_para_bin_handle != NULL)
    {
       sys_sd_fclose(p_waves_dae_para_bin_handle);
    }
}

static void write_waves_dae_para_vm(void)
{
    uint8 i = 0;
    uint8 block_num = 0;
    waves_dae_para_t* waves_dae_para = (waves_dae_para_t*)WAVES_DAE_PARA_START_ADDR;

    block_num = WAVES_DAE_PARA_LENGTH/VM_DATA_LENGTH + 1;

    for (i = 0; i < block_num; i++)
    {
        sys_vm_write(waves_dae_para + i, (VM_WAVES_AL_DATA1 + (i<<16)), VM_DATA_LENGTH);
    }   
}


static void read_waves_dae_para_vm(void)
{
    uint8 i = 0;
    uint8 block_num = 0;
    waves_dae_para_t* waves_dae_para = (waves_dae_para_t*)WAVES_DAE_PARA_START_ADDR;

    block_num = WAVES_DAE_PARA_LENGTH/VM_DATA_LENGTH + 1;

    for (i = 0; i < block_num; i++)
    {
        sys_vm_read(waves_dae_para + i, (VM_WAVES_AL_DATA1 + (i<<16)), VM_DATA_LENGTH);
    }   
}


int32 key_inner_set_waves_effect_param(uint32 set_type, void *param_ptr, void *null3)
{
     coeff_t *p_coeff = (coeff_t*)WAVES_ASET_COEFF_ADDR;
     coeff_property_t* p_coeff_property;

     uint32 cur_package_start_addr = 0;
     uint32 cur_package_length = 0;

     uint8  cur_vol = NULL;
     uint32 w_input_addr = NULL;
     int16  w_db = 0;
     uint32 w_input_val  = 0;
     uint32 coff_storagef_start_dsp_addr;
     
     switch (set_type)
     {
        case SET_WAVES_COEFF_PROPERTY:
        p_coeff_property = (coeff_property_t*)param_ptr; 
        
        p_coeff->parameter_address = p_coeff_property->memory_addr;    
        p_coeff->parameter_length = p_coeff_property->length;
             
        break;
        
        case SET_WAVES_EFFECT_PARAM:
            
        cur_package_start_addr = p_coeff->parameter_address; 
        cur_package_length = p_coeff->parameter_length;
        coff_storagef_start_dsp_addr = mips_to_dsp_address(WAVES_ASET_SHARE_MEM_START_ADDR);
            
        libc_memcpy((void*)(cur_package_start_addr - coff_storagef_start_dsp_addr + WAVES_ASET_SHARE_MEM_START_ADDR),(uint8*)param_ptr,cur_package_length);

        p_coeff->DAE_change_flag = 1;      
        break;

        case WAVES_DAE_WRITE_VRAM:
           
        write_waves_dae_para_vm();
        
        break;

        case WAVES_DAE_READ_VRAM:
           
        read_waves_dae_para_vm();
        
        break;

        
        case SET_WAVES_PARAM_FROM_SDFS:
           
        read_waves_dae_para();
        
        break;

        case SET_WAVES_INPUT_PARAM:
        cur_vol = *(uint32*)param_ptr;
      
        w_db = w_input_table[cur_vol];
        w_input_val = exp_fast(w_db);
        PRINT_INFO_INT("w_input_val",w_input_val);
          
        w_input_addr = dsp_to_mips_address(WAVES_INPUT_PARA_START_ADDR);
        
        *((int32*)w_input_addr) = w_input_val;

        break;
        
        default:
        break;
     }
    return 0;
}


