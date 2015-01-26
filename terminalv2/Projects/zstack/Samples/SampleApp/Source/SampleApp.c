/**************************************************************************************************
  Filename:       SampleApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Sample Application (no Profile).


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "mt_uart.h"//����ͷ�ļ�
#include "MT_APP.H"
#include "MT.h"//CMD_SERIAL_MSG����ID�����ﶨ���
#include <ioCC2530.h>
#include <stdlib.h>
/************����Э���ߵ�ʱ����Ҫ��ע��ȥ��**********/
//#define DEV_COOR
//#define DEBUG_STAGE
/*****************************/
#ifndef DEV_COOR
#include "ds18b20.H"
#endif
/**********************��������******************************/
#ifndef CMD_H
#define CMD_H

/***�������������CmdType****/
#define CMD_TYPE_CLOSELINK             0x00         /*�Ͽ�����*/
#define CMD_TYPE_QUERYREQUEST          0x01         /*��ѯ����*/
#define CMD_TYPE_QUERYRESPONSE         0x02         /*��ѯӦ��*/
#define CMD_TYPE_CONTROLREQUEST        0x03         /*��������*/
#define CMD_TYPE_CONTROLRESPONSE       0x04         /*����Ӧ��*/
/***cmd�����������CmdCtrlType*/
#define CMD_CONTROL_OPEN               0x00     // �� 
#define CMD_CONTROL_CLOSE              0x01     // �ر�
#define CMD_CONTROL_WRITE              0x02     // д 
#define CMD_CONTROL_READ               0x03     // �� 
#define CMD_CONTROL_START              0x04     // ��ʼ 
#define CMD_CONTROL_STOP               0x05     // ����
/*
CmdCtrlValue     0x00 - 0xff
*/
/***���巿��ID CmdRoomId***/
#define CMD_ROOM_LIVINGROOM             0x00       //����
#define CMD_ROOM_BEDROOM                0x01       //����
#define CMD_ROOM_KITCHEN                0x02       //����
/**cmd�豸���Ͷ���CmdDevType
  �������ȶ���
  */
#define CMD_DEV_LED                0x00      // ��
#define CMD_DEV_CURTAIN            0x01      //����
#define CMD_DEV_TEMPERATURE        0x02       //�¶�
#define CMD_DEV_LIGHTINTENSITY     0x03       //����ǿ��
#define CMD_DEV_AIR                0x04       //����
#define CMD_DEV_WINDOWMAGNETIC     0x05       //����
#define CMD_DEV_DOORBELL           0x06       //����
#define CMD_DEV_OUTPUT             0x07       //���Ԥ��
#define CMD_DEV_INPUT              0x08       //����Ԥ��
  
