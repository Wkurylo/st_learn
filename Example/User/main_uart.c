/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static void SystemClock_Config(void);
void uart_setup(UART_HandleTypeDef *huart);
void send_string_confirmation();
void send_string(char *s);
void send_confirmation(char *received_string);
void clear_table(char *received_string);
void send_uint(uint8_t *i);

/* Private functions ---------------------------------------------------------*/
UART_HandleTypeDef huart;
static uint16_t cnt = 0; 			// Licznik wyslanych wiadomosci
char data_received[50];			// Tablica przechowujaca odebrane wiadomosci
char data_sent[50];				// Tablica przechowujaca wysylanych wiadomosci
uint16_t size =0;					// Rozmiar wysylanej wiadomosci
HAL_StatusTypeDef status_uart;
char received_string[50];
char welcome_string[] = "Podaj pojedyncza cyfre 0- 9 lub grupe np: 2 3 4 i wcisnij return:";
char only_digits[] = "Jedynie cyfry akceptuje. Sprobuj ponownie";
char debug_1[] = "Jestem w lini 116";
char debug_2[] = "Jestem w lini 121";
char digit_confirmation[] = "Great ! you passed digit";
char debug_3[] = "Orzymalem /r EOL";


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t counter = 0;

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
  */
  HAL_Init();

  /* Configure the System clock to have a frequency of 80 MHz */
  SystemClock_Config();

  /* Add your application code here
     */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);  
  BSP_JOY_Init(JOY_MODE_EXTI);
  
  uart_setup(&huart);
  //uart_status = HAL_UART_Init(&huart);
  BSP_COM_Init(COM1, &huart);

  HAL_UART_Transmit(&huart, (uint8_t*)&welcome_string, strlen(welcome_string), 500);
  send_string("\r\n");

    /* Infinite loop */
  while (1)
  {
	  BSP_LED_Toggle(LED1);
	  HAL_Delay(100);
	  HAL_UART_Transmit(&huart, (uint8_t *)debug_1, strlen(debug_1), 500);
	  send_string("\r\n");

      while (1)
      {
    	  BSP_LED_Toggle(LED2);

    	  // COUNTER -DEBUG
    	  //send_uint(&counter);
    	  HAL_UART_Transmit(&huart, &counter, 1, 200);
    	  // COUNTER -DEBUG

    	  // zwraca status == uart , wkaznik ze received_string to unsigned int-8bit , 1 bit (1 znak), 100 ms timeout)
    	  status_uart = HAL_UART_Receive(&huart, (uint8_t *)&received_string[counter], 1, 100);

    	  if(status_uart == HAL_OK)
    	  {

    		HAL_UART_Transmit(&huart, (uint8_t *)&received_string[counter], 1, 500); // ECHO tego co wpisuje
    		send_string("\r\n");
    		if ((received_string[counter] >= '0' && received_string[counter] <= '9') || received_string[counter] == '\r')
    		{
    			//HAL_UART_Transmit(&huart, (uint8_t *)&digit_confirmation, strlen(digit_confirmation),500); // Sprawdza czy ostatanio wrowadzona wartosc to cyfra, jak tak to daje komunikat, jak nie to else
    			//send_string("\r\n");
    			if (received_string[counter] == '\r')
    			{
    				HAL_UART_Transmit(&huart, (uint8_t *)debug_3, strlen(debug_3), 500);
					send_confirmation(received_string);

					// VVVVV TUTAJ DODAC
					int32_t values;
					sscanf(received_string,"%d", &values); // %d oznacza, ze bedzie tu wartosc mieszczaca sie w zakresie int32

					// Wyslanie wiadomosci:
					char data_calculated[50];
					sprintf(data_calculated, "Liczba przemnozona x2: %d.\n\r", 2 * values);
					HAL_UART_Transmit(&huart, data_calculated, strlen(data_calculated), 2000);

					// ^^^^^^ TUTAJ DODAC

					clear_table(received_string);
					counter = 0;
					break;
    			} else {
    				// Modulo 0 - 50
    				counter = (counter + 1) % sizeof(received_string);
    			}
    		} else {
    			HAL_UART_Transmit(&huart, (uint8_t *)&only_digits, strlen(only_digits),500); // Infromacja o przyjomowaniu jedynie liczb
    			send_string("\r\n");
    			counter = 0;
    		}
    	  }
      }
  }
}


void uart_setup(UART_HandleTypeDef *huart)
{
	huart->Init.BaudRate = 115200;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
}

void send_string_confirmation()
{
	++cnt;					// Zwiekszanie licznika wiadomosci
	size = sprintf(data_sent, "Liczba wyslanych wiadomosci: %d.\n\r", cnt); // Stworzenie wiadomosci do wyslania oraz przypisanie ilosci wysylanych znakow do zmiennej size.
	HAL_UART_Transmit(&huart,(uint8_t*)data_sent,size,2000);
}

void send_confirmation(char *received_string)
{
	send_string("\r");
	send_string_confirmation();
	send_string("Odebrano wiadomosc : ");
	send_string(received_string);
	send_string("\r\n");
}

void clear_table(char *received_string)
{
	memset(received_string, 0, sizeof(*received_string)); // Uzupelnianie tabeli wartoscia 0 na miejsca okreslone w trzecim argumencie (np cala tabele)
}

void send_string(char *s)
{
	HAL_UART_Transmit(&huart, (uint8_t*)s, strlen(s), 2000);
}
void send_uint(uint8_t *i)
{

	HAL_UART_Transmit(&huart, i, 1 , 500);
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
