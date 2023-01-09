/******************** (C) COPYRIGHT 2022 << SAR >> *******************************
 * File Name          : HW_UART.c
 * Author             : SAR
 * Version            : V1.01
 * Date               : 03/2022
 * Description        : ���������� �������������� ������ ���������� UART
 ********************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "HW_Globals.h"
#include "main.h"
#include "HW_EBYTE.h"
#include "HW_UART.h"
#include "HW_I2C.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* External variables --------------------------------------------------------*/
// extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1; // ����������� ������� ����� ������������� ����� ��������� ������ ��� UART1 (main.c)
extern UART_HandleTypeDef huart2; // ����������� ������� ����� ������������� ����� ��������� ������ ��� UART2 (main.c)


//* ������ �������� ������ UART1 �� ��� ���, ���� � ��� �������� ������
//* ����������� ��� ������� �������� ������
/** ��������������������������������������������������������������������������������������������������������������
  @brief    CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 - ������� �������� ������ UART1

  @param    1)u32_MillisecondsCounter:              �������� �������� � ��, ������� ����� �� ������� ������
  @param    1)u32_WaitingTimeForBufferCleanup:      ����� (� �������������) �������� ������� ������ � ����� ����� UART
                                �������� ������� ��������� ������ ���� ������ �������� ������� ���������!!!!!!

  @retval   UART1_BUFFER_EMPTY: 	          ���������� ���������� ���������
  @retval 	UART1_CLEARING_BUFFER_ERROR: 	  ����� �� ��������, ������ �� ����� �������� � ��������, ������� �� �� �����

  @example
  
    CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (600, 500);

  �������������������������������������������������������������������������������������������������������������� */
Clearing_UART1_Buffer_Return_Status CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (u32 u32_MillisecondsWaitingCounter,
                                                                                                 u32 u32_WaitingTimeForBufferCleanup)
{
  u32 u32_TickCounter, u32_WaitingTime;
  u8 u8_InputByte;

  // ����������� �������� ��������� ��������
  u32_TickCounter = HAL_GetTick() + u32_MillisecondsWaitingCounter;
  u32_WaitingTime = HAL_GetTick() + u32_WaitingTimeForBufferCleanup;

  // �������� � ����� ���� ������� ������ � ������ �����
  while (HAL_GetTick() < u32_TickCounter)
  {
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) // ���� ������ �������
    {
      HAL_UART_Receive(&huart1, &u8_InputByte, 1, TIME_OUT_OF_EMPTY_BUFFER_UART1); // ������ ������ �� ������ �����
    }
    else if (HAL_GetTick() > u32_WaitingTime)
    {
      return UART1_BUFFER_EMPTY;
    }
  }
  return UART1_CLEARING_BUFFER_ERROR;
}








//* �������� ������ ����� ���� UART1
/** ��������������������������������������������������������������������������������������������������������������
  @brief    UART1_Transmit - �������� ������ ����� ���� UART1

  @param    1)*u8_pData:      ��������� �� ������������ uint8 ������ (uint8*)
  @param    2)u16_Size:       ������ � ������ ������������ ������ (uint16)
  @param    3)u32_Timeout     ����� uint32 �������� �������� (uint32)

  @retval   HAL_OK: 	  �� � �����
  @retval 	HAL_ERROR: 	  ������
  @retval 	HAL_BUSY:  	  ������
  @retval 	HAL_TIMEOUT:  ������

  @example
  
    u8 u8_Value=123;
    UART1_Transmit (&u8_Value, 1, 500);


  �������������������������������������������������������������������������������������������������������������� */
HAL_StatusTypeDef UART1_Transmit (u8 *u8_pData, u16 u16_Size, u32 u32_Timeout)
{
	return HAL_UART_Transmit(&huart1, u8_pData, u16_Size, u32_Timeout);
}








//* �������� ������ ����� ���� UART2
/** ��������������������������������������������������������������������������������������������������������������
  @brief    UART2_Transmit - �������� ������ ����� ���� UART2

  @param    1)*u8_pData:      ��������� �� ������������ uint8 ������ (uint8*)
  @param    2)u16_Size:       ������ � ������ ������������ ������ (uint16)
  @param    3)u32_Timeout     ����� uint32 �������� �������� (uint32)

  @retval   HAL_OK: 	  �� � �����
  @retval 	HAL_ERROR: 	  ������
  @retval 	HAL_BUSY:  	  ������
  @retval 	HAL_TIMEOUT:  ������

  @example
  
    u8 u8_Value=123;
    UART2_Transmit (&u8_Value, 1, 500);


  �������������������������������������������������������������������������������������������������������������� */
HAL_StatusTypeDef UART2_Transmit (u8 *u8_pData, u16 u16_Size, u32 u32_Timeout)
{
	return HAL_UART_Transmit(&huart2, u8_pData, u16_Size, u32_Timeout);
}






//* �������� ������, �������� �� ������� ( ���������� ��������� g_s_DataFromSensor (Globals.h) ), 
//* ����� ���� UART2 � ������� ������� �������
/** ��������������������������������������������������������������������������������������������������������������
  @brief    UART2_TransmitSensorsDataToBaseStation - �������� ������ ����� ���� UART2

  ���������, ������������ ������� �������, �������� � ���� ��� ��������:
    1. ��������� - 10 ����
    2. ������, ���������� �� ������� � ������������� (� �������� ��� �������� ������� float) �� ������� ModBus - �� 100 ����
    3. ����������� ����� - 2 �����

  @param    1)u32_Timeout     ����� uint32 �������� �������� (uint32) �� ������ �� TIMEOUT

  @retval   HAL_OK: 	  �� � �����
  @retval 	HAL_ERROR: 	  ������
  @retval 	HAL_BUSY:  	  ������
  @retval 	HAL_TIMEOUT:  ������

  @example
  
    UART2_TransmitSensorsDataToBaseStation (500);

  �������������������������������������������������������������������������������������������������������������� */