/**���������**/
#define  CmdFlag         0                             /*֡ͷ*/                        //0x7f
#define  CmdType         1                             /* �������� */
#define  CmdCtrlType     2                             /* ������Ƶ����� */
#define  CmdCtrlValue    3                             /* ����ֵ */
#define  CmdRoomId       4                             /* ����� */
#define  CmdDevType      5                             /* �豸���� */
#define  CmdDevId        6                             /* �豸�� */
/***************************************
CmdDevId 0x00 - 0xff

�������Ŀģ��Ϊ 7f 0X 0X 0X 0X 0X 0X
**************************************/
#define  CMD_LEN         7                 //һ����7��16λ����             
#endif
/*****************************����ΪCMD******************************************/
/*********�Դ�led����**********/
#define LED1    HAL_LED_1  //�Դ���LED          ���Ƶľ��� P1.0
#define LED2    HAL_LED_2                            //   p1.1
#define LED3    HAL_LED_3                      //��Ҫʹ��    P1.3         
#define LED_ON  HAL_LED_MODE_ON  //���Ϊ�ߵ�ƽ
#define LED_OFF HAL_LED_MODE_OFF
/*********************���豸����ģ�鶨��************************
��һ��������������������Ϣ��������Ӧ�Ķ���*/
/*****************************************************************
    ����ʵ�֣�DEV_LIGHT          ��      P0.0   ��� 
                                         P0.1   ���
                                         P0.4   ���
              DEV_CURTAIN       ����     P0.5   ���
              DEV_TEMPERATURE    �¶�    P0.6   ����
              DEV_LIGHT         ��ǿ     P0.7   ����
              DEV_IAR          ����      P1.2   ����
         DEV_WINDOWMAGNETIC     ����     P1.5   ����
            DEV_DOORBELL        ����     P2.0   �ж� 
          DEV_OUTPUT            ���Ԥ��  P1.6 P1.7
          DEV_INPUT             ����Ԥ��  p2.3 p2.4
***************************************************************/
//�� P0.4 ���
#ifdef  DEV_LIGHT
  #define DEV_LIGHT1_OPEN()        do{P0_0 = 1;}while(0);
  #define DEV_LIGHT1_CLOSE()       do{P0_0 = 0;}while(0);
  #define DEV_LIGHT1_BIT           (P0_0)
  #define DEV_LIGHT2_OPEN()        do{P0_1 = 1;}while(0);
  #define DEV_LIGHT2_CLOSE()       do{P0_1 = 0;}while(0);
  #define DEV_LIGHT2_BIT           (P0_1)
  #define DEV_LIGHT3_OPEN()        do{P0_4 = 1;}while(0);
  #define DEV_LIGHT3_CLOSE()       do{P0_4 = 0;}while(0);
  #define DEV_LIGHT3_BIT           (P0_4)
#endif
//���� P0.5 ���
#ifdef  DEV_CURTAIN
  #define DEV_CURTAIN_OPEN()       do{P0_5 = 1;}while(0);
  #define DEV_CURTAIN_CLOSE()      do{P0_5 = 0;}while(0);
  #define DEV_CURTAIN_BIT          (P0_5)
#endif


 //��ǿ p0.7
#ifdef DEV_LIGHTINTENSITY
  #define DEV_LIGHTINTENSITY_BIT            (P0_7)
#endif
   //���� p1.2
#ifdef DEV_IAR
  #define  DEV_IAR_BIT               (P1_2)
#endif
   //���� p1.5
#ifdef DEV_WINDOWMAGNETIC
  #define  DEV_WINDOWMAGNETIC_BIT     (P1_5)
#endif
   //���� p2.0
#ifdef DEV_DOORBELL
#define DEV_DOORBELL_BIT             (P2_0)
#endif
//Ԥ����� p1.6 1.7
#ifdef DEV_OUTPUT
 #define DEV_OUTPUT1_OPEN()      do{P1_6 = 1;}while(0);
 #define DEV_OUTPUT1_CLOSE()      do{P1_6 = 0;}while(0);
 #define DEV_OUTPUT1_BIT            (P1_6)
 #define DEV_OUTPUT2_OPEN()      do{P1_7 = 1;}while(0);
 #define DEV_OUTPUT2_CLOSE()      do{P1_7 = 0;}while(0);
 #define DEV_OUTPUT2_BIT            (P1_7)
#endif
   //Ԥ������ p2.3 p2.4
#ifdef DEV_OUTPUT
 #define DEV_OUTPUT1_OPEN()      do{P1_6 = 1;}while(0);
 #define DEV_OUTPUT1_CLOSE()      do{P1_6 = 0;}while(0);
 #define DEV_OUTPUT1_BIT            (P1_6)
 #define DEV_OUTPUT2_OPEN()      do{P1_7 = 1;}while(0);
 #define DEV_OUTPUT2_CLOSE()      do{P1_7 = 0;}while(0);
 #define DEV_OUTPUT2_BIT            (P1_7)
#endif
   //Ԥ������ 
#ifdef DEV_INPUT
   #define DEV_INPUT1_BIT            (P2_3)
   #define DEV_INPUT2_BIT            (P2_4)
#endif
/*****************����ģ�鶨�����**************************/

