#ifndef __PROCESS_PACKET_H
#define __PROCESS_PACKET_H

#include "gd32e23x.h"
#include "Spi_nand_driver.h"

#define PKT_HDLC_ESCAPE 0x7D
#define PKT_HDLC_ESCAPEL 0x5E
#define PKT_HDLC_ESCAPEH 0x5D

#define PKT_HDLC_START 0x55
#define PKT_HDLC_FLAG  0x7E
#define PKT_CRC_SIZE  2
#define PKT_RUNNING_START_ADDR 0x65008000
#define PKT_DATA_SIZE  0x8200   //0xE000

#define SPL_BLOCK_ID 0

#define SPL_START_PAGE_ID 0

#define SPL_MAX_PART_SIZE 0x40000
#define SPL_MTD_PART_SIZE 0x80000
#define SPL_PACKET_SIZE 1024
#define SPL_SUBPACKAGE_SIZE SPL_PACKET_SIZE


#define SPL_SIGNATURE_FILE_SIZE 1024
#define SPL_SIGNATURE_HEAD_SIZE 512

#define START_TIMEOUT_TIME 6000
#ifndef DEBUG_DATA
#define TRANS_TIMEOUT_TIME 60000
#else
#define TRANS_TIMEOUT_TIME 300000
#endif
#define PMIC_RESET_TIME 8000

typedef struct{
   uint8_t SPL_Buffer[SPL_PACKET_SIZE];
   uint8_t *SPL_Temp;
   uint32_t SPL_Size;
   SPARE_AREA blockArea[2];
   NAND_ADDRESS  SPL_ADDR;
   uint32_t SplBlock0_Flag;
}spl_nand_flash_t;

typedef struct{
   uint8_t  Timer_Start_Flag;
   int      Timer_Count;
   int      timeout_time;
}send_timer_t;


typedef enum 
{
    PKT_NONE = 0,
    PKT_HEAD,
    PKT_GATHER,
    PKT_RECV,
    PKT_ERROR
} pkt_flag_s;

typedef struct pkt_header_tag
{
    unsigned short pkt_type;
    unsigned short pkt_size;
}pkt_header, *pkt_header_ptr;

typedef enum CMD_PKT_TYPE
{
    BSL_PKT_TYPE_MIN,                   /* the bottom of the DL packet type range */
       
    /* Link Control */    
    BSL_CMD_CONNECT = BSL_PKT_TYPE_MIN,    

    /* Data Download */
    BSL_CMD_START_DATA,                  /* the start flag of the data downloading  */
    BSL_CMD_MIDST_DATA,                  /* the midst flag of the data downloading  */
    BSL_CMD_END_DATA,                    /* the end flag of the data downloading */
    //BSL_CMD_CHK_DATA,                    /* request verify the downloading data */
    BSL_CMD_EXEC_DATA,                   /* Execute from a certain address */
    BSL_CMD_UART_DOWNLOAD_END,                 /*Access the memory in the download mode*/
    BSL_CMD_SEND_NAND_ID,
    BSL_CMD_CHANGE_BAUD = 0x9,
    /* End of Data Download command*/
    /* End of the Command can be received by phone*/

    /* Start of the Command can be transmited by phone*/
    BSL_REP_TYPE_MIN = 0x80,

    BSL_REP_ACK = BSL_REP_TYPE_MIN,      /* The operation acknowledge */
    BSL_REP_VER,

    BSL_START_FLAG,
    BSL_UART_ENUM_FLAG,
    BSL_UART_RECIVE_FLAG,
    BSL_UART_WAKEUP_FLAG,
    SPL_TRANSPORT_TIMEOUT,

    BSL_PKT_TYPE_MAX = 0xffff
}cmd_pkt_type;


typedef enum{
   
   OPT_SUCCESS,
   CRC_ERROR = BSL_UART_RECIVE_FLAG+1,  // 95
   READ_NANDFLASH_ID_ERROR,
   READ_NANDFLASH_BLOCK0_ERROR,
   READ_NANDFLASH_BLOCK1_ERROR,
   READ_NANDFLASH_PAGE_LIMIT_ERROR,
   SECUREBOOT_SPL_CHEAK_FIALED,
   CHECK_SPL_SIZE_FAILED,
   CHECK_MAGIC_FAILED,
}error_code_t;

typedef struct sio_handle_tag
{
   int            sio_state;
   pkt_flag_s     pkt_state;
   cmd_pkt_type   pkt_type;
   int            pkt_count;
   int            buf_size;
   int            data_size;
   unsigned char  *buf_ptr;
   unsigned char  *write_ptr;
   unsigned char  *read_ptr;
}sio_handle,*sio_handle_ptr;

extern sio_handle  global_sio_handle;
extern spl_nand_flash_t  spl_handle;
extern  send_timer_t timer_handle;
//extern int start_flag;
//extern int runing_count;
typedef int (*pkt_proc_fun) (void);


int Process_packets(void);

int Change_Baud_CMD(void);

int Send_Connect_CMD(void);

int Send_Start_CMD(void);

int Send_Data_Midst_CMD(void);

int Send_Data_End_CMD(void);

int Send_Data_Exec_CMD(void);

int Uart_Download_end(void);

int Uart_SecureBoot_cheak(void);

void Send_Error_Code(unsigned short val);

uint8_t nand_flash_init(void);

int SPL_GetNandFlashData(int length);

uint8_t Spl_BackUp(void);

int Send_Nand_ID_CMD(void);

void check_failed_process(unsigned short val);

int check_spl_magic(void);

#endif