HAL_StatusTypeDef UART2_TransmitSensorsDataToBaseStation (u32 u32_Timeout)
{

  //* ��� ����, ����� �������� ������ �� UART2 ������� �������,
  //* ������ ����������� (union) ������ ��� �������� �� ������� ������ � �������� �������� ������
  union //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  {
    struct ArrayForSendingToBaseStation
    {
      // 1. ��������� (���������) ��������� - 10 ����
      u8 u8_a_Preamble [10];

    // 2. ������ ���� ��������� ReceivedDataFromSensor (Globals.h)
    struct ReceivedDataFromSensor s_DataForSendingToStation;

    // 3. ��� ����� ��� �����. ����� (� ��������� ����� �� ������������, �� ������ ��� ������� ������������� ������ � �������)
    u16 u16_CRC;

    } s_PreliminarySendingData;

    // �� ���������� ������ ��������� s_PreliminarySendingData ��������� ������ � �������� ��������� ���� ���������
    u8 u8_a_SendingDataArray[sizeof(s_PreliminarySendingData.u8_a_Preamble) + 
                              sizeof(s_PreliminarySendingData.s_DataForSendingToStation) +
                                sizeof(s_PreliminarySendingData.u16_CRC) ];

  } u_DataForSending; //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


  //* 1. �������� ��� ���������� =====================================================================================================
  //* ������ ����� ���� - ����� "VECTOR"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [0] = 86; // "V" 
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [1] = 69; // "E"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [2] = 67; // "C"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [3] = 84; // "T"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [4] = 79; // "O"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [5] = 82; // "R"
  
  //* ������� ���� - ������ ��������� (� ������) ��� ����� ���������� ����� "VECTOR", �� � ������ ���� ���� ����������� �����
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [6] = sizeof(u_DataForSending.u8_a_SendingDataArray) - 6;


  //* ������� ���� - ��������������� ����� � ��������� 0...5
  u8 u8_RandonValue;
  // ��������� ��������� ��������������� ����� ��������� ������ �� ���������� �������
  srand ( HAL_GetTick() %0xFF );
  // �������� �� ���������� ��������� ����� � ��������� 0...5
  u8_RandonValue = (rand() %5);

  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [7] = u8_RandonValue;

  //* ������� ���� - ������������ �������� ������ �� ������ ����� ���� ( [0]...[5], ����� �������� ������� ��������� - � ����������� �� ����� � ������� �����)
  //* �� ������� ����. �������� ������������ ���������� �� ������ 0xFF.

  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [8] =
   (u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [u8_RandonValue] * u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [6]) %0xFF;
  
  
  //* ������� ���� - ����� ������� (0...255)
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [9] = SENSOR_NUMBER;

  //*==================================================================================================================================
  //* 2. �������� ������ �� ���������� ��������� g_s_DataFromSensor � ��������� ������������ ������  ==================================
  u_DataForSending.s_PreliminarySendingData.s_DataForSendingToStation = g_s_DataFromSensor;

  //* 3. ������������ ����������� ����� ������������� ������� ��� ���� ��������� ����, � �������, ����������, � ����� ������ ����������� �����
  // ������ ����������� ����� � ������ ������ �������� ����� ������� ������� - ������ ���� ���� ������������� ������� � �������� �� 16 ���
  // ��� ����� ������������ � ��� �����
  u_DataForSending.s_PreliminarySendingData.u16_CRC = 0;  // 2-������� ���������� ��� ������������ �����

  // ������������ ����������� ����� ���� ������ ������� ������������ ������
  u8 u8_Counter;
  u16 u16_CRC = 0;
  for (u8_Counter = 0; u8_Counter < ( sizeof(u_DataForSending.u8_a_SendingDataArray) -2 ); u8_Counter++)
  {
    u16_CRC += (u16)u_DataForSending.u8_a_SendingDataArray[u8_Counter];
  }

  // ������������ ����� ����� � ��� ��������� ����� �������, ��������������� ��� ��������
  u_DataForSending.u8_a_SendingDataArray[u8_Counter++] = (u8) (u16_CRC & 0xFF);
  u_DataForSending.u8_a_SendingDataArray[u8_Counter++] = (u8) ((u16_CRC >> 8) & 0xFF);

  // OLED1_Font_8x6_printf(8, 1, "%u %04X", u8_Counter, u16_CRC); 
  // OLED1_Font_8x6_printf(5, 0, "%u %u %u %u %3.u", sizeof (u_DataForSending.s_PreliminarySendingData.s_DataForSendingToStation), 
  //                                         sizeof (u_DataForSending.s_PreliminarySendingData.u8_a_Preamble),
  //                                         sizeof (u_DataForSending.s_PreliminarySendingData.u16_CRC),
  //                                          sizeof (u_DataForSending.u8_a_SendingDataArray),
  //                                          u8_Counter );
  // OLED1_Font_8x6_printf(5, 0, "%u ����", sizeof (u_DataForSending.u8_a_SendingDataArray)); // ���-�� ���������� ������� ������ �� �������
  // OLED1_Font_8x6_printf(5, 0, "%u ����, %04X", sizeof (u_DataForSending.u8_a_SendingDataArray), u16_CRC); // ���-�� ���������� ������� ������ �� �������


  // for (u8 i = 0; i < 5; i++)
  // {
  //   OLED1_Font_8x6_printf(1, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[10+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(2, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[15+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(3, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[20+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(4, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[25+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(5, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[30+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(6, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[35+i]); // ���-�� ���������� ������� ������ �� �������
  //   OLED1_Font_8x6_printf(7, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[40+i]); // ���-�� ���������� ������� ������ �� �������
  // }

  // ������ ���� ��������� �������� �� ������� ������ (���� �����. �����) �� "������� �������"
  return UART2_Transmit(u_DataForSending.u8_a_SendingDataArray, u8_Counter, TIME_OUT_OF_TRANSMITTER_UART2);
}