/***�������еķ����**/
/********************************************
    LIVING_ROOM         ����
    BED_ROOM            ����
    KITCHEN_ROOM        ����
*******************************************/
#ifndef DEV_COOR
    // #error  outdev_coor
     #ifdef  LIVING_ROOM     //����
     #define ROOM_NUM   CMD_ROOM_LIVINGROOM //�ѷ���Ŷ���Ϊ����
     #endif
     #ifdef BED_ROOM         //����
       #define ROOM_NUM CMD_ROOM_BEDROOM
     #endif 
     #ifdef KITCHEN_ROOM      //����
       #define ROOM_NUM CMD_ROOM_KITCHEN
     #endif
     #define DEV_NUM 1
#endif
/***********************************************************/
/******************���巿���������豸������***************/
#ifdef DEV_COOR

  //#error  indev_coor

  #define ROOM_COUNT 3
  #define DEV_COUNT 1
#endif
/********************************************************/

//�����־����
#define FRAME_FLAG  (0x7f)   //���ݿ���ж�
#define ERROR_FLAG  (0x6f)   
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.





endPointDesc_t SampleApp_epDesc;
#ifndef DEV_COOR
   int DoorBellPressed = 0;//����
#endif



/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;
#ifdef DEBUG_STAGE          //debugģʽ�ṩASCII����
       uint8 asc_16[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#endif      
uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;
afAddrType_t SampleApp_Flash_DstAddr;
/**�Լ�����ĵ�Ե�Ĵ���**/

  afAddrType_t SampleApp_Point_to_Point_DstAddr;

/**************/
aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;
/*******·�ɱ�********/
#ifdef DEV_COOR
  uint16 Routing_Table[ROOM_COUNT][DEV_COUNT]={0};
#endif

//unsigned char temp_bank = 0;
/************/
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage( void );
void SampleApp_SendFlashMessage( uint16 flashTime );

/***my funtions**/
#ifdef DEV_COOR
  void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg);//���ڽ��ܵ����ݴ�����������
#endif
  //void P1_ISR(void);
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id; //����ID
  SampleApp_NwkState = DEV_INIT;//��������
  SampleApp_TransID = 0;// ���÷������ݵķ�ʽ��Ŀ�ĵ�ַ,
  /*****���ڳ�ʼ��******/
  MT_UartInit ();
  MT_UartRegisterTaskID(task_id);

  #ifndef DEV_COOR
    #ifdef DEBUG_STAGE
      #ifdef LIVING_ROOM
       HalUARTWrite(0,"livingroom\n",11);
      #endif
      #ifdef BED_ROOM
       HalUARTWrite(0,"bedroom\n",8);
      #endif
      #ifdef KIT_ROOM
       HalUARTWrite(0,"kitroom\n",8);
      #endif
    #endif
  #endif

  /******************�ն��豸��ģ��io��ʼ��*******************/
  /*
       �ڶ��������������Ҫ�����õ�IO��ʼ��
  ***********************************************************/
#ifndef DEV_COOR
    //�� P0.0 1 4 ���
   #ifdef  DEV_LIGHT
       P0SEL &= 0xec;//��1����I/O��  P0_0
       P0DIR |= 0x13;//LIGHT1 ���
       P0_0 = 0;
       P0_1 = 0;
       P0_4 = 0;      
   #endif
 //���� P0.5 ���  
   #ifdef DEV_CURTAIN
      P0SEL &= 0xDF;
      P0DIR |= 0x40;  //���  
      P0_5 = 0;
   #endif
      //�¶� P0.6
   #ifdef DEV_TEMPERATURE
      P0SEL &= 0xbf;         //DS18B20��io�ڳ�ʼ���¶ȳ�ʼ��
      Temp_test();//�����¶�
   #endif
     //��ǿ P0.7
   #ifdef DEV_LIGHTINTENSITY
      P0SEL &= 0x7f;
      P0DIR &= 0x7f;
   #endif 
      //���� P1.2
   #ifdef DEV_AIR
      P1SEL &= 0xfb;
      P1DIR &= 0xfb;  //����
   #endif
      //���� P1.5
   #ifdef DEV_WINDOWMAGNETIC
      P1SEL &= 0xef;
      P1DIR &= 0xef;
      P1INP &= 0xef;
   #endif
      //���� p2.0
   #ifdef DEV_DOORBELL   
  
   #endif
      //output
   #ifdef DEV_OUTPUT
      P1SEL &= 0x3f;
      P1DIR |= 0xc0;
   #endif
      //input
   #ifdef DEV_INPUT
     P2SEL  &= 0xf9;
     P2DIR  &= 0xe7;
   #endif
