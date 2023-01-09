/******************** (C) COPYRIGHT 2022 << SAR >> *******************************
 * File Name          : HW_UART.c
 * Author             : SAR
 * Version            : V1.01
 * Date               : 03/2022
 * Description        : Управление подпрограммами работы интерфейса UART
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
extern UART_HandleTypeDef huart1; // Заполненная данными после инициализации порта структура данных для UART1 (main.c)
extern UART_HandleTypeDef huart2; // Заполненная данными после инициализации порта структура данных для UART2 (main.c)


//* Чтение входного буфера UART1 до тех пор, пока в нем остаются данные
//* Применяется для очистки входного буфера
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 - Очистка входного буфера UART1

  @param    1)u32_MillisecondsCounter:              Значение таймаута в мс, которое даётся на очистку буфера
  @param    1)u32_WaitingTimeForBufferCleanup:      Время (в миллисекундах) ожидания прихода данных в буфер приёма UART
                                Значение первого аргумента должно быть больше значения второго аргумента!!!!!!

  @retval   UART1_BUFFER_EMPTY: 	          Нормальное завершение процедуры
  @retval 	UART1_CLEARING_BUFFER_ERROR: 	  Выход по таймауту, данные всё время приходят и приходят, сколько их не чисть

  @example
  
    CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (600, 500);

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
Clearing_UART1_Buffer_Return_Status CyclicDataReadingWhileReceiveBufferIsNotEmpty_UART1 (u32 u32_MillisecondsWaitingCounter,
                                                                                                 u32 u32_WaitingTimeForBufferCleanup)
{
  u32 u32_TickCounter, u32_WaitingTime;
  u8 u8_InputByte;

  // Настраиваем значения счётчиков ожидания
  u32_TickCounter = HAL_GetTick() + u32_MillisecondsWaitingCounter;
  u32_WaitingTime = HAL_GetTick() + u32_WaitingTimeForBufferCleanup;

  // Крутимся в цикле пока имеются данные в буфере приёма
  while (HAL_GetTick() < u32_TickCounter)
  {
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) // если данные имеются
    {
      HAL_UART_Receive(&huart1, &u8_InputByte, 1, TIME_OUT_OF_EMPTY_BUFFER_UART1); // читаем данные из буфера приёма
    }
    else if (HAL_GetTick() > u32_WaitingTime)
    {
      return UART1_BUFFER_EMPTY;
    }
  }
  return UART1_CLEARING_BUFFER_ERROR;
}








//* Передача данных через порт UART1
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    UART1_Transmit - Передача данных через порт UART1

  @param    1)*u8_pData:      указатель на передаваемые uint8 данные (uint8*)
  @param    2)u16_Size:       размер в байтах передаваемых данных (uint16)
  @param    3)u32_Timeout     время uint32 ожидания передачи (uint32)

  @retval   HAL_OK: 	  всё в норме
  @retval 	HAL_ERROR: 	  ошибка
  @retval 	HAL_BUSY:  	  ошибка
  @retval 	HAL_TIMEOUT:  ошибка

  @example
  
    u8 u8_Value=123;
    UART1_Transmit (&u8_Value, 1, 500);


  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
HAL_StatusTypeDef UART1_Transmit (u8 *u8_pData, u16 u16_Size, u32 u32_Timeout)
{
	return HAL_UART_Transmit(&huart1, u8_pData, u16_Size, u32_Timeout);
}








//* Передача данных через порт UART2
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    UART2_Transmit - Передача данных через порт UART2

  @param    1)*u8_pData:      указатель на передаваемые uint8 данные (uint8*)
  @param    2)u16_Size:       размер в байтах передаваемых данных (uint16)
  @param    3)u32_Timeout     время uint32 ожидания передачи (uint32)

  @retval   HAL_OK: 	  всё в норме
  @retval 	HAL_ERROR: 	  ошибка
  @retval 	HAL_BUSY:  	  ошибка
  @retval 	HAL_TIMEOUT:  ошибка

  @example
  
    u8 u8_Value=123;
    UART2_Transmit (&u8_Value, 1, 500);


  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
HAL_StatusTypeDef UART2_Transmit (u8 *u8_pData, u16 u16_Size, u32 u32_Timeout)
{
	return HAL_UART_Transmit(&huart2, u8_pData, u16_Size, u32_Timeout);
}






//* Передача данных, принятых от датчика ( глобальная структура g_s_DataFromSensor (Globals.h) ), 
//* через порт UART2 в сторону базовой станции
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    UART2_TransmitSensorsDataToBaseStation - Передача данных через порт UART2

  Сообщение, передаваемое базовой станции, содержит в себе три сегмента:
    1. Преамбула - 10 байт
    2. Данные, полученные от датчика и распакованные (в основном это касается формата float) из формата ModBus - до 100 байт
    3. Контрольная сумма - 2 байта

  @param    1)u32_Timeout     время uint32 ожидания передачи (uint32) до вылета по TIMEOUT

  @retval   HAL_OK: 	  всё в норме
  @retval 	HAL_ERROR: 	  ошибка
  @retval 	HAL_BUSY:  	  ошибка
  @retval 	HAL_TIMEOUT:  ошибка

  @example
  
    UART2_TransmitSensorsDataToBaseStation (500);

  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
HAL_StatusTypeDef UART2_TransmitSensorsDataToBaseStation (u32 u32_Timeout)
{

  //* Для того, чтобы передать данные по UART2 базовой станции,
  //* делаем объединение (union) данных для отправки по единому адресу с массивом отправки данных
  union //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  {
    struct ArrayForSendingToBaseStation
    {
      // 1. Преамбула (заголовок) сообщения - 10 байт
      u8 u8_a_Preamble [10];

    // 2. Создаём клон структуры ReceivedDataFromSensor (Globals.h)
    struct ReceivedDataFromSensor s_DataForSendingToStation;

    // 3. Два байта под контр. сумму (в программе далее не используется, но служит для понятия разпределения памяти в массиве)
    u16 u16_CRC;

    } s_PreliminarySendingData;

    // по начальному адресу структуры s_PreliminarySendingData размещаем массив с размером структуры этой структуры
    u8 u8_a_SendingDataArray[sizeof(s_PreliminarySendingData.u8_a_Preamble) + 
                              sizeof(s_PreliminarySendingData.s_DataForSendingToStation) +
                                sizeof(s_PreliminarySendingData.u16_CRC) ];

  } u_DataForSending; //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


  //* 1. Работаем над преамбулой =====================================================================================================
  //* первые шесть байт - слово "VECTOR"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [0] = 86; // "V" 
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [1] = 69; // "E"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [2] = 67; // "C"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [3] = 84; // "T"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [4] = 79; // "O"
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [5] = 82; // "R"
  
  //* Седьмой байт - размер сообщения (в байтах) без учёта начального слова "VECTOR", но с учётом двух байт контрольной суммы
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [6] = sizeof(u_DataForSending.u8_a_SendingDataArray) - 6;


  //* Восьмой байт - псевдослучайное число в диапазоне 0...5
  u8 u8_RandonValue;
  // Запускаем генератор псевдослучайных чисел случайным числом от системного таймера
  srand ( HAL_GetTick() %0xFF );
  // Получаем от генератора случайное число в диапазоне 0...5
  u8_RandonValue = (rand() %5);

  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [7] = u8_RandonValue;

  //* Девятый байт - произведение значения одного из первых шести байт ( [0]...[5], номер элемента массива преамбулы - в зависимости от числа в восьмом байте)
  //* на седьмой байт. Значение произведения ограничено по модулю 0xFF.

  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [8] =
   (u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [u8_RandonValue] * u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [6]) %0xFF;
  
  
  //* Десятый байт - номер датчика (0...255)
  u_DataForSending.s_PreliminarySendingData.u8_a_Preamble [9] = SENSOR_NUMBER;

  //*==================================================================================================================================
  //* 2. Копируем данные из глобальной структуры g_s_DataFromSensor в структуру отправляемых данных  ==================================
  u_DataForSending.s_PreliminarySendingData.s_DataForSendingToStation = g_s_DataFromSensor;

  //* 3. Рассчитываем контрольную сумму передаваемого массива без двух последних байт, в которых, собственно, и будет лежать контрольная сумма
  // Расчёт контрольной суммы в данном случае делается самым простым методом - суммой всех байт передаваемого массива с обрезкой до 16 бит
  // без учёта переполнения и без знака
  u_DataForSending.s_PreliminarySendingData.u16_CRC = 0;  // 2-байтная переменная под контрорльную сумму

  // Подсчитываем контрольную сумму всех байтов массива передаваемых данных
  u8 u8_Counter;
  u16 u16_CRC = 0;
  for (u8_Counter = 0; u8_Counter < ( sizeof(u_DataForSending.u8_a_SendingDataArray) -2 ); u8_Counter++)
  {
    u16_CRC += (u16)u_DataForSending.u8_a_SendingDataArray[u8_Counter];
  }

  // Получившуюся сумму кладём в два последних байта массива, подготовленного для отправки
  u_DataForSending.u8_a_SendingDataArray[u8_Counter++] = (u8) (u16_CRC & 0xFF);
  u_DataForSending.u8_a_SendingDataArray[u8_Counter++] = (u8) ((u16_CRC >> 8) & 0xFF);

  // OLED1_Font_8x6_printf(8, 1, "%u %04X", u8_Counter, u16_CRC); 
  // OLED1_Font_8x6_printf(5, 0, "%u %u %u %u %3.u", sizeof (u_DataForSending.s_PreliminarySendingData.s_DataForSendingToStation), 
  //                                         sizeof (u_DataForSending.s_PreliminarySendingData.u8_a_Preamble),
  //                                         sizeof (u_DataForSending.s_PreliminarySendingData.u16_CRC),
  //                                          sizeof (u_DataForSending.u8_a_SendingDataArray),
  //                                          u8_Counter );
  // OLED1_Font_8x6_printf(5, 0, "%u байт", sizeof (u_DataForSending.u8_a_SendingDataArray)); // кол-во полученных пакетов данных от датчика
  // OLED1_Font_8x6_printf(5, 0, "%u байт, %04X", sizeof (u_DataForSending.u8_a_SendingDataArray), u16_CRC); // кол-во полученных пакетов данных от датчика


  // for (u8 i = 0; i < 5; i++)
  // {
  //   OLED1_Font_8x6_printf(1, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[10+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(2, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[15+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(3, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[20+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(4, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[25+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(5, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[30+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(6, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[35+i]); // кол-во полученных пакетов данных от датчика
  //   OLED1_Font_8x6_printf(7, 22*i, "%3u", u_DataForSending.u8_a_SendingDataArray[40+i]); // кол-во полученных пакетов данных от датчика
  // }

  // Теперь надо отправить принятые от датчика данные (плюс контр. сумму) на "базовую станцию"
  return UART2_Transmit(u_DataForSending.u8_a_SendingDataArray, u8_Counter, TIME_OUT_OF_TRANSMITTER_UART2);
}









//* Передача данных через порт UART1 по протоколу ModBus
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    ModBusRequestTransmitUART1 - Передача данных через порт UART1 по протоколу ModBus

  @param    1)u8_SlaveAddress:      адрес подчинённого устройства
  @param    2)u8_FunctionCode:      номер блока регисторов
  @param    3)u8_SizeRequestedData  количество запрашиваемых байт данных от подчинённого

  @retval   HAL_OK: 	    всё в норме
  @retval 	HAL_ERROR: 	  ошибка
  @retval 	HAL_BUSY:  	  ошибка
  @retval 	HAL_TIMEOUT:  ошибка

  @example
  
  ModBusRequestTransmitUART1 (1, 4, 24); // Запрос на выдачу 24 байт от устройства с адресом 1 из его блока регистров №4
  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
HAL_StatusTypeDef ModBusRequestTransmitUART1 (u8 u8_SlaveAddress, u8 u8_FunctionCode, u8 u8_SizeRequestedData)
{
  u8 u8_a_ArrayForTransmit [8] = {0}; // чистим массив
  
  union
  {// размещаем по одному адресу двухбайтный массив и двухбайтную переменную для правильного распределения данных CRC
    struct 
    {
      u8 u8_u_a_ArrayCRC[2];
    };
    u16 u16_u_CRC;
  } u_crc;

  // Загружаем данные в подготавливаемый к отправке массив
  u8_a_ArrayForTransmit [0] = u8_SlaveAddress;  // адрес подчинённого
  u8_a_ArrayForTransmit [1] = u8_FunctionCode;  // адрес блока регисторов
  u8_a_ArrayForTransmit [2] = 0;                // холостые байты
  u8_a_ArrayForTransmit [3] = 0;
  u8_a_ArrayForTransmit [4] = 0;
  u8_a_ArrayForTransmit [5] = u8_SizeRequestedData;   // количество запрашиваемых байт

  // Вычисляем контрольную сумму первых шести [0...5] байт массива запроса и кладём результат в двухбатную переменную
  u_crc.u16_u_CRC = ModBusChecksumCalculation (u8_a_ArrayForTransmit, 6);

  // Извлекаем значение контрольной суммы из двухбайтного массива и помещаем их в два последних байта массива запроса
  u8_a_ArrayForTransmit [6] = u_crc.u8_u_a_ArrayCRC [0];   // CRC мл. байт
  u8_a_ArrayForTransmit [7] = u_crc.u8_u_a_ArrayCRC [1];   // CRC ст. байт

  // OLED1_Font_8x6_printf(7, 1, "%2X %2X",  u8_a_ArrayForTransmit [6], u8_a_ArrayForTransmit [7]);

  // Отправляем подготовленный массив по интерфейсу UART1

	return HAL_UART_Transmit(&huart1, u8_a_ArrayForTransmit, 8, TIME_OUT_OF_TRANSMITTER_UART1);
}









//* Передача команды через порт UART1 по протоколу ModBus, функция 16 (0x10) Write Multiple registers
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    ModBusCommandTransmitUART1 - Передача команды через порт UART1 по протоколу ModBus,
                                            функция 16 (0x10) Write Multiple registers

  @param    1)u8_SlaveAddress               адрес подчинённого устройства
  @param    2)u16_AddressOfFirstReg         адрес первого 16-битного регистра (Starting Address, 0x0000 to 0xFFFF)
  @param    3)u16_QuantityOfRegisters       количество заполняемых 16-битных регистров (0x0001 to 0x007B)
  @param    4)u8_NumberOfBytes              количество передаваемых в пакете байт (2 x N*)
  @param    5)u16* p_u16_TransmittedData    ссылка на массив передаваемых данных (N* x 2 Bytes)
                                                  где *N = количество регистров

  @retval   HAL_OK: 	    всё в норме
  @retval 	HAL_ERROR: 	  ошибка
  @retval 	HAL_BUSY:  	  ошибка
  @retval 	HAL_TIMEOUT:  ошибка

  @example
  
  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
HAL_StatusTypeDef ModBusCommandTransmitUART1 (u8 u8_SlaveAddress, u16 u16_AddressOfFirstReg,
                                                 u16 u16_QuantityOfRegisters, u8 u8_NumberOfBytes,  u16* p_u16_TransmittedData)
{
  u8 u8_a_ArrayForTransmit [SIZE_OF_TRANSMIT_BUFFER_UART1] = {0}; // чистим массив
  u8 u8_ByteCount=0;

  union // Для преобразования 16-битных переменных в два элемента 8-битного массива используем union
  {// размещаем по одному адресу двухбайтный массив и двухбайтную переменную для правильного распределения данных
    struct 
    {
      u8 u8_a_ArrayBYTE[2];
    };
    u16 u16_WORD;
  } data;

  // Проверка входных данных
  if (u16_QuantityOfRegisters > 128)
  {
    return HAL_ERROR;
  }
  
  // пример последовательности команды 0x10: 01 10 00 BE 00 02 04 00 00 00 01 B8 97, где
  // 01 Адрес
  // 10 Функция
  // 00 BE Начальный регистр
  // 00 02 Сколько регистров необходимо записать
  // 04    Кол-во байт для записи
  // 00 00 Значение для первого регистра
  // 00 01 Значение для второго регистра
  // B8 97 Контрольная сумма


  // Загружаем данные в подготавливаемый к отправке массив
  u8_a_ArrayForTransmit [u8_ByteCount++] = u8_SlaveAddress;  // адрес подчинённого
  u8_a_ArrayForTransmit [u8_ByteCount++] = 0x10;             // код функции

  data.u16_WORD = u16_AddressOfFirstReg;        // Адрес первого записываемого регистра 
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];               
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];

  data.u16_WORD = u16_QuantityOfRegisters;        // Количество заполняемых регистров 
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];               
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];

  u8_a_ArrayForTransmit [u8_ByteCount++] = u8_NumberOfBytes;   // Количество передаваемых байт

  // Переносим входные данные в массив для передачи подчинённому устройству
  u16 u16_Count;
  for (u16_Count = 0; u16_Count < u16_QuantityOfRegisters; u16_Count++)
  {
    data.u16_WORD = *(p_u16_TransmittedData + u16_Count);
    u8_a_ArrayForTransmit [u8_ByteCount++]  = data.u8_a_ArrayBYTE[1];               
    u8_a_ArrayForTransmit [u8_ByteCount++]  = data.u8_a_ArrayBYTE[0];
  }
  
  // Вычисляем контрольную сумму массива запроса и кладём результат в двухбатную переменную
  data.u16_WORD = ModBusChecksumCalculation (u8_a_ArrayForTransmit, u8_ByteCount);

  // Извлекаем значение контрольной суммы из двухбайтного массива и помещаем их в два последних байта массива запроса
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[0];   // CRC мл. байт
  u8_a_ArrayForTransmit [u8_ByteCount++] = data.u8_a_ArrayBYTE[1];   // CRC ст. байт

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

  // Отправляем подготовленный массив по интерфейсу UART1
	return HAL_UART_Transmit(&huart1, u8_a_ArrayForTransmit, u8_ByteCount, TIME_OUT_OF_TRANSMITTER_UART1);
}







//* Приём данных портом UART1 по протоколу ModBus от датчика
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    ModBusReceivingDataFromSensorByUART1 - Приём данных от датчика типа ПЛП1012U-Ex-У-0 через порт UART1 по протоколу ModBus.
                        Распределение принятых данных в структуру ReceivedDataFromSensor (HW_Globals.h)

  @retval   UART_RECEIVING_OK							  Нормальное штатное завершение процедуры, данные приняты, CRC совпадает
  @retval   UART_RECEIVING_CRC_ERROR        Какие-то данные приняты, но не корректно, т.к. имеются ошибки контрольной суммы
  @retval   UART_RECEIVING_TIME_OUT_ERROR		Данные или вообще не пришли, или пришли частично, но приём остановился
  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
ModBus_Receiving_Return_Status ModBusReceivingDataFromSensorByUART1 ()
{
  u8 u8_ArrayForReadFromSensor [SIZE_OF_RECEIVE_BUFFER_UART1] = {0}; // чистим массив для приёма
  u8 u8_ByteCounter;    // переменная подсчёта принятых байт
  u32 u32_TickCounter;  // переменная отсчёта времени (мс) ожидания
  huart1.Instance = USART1;

  // Цикл чтения пакета данных от подчинённого устройства
  for (u8_ByteCounter = 0; u8_ByteCounter < SIZE_OF_RECEIVE_BUFFER_UART1; )
  {
    // Настраиваем значение счётчика окончания ожидания приёма байта от датчика
    u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_RECEIVER_BYTE_UART1;

    // Крутимся в цикле пока не получим один байт в буфер приёма
    while (HAL_GetTick() < u32_TickCounter)
    {
      // Как дождались получение байта (появление флага занятости буфера), ....
      if (__HAL_UART_GET_FLAG (&huart1, UART_FLAG_RXNE))
      {
        // ... так сразу читаем этот байт в массив принятых данных
        HAL_UART_Receive(&huart1, &u8_ArrayForReadFromSensor [u8_ByteCounter++], 1, 500); // читаем
        break;
      }
    }

    // Если байты в буфер больше не поступают (время ожидания байта вышло), то начинаем анализ принятых данных
    if (HAL_GetTick() >= u32_TickCounter)
    {
      // Если не приняли ни одного байта, выход
      if (0 == u8_ByteCounter)
      {
        return UART_RECEIVING_TIME_OUT_ERROR;
      }
      
      union   // Готовим объединение для распределения двух байт (мл. и ст.) полученной контрольной суммы
      {
        u16 u16_CalculatedCRC;
        struct
        {
          u8 u8_Byte0;
          u8 u8_Byte1;
        };
      } crc;
                                                      

      // Вычисление CRC
      crc.u16_CalculatedCRC = ModBusChecksumCalculation (u8_ArrayForReadFromSensor, u8_ByteCounter-2);

      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // Если вычисленная и пришедшая контрольные суммы совпадают,
      // переписываем данные из входного массива u8_ArrayForReadFromSensor в глобальный массив данных g_s_DataFromSensor
      if (u8_ArrayForReadFromSensor [u8_ByteCounter-2] == crc.u8_Byte0 && u8_ArrayForReadFromSensor [u8_ByteCounter-1] == crc.u8_Byte1)
      {
        //* Адрес выдающего данные подчинённого устройства
        g_s_DataFromSensor.u8_AddressOfSlaveDevice = u8_ArrayForReadFromSensor [ADDRESS_ADDRESS_SLAVE_DEVICE];

        //* Адрес банка читаемых регистров
        g_s_DataFromSensor.u8_FunctionalCode = u8_ArrayForReadFromSensor [ADDRESS_FUNCTIONAL_CODE];

        //* Количество принятых от подчинённого устройства байт
        g_s_DataFromSensor.u8_NumberOfBytesReceived = u8_ByteCounter;

        //* Состояние позиционера №1
        g_s_DataFromSensor.u8_StatusOfPositioner_1 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_1];

        //* Состояние позиционера №2
        g_s_DataFromSensor.u8_StatusOfPositioner_2 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_2];

        //* Версия ПО
        g_s_DataFromSensor.u8_SoftwareVersion = u8_ArrayForReadFromSensor [ADDRESS_SOFTWARE_VERSION];

        //* Версия АО
        g_s_DataFromSensor.u8_AO_Version = u8_ArrayForReadFromSensor [ADDRESS_AO_VERSION];

        //* Температура окружающей среды
        g_s_DataFromSensor.u8_AmbientTemperature = u8_ArrayForReadFromSensor [ADDRESS_AMBIENT_TEMPERATURE];


        // Статус устройства (флаги состояния датчиков)
        // Занимает два байта, кладём два байта входного массива в 16-битную переменную
        union 
        {
          struct 
          {
            u8 u8_ArrayData[2];
          };
          u16 u16_Data;
        } status;

        //* Записываем два одиночных байта в двухбайтное число
        status.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE];
        status.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE+1];
        
        // Кладём правильное значение состояния в глобальный массив
        g_s_DataFromSensor.u16_DeviceStatus = status.u16_Data;
        // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%u", g_s_DataFromSensor.u16_DeviceStatus );


        // Т.к. в протоколе ModBus данные float передаются в нестандартной очерёдности байт,...
        union // ...преобразуем float ModBud (C-D-A-B) в стандартный float (A-B-C-D) с помощью наложения адресов
        {
          struct 
          {
            u8 u8_ArrayData[4];
          };
          float f_Data;
        } data;


        //* Меняем очерёдность следования байт в принимаемых float данных для позиционера №1
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+2];

        // Кладём правильное значение измеренного уровня позиционера №1 в глобальный массив
        g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 = data.f_Data;
        // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 );



        //* Меняем очерёдность следования байт в принимаемых float данных для позиционера №2
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+2];

        // Кладём правильное значение измеренного уровня позиционера №2 в глобальный массив
        g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 = data.f_Data;
        // OLED1_Font_16x12_printf (3, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 );



        //* Меняем очерёдность следования байт в принимаемых float данных для скорости звука
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+2];

        // Кладём правильное значение измеренного уровня скорости звука в глобальный массив
        g_s_DataFromSensor.f_SpeedOfSound = data.f_Data;
        // OLED1_Font_16x12_printf (5, 1, NORMAL, COMPACT, "%.2f", g_s_DataFromSensor.f_SpeedOfSound );


        // //* Меняем очерёдность следования байт в принимаемых float данных для температуры
        // data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+1];
        // data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE];
        // data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+3];
        // data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+2];

        // // Кладём правильное значение измеренного уровня температуры в глобальный массив
        // g_s_DataFromSensor.f_AmbientTemperature = data.f_Data;
        // // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%e", g_s_DataFromSensor.f_AmbientTemperature );


        //* Меняем очерёдность следования байт в принимаемых float данных для амплитуды выходного импульса
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+2];

        // Кладём правильное значение измеренного уровня амплитуды выходного импульса в глобальный массив
        g_s_DataFromSensor.f_OutputPulseAmplitude = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.4f", g_s_DataFromSensor.f_OutputPulseAmplitude );


        //* Меняем очерёдность следования байт в принимаемых float данных для Температура измерительного элемента
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+2];

        // Кладём правильное значение измеренного уровня Температура измерительного элемента в глобальный массив
        g_s_DataFromSensor.f_TemperatureOfMeasuringElement = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%f", g_s_DataFromSensor.f_TemperatureOfMeasuringElement );


        //* Меняем очерёдность следования байт в принимаемых float данных для Диапазон измерения
        data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+1];
        data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE];
        data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+3];
        data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+2];

        // Кладём правильное значение Диапазон измерения в глобальный массив
        g_s_DataFromSensor.f_MeasuringRange = data.f_Data;
        // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuringRange );

        return UART_RECEIVING_OK;
      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      }
      else
      {
        // Если контрольные суммы не совпадают, то выходим с сообщением об этой проблеме
        return UART_RECEIVING_CRC_ERROR;
        // OLED1_Font_8x6_printf(8, 90, "ERROR ");
      }
    }
  }
  return UART_RECEIVING_TIME_OUT_ERROR;
}



//* Приём данных портом UART2 от базовой станции
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    ReceivingDataFromBaseStationByUART2 - Приём данных от базовой станции через порт UART2
                        Распределение принятых данных в структуру ReceivedDataFromSensor (HW_Globals.h)

  @retval   UART_RECEIVING_OK							  Нормальное штатное завершение процедуры, данные приняты, CRC совпадает
  @retval   UART_RECEIVING_CRC_ERROR        Какие-то данные приняты, но не корректно, т.к. имеются ошибки контрольной суммы
  @retval   UART_RECEIVING_TIME_OUT_ERROR		Данные или вообще не пришли, или пришли частично, но приём остановился
  —————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
ModBus_Receiving_Return_Status ModBusReceivingDataFromSensorByUART2 ()
{
  u8 u8_ArrayForReadFromSensor [SIZE_OF_RECEIVE_BUFFER_UART2] = {0}; // чистим массив для приёма
  u8 u8_ByteCounter;    // переменная подсчёта принятых байт
  u32 u32_TickCounter;  // переменная отсчёта времени (мс) ожидания
  huart2.Instance = USART2;

  // Цикл чтения пакета данных от подчинённого устройства
  for (u8_ByteCounter = 0; u8_ByteCounter < SIZE_OF_RECEIVE_BUFFER_UART2; )
  {
    // Настраиваем значение счётчика окончания ожидания приёма байта от датчика
    u32_TickCounter = HAL_GetTick() + TIME_OUT_OF_RECEIVER_BYTE_UART2;

    // Крутимся в цикле пока не получим один байт в буфер приёма
    while (HAL_GetTick() < u32_TickCounter)
    {
      // Как дождались получение байта (появление флага занятости буфера), ....
      if (__HAL_UART_GET_FLAG (&huart2, UART_FLAG_RXNE))
      {
        // ... так сразу читаем этот байт в массив принятых данных
        HAL_UART_Receive(&huart2, &u8_ArrayForReadFromSensor [u8_ByteCounter++], 1, 500); // читаем
        break;
      }
    }

    // Если байты в буфер больше не поступают (время ожидания байта вышло), то начинаем анализ принятых данных
    if (HAL_GetTick() >= u32_TickCounter)
    {
      // Если не приняли ни одного байта, выход
      if (0 == u8_ByteCounter)
      {
        return UART_RECEIVING_TIME_OUT_ERROR;
      }
      
      union   // Готовим объединение (union) для распределения двух байт (мл. и ст.) полученной контрольной суммы
      {
        u16 u16_CRC;
        struct
        {
          u8 u8_Byte0;
          u8 u8_Byte1;
        };
      } crc;


        // Рассчитываем контрольную сумму массива (данных от датчика) без двух последних байт, в которых, собственно, и будет лежать контрольная сумма
        // Расчёт контрольной суммы в данном случае делается самым простым методом - суммой всех байт передаваемого массива с обрезкой до 16 бит
        // без учёта переполнения и без знака
        crc.u16_CRC=0;  // 2-байтная переменная под контрорльную сумму

        // Подсчитываем контрольную сумму данных всех байтов массива принятых данных
        u8 u8_Counter;
        for (u8_Counter = 0; u8_Counter < (u8_ByteCounter-2); u8_Counter++)
        {
          crc.u16_CRC += (u16)u8_ArrayForReadFromSensor [u8_Counter];
        }
        
        // Получившуюся сумму кладём в две u8 переменных
        crc.u8_Byte0 = (u8) (crc.u16_CRC & 0xFF);
        crc.u8_Byte1 = (u8) ((crc.u16_CRC >> 8) & 0xFF);

        //OLED1_Font_8x6_printf(8, 70, "%u", u8_Counter); // кол-во полученных пакетов данных от датчика

        OLED1_Font_8x6_printf(5, 0, "%u байт, %04X", u8_Counter, crc.u16_CRC); // кол-во полученных пакетов данных от датчика


      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // Если вычисленная и пришедшая контрольные суммы совпадают,
      // переписываем данные из входного массива u8_ArrayForReadFromSensor в глобальный массив данных g_s_DataFromSensor
      // if (u8_ArrayForReadFromSensor [u8_ByteCounter-2] == crc.u8_Byte0 && u8_ArrayForReadFromSensor [u8_ByteCounter-1] == crc.u8_Byte1)
      // {
      //   //* Адрес выдающего данные подчинённого устройства
      //   g_s_DataFromSensor.u8_AddressOfSlaveDevice = u8_ArrayForReadFromSensor [ADDRESS_ADDRESS_SLAVE_DEVICE];

      //   //* Адрес банка читаемых регистров
      //   g_s_DataFromSensor.u8_FunctionalCode = u8_ArrayForReadFromSensor [ADDRESS_FUNCTIONAL_CODE];

      //   //* Количество принятых от подчинённого устройства байт
      //   g_s_DataFromSensor.u8_NumberOfBytesReceived = u8_ByteCounter;

      //   //* Состояние позиционера №1
      //   g_s_DataFromSensor.u8_StatusOfPositioner_1 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_1];

      //   //* Состояние позиционера №2
      //   g_s_DataFromSensor.u8_StatusOfPositioner_2 = u8_ArrayForReadFromSensor [ADDRESS_STATUS_POSITIONER_2];

      //   //* Версия ПО
      //   g_s_DataFromSensor.u8_SoftwareVersion = u8_ArrayForReadFromSensor [ADDRESS_SOFTWARE_VERSION];

      //   //* Версия АО
      //   g_s_DataFromSensor.u8_AO_Version = u8_ArrayForReadFromSensor [ADDRESS_AO_VERSION];

      //   //* Температура окружающей среды
      //   g_s_DataFromSensor.u8_AmbientTemperature = u8_ArrayForReadFromSensor [ADDRESS_AMBIENT_TEMPERATURE];


      //   // Статус устройства (флаги состояния датчиков)
      //   // Занимает два байта, кладём два байта входного массива в 16-битную переменную
      //   union 
      //   {
      //     struct 
      //     {
      //       u8 u8_ArrayData[2];
      //     };
      //     u16 u16_Data;
      //   } status;

      //   //* Записываем два одиночных байта в двухбайтное число
      //   status.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE];
      //   status.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_STATUS_DEVISE+1];
        
      //   // Кладём правильное значение состояния в глобальный массив
      //   g_s_DataFromSensor.u16_DeviceStatus = status.u16_Data;
      //   // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%u", g_s_DataFromSensor.u16_DeviceStatus );


      //   // Т.к. в протоколе ModBus данные float передаются в нестандартной очерёдности байт,...
      //   union // ...преобразуем float ModBud (C-D-A-B) в стандартный float (A-B-C-D) с помощью наложения адресов
      //   {
      //     struct 
      //     {
      //       u8 u8_ArrayData[4];
      //     };
      //     float f_Data;
      //   } data;


      //   //* Меняем очерёдность следования байт в принимаемых float данных для позиционера №1
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_1+2];

      //   // Кладём правильное значение измеренного уровня позиционера №1 в глобальный массив
      //   g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 = data.f_Data;
      //   // OLED1_Font_16x12_printf (1, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_1 );



      //   //* Меняем очерёдность следования байт в принимаемых float данных для позиционера №2
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_POSITIONER_2+2];

      //   // Кладём правильное значение измеренного уровня позиционера №2 в глобальный массив
      //   g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 = data.f_Data;
      //   // OLED1_Font_16x12_printf (3, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuredValueOfPositioner_2 );



      //   //* Меняем очерёдность следования байт в принимаемых float данных для скорости звука
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_SPEED_OF_SOUND+2];

      //   // Кладём правильное значение измеренного уровня скорости звука в глобальный массив
      //   g_s_DataFromSensor.f_SpeedOfSound = data.f_Data;
      //   // OLED1_Font_16x12_printf (5, 1, NORMAL, COMPACT, "%.2f", g_s_DataFromSensor.f_SpeedOfSound );


      //   // //* Меняем очерёдность следования байт в принимаемых float данных для температуры
      //   // data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+1];
      //   // data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE];
      //   // data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+3];
      //   // data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_AMBIENT_TEMPERATURE+2];

      //   // // Кладём правильное значение измеренного уровня температуры в глобальный массив
      //   // g_s_DataFromSensor.f_AmbientTemperature = data.f_Data;
      //   // // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%e", g_s_DataFromSensor.f_AmbientTemperature );


      //   //* Меняем очерёдность следования байт в принимаемых float данных для амплитуды выходного импульса
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_LEVEL_OUT_PULSE+2];

      //   // Кладём правильное значение измеренного уровня амплитуды выходного импульса в глобальный массив
      //   g_s_DataFromSensor.f_OutputPulseAmplitude = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.4f", g_s_DataFromSensor.f_OutputPulseAmplitude );


      //   //* Меняем очерёдность следования байт в принимаемых float данных для Температура измерительного элемента
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_ELEMENT_TEMP+2];

      //   // Кладём правильное значение измеренного уровня Температура измерительного элемента в глобальный массив
      //   g_s_DataFromSensor.f_TemperatureOfMeasuringElement = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%f", g_s_DataFromSensor.f_TemperatureOfMeasuringElement );


      //   //* Меняем очерёдность следования байт в принимаемых float данных для Диапазон измерения
      //   data.u8_ArrayData[0] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+1];
      //   data.u8_ArrayData[1] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE];
      //   data.u8_ArrayData[2] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+3];
      //   data.u8_ArrayData[3] = u8_ArrayForReadFromSensor[ADDRESS_MEAS_RANGE+2];

      //   // Кладём правильное значение Диапазон измерения в глобальный массив
      //   g_s_DataFromSensor.f_MeasuringRange = data.f_Data;
      //   // OLED1_Font_16x12_printf (7, 1, NORMAL, COMPACT, "%.5f", g_s_DataFromSensor.f_MeasuringRange );

      //   return UART_RECEIVING_OK;
      // // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // // *——————————————————————————————————————————————————————————————————————————————————————————————————————————————
      // }
      // else
      // {
      //   // Если контрольные суммы не совпадают, то выходим с сообщением об этой проблеме
      //   return UART_RECEIVING_CRC_ERROR;
      //   // OLED1_Font_8x6_printf(8, 90, "ERROR ");
      // }
    }
  }
  return UART_RECEIVING_TIME_OUT_ERROR;
}


//* Подсчёт контрольной суммы ModBus
/** ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  @brief    ModBusChecksumCalculation - подсчёт контрольной суммы массива данных ModBus

  @param    1)*p_u8_InputData:      указатель на массив данных (uint8*)
  @param    2)u8_LengthInputData:   количество байт в массиве (uint8)

  @retval   Контрольная сумма (u16), 

  @example
  OLED1_Font_8x6_printf(8, 1, "%4X", ModBusChecksumCalculation (u8_ArrayForReadFromSensor, u8_ByteCounter-2));
————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
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





/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————
  Function Name  |  Read_UART_BufferPointer
—————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
  Description    | • Читает буфер данных, принятых по UART, заполняет структуру UART_ReceiveVALs (см. HW_UART.h)
—————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
  Arguments      | 1) Константа порта (UART0 или UART1)
—————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
  Return         | Возвращается структура UART_ReceiveVALs:
                 |
                 | // Адрес нулевого элемента буфера приема
                 | u8* pu8_UART_ReceiveRingBufferAddress -
                 |
                 | // Номер элемента в буфере приема, следующего за последним принятым
                 | // (например, если индекс последнего принятого 12, то UART_ReceiveRingBufferLastIndex=13).
                 | u16 u16_UART_ReceiveRingBufferLastIndex;
                 |
                 | // Размер кольцевого приемного буфера
                 | u16 u16_SizeUartReceiveRingBuffer;
                 |
                 | -------------- Пример применения функции (для UART0): ---------------------------------------
                 |
                 | struct UART_ReceiveVALs STR; // Инициализация структуры с именем STR по прототипу UART_ReceiveVALs
                 | u8 u8_Cnt;                   // Служебная переменная для организации цикла
                 |
                 | // Проверяем, пришло ли что-либо в буфер UART0
                 | if (BUFFER_NOT_EMPTY == Check_UART_Buffer (UART0))
                 | {
                 |   // Получение ссылок на данные, принятых по UART0, в структуру STR
                 |   STR = Read_UART_BufferPointer (UART0);
                 |
                 |   // Выводим данные на ЖКИ
                 |   // Цикл от нуля до последнего принятого элемента
                 |   for (u8_Cnt=0; u8_Cnt < STR.u16_UART_ReceiveRingBufferLastIndex; u8_Cnt++)
                 |   {
                 |     // Берем по адресу+смещение принятый байт и кладем в строку вывода
                 |     sprintf (caString, "%u", *(STR.pu8_UART_ReceiveRingBufferAddress+u8_Cnt));
                 |     lcd_print (u8_Cnt, 0, caString);
                 |   }
                 |
                 |   // Сброс флага прочитанных байтов по UART0 (ОБЯЗАТЕЛЬНО !!!!!!!)
                 |   g_UART0_BufferEmptyFlag = BUFFER_EMPTY;
                 | }
                 |
———————————————————————————————————————————————————————————————————————————————————————————————————————————————*/
// struct UART_ReceiveVALs Read_UART_BufferPointer (UART_TypeDef* UARTx)
// {
//   struct UART_ReceiveVALs ReturnData={0};

