/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

// Объявление HW_MAIN
#define HW_MAIN



/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "HW_Globals.h"
//#include "HW_Encoder.h"
#include "HW_I2C.h"
//#include "HW_Analog.h"
#include "HW_UART.h"
#include "HW_TIMERS.h"
#include "HW_SPI.h"
#include "HW_POWER.h"
#include "HW_EBYTE.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#include "stm32l1xx_hal_conf.h"
#include "stm32l1xx_hal_rcc.h"
#include "stm32l1xx_ll_rcc.h"



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  // __disable_irq();


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  
  // ________________________________________ Системные инициализации ___________________________________________________
  __enable_irq();
  HAL_MspInit();


  OLED1_Init();
  OLED1_Clear();



  // _______________________________________ Инициализация цепей питания ________________________________________________
  g_u8_PowerLines = ALL_POWER_FLAGS_RESET; // Сброс маски включенных устройств

  PowerSwitch (FRAM_POWER, POWER_OFF);

  // Изначально выводы UART2 инициализируем как входные порты, чтобы не запитывать RF-модуль
  PowerSwitch (RF_MODULE_POWER, POWER_OFF); // эта инициализация присутствует в процедуре PowerSwitch
  // PowerSwitch (RF_MODULE_POWER, POWER_ON); 

  // if ( HAL_OK != SetParametersOfRFmodule( DO_NOT_SAVE_THE_PARAMETERS_WHEN_POWER_DOWN,
  //                                           UART_BAUD_RATE_9600bps,
  //                                             UART_8N1,
  //                                               12,
  //                                                 AIR_BAUD_RATE_2400bps,
  //                                                   CHANNEL_7_FREQUENCY_869_MHZ,
  //                                                   // CHANNEL_6_FREQUENCY_868_MHZ,
  //                                                     TRANSPARENT_TRANSMISSION_MODE,
  //                                                       PUSH_PULL_OUTPUTS_AND_PULL_UP_INPUTS,
  //                                                         WAKE_UP_TIME_250MS,
  //                                                           FEC_DISABLE,
  //                                                           // FEC_ENABLE,
  //                                                             RF_POWER_10dBm ) )
  // {
  //   OLED1_Font_8x6_printf(1, 1, "Ошибка SetParameters");
  //   while (1);
  // }

  // SetOperatingModeRFmodule_E32_900T20S (MODE_NORMAL);

  // ________________________________________ Инициализация переменных __________________________________________________




  //  g_TEST_u8_SampleCounter = 0; // тестовая переменная отработки прерывания кнопки энкодера
  //
  g_TEST_u32_Val1 = 0;
  //  g_TEST_u32_Val2 = 0;
  //  g_TEST_u32_Val3 = 0;
  //  g_TEST_u32_Val4 = 0;
  //  g_TEST_u32_Val5 = 0;
  //  g_TEST_u32_Val6 = 0;

  //*———————————————————— Вывод заставки с номером версии (константа VERSION (Globals.h))
  OLED1_Font_8x6_printf(1, 15, "Запуск программы");
  OLED1_Font_16x12_printf(3, 25, NORMAL, NORMAL, "VECTOR");
  OLED1_Font_16x12_printf(6, 25, NORMAL, NORMAL, VERSION);
  HAL_Delay(5000); // подержали секунду приветствие
  OLED1_Clear();   //  и очистили экран



  ModBus_Receiving_Return_Status ReturnStatus;
  u8 u8_InputByte, u8_ByteForTransmitToFRAM;
  u16 u16_MeasureCounter = 1; // счётчик рабочих циклов полного обмена с датчиком
  u32 u32_TickCounter;

  RTC_TimeTypeDef s_Time = {0};
  RTC_DateTypeDef s_Date = {0};
  RTC_AlarmTypeDef s_Alarm = {0};
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————

  

  // u8_ByteForTransmitToFRAM = 12;


  // DataTransmitToFRAM_SPI2 (u_TransmitData.u8_ArrayData, sizeof (g_s_DataFromSensor), 0x5A);

  // DataReceiveFromFRAM_SPI2 (u_TransmitData.u8_ArrayData, sizeof (g_s_DataFromSensor), 0x5A);





  // Проверяем, прерывалось ли питание
  if ( SUPPLY_VOLTAGE_WAS_INTERRUPTED == WhatWasStateOfSupplyVoltage () )
  {
    
    // Если питание прерывалось, то устанавливаем время
    
    s_Time.Hours = 10;
    s_Time.Minutes = 10;
    s_Time.Seconds = 0;
    s_Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    s_Time.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &s_Time, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }
    s_Date.WeekDay = RTC_WEEKDAY_SATURDAY;
    s_Date.Month = RTC_MONTH_DECEMBER;
    s_Date.Date = 17;
    s_Date.Year = 22;

    if (HAL_RTC_SetDate(&hrtc, &s_Date, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }

    /** Enable the Alarm A
    */
    s_Alarm.AlarmTime.Hours = 10;
    s_Alarm.AlarmTime.Minutes = 10;
    s_Alarm.AlarmTime.Seconds = 45;
    s_Alarm.AlarmTime.SubSeconds = 0;
    s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    s_Alarm.AlarmMask = RTC_ALARMMASK_MINUTES;
    // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
    s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    s_Alarm.AlarmDateWeekDay = 17;
    s_Alarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }

    s_Alarm.AlarmTime.Hours = 10;
    s_Alarm.AlarmTime.Minutes = 10;
    s_Alarm.AlarmTime.Seconds = 45;
    s_Alarm.AlarmTime.SubSeconds = 0;
    s_Alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    s_Alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    s_Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
    // s_Alarm.AlarmMask = RTC_ALARMMASK_MINUTES;
    // s_Alarm.AlarmMask = RTC_ALARMMASK_NONE;
    s_Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    s_Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    s_Alarm.AlarmDateWeekDay = 17;
    s_Alarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &s_Alarm, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }




  }









  //  ВЫКЛючение синего светодиода
  HAL_GPIO_WritePin(LED_VD7_GPIO_Port, LED_VD7_Pin, LOW);

  u8 u8_Sec;
  HAL_RTC_GetTime(&hrtc, &s_Time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &s_Date, RTC_FORMAT_BIN);



  

  while (1)
  {
    
    OLED1_Font_8x6_printf(1, 1, "Время %02u:%02u:%02u", s_Time.Hours, s_Time.Minutes, s_Time.Seconds);
    OLED1_Font_8x6_printf(3, 1, "Дата  %02u/%02u/%02u", s_Date.Date, s_Date.Month, s_Date.Year);
    u8_Sec = s_Time.Seconds;

    ResetExtWDT (); // сброс WDT таймера-микросхему






    while (u8_Sec == s_Time.Seconds)
    {
      HAL_RTC_GetTime(&hrtc, &s_Time, RTC_FORMAT_BIN);
    }


    // // выводим побитное представление статуса устройства
    // OLED1_Font_8x6_printf(6, 0, PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(PWR->CSR));

    
    if (15 == s_Time.Seconds)
    {
      OLED1_Clear();   //  и очистили экран
      OLED1_Font_8x6_printf(1, 20, "Засыпаем ...");
      GoToSTANDBYmode ();
    }
    


    HAL_RTC_GetDate(&hrtc, &s_Date, RTC_FORMAT_BIN);

    // HAL_GPIO_TogglePin(LED_VD7_GPIO_Port, LED_VD7_Pin);

  }

  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————

  // u8 u8_a_DataToSend[100];
  union 
  {
    struct ReceivedDataFromSensor s_Struct;
    u8 u8_ArrayData[sizeof(g_s_DataFromSensor)];
  } u_TransmitData;



  // // Выключение питания FRAM
  // HAL_GPIO_WritePin(DRV_FRAM_GPIO_Port, DRV_FRAM_Pin, GPIO_PIN_RESET);


  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————

  // // Запрос параметров RF-модуля
  // if (UART2_TRANSMIT_OK == RequestParametersFromRFmodule())
  // {
  //   ReceivingParametersFromRFmodule ();
  // }

    // OLED1_Font_8x6_printf(1, 10, "Param. E32-900T20S");
    // OLED1_Font_8x6_printf(3, 1, "ADDR:%u F:%uMHz CH:%u", (((u16) g_u_ParametersOfRFmodule.u8_a_ParametersArray[1]) << 8) | (u16)(g_u_ParametersOfRFmodule.u8_a_ParametersArray[2]),
    //                                                           g_u_ParametersOfRFmodule.u8_a_ParametersArray[4]+862, g_u_ParametersOfRFmodule.u8_a_ParametersArray[4]);

    // OLED1_Font_8x6_printf(4, 1, PRINTF_BINARY_PATTERN_INT8 " - Rate", PRINTF_BYTE_TO_BINARY_INT8 (g_u_ParametersOfRFmodule.u8_a_ParametersArray[3]));
    // OLED1_Font_8x6_printf(5, 1, PRINTF_BINARY_PATTERN_INT8 " - Options", PRINTF_BYTE_TO_BINARY_INT8 (g_u_ParametersOfRFmodule.u8_a_ParametersArray[5]));

    // HAL_Delay(1000); // подержали секунду приветствие
    // OLED1_Clear();   //  и очистили экран

  //*! **************************************************************************************************************************************************
  // Тестовая последовательность для проверки дальности работы канала связи
  // Работа в 7-м канале с десятичным адресом 12
  // Скорость обмена 2400, без компенсации ошибок

  // TODO ПЕРЕД ТЕСТИРОВАНИЕМ ИЗМЕНИТЬ ВЫХОДНУЮ МОЩНОСТЬ ПЕРЕДАТЧИКОВ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  u8 u8_TestTransmitArray [116] = {0xAA};  // массив сообщения
  u16 u16_CounterCommunicationSession = 1;  // будем считать количество сеансов связи и выводить это значение на экран
  u16 u16_CounterSuccessfulCommunicationSession = 0;  // будем считать количество удачных сеансов связи и выводить это значение на экран
  u16 u16_CounterFailedCommunicationSession = 0;  // будем считать количество НЕудачных сеансов связи и выводить это значение на экран
  u16 u16_CRC;  // контрольная сумма
  u16 u16_RandomValue, u16_PreviousRandomValue = 0;
  HAL_StatusTypeDef TransmitStatus;
  // Первые два байта тестовой последовательности - порядковый номер сеанса, вторые два байта - случайные числа для проверки достоверности данных

  u8_TestTransmitArray [0] = 0x56; // "V"
  u8_TestTransmitArray [1] = 0x45; // "E"
  u8_TestTransmitArray [2] = 0x43; // "C"
  u8_TestTransmitArray [3] = sizeof (u8_TestTransmitArray); // четвёртый байт - размер сообщения


  // инициализируем массив отправляемых данных инкрементальными числами
  for (u8 u8_LoopCounter = 7; u8_LoopCounter < ( sizeof(u8_TestTransmitArray)-2 ); u8_LoopCounter++)
  {
    u8_TestTransmitArray [u8_LoopCounter] = u8_LoopCounter - 7;
  }
  
  while (1)
  {
    u8_TestTransmitArray [4] = (u8)(u16_CounterCommunicationSession & 0x00FF);              // младшая часть счётчика сессий
    u8_TestTransmitArray [5] = (u8)((u16_CounterCommunicationSession >> 8) & 0x00FF);       // старшая часть счётчика сессий

    while ( (u16_RandomValue = rand() % 0xFF) == u16_PreviousRandomValue); // получаем случайное число...
    u8_TestTransmitArray [6] = u16_RandomValue; // ...и кладём это число в 7-ю ячейку массива

    u16_CRC = ModBusChecksumCalculation ( u8_TestTransmitArray, sizeof(u8_TestTransmitArray)-2 ); // подсчёт контр. суммы

    // if (u16_CounterCommunicationSession > 10 && u16_CounterCommunicationSession < 20)
    // {
    //   u16_CRC+=3;
    // }


    // Контрю сумма в двух последних байтах массива
    u8_TestTransmitArray [sizeof(u8_TestTransmitArray)-2] = (u8)(u16_CRC & 0x00FF);
    u8_TestTransmitArray [sizeof(u8_TestTransmitArray)-1] = (u8)((u16_CRC >> 8) & 0x00FF);


    HAL_GPIO_WritePin(LED_VD7_GPIO_Port, LED_VD7_Pin, HIGH); // включаем светодиод

    // отправляем сообщение
    if (HAL_OK != (TransmitStatus = TransmitDataToRFmodule_E32_900T20S (u8_TestTransmitArray, sizeof (u8_TestTransmitArray))))
    {
      OLED1_Font_8x6_printf(8, 1, "Ошибка %u в сессии %u", TransmitStatus, u16_CounterCommunicationSession); // сообщение об ошиюке
    }
    else
    {
      OLED1_Font_8x6_printf(2, 1, "Сессия");
      OLED1_Font_16x12_printf (1, 50, NORMAL, NORMAL, "%u", u16_CounterCommunicationSession);

      OLED1_Font_8x6_printf(4, 1, "Случ. число");
      OLED1_Font_16x12_printf (3, 75, NORMAL, NORMAL, "%-3u", u16_RandomValue);

      OLED1_Font_8x6_printf(6, 1, "Контр. сумма");
      OLED1_Font_16x12_printf (5, 77, NORMAL, NORMAL, "%04X", u16_CRC);
    }








    HAL_GPIO_WritePin (LED_VD7_GPIO_Port, LED_VD7_Pin, LOW); // выключаем светодиод
    u16_CounterCommunicationSession++; // инкремент счётчика сессий
    u16_PreviousRandomValue = u16_RandomValue; // запоминаем случ. число от текущей сессии, чтобы оно не повторилось на следующую сессию
    
    HAL_Delay(773);
  }
  



  //*! **************************************************************************************************************************************************


  // Непосредственно сама основная программа:
  while (1)
  {


    //* Очистка входного буфера
    CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (TIME_OUT_OF_RECEIVER_UART1, TIME_OUT_OF_EMPTY_BUFFER_UART1);

    //* Побудка датчика тремя байтами 0xFF 0xFF 0xFF
    // HAL_GPIO_WritePin(LED_VD7_GPIO_Port, LED_VD7_Pin, GPIO_PIN_SET);
    u8 u8_a_PreambleToWakeUpTheSensor[3] = {0xFF, 0xFF, 0xFF};
    HAL_UART_Transmit(&huart1, u8_a_PreambleToWakeUpTheSensor, 3, TIME_OUT_OF_TRANSMITTER_UART1);


    // Настраиваем значение счётчика окончания ожидания приёма байта от датчика
    u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_RECEIVER_UART1;

    //* Крутимся в цикле пока не получим один байт в буфер приёма
    while (HAL_GetTick() < u32_TickCounter)
    {
      // Как дождались получение байта (появление флага занятости буфера), ....
      if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE))
      {
        // ... так сразу читаем этот байт
        HAL_UART_Receive(&huart1, &u8_InputByte, 1, 500); //* читаем байт
        break;
      }
    }


    //* Если вылетели из цикла по окончании времени ожидания (нет приёма от датчика)
    if (HAL_GetTick() >= u32_TickCounter)
    { 

      //*!!!!! контрольный сброс параметров USART1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      MX_USART1_UART_Init();

      //*! Вывод на экран сообщения "Нет ответа датчика!"
      OLED1_Clear();
      // OLED1_Font_8x6_printf(8, 90, "%u", u16_MeasureCounter); // кол-во полученных пакетов данных от датчика
      OLED1_Font_8x6_printf(6, 1, "Нет ответа датчика!");
    }


    // //*! выводим на экран то, что прочитали во входном буфере
    // OLED1_Clear();
    // OLED1_Font_8x6_printf(1, 110, "%X", u8_InputByte);


    //* Если приняли от датчика "кодовое послание" 0xAC ("готов к отправке данных")
    if (0xAC == u8_InputByte)
    {

      //* Запрашиваем данные у датчика
      ModBusRequestTransmitUART1(1, 4, 0x18); // устройство 1, регистр 4, 24 байта данных

      //* Читаем ответ датчика
      if ((ReturnStatus = ModBusReceivingDataFromSensorByUART1()) == UART_RECEIVING_OK)
      {

        //* Посылка команды датчику на переход в режим пониженного энергопотребления ("усыпление датчика")
        // (запись в регистр 0 команды 400, modbus команда 0x10 – Write Holding Registers);
        u16 u16_GoToSleep = COMMAND_SET_SENSOR_TO_SLEEP;
        ModBusCommandTransmitUART1(1, 0, 1, 2, &u16_GoToSleep);

        // Ответ датчика не анализируем, просто чистим в цикле буфер приёма
        //TODO надо ли анализировать ответ датчика на засыпание?

        // Настраиваем значение счётчика окончания ожидания приёма байта от датчика
        // u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_EMPTY_BUFFER_UART1;

        // // Крутимся в цикле пока не примем все байты, отправленные датчиком в ответ на команду на засыпание
        // while (HAL_GetTick() < u32_TickCounter)
        // {
        //   if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE))
        //   {
        //     HAL_UART_Receive(&huart1, &u8_InputByte, 1, 500); // читаем буфер приёма
        //   }
        // }


        //* Очистка входного буфера
        CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (100, 50);

        //*! Выводим результаты чтения данных датчика --------------------------------------------------------------------------------------------
        
        # ifdef INFO_FROM_SENSOR_OLED  // #######################################################################################################
        
          OLED1_Font_8x6_printf(8, 100, "%u", u16_MeasureCounter++); // кол-во полученных пакетов данных от датчика
          OLED1_Font_8x6_printf(1, 1, "%u %u %u %u %u", g_s_DataFromSensor.u16_DeviceStatus,
                                g_s_DataFromSensor.u8_StatusOfPositioner_1,
                                g_s_DataFromSensor.u8_StatusOfPositioner_2,
                                g_s_DataFromSensor.u8_SoftwareVersion,
                                g_s_DataFromSensor.u8_AO_Version);

          // выводим побитное представление статуса устройства
          OLED1_Font_8x6_printf(2, 0, PRINTF_BINARY_PATTERN_INT16,
                      PRINTF_BYTE_TO_BINARY_INT16(g_s_DataFromSensor.u16_DeviceStatus));


          OLED1_Font_8x6_printf(4, 1, "%3.3fmm  %3.3fmm", g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 * 1000.0, g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 * 1000.0);
          OLED1_Font_8x6_printf(6, 1, "%.5f   %u", g_s_DataFromSensor.f_SpeedOfSound, g_s_DataFromSensor.u8_AmbientTemperature);
          OLED1_Font_8x6_printf(7, 1, "%.5f  %.5f", g_s_DataFromSensor.f_OutputPulseAmplitude, g_s_DataFromSensor.f_TemperatureOfMeasuringElement);
          OLED1_Font_8x6_printf(8, 1, "%.5f", g_s_DataFromSensor.f_MeasuringRange);

          OLED1_Font_8x6_printf(1, 85, "%uбайт", sizeof (g_s_DataFromSensor)); // кол-во полученных пакетов данных от датчика

          OLED1_Font_8x6_printf(3, 1, "ADDR:%u F:%uM Ch:%u", (((u16) g_u_ParametersOfRFmodule.u8_a_ParametersArray[1]) << 8) | (u16)(g_u_ParametersOfRFmodule.u8_a_ParametersArray[2]),
                                                          g_u_ParametersOfRFmodule.u8_a_ParametersArray[4]+862, g_u_ParametersOfRFmodule.u8_a_ParametersArray[4]);


        #endif  // ##############################################################################################################################

        //*!--------------------------------------------------------------------------------------------------------------------------------------

        //! *****************************************************************************************************************************
        //! *****************************************************************************************************************************
        //! *****************************************************************************************************************************

        // Отправляем полученные данные на базовую станцию
        // UART2_TransmitSensorsDataToBaseStation (TIME_OUT_OF_TRANSMITTER_UART2);


        u_TransmitData.s_Struct = g_s_DataFromSensor;

        // "1"--CS2-->"0" 
        // HAL_GPIO_WritePin(GPIOB, SPI2_CS2_Pin, GPIO_PIN_RESET);

        DataTransmitToFRAM_SPI2 (u_TransmitData.u8_ArrayData, sizeof (g_s_DataFromSensor), 0x5A);

        DataReceiveFromFRAM_SPI2 (u_TransmitData.u8_ArrayData, sizeof (g_s_DataFromSensor), 0x5A);


        // HAL_GPIO_WritePin(GPIOB, SPI2_CS2_Pin, GPIO_PIN_SET);

        TransmitDataToRFmodule_E32_900T20S (u_TransmitData.u8_ArrayData, sizeof (g_s_DataFromSensor));


        //! *****************************************************************************************************************************
        //! *****************************************************************************************************************************
        //! *****************************************************************************************************************************

      }
      else
      {
        //*! Если пакет принят с ошибками, то выводим сообщения в зависимости от типа возвращённой ошибки
        OLED1_Clear();
        
        switch (ReturnStatus)
        {
        case UART_RECEIVING_CRC_ERROR: // ошибка контрольной суммы
          OLED1_Font_8x6_printf(4, 1, "Ошибка к.c.! %X!", u8_InputByte);
          break;

        case UART_RECEIVING_TIME_OUT_ERROR: // вообще ничего не приняли
          OLED1_Font_8x6_printf(4, 1, "Нет ответа! %X!", u8_InputByte);
          break;
        }
      }
    }

    // HAL_GPIO_WritePin(LED_VD7_GPIO_Port, LED_VD7_Pin, GPIO_PIN_SET);

    HAL_Delay(1000);

    HAL_GPIO_TogglePin(LED_VD7_GPIO_Port, LED_VD7_Pin);
    ResetExtWDT (); // сброс WDT таймера-микросхему



  }

  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  //* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  //*! ———————————————————— Дополнения по I2C1 ————————————————————————————————————————

  HAL_I2C_MspInit(&hi2c1);
  I2C1_Struct = hi2c1;

  //*! ———————————————————— Дополнения по I2C1 ————————————————————————————————————————


  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */



  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */


  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  //*! ———————————————————— Дополнения №1 по RTC ————————————————————————————————————————

  // Проверяем, прерывалось ли питание
  if ( POWER_SUPPLY_WAS_OK == WhatWasStateOfSupplyVoltage () ) // если питание не прерывалось, то...
  {
    // ...отставляем запущенными RTC-прерывания и тактирование часов, и...
    HAL_RTC_MspInit(&hrtc);

    // ...сбрасываем флаг будильника "А", чтобы будильник мог сработать в следующий раз, и...
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

    // ...выходим без дальнейшей инициализации часов
    return;
  }  

  //* Подобранные мною значения делителей для работы от внутреннего RC-генератора (37 кГц)
  // hrtc.Init.AsynchPrediv = 124;
  // hrtc.Init.SynchPrediv = 318;
  // if (HAL_RTC_Init(&hrtc) != HAL_OK)
  // {
  //   Error_Handler();
  // }

  


  //*! ———————————————————— Дополнения №1 по RTC ————————————————————————————————————————

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_DECEMBER;
  sDate.Date = 17;
  sDate.Year = 22;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 10;
  sAlarm.AlarmTime.Minutes = 10;
  sAlarm.AlarmTime.Seconds = 30;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 17;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  //*! ———————————————————— Дополнения №2 по RTC ————————————————————————————————————————



  //! Закомментировать строку: 
  //!     s_Time->SubSeconds = (uint32_t)((hrtc->Instance->SSR) & RTC_SSR_SS);
  //!  в файле stm32l1xx_hal_rtc.c в репозитории, находящемся по адресу
  //!  c:\Users\SAR\STM32Cube\Repository\STM32Cube_FW_L1_V1.10.3\Drivers\STM32L1xx_HAL_Driver\Src\
  //!  это исправление бага

  // Запуск RTC прерываний и тактирования часов
  HAL_RTC_MspInit(&hrtc);



  //*! ———————————————————— Дополнения №2 по RTC ————————————————————————————————————————
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
  //*! ———————————————————— Дополнения по SPI2 ————————————————————————————————————————

  HAL_SPI_MspInit(&hspi2);
  // __HAL_SPI_ENABLE(&hspi2);


  //*! ———————————————————— Дополнения по SPI2 ————————————————————————————————————————
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  //*! ———————————————————— Дополнения по UART1 ————————————————————————————————————————

  //* Установка скорости обмена по UART1 (Globals.h)
  huart1.Init.BaudRate = BAUD_RATE_UART1;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  //  Разрешаем работу UART1
  HAL_UART_MspInit(&huart1);

  //*! ———————————————————— Дополнения по UART1 ————————————————————————————————————————

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

    //*! ———————————————————— Дополнения по UART2 ————————————————————————————————————————

  //* Установка скорости обмена по UART2 (Globals.h)
  huart2.Init.BaudRate = BAUD_RATE_UART2;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }



  //  Разрешать работу UART2 нельзя, поскольку запитается RF-модуль через свой вход Rx,
  // поэтому следующая строка закомментирована
  // HAL_UART_MspInit(&huart2);

  //*! ———————————————————— Дополнения по UART2 ————————————————————————————————————————


  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_VD7_GPIO_Port, LED_VD7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TxE1_Pin|SPI1_CS1_Pin|TxE3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI2_CS2_Pin|WDT_DONE_Pin|E32_M0_Pin|E32_M1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DRV_RF_GPIO_Port, DRV_RF_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DRV_RTC_Pin|DRV_FRAM_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_VD7_Pin */
  GPIO_InitStruct.Pin = LED_VD7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_VD7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TxE1_Pin TxE3_Pin */
  GPIO_InitStruct.Pin = TxE1_Pin|TxE3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS1_Pin */
  GPIO_InitStruct.Pin = SPI1_CS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SPI1_CS1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_CS2_Pin */
  GPIO_InitStruct.Pin = SPI2_CS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SPI2_CS2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : WDT_DONE_Pin */
  GPIO_InitStruct.Pin = WDT_DONE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WDT_DONE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DRV_RF_Pin */
  GPIO_InitStruct.Pin = DRV_RF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DRV_RF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DRV_RTC_Pin DRV_FRAM_Pin */
  GPIO_InitStruct.Pin = DRV_RTC_Pin|DRV_FRAM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : E32_M0_Pin E32_M1_Pin */
  GPIO_InitStruct.Pin = E32_M0_Pin|E32_M1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : E32_AUX_Pin */
  GPIO_InitStruct.Pin = E32_AUX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(E32_AUX_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//*! ———————————————————— Дополнения по GPIO ————————————————————————————————————————



//*! ———————————————————— Дополнения по GPIO ————————————————————————————————————————
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