//* �������� ������ ����� ���� UART1 �� ��������� ModBus
/** ��������������������������������������������������������������������������������������������������������������
  @brief    ModBusRequestTransmitUART1 - �������� ������ ����� ���� UART1 �� ��������� ModBus

  @param    1)u8_SlaveAddress:      ����� ����������� ����������
  @param    2)u8_FunctionCode:      ����� ����� ����������
  @param    3)u8_SizeRequestedData  ���������� ������������� ���� ������ �� �����������

  @retval   HAL_OK: 	    �� � �����
  @retval 	HAL_ERROR: 	  ������
  @retval 	HAL_BUSY:  	  ������
  @retval 	HAL_TIMEOUT:  ������

  @example
  
  ModBusRequestTransmitUART1 (1, 4, 24); // ������ �� ������ 24 ���� �� ���������� � ������� 1 �� ��� ����� ��������� �4
  �������������������������������������������������������������������������������������������������������������� */
HAL_StatusTypeDef ModBusRequestTransmitUART1 (u8 u8_SlaveAddress, u8 u8_FunctionCode, u8 u8_SizeRequestedData)
{
  u8 u8_a_ArrayForTransmit [8] = {0}; // ������ ������
  
  union
  {// ��������� �� ������ ������ ����������� ������ � ����������� ���������� ��� ����������� ������������� ������ CRC
    struct 
    {
      u8 u8_u_a_ArrayCRC[2];
    };
    u16 u16_u_CRC;
  } u_crc;

  // ��������� ������ � ���������������� � �������� ������
  u8_a_ArrayForTransmit [0] = u8_SlaveAddress;  // ����� �����������
  u8_a_ArrayForTransmit [1] = u8_FunctionCode;  // ����� ����� ����������
  u8_a_ArrayForTransmit [2] = 0;                // �������� �����
  u8_a_ArrayForTransmit [3] = 0;
  u8_a_ArrayForTransmit [4] = 0;
  u8_a_ArrayForTransmit [5] = u8_SizeRequestedData;   // ���������� ������������� ����

  // ��������� ����������� ����� ������ ����� [0...5] ���� ������� ������� � ����� ��������� � ���������� ����������
  u_crc.u16_u_CRC = ModBusChecksumCalculation (u8_a_ArrayForTransmit, 6);

  // ��������� �������� ����������� ����� �� ������������ ������� � �������� �� � ��� ��������� ����� ������� �������
  u8_a_ArrayForTransmit [6] = u_crc.u8_u_a_ArrayCRC [0];   // CRC ��. ����
  u8_a_ArrayForTransmit [7] = u_crc.u8_u_a_ArrayCRC [1];   // CRC ��. ����

  // OLED1_Font_8x6_printf(7, 1, "%2X %2X",  u8_a_ArrayForTransmit [6], u8_a_ArrayForTransmit [7]);

  // ���������� �������������� ������ �� ���������� UART1

	return HAL_UART_Transmit(&huart1, u8_a_ArrayForTransmit, 8, TIME_OUT_OF_TRANSMITTER_UART1);
}









//* �������� ������� ����� ���� UART1 �� ��������� ModBus, ������� 16 (0x10) Write Multiple registers
/** ��������������������������������������������������������������������������������������������������������������
  @brief    ModBusCommandTransmitUART1 - �������� ������� ����� ���� UART1 �� ��������� ModBus,
                                            ������� 16 (0x10) Write Multiple registers

  @param    1)u8_SlaveAddress               ����� ����������� ����������
  @param    2)u16_AddressOfFirstReg         ����� ������� 16-������� �������� (Starting Address, 0x0000 to 0xFFFF)
  @param    3)u16_QuantityOfRegisters       ���������� ����������� 16-������ ��������� (0x0001 to 0x007B)
  @param    4)u8_NumberOfBytes              ���������� ������������ � ������ ���� (2 x N*)
  @param    5)u16* p_u16_TransmittedData    ������ �� ������ ������������ ������ (N* x 2 Bytes)
                                                  ��� *N = ���������� ���������

  @retval   HAL_OK: 	    �� � �����
  @retval 	HAL_ERROR: 	  ������
  @retval 	HAL_BUSY:  	  ������
  @retval 	HAL_TIMEOUT:  ������

  @example
  
  �������������������������������������������������������������������������������������������������������������� */