//   if (UART0 == UARTx)
//   {
//     // Возвращаем указатель на начальный элемент буфера
//     ReturnData.pu8_UART_ReceiveRingBufferAddress = &g_u8_UART0_ReceiveRingBuffer[0];
//     // Возвращаем номер первого пустого (не заполненного еще портом) элемента
//     ReturnData.u16_UART_ReceiveRingBufferLastIndex = g_u16_UART0_ReceiveRingBufferIndex;
//     // Возвращаем размер буфера в байтах
//     ReturnData.u16_SizeUartReceiveRingBuffer = (u16) SIZE_RECEIVE_RING_BUFFER_UART0;
//   }
//   else
//   {
//     // Возвращаем указатель на начальный элемент буфера
//     ReturnData.pu8_UART_ReceiveRingBufferAddress = &g_u8_UART1_ReceiveRingBuffer[0];
//     // Возвращаем номер первого пустого (не заполненного еще портом) элемента
//     ReturnData.u16_UART_ReceiveRingBufferLastIndex = g_u16_UART1_ReceiveRingBufferIndex;
//     // Возвращаем размер буфера в байтах
//     ReturnData.u16_SizeUartReceiveRingBuffer = (u16) SIZE_RECEIVE_RING_BUFFER_UART1;
//   }

// 	return ReturnData;
// }

// /*——————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   Function Name  |  Check_UART_Buffer
// —————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
//   Description    | • Проверяет, есть ли новые данные, пришедшие по UART
// —————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
//   Arguments      | 1) Константа порта (UART0 или UART1)
// —————————————————+——————————————————————————————————————————————————————————————————————————————————————————————
//   Return         | 1) BUFFER_EMPTY - Новой информации в буфере нет
//                  | 2) BUFFER_NOT_EMPTY - Есть новая информация в буфере
// ———————————————————————————————————————————————————————————————————————————————————————————————————————————————*/
// UART_Buffer_Empty_Flag Check_UART_Buffer (UART_TypeDef* UARTx)
// {
//   if (UART0 == UARTx)
//     return g_UART0_BufferEmptyFlag;
//   else
//     return g_UART1_BufferEmptyFlag;
// }


/*********************************** END OF FILE ************************************/
