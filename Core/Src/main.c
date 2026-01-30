/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"
#include "bmp.h"

/* BSP_LCD_... */
#include "stm32_adafruit_lcd.h"

/* BMP180 */
#include "BMP180.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Freertos also measures cpu usage
   - 0: measure off
   - 1: measure on */
#define POWERMETER    1

/* Chapter delays */
#define DELAY_CHAPTER    1000

#define Delay(t)              HAL_Delay(t)
#define GetTime()             HAL_GetTick()

#define POWERMETER_START
#define POWERMETER_STOP
#define POWERMETER_REF
#define POWERMETER_PRINT      Delay(10); printf("\r\n")

#if LCD_REVERSE16 == 0
#define RD(a)                 a
#endif

/* 16bit data byte change */
#if LCD_REVERSE16 == 1
#define RD(a)                 __REVSH(a)
#endif

/* BMP180 */
float Temperature = 0;
float Pressure = 0;
float Altitude = 0;

char Temperature1[10];
char Pressure1[10];
char Altitude1[10];

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
void mainApp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint32_t PixelTest(uint32_t n)
{
  uint16_t c, x, y;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    x = random() % BSP_LCD_GetXSize();
    y = random() % BSP_LCD_GetYSize();
    c = random() % 0xFFFF;
    BSP_LCD_DrawPixel(x, y, c);
  }
  return(GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t LineTest(uint32_t n)
{
  uint16_t x1, y1, x2, y2;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    x1 = random() % BSP_LCD_GetXSize();
    y1 = random() % BSP_LCD_GetYSize();
    x2 = random() % BSP_LCD_GetXSize();
    y2 = random() % BSP_LCD_GetYSize();
    BSP_LCD_SetTextColor(RD(random() % 0xFFFF));
    BSP_LCD_DrawLine(x1, y1, x2, y2);
  }
  return(GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t FillRectTest(uint32_t n)
{
  uint16_t x, y, w, h;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    w = random() % (BSP_LCD_GetXSize() >> 1);
    h = random() % (BSP_LCD_GetYSize() >> 1);
    x = random() % (BSP_LCD_GetXSize() - w);
    y = random() % (BSP_LCD_GetYSize() - h);
    BSP_LCD_SetTextColor(RD(random() % 0xFFFF));
    BSP_LCD_FillRect(x, y, w, h);
  }
  return(GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t CharTest(uint32_t n)
{
  uint16_t x, y;
  uint8_t  c;
  sFONT * fp;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    c = random() % 5;
    if(c == 0)
      BSP_LCD_SetFont(&Font8);
    else if(c == 1)
      BSP_LCD_SetFont(&Font12);
    else if(c == 2)
      BSP_LCD_SetFont(&Font16);
    else if(c == 3)
      BSP_LCD_SetFont(&Font20);
    else if(c == 4)
      BSP_LCD_SetFont(&Font24);
    fp = BSP_LCD_GetFont();

    x = random() % (BSP_LCD_GetXSize() - fp->Width);
    y = random() % (BSP_LCD_GetYSize() - fp->Height);
    BSP_LCD_SetTextColor(RD(random() % 0xFFFF));
    BSP_LCD_SetBackColor(RD(random() % 0xFFFF));

    c = random() % 96 + ' ';
    BSP_LCD_DisplayChar(x, y, c);
  }
  return(GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t CircleTest(uint32_t n)
{
  uint16_t c, x, y, r, rmax;

  rmax = BSP_LCD_GetXSize();
  if(rmax > BSP_LCD_GetYSize())
    rmax = BSP_LCD_GetYSize();
  rmax >>= 2;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    do
      r = random() % rmax;
    while(r == 0);

    x = random() % (BSP_LCD_GetXSize() - (r << 1)) + r;
    y = random() % (BSP_LCD_GetYSize() - (r << 1)) + r;
    c = random() % 0xFFFF;
    BSP_LCD_SetTextColor(RD(c));
    BSP_LCD_DrawCircle(x, y, r);
  }
  return(GetTime() - ctStartT);
}

//-----------------------------------------------------------------------------
uint32_t FillCircleTest(uint32_t n)
{
  uint16_t c, x, y, r, rmax;

  rmax = BSP_LCD_GetXSize();
  if(rmax > BSP_LCD_GetYSize())
    rmax = BSP_LCD_GetYSize();
  rmax >>= 2;

  uint32_t ctStartT = GetTime();
  for(uint32_t i = 0; i < n; i++)
  {
    do
      r = random() % rmax;
    while(r == 0);

    x = random() % (BSP_LCD_GetXSize() - (r << 1)) + r;
    y = random() % (BSP_LCD_GetYSize() - (r << 1)) + r;
    c = random() % 0xFFFF;
    BSP_LCD_SetTextColor(RD(c));
    BSP_LCD_FillCircle(x, y, r);
  }
  return(GetTime() - ctStartT);
}


//-----------------------------------------------------------------------------

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75


int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;

int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;

float Ax, Ay, Az, Gx, Gy, Gz;

void MPU6050_Init (void)
{
  uint8_t check;
  uint8_t Data;

  // check device ID WHO_AM_I
  HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);
  if (check == 0x68)  // 0x68 will be returned by the sensor if everything goes well
  {
    // power management register 0X6B we should write all 0's to wake the sensor up
    Data = 0;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1,&Data, 1, 1000);

    // Set DATA RATE of 1KHz by writing SMPLRT_DIV register
    Data = 0x07;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x19, 1, &Data, 1, 1000);

    // Set Gyroscopic configuration in GYRO_CONFIG Register
    Data = 0x00;  // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 250 ̐/s
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1B, 1, &Data, 1, 1000);

    // Set accelerometer configuration in ACCEL_CONFIG Register
    Data = 0x00;  // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1C, 1, &Data, 1, 1000);
  }

}

