#include "BSP_OLED.h"
#include "BSP_DWT.h"


/*��������*********************/

/**
  * ��    ����OLEDдSCL�ߵ͵�ƽ
  * ��    ����Ҫд��SCL�ĵ�ƽֵ����Χ��0/1
  * �� �� ֵ����
  * ˵    �������ϲ㺯����ҪдSCLʱ���˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ����SCL��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ����SCLΪ�͵�ƽ������������1ʱ����SCLΪ�ߵ�ƽ
  */
void OLED_W_SCL(uint8_t BitValue)
{
	/*����BitValue��ֵ����SCL�øߵ�ƽ���ߵ͵�ƽ*/
	HAL_GPIO_WritePin(OLED_SCK_GPIO_Port, OLED_SCK_Pin, (GPIO_PinState)BitValue);
	DWT_Delay_us(2);
	/*�����Ƭ���ٶȹ��죬���ڴ�����������ʱ���Ա��ⳬ��I2Cͨ�ŵ�����ٶ�*/
	//...
}

/**
  * ��    ����OLEDдSDA�ߵ͵�ƽ
  * ��    ����Ҫд��SDA�ĵ�ƽֵ����Χ��0/1
  * �� �� ֵ����
  * ˵    �������ϲ㺯����ҪдSDAʱ���˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ����SDA��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ����SDAΪ�͵�ƽ������������1ʱ����SDAΪ�ߵ�ƽ
  */
void OLED_W_SDA(uint8_t BitValue)
{
	/*����BitValue��ֵ����SDA�øߵ�ƽ���ߵ͵�ƽ*/
	HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, (GPIO_PinState)BitValue);
	DWT_Delay_us(2);
	/*�����Ƭ���ٶȹ��죬���ڴ�����������ʱ���Ա��ⳬ��I2Cͨ�ŵ�����ٶ�*/
	//...
}


/**
  * º¯    ʽ£ºOLEDҽ½ųõʼ»¯
  * ²Π   ʽ£ºΞ
  * ·µ »ؠֵ£ºΞ
  * ˵    ÷£ºµ±ɏ²㺯ʽШҪ³õʼ»¯ʱ£¬´˺¯ʽ»ᱻµ÷Ӄ
  *           Ӄ»§ШҪ½«SCLº͓DAҽ½ųõʼ»¯Ϊ¿ª©ģʽ£¬²¢ʍ·Œý½ō
  */
void OLED_GPIO_Init(void)
{
	uint32_t i, j;
	
	/*Ԛ³õʼ»¯ǰ£¬¼ӈ늊Á¿ѓʱ£¬´ýOLED¹©µ玈¶¨*/
	for (i = 0; i < 2000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
	
//	/*½«SCLº͓DAҽ½ųõʼ»¯Ϊ¿ª©ģʽ*/
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*ʍ·œCLº͓DA*/
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/*********************ҽ½Ņ䖃*/


/*ͨЅЭҩ*********************/

/**
  * º¯    ʽ£ºI2Cưʼ
  * ²Π   ʽ£ºΞ
  * ·µ »ؠֵ£ºΞ
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);		//ʍ·œDA£¬ȷ±£SDAΪ¸ߵ熽
	OLED_W_SCL(1);		//ʍ·œCL£¬ȷ±£SCLΪ¸ߵ熽
	OLED_W_SDA(0);		//ԚSCL¸ߵ熽ƚ¼䣬À­µ͓DA£¬²úɺưʼЅºō
	OLED_W_SCL(0);		//ưʼº󰑓CLҲÀ­µͣ¬¼´ΪÁ˕¼Ӄלϟ£¬ҲΪÁ˷½±㗜ϟʱвµĆ´½Ӎ
}

/**
  * º¯    ʽ£ºI2Cֹ֕
  * ²Π   ʽ£ºΞ
  * ·µ »ؠֵ£ºΞ
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);		//À­µ͓DA£¬ȷ±£SDAΪµ͵熽
	OLED_W_SCL(1);		//ʍ·œCL£¬ʹSCL³ʏָߵ熽
	OLED_W_SDA(1);		//ԚSCL¸ߵ熽ƚ¼䣬ʍ·œDA£¬²úɺֹ֕Ѕºō
}

/**
  * º¯    ʽ£ºI2C·¢ˍһ¸öז½ڍ
  * ²Π   ʽ£ºByte Ҫ·¢ˍµĒ»¸öז½ڊý¾ݣ¬·¶Χ£º0x00~0xFF
  * ·µ »ؠֵ£ºΞ
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	/*ѭ»·8´Σ¬ַ»úҀ´η¢ˍʽ¾ݵă¿һλ*/
	for (i = 0; i < 8; i++)
	{
		/*ʹӃњ«µķ½ʽȡ³öByteµĖ¸¶¨һλʽ¾ݲ¢дȫµ½SDAϟ*/
		/*Á½¸ö!µė÷Ӄʇ£¬ȃ˹Ӑ·ǁ㵄ֵ±䎪1*/
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);	//ʍ·œCL£¬´ӻúԚSCL¸ߵ熽ƚ¼䶁ȡSDA
		OLED_W_SCL(0);	//À­µ͓CL£¬ַ»ú¿ªʼ·¢ˍςһλʽ¾ݍ
	}
	
	OLED_W_SCL(1);		//¶µĒ»¸öʱ֓£¬²»´¦À퓦´𐅺ō
	OLED_W_SCL(0);
}


