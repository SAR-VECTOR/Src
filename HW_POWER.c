/******************** (C) COPYRIGHT 2022 << VECTOR >> ********************
 * File Name          : HW_POWER.c
 * Author             : Electronic Devices Application Team
 * Version            : V1.01
 * Date               : 11/2022
 * Description        : ���������� �������������� ���������� ����� �������
 ********************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "HW_Globals.h"
#include "HW_POWER.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/








/** ��������������������������������������������������������������������������������������������������������������
  @brief    PowerSwitch - ������������� ������� ����� �����

  @param    1)PowerCircuit:             ������������� ���� �������
                      RTC_POWER				  ������� ���������� �����
	                    FRAM_POWER				������� ���������� FRAM
	                    RF_MODULE_POWER		������� ����������� LoRa

  @param    2)PowerStatus:              ��������� ������� ���� ����� ������ ������������
                      POWER_ON				          ��������� �������
                        * ��������������� � "�������" ��� ������� �����, �� ������� ������ �������
	                    POWER_OFF				          ���������� �������
                        * ��������������� � "����" ��� ������� �����, �� ������� ������ �������

  @retval   void

  @example
    // ��������� ������� FRAM
    PowerSwitch (FRAM_POWER, POWER_ON);
  �������������������������������������������������������������������������������������������������������������� */
void PowerSwitch (Switched_power_supply_circuit PowerCircuit, Power_supply_circuit_status_after_switching PowerStatus)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  UART_HandleTypeDef huart2;

  switch (PowerStatus)
  {
    case POWER_ON:
      switch (PowerCircuit)
      {
        case RTC_POWER:
          HAL_GPIO_WritePin(DRV_RTC_GPIO_Port, DRV_RTC_Pin, LOW);
          g_u8_PowerLines |= RTC_POWER_FLAG;
        return;
      
        case FRAM_POWER:
          HAL_GPIO_WritePin(DRV_FRAM_GPIO_Port, DRV_FRAM_Pin, LOW);
          g_u8_PowerLines |= FRAM_POWER_FLAG;

        return;

        case RF_MODULE_POWER:
          HAL_GPIO_WritePin(DRV_RF_GPIO_Port, DRV_RF_Pin, LOW);
          g_u8_PowerLines |= RF_MODULE_POWER_FLAG;
          HAL_Delay(5);

          // �������������� ����� UART2 ��� Rx � Tx (�� ����� ��� ���� ������������������� ��� ����� ������, ����� �� ���������� RF-������)
          huart2.Instance = USART2;
          HAL_UART_MspInit(&huart2);
        return;
      }

    case POWER_OFF:
      switch (PowerCircuit)
      {
        case RTC_POWER:
          HAL_GPIO_WritePin(DRV_RTC_GPIO_Port, DRV_RTC_Pin, HIGH);
          g_u8_PowerLines &= ~RTC_POWER_FLAG;
        return;
      
        case FRAM_POWER:
          // CS2: "1" --> "0" // ������������� CS2 � "0", ����� � ���������� �� ���������� �� ���� ����� FRAM
          HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, LOW);
          // ��������� ������� FRAM
          HAL_GPIO_WritePin(DRV_FRAM_GPIO_Port, DRV_FRAM_Pin, HIGH);
          g_u8_PowerLines &= ~FRAM_POWER_FLAG;
        return;

        case RF_MODULE_POWER:
          // ���� UART2 ��������� � �����, ����� ����� ���������� ������� �� ���������� RF-������
          
          GPIO_InitStruct.Pin = UART2_Rx_Pin;
          GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull = GPIO_PULLDOWN;
          HAL_GPIO_Init(UART2_GPIO_Port, &GPIO_InitStruct);


          HAL_GPIO_WritePin(UART2_GPIO_Port, UART2_Tx_Pin, LOW);

          GPIO_InitStruct.Pin = UART2_Tx_Pin;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Pull = GPIO_PULLDOWN;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          HAL_GPIO_Init(UART2_GPIO_Port, &GPIO_InitStruct);

          // ��������� RF-������
          HAL_GPIO_WritePin(DRV_RF_GPIO_Port, DRV_RF_Pin, HIGH);
          g_u8_PowerLines &= ~RF_MODULE_POWER_FLAG;
        return;
      }
  } // ------------ switch (PowerStatus) -----------------------------
}



/** ��������������������������������������������������������������������������������������������������������������
  @brief    SettingWakeUpTime - ��������� ������� "�����������"

  @param    

  @retval   void

  @example

  �������������������������������������������������������������������������������������������������������������� */
// HAL_StatusTypeDef SetWakeUpTime(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)


// HAL_RTCEx_SetWakeUpTimer_IT




/** ��������������������������������������������������������������������������������������������������������������
  @brief    GoToSTANDBYmode - ���� � "���" (����� ����������� ����������������� STANDBY)

  @param    
  @retval   void
  @example

  �������������������������������������������������������������������������������������������������������������� */
void GoToSTANDBYmode ()
{

  // ��������� ������� ���� ������� ���������, ���� � ��, �����, ����� ������� ���������
  PowerSwitch (FRAM_POWER, POWER_OFF);
  PowerSwitch (RF_MODULE_POWER, POWER_OFF);

  // ����� ����� "���" � "�����������"
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); // ���������� ���� "�����������", ����� ��������� "��������", � �� ���� � ������������


  RTC_HandleTypeDef hrtc;
  hrtc.Instance = RTC;

  // ���������� ���� ���������� "�", ����� ��������� ��� ��������� � ��������� ���
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

  // ������ � "���"
  HAL_PWR_EnterSTANDBYMode();
}




