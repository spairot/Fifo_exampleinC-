#include <iostream>
#include <conio.h>
#include <stdio.h>
#include "fifo.h"

using namespace std;

typedef struct {
  UCHR to;
  UCHR event;
  union MSG_DATA data;
} ST_SEND_CMN_QUEUE, *PST_SEND_CMN_QUEUE;


#define CFIFO_BUFFER_SEND_MESSAGE_SIZE (14*(sizeof(ST_SEND_CMN_QUEUE)))

static UCHR rFifoBufferMsgQue[CFIFO_BUFFER_SEND_MESSAGE_SIZE];

enum {
   CFIFO_BUFFER_INDEX_SEND_COMMAND = 0x00,
   CFIFO_BUFFER_INDEX_MAX,
 };

static ST_FIFO_CONTROL stFifoControl[CFIFO_BUFFER_INDEX_MAX];


//------------------------------------------------------
void FifoInit(void)
{
  PST_FIFO_CONTROL p_ctrl;

  p_ctrl = (PST_FIFO_CONTROL)&stFifoControl[CFIFO_BUFFER_INDEX_SEND_COMMAND];
  
  p_ctrl->WriteIndex = (UCHR)0x00;
  p_ctrl->ReadIndex = (UCHR)0x00;
  p_ctrl->ReservedSize = (UCHR)0x00;
  p_ctrl->BuffSize = (UCHR)sizeof(rFifoBufferMsgQue);
  p_ctrl->pBuff = (UCHR *)rFifoBufferMsgQue;
}

/*------------------------------------------------------
/*[Brief] : write data queue
/*[Param in] : p_ctrl control data read buffer
/*[Param in] : p_info data buffer for store data read
    BuffSize (FIFO buffer)
    ReservedSize (data index)
    p_info->Length (sizeof data(send/receive) buffer)
-------------------------------------------------------*/
UCHR FifoWrite(PST_FIFO_CONTROL p_ctrl, PST_DATA_INFO p_info)
{
  UCHR i;
  UCHR result = (UCHR)0;

  printf("Write-> Buf:%d Reserv:%d Len:%d => ",p_ctrl->BuffSize,p_ctrl->ReservedSize,p_info->Length);
  
  if((p_ctrl->BuffSize - p_ctrl->ReservedSize) >= p_info->Length){ /*buf size not overflow*/
    for(i = (UCHR)0; i < p_info->Length; i++){
      p_ctrl->pBuff[p_ctrl->WriteIndex] = p_info->pData[i]; /*store data*/
      p_ctrl->ReservedSize++;  /*add reserve size*/
      if((++p_ctrl->WriteIndex) >= p_ctrl->BuffSize){ /*add and check index*/
        p_ctrl->WriteIndex = (UCHR)0;
      }
     // p_ctrl->WriteIndex = ++p_ctrl->WriteIndex % p_ctrl->BuffSize;
    }
    result = (UCHR)1;
  }
  

  for(int j=0;j<p_info->Length;j++){
    printf("%d ",p_ctrl->pBuff[j]);
  }
  printf("\n");

  return result;
}

/*------------------------------------------------------
/*[Brief] : Read data queue
/*[Param in] : p_ctrl control data read buffer
/*[Param in] : p_info data buffer for store data read
-------------------------------------------------------*/
UCHR FifoRead(PST_FIFO_CONTROL p_ctrl, PST_DATA_INFO p_info)
{
  UCHR i;
  UCHR result = (UCHR)0;
  
  printf("Read-> Reserv:%d Len:%d =>  ",p_ctrl->ReservedSize, p_info->Length);
  
  if(p_ctrl->ReservedSize >= p_info->Length){
    for(i = (UCHR)0; i < p_info->Length; i++){
      p_info->pData[i] = p_ctrl->pBuff[p_ctrl->ReadIndex];
      p_ctrl->ReservedSize--;
      if((++p_ctrl->ReadIndex) >= p_ctrl->BuffSize){
        p_ctrl->ReadIndex = (UCHR)0;
      }
    }
    result = (UCHR)1;
  }
  
  for(int j=0;j<p_info->Length;j++){
    printf("%d ",p_info->pData[j]);
  }
  printf("\n");
  return result;
}