HAL_StatusTypeDef ModBusCommandTransmitUART1 (u8 u8_SlaveAddress, u16 u16_AddressOfFirstReg,
                                                 u16 u16_QuantityOfRegisters, u8 u8_NumberOfBytes,  u16* p_u16_TransmittedData)
{
  u8 u8_a_ArrayForTransmit [SIZE_OF_TRANSMIT_BUFFER_UART1] = {0}; // ������ ������
  u8 u8_ByteCount=0;

  union // ��� �������������� 16-������ ���������� � ��� �������� 8-������� ������� ���������� union
  {// ��������� �� ������ ������ ����������� ������ � ����������� ���������� ��� ����������� ������������� ������
    struct 
    {
      u8 u8_a_ArrayBYTE[2];
    };
    u16 u16_WORD;
  } data;

  // �������� ������� ������
  if (u16_QuantityOfRegisters > 128)
  {
    return HAL_ERROR;
  }
  
  // ������ ������������������ ������� 0x10: 01 10 00 BE 00 02 04 00 00 00 01 B8 97, ���
  // 01 �����
  // 10 �������
  // 00 BE ��������� �������
  // 00 02 ������� ��������� ���������� ��������
  // 04    ���-�� ���� ��� ������
  // 00 00 �������� ��� ������� ��������
  // 00 01 �������� ��� ������� ��������
  // B8 97 ����������� �����


  // ��������� ������ � ���������������� � �������� ������
  u8_a_ArrayForTransmit [u8_ByteCount++] = u8_SlaveAddress;  // ����� �����������
  u8_a_ArrayForTransmit [u8_ByteCount++] = 0x10;             // ��� �������

  data.u16_WORD = u16_AddressOfFirstReg;        // ����� ������� ������������� �������� 
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];               
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];

  data.u16_WORD = u16_QuantityOfRegisters;        // ���������� ����������� ��������� 
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];               
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];

  u8_a_ArrayForTransmit [u8_ByteCount++] = u8_NumberOfBytes;   // ���������� ������������ ����

  // ��������� ������� ������ � ������ ��� �������� ����������� ����������
  u16 u16_Count;
  for (u16_Count = 0; u16_Count < u16_QuantityOfRegisters; u16_Count++)
  {
    data.u16_WORD = *(p_u16_TransmittedData + u16_Count);
    u8_a_ArrayForTransmit [u8_ByteCount++]  = data.u8_a_ArrayBYTE[1];               
    u8_a_ArrayForTransmit [u8_ByteCount++]  = data.u8_a_ArrayBYTE[0];
  }
  
  // ��������� ����������� ����� ������� ������� � ����� ��������� � ���������� ����������
  data.u16_WORD = ModBusChecksumCalculation (u8_a_ArrayForTransmit, u8_ByteCount);

  // ��������� �������� ����������� ����� �� ������������ ������� � �������� �� � ��� ��������� ����� ������� �������
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];   // CRC ��. ����
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];   // CRC ��. ����

  // u8 u8_Count=0;
  // for (u8 u8_Cnt = 0; u8_Cnt < 6; u8_Cnt++)
  // {
  //   OLED1_Font_8x6_printf(1, u8_Cnt*21, "%02X", u8_a_ArrayForTransmit [u8_Count++]);  
  // }
  
  // for (u8 u8_Cnt = 0; u8_Cnt < 6; u8_Cnt++)
  // {
  //   OLED1_Font_8x6_printf(3, u8_Cnt*21, "%02X", u8_a_ArrayForTransmit [u8_Count++]);  
  // }

  // for (u8 u8_Cnt = 0; u8_Cnt < 6; u8_Cnt++)
  // {
  //   OLED1_Font_8x6_printf(5, u8_Cnt*21, "%02X", u8_a_ArrayForTransmit [u8_Count++]);  
  // }

  // ���������� �������������� ������ �� ���������� UART1
	return HAL_UART_Transmit(&huart1, u8_a_ArrayForTransmit, u8_ByteCount, TIME_OUT_OF_TRANSMITTER_UART1);
}







//* ���� ������ ������ UART1 �� ��������� ModBus �� �������
/** ��������������������������������������������������������������������������������������������������������������
  @brief    ModBusReceivingDataFromSensorByUART1 - ���� ������ �� ������� ���� ���1012U-Ex-�-0 ����� ���� UART1 �� ��������� ModBus.
                        ������������� �������� ������ � ��������� ReceivedDataFromSensor (HW_Globals.h)

  @retval   UART_RECEIVING_OK							  ���������� ������� ���������� ���������, ������ �������, CRC ���������
  @retval   UART_RECEIVING_CRC_ERROR        �����-�� ������ �������, �� �� ���������, �.�. ������� ������ ����������� �����
  @retval   UART_RECEIVING_TIME_OUT_ERROR		������ ��� ������ �� ������, ��� ������ ��������, �� ���� �����������
  �������������������������������������������������������������������������������������������������������������� */