/** ��������������������������������������������������������������������������������������������������������������
  @brief    WhatWasStateOfSupplyVoltage - ��������� �������� �� ������� ������������ �������

  @param    

  @retval   
      SUPPLY_VOLTAGE_WAS_INTERRUPTED  - ���� ������� � ����������� ������� ��� ��� ����� ������ ���������
      POWER_SUPPLY_WAS_OK             - �������� ������� �� ���� ��� ���������� ���� �� "���"

  @example

  // ���������, ����������� �� �������
  if ( POWER_SUPPLY_WAS_OK == WhatWasStateOfSupplyVoltage () ) // ���� ������� �� �����������, ��...
  {
    // ...���������� ����������� RTC-���������� � ������������ ����� �...
    HAL_RTC_MspInit(&hrtc);
  }  


  �������������������������������������������������������������������������������������������������������������� */
Continuity_Of_Supply_Voltage WhatWasStateOfSupplyVoltage ()
{
  if ( 0 == (__HAL_PWR_GET_FLAG (PWR_FLAG_SB | PWR_FLAG_WU)) )
    return SUPPLY_VOLTAGE_WAS_INTERRUPTED;
  else
    return POWER_SUPPLY_WAS_OK;
}



/** ��������������������������������������������������������������������������������������������������������������
  @brief    WakingUpInNextMinuteAtRightSecond - "�����������" �� ���������� � ������� ������������������� �������


  @param
              1. u8_WakeUpSeconds: ������� "�����������", ����� 6...59
                                  ��������, � ���������� ��������� ��������� �������� 45, ��:
              a) ���� � ������ ������ �� ���������� ����� ��� 38-� �������, �� "�����������" �������� ����� 7 ������
              b) ���� � ������ ������ �� ���������� ����� ��� 47-� �������, �� "�����������" �������� ����� 58 ������

  @retval   

  @example

    WakingUpInNextMinuteAtRightSecond (45); // ����������� ����� ������ 45-� �������

  �������������������������������������������������������������������������������������������������������������� */
void WakingUpInNextMinuteAtRightSecond (u8 u8_WakeUpSeconds)
{
  RTC_AlarmTypeDef s_Alarm;
  RTC_HandleTypeDef hrtc;

  // �������� ������� ����������, �������� ������ ���� � ��������� 6...59 ������
  ASSERT ((u8_WakeUpSeconds > 5) && (u8_WakeUpSeconds < 60));

  hrtc.Instance = RTC;

  // Pfgbcsdftv � ��������� ������� ��������� ����������,
  HAL_RTC_GetAlarm(&hrtc, &s_Alarm, RTC_ALARM_A, RTC_FORMAT_BIN);

  // ��������� �������, ���, ���� � ������ �� �������
  s_Alarm.AlarmTime.Seconds = u8_WakeUpSeconds;

  // ��������� ��������� �� ������ ������ ����������� �����:
  s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
  // ��������� "�����������" �� ���� ��������� ����� ������ (������� ���������� ����)
  s_Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  s_Alarm.Alarm = RTC_ALARM_A;

  // ����������� � �������� ����������
  if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // ...���������� ����������� RTC-���������� � ������������ ����� �...
  HAL_RTC_MspInit(&hrtc);

  // ���������� ���� ���������� "�", ����� ��������� ��� ��������� � ��������� ���
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

}



/** ��������������������������������������������������������������������������������������������������������������
  @brief    WakingUpAtAppointedTime - "�����������" �� ���������� � ����������� ����� (�� ��������� �� ����)


  @param
              1. u8_WakeUpSeconds:  ������� "�����������", ����� 0...59
              2. u8_WakeUpMinutes:  ������ "�����������", ����� 0...59
              2. u8_WakeUpHours:    ��� "�����������", ����� 0...23
  @retval   

  @example

    WakingUpInNextMinuteAtRightSecond (45); // ����������� ����� ������ 45-� �������

  �������������������������������������������������������������������������������������������������������������� */
void WakingUpAtAppointedTime (u8 u8_WakeUpHours, u8 u8_WakeUpMinutes, u8 u8_WakeUpSeconds)
{
  RTC_AlarmTypeDef s_Alarm;
  RTC_HandleTypeDef hrtc;

  // �������� ������� ����������, �������� ������ ���� � ��������� 0...59 ������
  ASSERT (u8_WakeUpSeconds < 60);

  // �������� ������� ����������, �������� ������ ���� � ��������� 0...59 �����
  ASSERT (u8_WakeUpMinutes < 60);

  // �������� ������� ����������, �������� ������ ���� � ��������� 0...23 �����
  ASSERT (u8_WakeUpHours < 24);



  hrtc.Instance = RTC;

  // Pfgbcsdftv � ��������� ������� ��������� ����������,
  HAL_RTC_GetAlarm(&hrtc, &s_Alarm, RTC_ALARM_A, RTC_FORMAT_BIN);

  // ��������� �������, ���, ���� � ������ �� �������
  s_Alarm.AlarmTime.Seconds = u8_WakeUpSeconds;

  // ��������� ��������� �� ������ ������ ����������� �����:
  s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
  // ��������� "�����������" �� ���� ��������� ����� ������ (������� ���������� ����)
  s_Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  s_Alarm.Alarm = RTC_ALARM_A;

  // ����������� � �������� ����������
  if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // ...���������� ����������� RTC-���������� � ������������ ����� �...
  HAL_RTC_MspInit(&hrtc);

  // ���������� ���� ���������� "�", ����� ��������� ��� ��������� � ��������� ���
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

}



/*********************************** END OF FILE ************************************/
