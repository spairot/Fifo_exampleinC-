#ifndef FIFO_H
#define FIFO_H

#undef  EXT
#ifdef  FIFO_C
#define EXT
#else     
#define EXT extern
#endif   

typedef unsigned char   UCHR;

typedef struct {
  UCHR WriteIndex;
  UCHR ReadIndex;
  UCHR ReservedSize;
  UCHR BuffSize;
  UCHR *pBuff;
} ST_FIFO_CONTROL, *PST_FIFO_CONTROL;

typedef struct {
  UCHR Length;
  UCHR *pData;
} ST_DATA_INFO, *PST_DATA_INFO;

typedef struct{
  UCHR SourceID;
  UCHR Status;
  UCHR Error;
  UCHR Alarm;
} ST_FUNCIF_PLAY_STATUS, *PST_FUNCIF_PLAY_STATUS;

typedef struct{
  UCHR SourceID;
  UCHR Status;
  UCHR Error;
} ST_FUNCIF_TUN_STATUS, *PST_FUNCIF_TUN_STATUS;

union MSG_DATA {
  ST_FUNCIF_TUN_STATUS   tun_status;
  ST_FUNCIF_PLAY_STATUS  play_status;
};



//EXT void MaPcSendMessage(UCHR to,UCHR event, union PC_MSG_DATA* p_data);

EXT void MaPcFifoInit(void);
EXT void MaPcFifoMain(void);
EXT UCHR MaPcFifoWrite(PST_FIFO_CONTROL p_ctrl, PST_DATA_INFO p_info);
EXT UCHR MaPcFifoRead(PST_FIFO_CONTROL p_ctrl, PST_DATA_INFO p_info);







#endif