ModBus_Receiving_Return_Status ModBusReceivingDataFromSensorByUART1 ()
{
  u8 u8_ArrayForReadFromSensor [SIZE_OF_RECEIVE_BUFFER_UART1] = {0}; // ������ ������ ��� �����
  u8 u8_ByteCounter;    // ���������� �������� �������� ����
  u32 u32_TickCounter;  // ���������� ������� ������� (��) ��������
  huart1.Instance = USART1;

  // ���� ������ ������ ������ �� ����������� ����������
  for (u8_ByteCounter = 0; u8_ByteCounter < SIZE_OF_RECEIVE_BUFFER_UART1; )
  {
    // ����������� �������� �������� ��������� �������� ����� ����� �� �������
    u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_RECEIVER_BYTE_UART1;

    // �������� � ����� ���� �� ������� ���� ���� � ����� �����
    while (HAL_GetTick() < u32_TickCounter)
    {
      // ��� ��������� ��������� ����� (��������� ����� ��������� ������), ....
      if (__HAL_UART_GET_FLAG (&huart1, UART_FLAG_RXNE))
      {
        // ... ��� ����� ������ ���� ���� � ������ �������� ������
        HAL_UART_Receive(&huart1, &u8_ArrayForReadFromSensor [u8_ByteCounter++], 1, 500); // ������
        break;
      }
    }

    // ���� ����� � ����� ������ �� ��������� (����� �������� ����� �����), �� �������� ������ �������� ������
    if (HAL_GetTick() >= u32_TickCounter)
    {
      // ���� �� ������� �� ������ �����, �����
      if (0 == u8_ByteCounter)
      {
        return UART_RECEIVING_TIME_OUT_ERROR;
      }
      
      union   // ������� ����������� ��� ������������� ���� ���� (��. � ��.) ���������� ����������� �����
      {
        u16 u16_CalculatedCRC;
        struct
        {
          u8 u8_Byte0;
          u8 u8_Byte1;
        };
      } crc;
                                                      

      // ���������� CRC
      crc.u16_CalculatedCRC = ModBusChecksumCalculation (u8_ArrayForReadFromSensor, u8_ByteCounter-2);

      // *��������������������������������������������������������������������������������������������������������������
      // *��������������������������������������������������������������������������������������������������������������
      // ���� ����������� � ��������� ����������� ����� ���������,
      // ������������ ������ �� �������� ������� u8_ArrayForReadFromSensor � ���������� ������ ������ g_s_DataFromSensor
      if (u8_ArrayForReadFromSensor [u8_ByteCounter-2] == crc.u8_Byte0 && u8_ArrayForReadFromSensor [u8_ByteCounter-1] == crc.u8_Byte1)
      {
        //* ����� ��������� ������ ����������� ����������
        g_s_DataFromSensor.u8_AddressOfSlaveDevice = u8_ArrayForReadFromSensor [ADDRESS_ADDRESS_SLAVE_DEVICE];

        //* ����� ����� �������� ���������
        g_s_DataFromSensor.u8_FunctionalCode = u8_ArrayForReadFromSensor [ADDRESS_FUNCTIONAL_CODE];

        //* ���������� �������� �� ����������� ���������� ����
        g_s_DataFromSensor.u8_NumberOfBytesReceived = u8_ByteCounter;

        //* ��������� ����������� �1
        g_s_DataFromSensor.u8_StatusOfPositioner_1 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_1];

        //* ��������� ����������� �2
        g_s_DataFromSensor.u8_StatusOfPositioner_2 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_2];

        //* ������ ��
        g_s_DataFromSensor.u8_SoftwareVersion = u8_ArrayForReadFromSensor [ADDRESS_SOFTWARE_VERSION];

        //* ������ ��
        g_s_DataFromSensor.u8_AO_Version = u8_ArrayForReadFromSensor [ADDRESS_AO_VERSION];

        //* ����������� ���������� �����
        g_s_DataFromSensor.u8_AmbientTemperature = u8_ArrayForReadFromSensor [ADDRESS_AMBIENT_TEMPERATURE];


        // ������ ���������� (����� ��������� ��������)
        // �������� ��� �����, ����� ��� ����� �������� ������� � 16-������ ����������
        union 
        {
          struct 
          {
            u8 u8_ArrayData[2];
          };
          u16 u16_Data;
        } status;

        //* ���������� ��� ��������� ����� � ����������� �����
        status.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE];
        status.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE+1];
        
        // ����� ���������� �������� ��������� � ���������� ������
        g_s_DataFromSensor.u16_DeviceStatus = status.u16_Data;
        // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%u", g_s_DataFromSensor.u16_DeviceStatus );


        // �.�. � ��������� ModBus ������ float ���������� � ������������� ���������� ����,...
        union // ...����������� float ModBud (C-D-A-B) � ����������� float (A-B-C-D) � ������� ��������� �������
        {
          struct 
          {
            u8 u8_ArrayData[4];
          };
          float f_Data;
        } data;


        //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �1
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+2];

        // ����� ���������� �������� ����������� ������ ����������� �1 � ���������� ������
        g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 = data.f_Data;
        // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 );



        //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �2
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+2];

        // ����� ���������� �������� ����������� ������ ����������� �2 � ���������� ������
        g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 = data.f_Data;
        // OLED1_Font_16x12_printf (3, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 );



        //* ������ ���������� ���������� ���� � ����������� float ������ ��� �������� �����
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+2];

        // ����� ���������� �������� ����������� ������ �������� ����� � ���������� ������
        g_s_DataFromSensor.f_SpeedOfSound = data.f_Data;
        // OLED1_Font_16x12_printf (5, 1, NORMAL, COMPACT, "%.2f", g_s_DataFromSensor.f_SpeedOfSound );


        // //* ������ ���������� ���������� ���� � ����������� float ������ ��� �����������
        // data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+1];
        // data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE];
        // data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+3];
        // data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+2];

        // // ����� ���������� �������� ����������� ������ ����������� � ���������� ������
        // g_s_DataFromSensor.f_AmbientTemperature = data.f_Data;
        // // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%e", g_s_DataFromSensor.f_AmbientTemperature );


        //* ������ ���������� ���������� ���� � ����������� float ������ ��� ��������� ��������� ��������
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+2];

        // ����� ���������� �������� ����������� ������ ��������� ��������� �������� � ���������� ������
        g_s_DataFromSensor.f_OutputPulseAmplitude = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.4f", g_s_DataFromSensor.f_OutputPulseAmplitude );


        //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �������������� ��������
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+2];

        // ����� ���������� �������� ����������� ������ ����������� �������������� �������� � ���������� ������
        g_s_DataFromSensor.f_TemperatureOfMeasuringElement = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%f", g_s_DataFromSensor.f_TemperatureOfMeasuringElement );


        //* ������ ���������� ���������� ���� � ����������� float ������ ��� �������� ���������
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+2];

        // ����� ���������� �������� �������� ��������� � ���������� ������
        g_s_DataFromSensor.f_MeasuringRange = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuringRange );

        return UART_RECEIVING_OK;
      // *��������������������������������������������������������������������������������������������������������������
      // *��������������������������������������������������������������������������������������������������������������
      }
      else
      {
        // ���� ����������� ����� �� ���������, �� ������� � ���������� �� ���� ��������
        return UART_RECEIVING_CRC_ERROR;
        // OLED1_Font_8x6_printf(8, 90, "ERROR ");
      }
    }
  }
  return UART_RECEIVING_TIME_OUT_ERROR;
}