void MPU6050_Read_Accel (void)
{
  uint8_t Rec_Data[6];

  // Read 6 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
  HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, 0x3B, 1, Rec_Data, 6, 1000);

  Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
  Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
  Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

  /*** convert the RAW values into acceleration in 'g'
       we have to divide according to the Full scale value set in FS_SEL
       I have configured FS_SEL = 0. So I am dividing by 16384.0
       for more details check ACCEL_CONFIG Register              ****/

  Ax = (float)Accel_X_RAW/16384.0;
  Ay = (float)Accel_Y_RAW/16384.0;
  Az = (float)Accel_Z_RAW/16384.0;
}

void MPU6050_Read_Gyro (void)
{
  uint8_t Rec_Data[6];

  // Read 6 BYTES of data starting from GYRO_XOUT_H register
  HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, 0x43, 1, Rec_Data, 6, 1000);

  Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
  Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
  Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

  /*** convert the RAW values into dps (ｰ/s)
       we have to divide according to the Full scale value set in FS_SEL
       I have configured FS_SEL = 0. So I am dividing by 131.0
       for more details check GYRO_CONFIG Register              ****/

  Gx = (float)Gyro_X_RAW/131.0;
  Gy = (float)Gyro_Y_RAW/131.0;
  Gz = (float)Gyro_Z_RAW/131.0;
}

//-----------------------------------------------------------------------------

#define DS3231_ADDRESS 0xD0

// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hour;
  uint8_t dayofweek;
  uint8_t dayofmonth;
  uint8_t month;
  uint8_t year;
} TIME;

TIME time;

// function to set time

void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
  uint8_t set_time[7];
  set_time[0] = decToBcd(sec);
  set_time[1] = decToBcd(min);
  set_time[2] = decToBcd(hour);
  set_time[3] = decToBcd(dow);
  set_time[4] = decToBcd(dom);
  set_time[5] = decToBcd(month);
  set_time[6] = decToBcd(year);

  HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

void Get_Time (void)
{
  uint8_t get_time[7];
  HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
  time.seconds = bcdToDec(get_time[0]);
  time.minutes = bcdToDec(get_time[1]);
  time.hour = bcdToDec(get_time[2]);
  time.dayofweek = bcdToDec(get_time[3]);
  time.dayofmonth = bcdToDec(get_time[4]);
  time.month = bcdToDec(get_time[5]);
  time.year = bcdToDec(get_time[6]);
}

float Get_Temp (void)
{
  uint8_t temp[2];

  HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);
  return ((temp[0])+(temp[1]>>6)/4.0);
}

void force_temp_conv (void)
{
  uint8_t status=0;
  uint8_t control=0;
  HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);  // read status register
  if (!(status&0x04))
  {
    HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x0E, 1, &control, 1, 100);  // read control register
    // HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x0E, 1, (uint8_t *)(control|(0x20)), 1, 100);
    HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x0E, 1, (uint8_t *)&control, 1, 100);
  }
}