void SendCommand(UCHR to,UCHR event, union MSG_DATA *data)
{
  printf("SendCmd-> to:%d event:%d data:%d %d %d %d ",to,event,data->play_status.SourceID,data->play_status.Status,data->play_status.Error,data->play_status.Alarm);
  for(int j=0;j<4;j++){
    //prinf("%d",data->data[j]);
  }
  printf("\n");
#if 0
  struct STR_MSG_CMN_HEADER stCmnHeader;
  union BASE_MSG_DATA  unBaseMsg;
  
    memset(&stCmnHeader, (UCHR)0, sizeof(stCmnHeader));
    memset(&unBaseMsg, (UCHR)0, sizeof(unBaseMsg));

  if(to == C_ADDR_PC_GRP){
    unBaseMsg.str_pc.rad                  = C_ADDR_PC_GRP;                         /* receive address */
    unBaseMsg.str_pc.tad                  = C_ADDR_MASTER;                         /* transmitter address */
    unBaseMsg.str_pc.event                = event;
    unBaseMsg.str_pc.data                 = data;
    stCmnHeader.base_msg_id   = C_MSG_BASE_PC_REQ_MESSAGE;
    stCmnHeader.msg_data      = &unBaseMsg;
    m_if_cmn_req(&stCmnHeader);
  }else if(to == C_ADDR_MASTER){
    unBaseMsg.str_pc.rad                  = C_ADDR_MASTER;                         /* receive address */
    unBaseMsg.str_pc.tad                  = C_ADDR_PC_GRP;                         /* transmitter address */
    unBaseMsg.str_pc.event                = event;
    unBaseMsg.str_pc.data                 = data;
    stCmnHeader.base_msg_id   = C_MSG_BASE_PC_MESSAGE;
    stCmnHeader.msg_data      = &unBaseMsg;
    m_if_pc_rcv(&stCmnHeader);
  }else if(to == C_ADDR_DISPLAY_GRP){
    unBaseMsg.str_pc.rad                  = C_ADDR_DISPLAY_GRP;                    /* receive address */
    unBaseMsg.str_pc.tad                  = C_ADDR_MASTER;                         /* transmitter address */
    unBaseMsg.str_pc.event                = event;
    unBaseMsg.str_pc.data                 = data;
    stCmnHeader.base_msg_id   = C_MSG_BASE_PC_MESSAGE;
    stCmnHeader.msg_data      = &unBaseMsg;
    m_if_pc_rcv(&stCmnHeader);
  }
#endif
  return;
}

//------------------------------------------------------
void FifoMain(void)
{
  PST_FIFO_CONTROL p_ctrl = (PST_FIFO_CONTROL)&stFifoControl[CFIFO_BUFFER_INDEX_SEND_COMMAND];
  ST_DATA_INFO info;
  
  ST_SEND_CMN_QUEUE data;
  
  info.Length = (UCHR)sizeof(data);
  info.pData = (UCHR *)&data;

  if(FifoRead(p_ctrl, &info) == 1){
    /*send command */
    SendCommand(data.to,data.event,&data.data);
  }else{
    /*NOP*/
  }
}

//------------------------------------------------------
void SendMessage(UCHR to,UCHR event, union MSG_DATA* p_data)
{
  PST_FIFO_CONTROL p_ctrl = (PST_FIFO_CONTROL)&stFifoControl[CFIFO_BUFFER_INDEX_SEND_COMMAND];
  ST_DATA_INFO info;
  ST_SEND_CMN_QUEUE data_q;

  data_q.to = to;
  data_q.event = event;
  data_q.data = *p_data;
  info.Length = (UCHR)sizeof(data_q);
  info.pData = (UCHR *)&data_q;
  FifoWrite(p_ctrl, &info);
  printf("Send->to: %d event: %d Sid:%d Sts:%d err:%d alm:%d\n",data_q.to,data_q.event,data_q.data.play_status.SourceID,data_q.data.play_status.Status,data_q.data.play_status.Error,data_q.data.play_status.Alarm );

  return;
}

//------------------------------------------------------
main(){
  union MSG_DATA buff;
  PST_FUNCIF_PLAY_STATUS p_play_sts_info = &buff.play_status;
  FifoInit();
  //-----------------------send msg
  p_play_sts_info->SourceID = 10;
  p_play_sts_info->Status   = 11;
  p_play_sts_info->Error    = (UCHR)12;
  p_play_sts_info->Alarm    = (UCHR)13;
  SendMessage(8,9, &buff);
  
  while(1){
    FifoMain();      
    getch();
  }
}