//* ���� ������ ������ UART2 �� ������� �������
/** ��������������������������������������������������������������������������������������������������������������
  @brief    ReceivingDataFromBaseStationByUART2 - ���� ������ �� ������� ������� ����� ���� UART2
                        ������������� �������� ������ � ��������� ReceivedDataFromSensor (HW_Globals.h)

  @retval   UART_RECEIVING_OK							  ���������� ������� ���������� ���������, ������ �������, CRC ���������
  @retval   UART_RECEIVING_CRC_ERROR        �����-�� ������ �������, �� �� ���������, �.�. ������� ������ ����������� �����
  @retval   UART_RECEIVING_TIME_OUT_ERROR		������ ��� ������ �� ������, ��� ������ ��������, �� ���� �����������
  �������������������������������������������������������������������������������������������������������������� */
ModBus_Receiving_Return_Status ModBusReceivingDataFromSensorByUART2 ()
{
  u8 u8_ArrayForReadFromSensor [SIZE_OF_RECEIVE_BUFFER_UART2] = {0}; // ������ ������ ��� �����
  u8 u8_ByteCounter;    // ���������� �������� �������� ����
  u32 u32_TickCounter;  // ���������� ������� ������� (��) ��������
  huart2.Instance = USART2;

  // ���� ������ ������ ������ �� ����������� ����������
  for (u8_ByteCounter = 0; u8_ByteCounter < SIZE_OF_RECEIVE_BUFFER_UART2; )
  {
    // ����������� �������� �������� ��������� �������� ����� ����� �� �������
    u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_RECEIVER_BYTE_UART2;

    // �������� � ����� ���� �� ������� ���� ���� � ����� �����
    while (HAL_GetTick() < u32_TickCounter)
    {
      // ��� ��������� ��������� ����� (��������� ����� ��������� ������), ....
      if (__HAL_UART_GET_FLAG (&huart2, UART_FLAG_RXNE))
      {
        // ... ��� ����� ������ ���� ���� � ������ �������� ������
        HAL_UART_Receive(&huart2, &u8_ArrayForReadFromSensor [u8_ByteCounter++], 1, 500); // ������
        break;
      }
    }

    // ���� ����� � ����� ������ �� ��������� (����� �������� ����� �����), �� �������� ������ �������� ������
    if (HAL_GetTick() >= u32_TickCounter)
    {
      // ���� �� ������� �� ������ �����, �����
      if (0 == u8_ByteCounter)
      {
        return UART_RECEIVING_TIME_OUT_ERROR;
      }
      
      union   // ������� ����������� (union) ��� ������������� ���� ���� (��. � ��.) ���������� ����������� �����
      {
        u16 u16_CRC;
        struct
        {
          u8 u8_Byte0;
          u8 u8_Byte1;
        };
      } crc;


        // ������������ ����������� ����� ������� (������ �� �������) ��� ���� ��������� ����, � �������, ����������, � ����� ������ ����������� �����
        // ������ ����������� ����� � ������ ������ �������� ����� ������� ������� - ������ ���� ���� ������������� ������� � �������� �� 16 ���
        // ��� ����� ������������ � ��� �����
        crc.u16_CRC=0;  // 2-������� ���������� ��� ������������ �����

        // ������������ ����������� ����� ������ ���� ������ ������� �������� ������
        u8 u8_Counter;
        for (u8_Counter = 0; u8_Counter < (u8_ByteCounter-2); u8_Counter++)
        {
          crc.u16_CRC += (u16)u8_ArrayForReadFromSensor [u8_Counter];
        }
        
        // ������������ ����� ����� � ��� u8 ����������
        crc.u8_Byte0 = (u8) (crc.u16_CRC & 0xFF);
        crc.u8_Byte1 = (u8) ((crc.u16_CRC >> 8) & 0xFF);

        //OLED1_Font_8x6_printf(8, 70, "%u", u8_Counter); // ���-�� ���������� ������� ������ �� �������

        OLED1_Font_8x6_printf(5, 0, "%u ����, %04X", u8_Counter, crc.u16_CRC); // ���-�� ���������� ������� ������ �� �������


      // *��������������������������������������������������������������������������������������������������������������
      // *��������������������������������������������������������������������������������������������������������������
      // ���� ����������� � ��������� ����������� ����� ���������,
      // ������������ ������ �� �������� ������� u8_ArrayForReadFromSensor � ���������� ������ ������ g_s_DataFromSensor
      // if (u8_ArrayForReadFromSensor [u8_ByteCounter-2] == crc.u8_Byte0 && u8_ArrayForReadFromSensor [u8_ByteCounter-1] == crc.u8_Byte1)
      // {
      //   //* ����� ��������� ������ ����������� ����������
      //   g_s_DataFromSensor.u8_AddressOfSlaveDevice = u8_ArrayForReadFromSensor [ADDRESS_ADDRESS_SLAVE_DEVICE];

      //   //* ����� ����� �������� ���������
      //   g_s_DataFromSensor.u8_FunctionalCode = u8_ArrayForReadFromSensor [ADDRESS_FUNCTIONAL_CODE];

      //   //* ���������� �������� �� ����������� ���������� ����
      //   g_s_DataFromSensor.u8_NumberOfBytesReceived = u8_ByteCounter;

      //   //* ��������� ����������� �1
      //   g_s_DataFromSensor.u8_StatusOfPositioner_1 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_1];

      //   //* ��������� ����������� �2
      //   g_s_DataFromSensor.u8_StatusOfPositioner_2 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_2];

      //   //* ������ ��
      //   g_s_DataFromSensor.u8_SoftwareVersion = u8_ArrayForReadFromSensor [ADDRESS_SOFTWARE_VERSION];

      //   //* ������ ��
      //   g_s_DataFromSensor.u8_AO_Version = u8_ArrayForReadFromSensor [ADDRESS_AO_VERSION];

      //   //* ����������� ���������� �����
      //   g_s_DataFromSensor.u8_AmbientTemperature = u8_ArrayForReadFromSensor [ADDRESS_AMBIENT_TEMPERATURE];


      //   // ������ ���������� (����� ��������� ��������)
      //   // �������� ��� �����, ����� ��� ����� �������� ������� � 16-������ ����������
      //   union 
      //   {
      //     struct 
      //     {
      //       u8 u8_ArrayData[2];
      //     };
      //     u16 u16_Data;
      //   } status;

      //   //* ���������� ��� ��������� ����� � ����������� �����
      //   status.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE];
      //   status.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE+1];
        
      //   // ����� ���������� �������� ��������� � ���������� ������
      //   g_s_DataFromSensor.u16_DeviceStatus = status.u16_Data;
      //   // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%u", g_s_DataFromSensor.u16_DeviceStatus );


      //   // �.�. � ��������� ModBus ������ float ���������� � ������������� ���������� ����,...
      //   union // ...����������� float ModBud (C-D-A-B) � ����������� float (A-B-C-D) � ������� ��������� �������
      //   {
      //     struct 
      //     {
      //       u8 u8_ArrayData[4];
      //     };
      //     float f_Data;
      //   } data;


      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �1
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+2];

      //   // ����� ���������� �������� ����������� ������ ����������� �1 � ���������� ������
      //   g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 = data.f_Data;
      //   // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 );



      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �2
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+2];

      //   // ����� ���������� �������� ����������� ������ ����������� �2 � ���������� ������
      //   g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 = data.f_Data;
      //   // OLED1_Font_16x12_printf (3, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 );



      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� �������� �����
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+2];

      //   // ����� ���������� �������� ����������� ������ �������� ����� � ���������� ������
      //   g_s_DataFromSensor.f_SpeedOfSound = data.f_Data;
      //   // OLED1_Font_16x12_printf (5, 1, NORMAL, COMPACT, "%.2f", g_s_DataFromSensor.f_SpeedOfSound );


      //   // //* ������ ���������� ���������� ���� � ����������� float ������ ��� �����������
      //   // data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+1];
      //   // data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE];
      //   // data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+3];
      //   // data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+2];

      //   // // ����� ���������� �������� ����������� ������ ����������� � ���������� ������
      //   // g_s_DataFromSensor.f_AmbientTemperature = data.f_Data;
      //   // // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%e", g_s_DataFromSensor.f_AmbientTemperature );


      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� ��������� ��������� ��������
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+2];

      //   // ����� ���������� �������� ����������� ������ ��������� ��������� �������� � ���������� ������
      //   g_s_DataFromSensor.f_OutputPulseAmplitude = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.4f", g_s_DataFromSensor.f_OutputPulseAmplitude );


      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� ����������� �������������� ��������
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+2];

      //   // ����� ���������� �������� ����������� ������ ����������� �������������� �������� � ���������� ������
      //   g_s_DataFromSensor.f_TemperatureOfMeasuringElement = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%f", g_s_DataFromSensor.f_TemperatureOfMeasuringElement );


      //   //* ������ ���������� ���������� ���� � ����������� float ������ ��� �������� ���������
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+2];

      //   // ����� ���������� �������� �������� ��������� � ���������� ������
      //   g_s_DataFromSensor.f_MeasuringRange = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuringRange );

      //   return UART_RECEIVING_OK;
      // // *��������������������������������������������������������������������������������������������������������������
      // // *��������������������������������������������������������������������������������������������������������������
      // }
      // else
      // {
      //   // ���� ����������� ����� �� ���������, �� ������� � ���������� �� ���� ��������
      //   return UART_RECEIVING_CRC_ERROR;
      //   // OLED1_Font_8x6_printf(8, 90, "ERROR ");
      // }
    }
  }
  return UART_RECEIVING_TIME_OUT_ERROR;
}


