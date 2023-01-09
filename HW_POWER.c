/******************** (C) COPYRIGHT 2022 << VECTOR >> ********************
 * File Name          : HW_POWER.c
 * Author             : Electronic Devices Application Team
 * Version            : V1.01
 * Date               : 11/2022
 * Description        : Óïðàâëåíèå ïîäïðîãðàììàìè êîììóòàöèè öåïåé ïèòàíèÿ
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








/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    PowerSwitch - ïåðåêëþ÷àòåëü ïèòàíèÿ öåïåé ïëàòû

  @param    1)PowerCircuit:             êîììóòèðóåìàÿ öåïü ïèòàíèÿ
                      RTC_POWER				  Ïèòàíèå ìèêðîñõåìû ÷àñîâ
	                    FRAM_POWER				Ïèòàíèå ìèêðîñõåìû FRAM
	                    RF_MODULE_POWER		Ïèòàíèå ðàäèîìîäóëÿ LoRa

  @param    2)PowerStatus:              ñîñòîÿíèå ïèòàíèÿ öåïè ïîñëå âûçîâà ïîäïðîãðàììû
                      POWER_ON				          ÂÊËþ÷åíèå ïèòàíèÿ
                        * Óñòàíàâëèâàåòñÿ â "åäèíèöó" áèò ïèòàíèÿ ëèíèè, íà êîòîðóþ ïîäàíî ïèòàíèå
	                    POWER_OFF				          ÂÛÊËþ÷åíèå ïèòàíèÿ
                        * Óñòàíàâëèâàåòñÿ â "íîëü" áèò ïèòàíèÿ ëèíèè, íà êîòîðóþ ïîäàíî ïèòàíèå

  @retval   void

  @example
    // ÂÊËþ÷åíèå ïèòàíèÿ FRAM
    PowerSwitch (FRAM_POWER, POWER_ON);
  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
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

          // Èíèöèàëèçèðóåì ëèíèè UART2 êàê Rx è Tx (äî ýòîãî îíè áûëè ïðîèíèöèàëèçèðîâàíû êàê âõîäû ïîðòîâ, ÷òîáû íå çàïèòûâàòü RF-ìîäóëü)
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
          // CS2: "1" --> "0" // óñòàíàâëèâàåì CS2 â "0", ÷òîáû â äàëüíåéøåì íå çàïèòûâàòü ïî ýòîé ëèíèè FRAM
          HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, LOW);
          // Âûêëþ÷àåì ïèòàíèå FRAM
          HAL_GPIO_WritePin(DRV_FRAM_GPIO_Port, DRV_FRAM_Pin, HIGH);
          g_u8_PowerLines &= ~FRAM_POWER_FLAG;
        return;

        case RF_MODULE_POWER:
          // Ïèíû UART2 ïåðåâîäèì â ïîðòû, ÷òîáû ïîñëå ÂÛÊËþ÷åíèÿ ïèòàíèÿ íå çàïèòûâàòü RF-ìîäóëü
          
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

          // Âûêëþ÷àåì RF-ìîäóëü
          HAL_GPIO_WritePin(DRV_RF_GPIO_Port, DRV_RF_Pin, HIGH);
          g_u8_PowerLines &= ~RF_MODULE_POWER_FLAG;
        return;
      }
  } // ------------ switch (PowerStatus) -----------------------------
}



/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    SettingWakeUpTime - óñòàíîâêà âðåìåíè "ïðîáóæäåíèÿ"

  @param    

  @retval   void

  @example

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
// HAL_StatusTypeDef SetWakeUpTime(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)


// HAL_RTCEx_SetWakeUpTimer_IT




/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    GoToSTANDBYmode - óõîä â "ñîí" (ðåæèì ïîíèæåííîãî ýíåðãîïîòðåáëåíèÿ STANDBY)

  @param    
  @retval   void
  @example

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
void GoToSTANDBYmode ()
{

  // Âûêëþ÷àåì ïèòàíèå âñåõ âíåøíèõ óñòðîéñòâ, åñëè ÿ èõ, âäðóã, çàáûë çàðàíåå âûêëþ÷èòü
  PowerSwitch (FRAM_POWER, POWER_OFF);
  PowerSwitch (RF_MODULE_POWER, POWER_OFF);

  // Ñáðîñ ôëàãè "ñíà" è "ïðîáóæäåíèÿ"
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); // Ñáðàñûâàåì ôëàã "ïðîáóæäåíèÿ", ÷òîáû íîðìàëüíî "çàñûïàòü", à íå èäòè â ïåðåçàãðóçêó


  RTC_HandleTypeDef hrtc;
  hrtc.Instance = RTC;

  // Ñáðàñûâàåì ôëàã áóäèëüíèêà "À", ÷òîáû áóäèëüíèê ìîã ñðàáîòàòü â ñëåäóþùèé ðàç
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

  // Óõîäèì â "ñîí"
  HAL_PWR_EnterSTANDBYMode();
}




/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    WhatWasStateOfSupplyVoltage - ïðîöåäóðà ïðîâåðêè íà íàëè÷èå èñ÷åçíîâåíèÿ ïèòàíèÿ

  @param    

  @retval   
      SUPPLY_VOLTAGE_WAS_INTERRUPTED  - áûëè ïåðåáîè ñ íàïðÿæåíèåì ïèòàíèÿ èëè ýòî ñàìîå ïåðâîå âêëþ÷åíèå
      POWER_SUPPLY_WAS_OK             - ïåðåáîåâ ïèòàíèÿ íå áûëî èëè óñòðîéñòâî áûëî âî "ñíå"

  @example

  // Ïðîâåðÿåì, ïðåðûâàëîñü ëè ïèòàíèå
  if ( POWER_SUPPLY_WAS_OK == WhatWasStateOfSupplyVoltage () ) // åñëè ïèòàíèå íå ïðåðûâàëîñü, òî...
  {
    // ...îòñòàâëÿåì çàïóùåííûìè RTC-ïðåðûâàíèÿ è òàêòèðîâàíèå ÷àñîâ è...
    HAL_RTC_MspInit(&hrtc);
  }  


  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
Continuity_Of_Supply_Voltage WhatWasStateOfSupplyVoltage ()
{
  if ( 0 == (__HAL_PWR_GET_FLAG (PWR_FLAG_SB | PWR_FLAG_WU)) )
    return SUPPLY_VOLTAGE_WAS_INTERRUPTED;
  else
    return POWER_SUPPLY_WAS_OK;
}



/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    WakingUpInNextMinuteAtRightSecond - "ïðîáóæäåíèå" ïî ïðåðûâàíèþ â áóäóùóþ çàïðîãðàììèðîâàííóþ ñåêóíäó


  @param
              1. u8_WakeUpSeconds: ñåêóíäà "ïðîáóæäåíèÿ", ÷èñëî 6...59
                                  Íàïðèìåð, â ïàðàìåòðàõ ïðîöåäóðå ïåðåäà¸òñÿ çíà÷åíèå 45, òî:
              a) Åñëè â äàííûé ìîìåíò íà âíóòðåííèõ ÷àñàõ èä¸ò 38-ÿ ñåêóíäà, òî "ïðîáóæäåíèå" íàñòóïèò ÷åðåç 7 ñåêóíä
              b) Åñëè â äàííûé ìîìåíò íà âíóòðåííèõ ÷àñàõ èä¸ò 47-ÿ ñåêóíäà, òî "ïðîáóæäåíèå" íàñòóïèò ÷åðåç 58 ñåêóíä

  @retval   

  @example

    WakingUpInNextMinuteAtRightSecond (45); // Ïðîñûïàòüñÿ áóäåì êàæäóþ 45-þ ñåêóíäó

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
void WakingUpInNextMinuteAtRightSecond (u8 u8_WakeUpSeconds)
{
  RTC_AlarmTypeDef s_Alarm;
  RTC_HandleTypeDef hrtc;

  // Ïðîâåðêà âõîäíûõ ïàðàìåòðîâ, ïàðàìåòð äîëæåí áûòü â äèàïàçîíå 6...59 ñåêóíä
  ASSERT ((u8_WakeUpSeconds > 5) && (u8_WakeUpSeconds < 60));

  hrtc.Instance = RTC;

  // Pfgbcsdftv â ñòðóêòóðó òåêóùèå óñòàíîâêè áóäèëüíèêà,
  HAL_RTC_GetAlarm(&hrtc, &s_Alarm, RTC_ALARM_A, RTC_FORMAT_BIN);

  // Âïèñûâàåì ñåêóíäû, äíè, ÷àñû è ìèíóòû íå òðîãàåì
  s_Alarm.AlarmTime.Seconds = u8_WakeUpSeconds;

  // Íåêîòîðûå ïàðàìåòðû íà âñÿêèé ñëó÷àé ïðîïèñûâàåì ñíîâà:
  s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
  // Îòêëþ÷àåì "ïðîáóæäåíèå" ïî âñåì êðèòåðèÿì êðîìå ñåêóíä (êîòîðûå óñòàíîâèëè âûøå)
  s_Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  s_Alarm.Alarm = RTC_ALARM_A;

  // Ïðîïèñûâàåì â ðåãèñòðû áóäèëüíèêà
  if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // ...îòñòàâëÿåì çàïóùåííûìè RTC-ïðåðûâàíèÿ è òàêòèðîâàíèå ÷àñîâ è...
  HAL_RTC_MspInit(&hrtc);

  // Ñáðàñûâàåì ôëàã áóäèëüíèêà "À", ÷òîáû áóäèëüíèê ìîã ñðàáîòàòü â ñëåäóþùèé ðàç
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

}



/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    WakingUpAtAppointedTime - "ïðîáóæäåíèå" ïî ïðåðûâàíèþ â íàçíà÷åííîå âðåìÿ (íå çàâèñÿùåå îò äàòû)


  @param
              1. u8_WakeUpSeconds:  ñåêóíäà "ïðîáóæäåíèÿ", ÷èñëî 0...59
              2. u8_WakeUpMinutes:  ìèíóòà "ïðîáóæäåíèÿ", ÷èñëî 0...59
              2. u8_WakeUpHours:    ÷àñ "ïðîáóæäåíèÿ", ÷èñëî 0...23
  @retval   

  @example

    WakingUpInNextMinuteAtRightSecond (45); // Ïðîñûïàòüñÿ áóäåì êàæäóþ 45-þ ñåêóíäó

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
void WakingUpAtAppointedTime (u8 u8_WakeUpHours, u8 u8_WakeUpMinutes, u8 u8_WakeUpSeconds)
{
  RTC_AlarmTypeDef s_Alarm;
  RTC_HandleTypeDef hrtc;

  // Ïðîâåðêà âõîäíûõ ïàðàìåòðîâ, ïàðàìåòð äîëæåí áûòü â äèàïàçîíå 0...59 ñåêóíä
  ASSERT (u8_WakeUpSeconds < 60);

  // Ïðîâåðêà âõîäíûõ ïàðàìåòðîâ, ïàðàìåòð äîëæåí áûòü â äèàïàçîíå 0...59 ìèíóò
  ASSERT (u8_WakeUpMinutes < 60);

  // Ïðîâåðêà âõîäíûõ ïàðàìåòðîâ, ïàðàìåòð äîëæåí áûòü â äèàïàçîíå 0...23 ÷àñîâ
  ASSERT (u8_WakeUpHours < 24);



  hrtc.Instance = RTC;

  // Pfgbcsdftv â ñòðóêòóðó òåêóùèå óñòàíîâêè áóäèëüíèêà,
  HAL_RTC_GetAlarm(&hrtc, &s_Alarm, RTC_ALARM_A, RTC_FORMAT_BIN);

  // Âïèñûâàåì ñåêóíäû, äíè, ÷àñû è ìèíóòû íå òðîãàåì
  s_Alarm.AlarmTime.Seconds = u8_WakeUpSeconds;

  // Íåêîòîðûå ïàðàìåòðû íà âñÿêèé ñëó÷àé ïðîïèñûâàåì ñíîâà:
  s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
  // Îòêëþ÷àåì "ïðîáóæäåíèå" ïî âñåì êðèòåðèÿì êðîìå ñåêóíä (êîòîðûå óñòàíîâèëè âûøå)
  s_Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  s_Alarm.Alarm = RTC_ALARM_A;

  // Ïðîïèñûâàåì â ðåãèñòðû áóäèëüíèêà
  if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  // ...îòñòàâëÿåì çàïóùåííûìè RTC-ïðåðûâàíèÿ è òàêòèðîâàíèå ÷àñîâ è...
  HAL_RTC_MspInit(&hrtc);

  // Ñáðàñûâàåì ôëàã áóäèëüíèêà "À", ÷òîáû áóäèëüíèê ìîã ñðàáîòàòü â ñëåäóþùèé ðàç
  __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

}



/*********************************** END OF FILE ************************************/