float TEMP;
// char buffer[15];

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  MPU6050_Init();
  BMP180_Start();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t t;

  Delay(300);

  BSP_LCD_Init();

  t = random();

  Delay(100);
  printf("Display ID = %X\r\n", (unsigned int)BSP_LCD_ReadID());

  char line_0[15];
  char line_1[15];
  char line_2[15];
  char line_3[15];
  char line_4[15];
  char line_5[15];
  char line_6[15];
  char line_7[15];
  char line_8[15];
  char line_9[15];
  char line_10[15];
  
  BSP_LCD_SetFont(&Font20);

  // snprintf(line_0, sizeof(line_0), "MPU6050 TEST");
  // BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)line_0, LEFT_MODE);

  //uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year
  // Set_Time(00, 52, 14, 3, 21, 1, 26);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    // HAL_Delay(1000);
    // continue;

    {
      // read the Accelerometer and Gyro values
      // MPU6050_Read_Accel();
      // MPU6050_Read_Gyro();

      // if (Ay > 0) {
      //   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
      // }
      // else {
      //   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
      // }

      Get_Time();
      sprintf (line_0, "%d-%d-20%d %02d:%02d:%02d ", time.dayofmonth, time.month, time.year, time.hour, time.minutes, time.seconds);
      BSP_LCD_DisplayStringAtLine(0, (uint8_t *)line_0);

      // snprintf(line_1, sizeof(line_1), "Ax=%.3fg       ", Ax);
      // BSP_LCD_DisplayStringAtLine(1, (uint8_t *)line_1);
      // snprintf(line_2, sizeof(line_2), "Ay=%.3fg       ", Ay);
      // BSP_LCD_DisplayStringAtLine(2, (uint8_t *)line_2);
      // snprintf(line_3, sizeof(line_3), "Az=%.3fg       ", Az);
      // BSP_LCD_DisplayStringAtLine(3, (uint8_t *)line_3);
      // snprintf(line_4, sizeof(line_4), "Gx=%.3fg       ", Gx);
      // BSP_LCD_DisplayStringAtLine(4, (uint8_t *)line_4);
      // snprintf(line_5, sizeof(line_5), "Gy=%.3fg       ", Gy);
      // BSP_LCD_DisplayStringAtLine(5, (uint8_t *)line_5);
      // snprintf(line_6, sizeof(line_6), "Gz=%.3fg       ", Gz);
      // BSP_LCD_DisplayStringAtLine(6, (uint8_t *)line_6);

      // c = random() % 96 + ' ';
      // snprintf(line_7, sizeof(line_7), "random: %c", c);
      // snprintf(line_7, sizeof(line_7), "%c%c", 0x50,0b01110000);
      // BSP_LCD_DisplayStringAtLine(7, (uint8_t *)line_7);

      // sprintf (buffer, "Time: %02d:%02d:%02d", time.hour, time.minutes, time.seconds);
      // BSP_LCD_DisplayStringAtLine(8, (uint8_t *)buffer);
      // sprintf (buffer, "Date: %d-%d-20%d", time.dayofmonth, time.month, time.year);
      // BSP_LCD_DisplayStringAtLine(9, (uint8_t *)buffer);

      force_temp_conv();
      TEMP = Get_Temp();
      sprintf (line_7, "Temp: %.2fC", TEMP);
      BSP_LCD_DisplayStringAtLine(7, (uint8_t *)line_7);

      /* BMP180 */
      Temperature = BMP180_GetTemp();
      Pressure = BMP180_GetPressMmhg(3);
      Altitude = BMP180_GetAlt(3);

      sprintf (line_8, "Temperature: %.2fC", Temperature);
      BSP_LCD_DisplayStringAtLine(8, (uint8_t *)line_8);
      sprintf (line_9, "Pressure: %.2fmmHg", Pressure);
      BSP_LCD_DisplayStringAtLine(9, (uint8_t *)line_9);
      sprintf (line_10, "Altitude: %.2fm", Altitude);
      BSP_LCD_DisplayStringAtLine(10, (uint8_t *)line_10);


      HAL_Delay (100);  // wait for a while

      continue;
    }


    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = PixelTest(100000);
    POWERMETER_STOP;
    printf("Pixel Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = LineTest(1000);
    POWERMETER_STOP;
    printf("Line Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = FillRectTest(250);
    POWERMETER_STOP;
    printf("Fill Rect Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = CircleTest(DELAY_CHAPTER);
    POWERMETER_STOP;
    printf("Circle Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = FillCircleTest(250);
    POWERMETER_STOP;
    printf("Fill Circle Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    POWERMETER_START;
    t = CharTest(5000);
    POWERMETER_STOP;
    printf("Char Test: %d ms", (int)t);
    POWERMETER_PRINT;
    Delay(DELAY_CHAPTER);

  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