//* ������� ����������� ����� ModBus
/** ��������������������������������������������������������������������������������������������������������������
  @brief    ModBusChecksumCalculation - ������� ����������� ����� ������� ������ ModBus

  @param    1)*p_u8_InputData:      ��������� �� ������ ������ (uint8*)
  @param    2)u8_LengthInputData:   ���������� ���� � ������� (uint8)

  @retval   ����������� ����� (u16), 

  @example
  OLED1_Font_8x6_printf(8, 1, "%4X", ModBusChecksumCalculation (u8_ArrayForReadFromSensor, u8_ByteCounter-2));
����������������������������������������������������������������������������������������������������������������� */
u16 ModBusChecksumCalculation ( u8* p_u8_InputData, u8 u8_LengthInputData )
{
  register u16 j;
  register u16 reg_crc = 0xFFFF;
  while (u8_LengthInputData--)
  {
    reg_crc ^= *p_u8_InputData++;
    for(j=0;j<8;j++)
    {
    if(reg_crc & 0x01)
        {
        reg_crc = (reg_crc >> 1) ^ 0xA001;
        }
    else
        {
        reg_crc = reg_crc >> 1;
      }
    }
  }
  return reg_crc;
}





/* ��������������������������������������������������������������������������������������������������������������
  Function Name  |  Read_UART_BufferPointer
�����������������+����������������������������������������������������������������������������������������������
  Description    | � ������ ����� ������, �������� �� UART, ��������� ��������� UART_ReceiveVALs (��. HW_UART.h)
�����������������+����������������������������������������������������������������������������������������������
  Arguments      | 1) ��������� ����� (UART0 ��� UART1)
�����������������+����������������������������������������������������������������������������������������������
  Return         | ������������ ��������� UART_ReceiveVALs:
                 |
                 | // ����� �������� �������� ������ ������
                 | u8* pu8_UART_ReceiveRingBufferAddress -
                 |
                 | // ����� �������� � ������ ������, ���������� �� ��������� ��������
                 | // (��������, ���� ������ ���������� ��������� 12, �� UART_ReceiveRingBufferLastIndex=13).
                 | u16 u16_UART_ReceiveRingBufferLastIndex;
                 |
                 | // ������ ���������� ��������� ������
                 | u16 u16_SizeUartReceiveRingBuffer;
                 |
                 | -------------- ������ ���������� ������� (��� UART0): ---------------------------------------
                 |
                 | struct UART_ReceiveVALs STR; // ������������� ��������� � ������ STR �� ��������� UART_ReceiveVALs
                 | u8 u8_Cnt;                   // ��������� ���������� ��� ����������� �����
                 |
                 | // ���������, ������ �� ���-���� � ����� UART0
                 | if (BUFFER_NOT_EMPTY == Check_UART_Buffer (UART0))
                 | {
                 |   // ��������� ������ �� ������, �������� �� UART0, � ��������� STR
                 |   STR = Read_UART_BufferPointer (UART0);
                 |
                 |   // ������� ������ �� ���
                 |   // ���� �� ���� �� ���������� ��������� ��������
                 |   for (u8_Cnt=0; u8_Cnt < STR.u16_UART_ReceiveRingBufferLastIndex; u8_Cnt++)
                 |   {
                 |     // ����� �� ������+�������� �������� ���� � ������ � ������ ������
                 |     sprintf (caString, "%u", *(STR.pu8_UART_ReceiveRingBufferAddress+u8_Cnt));
                 |     lcd_print (u8_Cnt, 0, caString);
                 |   }
                 |
                 |   // ����� ����� ����������� ������ �� UART0 (����������� !!!!!!!)
                 |   g_UART0_BufferEmptyFlag = BUFFER_EMPTY;
                 | }
                 |
���������������������������������������������������������������������������������������������������������������*/
// struct UART_ReceiveVALs Read_UART_BufferPointer (UART_TypeDef* UARTx)
// {
//   struct UART_ReceiveVALs ReturnData={0};