#endif
/************IO��ʼ������**********************/
      
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;

  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;
/***�趨��Ե�ĵ�ַ����****/
  SampleApp_Point_to_Point_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  SampleApp_Point_to_Point_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0xffff;//���ݴ��ն˷���Э����
/********************************/
  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent �������������������Ƕ�����������¼����д���
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )//���ʹ�����
{
  afIncomingMSGPacket_t *MSGpkt;
  #ifndef DEV_COOR//��ż������������
     uint8 buffer[4];
  #endif
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
#ifdef DEV_COOR
        case CMD_SERIAL_MSG://�����յ����ݺ���MT_UART�㴫�ݹ��������ݣ������䷽�����գ�����ʱ������MT������ݣ� /***������������Ե��ԵĴ������ݴ�����***/
           SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);//�����յ���Ϣ���¼��� CMD_SERIAL_MSG �ͻᱻ�Ǽǣ������ CMD_SERIAL_MSG: ִ�� SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);         
           break;
#endif
            
        // Received when a key is pressed
        case KEY_CHANGE:
          //�������͸ı�
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE://�������״̬�������仯
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( //(SampleApp_NwkState == DEV_ZB_COORD)||//Э���߲����Լ��㲥
               //(SampleApp_NwkState == DEV_ROUTER)|| //Э���߲���·�ɵ㲥
              (SampleApp_NwkState == DEV_END_DEVICE))
          {
 #ifndef DEV_COOR//�ն���Э�������ͼ�����������ݰ� ���ݰ���ʽΪ    m ����ţ�ROOM_NUM�� �豸�ţ�DEV_NUM�� g
            buffer[0] = 'm';
            buffer[1] = ROOM_NUM;//          
            buffer[2] = DEV_NUM;
            buffer[3] = 'g';
            SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
            if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr,
                       &SampleApp_epDesc,
                       SAMPLEAPP_ADDNET_CLUSTERID,
                       4,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE | AF_ACK_REQUEST,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                   {
                      #ifdef DEBUG_STAGE
                               HalUARTWrite(0,"Init EndPoint\n\r",sizeof("Init EndPoint\n\r"));
                             //HalLedSet(LED1,LED_ON);
                      #endif 
                   }
                else
                  {
                  // Error occurred in request to send.
                   }
            /*****ԭ������ע��
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( SampleApp_TaskID,
                              SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                              SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
            */
#endif
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )//���������һֱ��ִ�����״̬
  {
    // Send the periodic message
    SampleApp_SendPeriodicMessage();
    /**********�޸�ʹ�õĺ���********/
    //SampleApp_PointToPointMessage();
    /******/
    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
        (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    // return unprocessed events
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.���а���������
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    /* This key sends the Flash Command is sent to Group 1.
     * This device will not receive the Flash Command from this
     * device (even if it belongs to group 1).
     */
     #ifdef DEV_DOORBELL
       DoorBellPressed =1;
     #endif
    //HalLedBlink(HAL_LED_1,2,50,500);//
   // SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    /*
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
    */
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB���մ�������
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )//�������ݴ�����
{
 // uint16 flashTime;
  uint8 *str = NULL; 
  #ifndef DEV_COOR
     uint8 reply_buf[CMD_LEN]; //reply_buf[7]
     #ifdef DEBUG_STAGE
       int i;
     #endif
  #endif
  #ifdef DEV_COOR
      uint8 buffer[4];//���ڴ���Ӧ��
      uint16 shortaddr;//������Ŷ̵�ַ
  #endif
  str = pkt->cmd.Data;
  switch ( pkt->clusterId )
  {
    /***�Լ���ӵ�������֧***/
    /*****
    ������������Լ���״̬��
    ******/
    case SAMPLEAPP_POINT_TO_POINT_CLUSTERID:
      #ifndef DEV_COOR                         
          #ifdef DEBUG_STAGE
             for(i=0;i<CMD_LEN;i++)
             HalUARTWrite(0,str+i,1 ); 
          #endif 
            HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ�õ�����
            if(str[CmdFlag] == FRAME_FLAG && pkt->srcAddr.addr.shortAddr == 0x0000)   //ȷ��֡ͷ��Դ��ַ 
            {
              reply_buf[CmdFlag] = FRAME_FLAG;//����ж�
              switch(str[CmdDevType])
              {
                /************************��******************/
                #ifdef DEV_LIGHT
                  case CMD_DEV_LED:
                    if(str[CmdType] == CMD_TYPE_CONTROLREQUEST)//�Ƿ�Ϊ����
                     {
                          switch(str[CmdCtrlType])//������Ƶ�����
                          {
                                case CMD_CONTROL_OPEN:
                                              {
                                                  reply_buf[CmdCtrlValue] = 1; 
                                                  switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_LIGHT1_OPEN()
                                                        break;
                                                     case 1:
                                                        DEV_LIGHT2_OPEN()
                                                        break;
                                                     case 2:
                                                        DEV_LIGHT3_OPEN()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                              }
                               break;
                               case CMD_CONTROL_CLOSE:
                                            {
                                               reply_buf[CmdCtrlValue] = 0; 
                                               switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_LIGHT1_CLOSE()
                                                        break;
                                                     case 1:
                                                        DEV_LIGHT2_CLOSE()
                                                        break;
                                                     case 2:
                                                        DEV_LIGHT3_CLOSE()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                            }
                               break;
                               default:
                                     reply_buf[CmdCtrlType] = ERROR_FLAG;   
                          }
                          reply_buf[CmdType] = CMD_TYPE_CONTROLRESPONSE; 
                     }                  
                    else if(str[CmdType] == CMD_TYPE_CONTROLREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)//��ѯ
                    {
                      switch(str[CmdDevId])
                      {
                        case 0:
                            if(DEV_LIGHT1_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                        break;
                       case 1:
                            if(DEV_LIGHT2_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                         break;
                       case 2:
                            if(DEV_LIGHT3_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                         break;
                        default:
                             reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
                    }
                    reply_buf[CmdDevType] = CMD_DEV_LED;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
             #endif
                  /**************************����*************************************/
             #ifdef DEV_CURTAIN
                  case CMD_DEV_CURTAIN:
                    if(str[CmdType] == CMD_TYPE_CONTROLREQUEST)//�Ƿ�Ϊ����
                     {
                          switch(str[CmdCtrlType])//������Ƶ�����
                          {
                                case CMD_CONTROL_OPEN:
                                              {
                                                  reply_buf[CmdCtrlValue] = 1; 
                                                  switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_CURTAIN_OPEN()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                              }
                               break;
                               case CMD_CONTROL_CLOSE:
                                            {
                                               reply_buf[CmdCtrlValue] = 0; 
                                               switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_CURTAIN_CLOSE()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                            }
                               break;
                               default:
                                     reply_buf[CmdCtrlType] = ERROR_FLAG;   
                          }
                          reply_buf[CmdType] = CMD_TYPE_CONTROLRESPONSE; 
                     }                  
                    else if(str[CmdType] == CMD_TYPE_CONTROLREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)//��ѯ
                    {
                      switch(str[CmdDevId])
                      {
                        case 0:
                            if(DEV_CURTAIN_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                        break;
                        default:
                             reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
                    }
                    reply_buf[CmdDevType] = CMD_DEV_LED;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
             #endif
              /*******************************************�¶�**********************************************/
          #ifdef DEV_TEMPERATURE
           case  CMD_DEV_TEMPERATURE:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
           // P0SEL &= 0xbf;         //DS18B20��io�ڳ�ʼ��
            switch(str[CmdDevId])
              {
                case 0:
                  Temp_test();
               break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_TEMPERATURE;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif
                   /*******************************************��ǿ**********************************************/
         #ifdef DEV_LIGHTINTENSITY
                  case  CMD_DEV_LIGHTINTENSITY:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
         
            switch(str[CmdDevId])
              {
                case 0:
                  if(DEV_LIGHTINTENSITY_BIT)
                     reply_buf[CmdCtrlValue] = 1;
                  else
                     reply_buf[CmdCtrlValue] = 0;
                break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_LIGHTINTENSITY;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif    
                       /*******************************************����**********************************************/
         #ifdef DEV_AIR
                  case  CMD_DEV_AIR:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
         
            switch(str[CmdDevId])
              {
                case 0:
                  if(DEV_AIR_BIT)
                     reply_buf[CmdCtrlValue] = 1;
                  else
                     reply_buf[CmdCtrlValue] = 0;
                break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_AIR;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif 
                  /******************************����*************************************/
         #ifdef DEV_WINDOWMAGNETIC
            case  CMD_DEV_WINDOWMAGNETIC:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
         
            switch(str[CmdDevId])
              {
                case 0:
                  if(DEV_WINDOWMAGNETIC_BIT)
                     reply_buf[CmdCtrlValue] = 1;
                  else
                     reply_buf[CmdCtrlValue] = 0;
                break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_WINDOWMAGNETIC;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif
         /******************************����*************************************/
         #ifdef DEV_DOORBELL 
            case  CMD_DEV_DOORBELL:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
         
            switch(str[CmdDevId])
              {
                case 0:
                  if(DEV_DOORBELL_BIT == 0)
                     reply_buf[CmdCtrlValue] = 0;
                  else
                  {
                       reply_buf[CmdCtrlValue] = 1;
                       DoorBellPressed = 0;
                  }
                break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_DOORBELL;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif  
                   /************************���Ԥ��******************/
                #ifdef DEV_OUTPUT
                  case CMD_DEV_OUTPUT:
                    if(str[CmdType] == CMD_TYPE_CONTROLREQUEST)//�Ƿ�Ϊ����
                     {
                          switch(str[CmdCtrlType])//������Ƶ�����
                          {
                                case CMD_CONTROL_OPEN:
                                              {
                                                  reply_buf[CmdCtrlValue] = 1; 
                                                  switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_OUTPUT1_OPEN()
                                                        break;
                                                     case 1:
                                                        DEV_OUTPUT2_OPEN()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                              }
                               break;
                               case CMD_CONTROL_CLOSE:
                                            {
                                               reply_buf[CmdCtrlValue] = 0; 
                                               switch(str[CmdDevId])
                                                  {
                                                     case 0:
                                                        DEV_OUTPUT1_CLOSE()
                                                        break;
                                                     case 1:
                                                        DEV_OUTPUT2_CLOSE()
                                                        break;
                                                     default:
                                                            reply_buf[CmdCtrlValue] = ERROR_FLAG;
                                                  }
                                            }
                               break;
                               default:
                                     reply_buf[CmdCtrlType] = ERROR_FLAG;   
                          }
                          reply_buf[CmdType] = CMD_TYPE_CONTROLRESPONSE; 
                     }                  
                    else if(str[CmdType] == CMD_TYPE_CONTROLREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)//��ѯ
                    {
                      switch(str[CmdDevId])
                      {
                        case 0:
                            if(DEV_OUTPUT1_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                        break;
                       case 1:
                            if(DEV_OUTPUT2_BIT)
                               reply_buf[CmdCtrlValue] = 1;
                            else
                               reply_buf[CmdCtrlValue] = 0;
                         break;
                      
                        default:
                             reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
                    }
                    reply_buf[CmdDevType] = CMD_DEV_OUTPUT;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
             #endif
                    /******************************����Ԥ��*************************************/
         #ifdef DEV_INPUT
            case  CMD_DEV_INPUT:
          if(str[CmdType] == CMD_TYPE_QUERYREQUEST && str[CmdCtrlType] == CMD_CONTROL_READ)
          {
         
            switch(str[CmdDevId])
              {
                case 0:
                  if(DEV_INPUT_BIT1)
                     reply_buf[CmdCtrlValue] = 1;
                  else
                     reply_buf[CmdCtrlValue] = 0;
                break;
                case 1:
                  if(DEV_INPUT_BIT2)
                     reply_buf[CmdCtrlValue] = 1;
                  else
                     reply_buf[CmdCtrlValue] = 0;
                break;
               default:
                          reply_buf[CmdCtrlValue] = ERROR_FLAG;  
                      }
                      reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE; 
               }
                    reply_buf[CmdDevType] = CMD_DEV_INPUT;
                    reply_buf[CmdCtrlType] = str[CmdCtrlType];
                    reply_buf[CmdDevId] = str[CmdDevId];
                    reply_buf[CmdRoomId] = ROOM_NUM;
                    SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                        {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                        }
                   else
                    {
                          HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                     }
                  break;
         #endif
              default://û�ҵ��豸
                reply_buf[CmdType] = CMD_TYPE_QUERYRESPONSE;
                reply_buf[CmdCtrlType] = ERROR_FLAG;
                reply_buf[CmdCtrlValue]= ERROR_FLAG;
                reply_buf[CmdRoomId] = ROOM_NUM;
                reply_buf[CmdDevType] = str[CmdDevType];
                reply_buf[CmdDevId] = str[CmdDevId];
                SampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0x0000;
                    if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                           CMD_LEN,
                           reply_buf,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                      {
                           HalLedSet(LED2,LED_OFF);//�ն�led2�� ��ʾ��������ɹ�
                       }
                   else
                      {
                           HalLedSet(LED2,LED_ON);//�ն�led2�� ��ʾ��������ɹ�
                          // Error occurred in request to send.
                      }
                  break;
              }         
            }
      #else//Э���������ն˷��صĻ�Ӧ��Ϣ�����͸���λ��
      if(str[CmdType] == CMD_TYPE_QUERYRESPONSE || str[CmdType] == CMD_TYPE_CONTROLRESPONSE)
      {
        HalUARTWrite(0,str,CMD_LEN);
      }      
      #endif
    break;
    /*************************/
    case SAMPLEAPP_ADDNET_CLUSTERID://��������
      #ifdef DEV_COOR
          if(pkt->cmd.Data[0]=='m' && pkt->cmd.Data[3]=='g' && pkt->srcAddr.addr.shortAddr != 0)
          {
            HalLedSet(LED1,LED_ON);//Э����led1�� ��ʾ�ҵ�������
            shortaddr = pkt->srcAddr.addr.shortAddr;//���յ����ն˵ĵ�ַ����
            Routing_Table[pkt->cmd.Data[1]][pkt->cmd.Data[2]] = shortaddr;  //��Ӧ�ķ����豸��·�ɱ���                    
            #ifdef DEBUG_STAGE 
                     HalUARTWrite(0,&asc_16[Routing_Table[pkt->cmd.Data[1]][pkt->cmd.Data[2]]/4096],1);
                     HalUARTWrite(0,&asc_16[Routing_Table[pkt->cmd.Data[1]][pkt->cmd.Data[2]]%4096/256],1);
                     HalUARTWrite(0,&asc_16[Routing_Table[pkt->cmd.Data[1]][pkt->cmd.Data[2]]%256/16],1);
                     HalUARTWrite(0,&asc_16[Routing_Table[pkt->cmd.Data[1]][pkt->cmd.Data[2]]%16],1);
            #endif           
             buffer[0] = 'r';//���лظ�          
             buffer[1] = 0;
             buffer[2] = 0;
             buffer[3] = 'a';
             SampleApp_Point_to_Point_DstAddr.addr.shortAddr = shortaddr;//����Ӧ����Ϣ���͸��ն� �����ɹ�
             if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr, &SampleApp_epDesc,
                           SAMPLEAPP_ADDNET_CLUSTERID,
                           4,
                           buffer,
                           &SampleApp_TransID,
                           AF_DISCV_ROUTE | AF_ACK_REQUEST,
                           AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
            {
             //  HalUARTWrite(0,"get searchmsg\n\r",sizeof("get searchmsg\n\r"));      
             //  HalUARTWrite(0,"bind one end",12);
               HalLedSet(LED1,LED_OFF);//Э����led1���� ��ʾ���ҵ�����
            }
            else
            {
              // Error occurred in request to send.
            }
         }
    #else        //�ն˴�������Ӧ��
    
     if((pkt->cmd.Data[0]=='r' && pkt->cmd.Data[3]=='a') && pkt->srcAddr.addr.shortAddr == 0x0000)
     {
      #ifdef DEBUG_STAGE
        HalUARTWrite(0,"bind success",12);
      #endif
        HalLedSet(LED1,LED_ON);//�ҵ�����Ϳ���LED1
     }
     else
     {
        #ifdef DEBUG_STAGE
            HalUARTWrite(0,"bind fail",9);
        #endif
        HalLedSet(LED1,LED_OFF);//δ�ҵ�����͹ر�LED1
     }
     #endif
      break;
    case SAMPLEAPP_PERIODIC_CLUSTERID:
      break;

    case SAMPLEAPP_FLASH_CLUSTERID:
    /*  flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );*/
      break;
  }
}

/*********************************************************************
 * @fn      SampleApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_SendPeriodicMessage( void )
{
  /*
  if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_PERIODIC_CLUSTERID,
                       1,
                       (uint8*)&SampleAppPeriodicCounter,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
  */
}

/*********************************************************************
 * @fn      SampleApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */

void SampleApp_SendFlashMessage( uint16 flashTime )
{
  /*
  uint8 buffer[3];
  buffer[0] = (uint8)(SampleAppFlashCounter++);
  buffer[1] = LO_UINT16( flashTime );
  buffer[2] = HI_UINT16( flashTime );

  if ( AF_DataRequest( &SampleApp_Flash_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       3,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
  */
}


/*********************************************************************
 * @fn      SampleApp_SerialCMD
 *
 * @brief   ���ڸ������ݴ���
 *
 *
 * @return  none
 */
#ifdef DEV_COOR
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg) 
{
  uint8 dev_num,len,*str = NULL;//len���õ����ݳ���
  str = (cmdMsg->msg+1);//ָ������������
  len = *(cmdMsg->msg);//ָ������ͷ �����ж�������
  /****************����Ϊ���ڴ�ӡ���յ�������*/
  #ifdef DEBUG_STAGE
    int i;
    for(i = 0; i <= len; i++)
    HalUARTWrite(0,str+i,1);
    HalUARTWrite(0,"\n",1);
  #endif
    /******���յ�Ե�ķ���ģʽ***/
    /*
    afAddrType_t SampleApp_Point_to_Point_DstAddr;
    SampleApp_Point_to_Point_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    SampleApp_Point_to_Point_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
    ampleApp_Point_to_Point_DstAddr.addr.shortAddr = 0xffff;//���ݴ��ն˷���Э����
    */
    if(str[CmdFlag] == FRAME_FLAG && len == CMD_LEN)//�жϿ����û����ȷ ������û����ȷ
    {
      for(dev_num = 1; dev_num <= DEV_COUNT; dev_num++)
      {
        if(Routing_Table[str[CmdRoomId]][dev_num] == 0)
        {
           str[CmdRoomId] = ERROR_FLAG;
           HalUARTWrite(0,str,CMD_LEN);
           return;
        }
       SampleApp_Point_to_Point_DstAddr.addr.shortAddr = Routing_Table[str[CmdRoomId]][dev_num];
             if ( AF_DataRequest( &SampleApp_Point_to_Point_DstAddr,
                       &SampleApp_epDesc,
                       SAMPLEAPP_POINT_TO_POINT_CLUSTERID,
                       len,//���ݳ���
                       str,//��������
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
                {
                  }
             else
             {
                // Error occurred in request to send.
             }
     }
 }
}
#endif