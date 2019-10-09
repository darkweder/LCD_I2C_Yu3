#include "lcd.h"
//------------------------------------------------
uint8_t buf[1]={0};
extern I2C_HandleTypeDef hi2c1;
char str1[100];
uint8_t portlcd = 0; //������ ��� �������� ������ ����� ���������� ����������
extern volatile uint16_t Adr_i2c; //����� ����� PCF8574T
//------------------------------------------------
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
	micros *=(SystemCoreClock / 1000000) / 5;
	while (micros--);
}
//------------------------------------------------
void LCD_WriteByteI2CLCD(uint8_t bt)
{
	buf[0]=bt;
	//HAL_I2C_Master_Transmit(&hi2c1,(uint16_t) 0x4E,buf,1,1000);
    HAL_I2C_Master_Transmit(&hi2c1, Adr_i2c * 2,buf,1,1000);
}
//------------------------------------------------
void sendhalfbyte(uint8_t c)
{
	c<<=4;
	e_set();//�������� ����� E
	DelayMicro(50);
	LCD_WriteByteI2CLCD(portlcd|c);
	e_reset();//��������� ����� E
	DelayMicro(50);
}
//------------------------------------------------
void sendbyte(uint8_t c, uint8_t mode)
{
	if(mode==0) rs_reset();
	else rs_set();
	uint8_t hc=0;
	hc=c>>4;
	sendhalfbyte(hc);sendhalfbyte(c);
}
//------------------------------------------------
void LCD_Clear(void)
{
	sendbyte(0x01,0);
	DelayMicro(1500); 
}
//------------------------------------------------
void LCD_SendChar(char ch)
{
	sendbyte(ch,1);
}
//------------------------------------------------
void LCD_String(char* st)
{
	uint8_t i=0;
	while(st[i]!=0)
	{
		sendbyte(st[i],1);
		i++;
	}
}
//------------------------------------------------
void LCD_SetPos(uint8_t x, uint8_t y)
{
	switch(y)
	{
		case 0:
			sendbyte(x|0x80,0);
			HAL_Delay(1);
			break;
		case 1:
			sendbyte((0x40+x)|0x80,0);
			HAL_Delay(1);
			break;
		case 2:
			sendbyte((0x14+x)|0x80,0);
			HAL_Delay(1);
			break;
		case 3:
			sendbyte((0x54+x)|0x80,0);
			HAL_Delay(1);
			break;
	}
}
//------------------------------------------------
void LCD_ini(void)
{
	HAL_Delay(15);
	sendhalfbyte(0x03);
	HAL_Delay(4);
	sendhalfbyte(0x03);
	DelayMicro(100);
	sendhalfbyte(0x03);
	HAL_Delay(1);
	sendhalfbyte(0x02);
	HAL_Delay(1);
	sendbyte(0x28,0);//����� 4 ���, 2 ����� (��� ������ �������� ������� ��� 4 �����, ����� 5�8	
	HAL_Delay(1);
	sendbyte(0x0C,0);//������� �������� (D=1), ������� ������� �� �����
	HAL_Delay(1);
	sendbyte(0x01,0);//������ �����
	HAL_Delay(2);
	sendbyte(0x06,0);//����� �����
	HAL_Delay(1);
	setled();//���������
	setwrite();//������
}
//------------------------------------------------
void lcd_detect(I2C_HandleTypeDef *hi2c)
{

  	HAL_StatusTypeDef result;
	HAL_Delay(500);	//500ms to let VDD stabilize and LCD to power up

  	//searching 7-Bit address space
   	for (uint8_t i=0; i<128; i++)
   	{
   	  /*
   	   * retries 2
   	   * timeout 200ms
   	   */

   		result = HAL_I2C_IsDeviceReady(hi2c, (uint16_t) i, 2, 200);
   		if (result != HAL_OK) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
   		{
   			//do nothing
   		}
   		if (result == HAL_OK)
   		{
   			Adr_i2c = i/2;
   			LCD_ini();

   			//binary_to_string(address);
   			i++; //skip the read adress of same device
   		}
   	}
}