//   if (UART0 == UARTx)
//   {
//     // ���������� ��������� �� ��������� ������� ������
//     ReturnData.pu8_UART_ReceiveRingBufferAddress = &g_u8_UART0_ReceiveRingBuffer[0];
//     // ���������� ����� ������� ������� (�� ������������ ��� ������) ��������
//     ReturnData.u16_UART_ReceiveRingBufferLastIndex = g_u16_UART0_ReceiveRingBufferIndex;
//     // ���������� ������ ������ � ������
//     ReturnData.u16_SizeUartReceiveRingBuffer = (u16) SIZE_RECEIVE_RING_BUFFER_UART0;
//   }
//   else
//   {
//     // ���������� ��������� �� ��������� ������� ������
//     ReturnData.pu8_UART_ReceiveRingBufferAddress = &g_u8_UART1_ReceiveRingBuffer[0];
//     // ���������� ����� ������� ������� (�� ������������ ��� ������) ��������
//     ReturnData.u16_UART_ReceiveRingBufferLastIndex = g_u16_UART1_ReceiveRingBufferIndex;
//     // ���������� ������ ������ � ������
//     ReturnData.u16_SizeUartReceiveRingBuffer = (u16) SIZE_RECEIVE_RING_BUFFER_UART1;
//   }

// 	return ReturnData;
// }

// /*��������������������������������������������������������������������������������������������������������������
//   Function Name  |  Check_UART_Buffer
// �����������������+����������������������������������������������������������������������������������������������
//   Description    | � ���������, ���� �� ����� ������, ��������� �� UART
// �����������������+����������������������������������������������������������������������������������������������
//   Arguments      | 1) ��������� ����� (UART0 ��� UART1)
// �����������������+����������������������������������������������������������������������������������������������
//   Return         | 1) BUFFER_EMPTY - ����� ���������� � ������ ���
//                  | 2) BUFFER_NOT_EMPTY - ���� ����� ���������� � ������
// ���������������������������������������������������������������������������������������������������������������*/
// UART_Buffer_Empty_Flag Check_UART_Buffer (UART_TypeDef* UARTx)
// {
//   if (UART0 == UARTx)
//     return g_UART0_BufferEmptyFlag;
//   else
//     return g_UART1_BufferEmptyFlag;
// }


/*********************************** END OF FILE ************************************/
