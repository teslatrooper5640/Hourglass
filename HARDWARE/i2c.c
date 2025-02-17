#include "i2c.h"
 
 
 
/**
  * @brief  IIC��ʱ
  * @param  None
  * @retval None
  */
static void IIC_Delay(void)
{
	uint8_t i;
	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 10; i++);
}
/**
  * @brief  SDA�����������
  * @param  None
  * @retval None
  */
void Set_IIC_SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(IIC_SDA_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure); 						
}
/**
  * @brief  SDA���뷽������
  * @param  None
  * @retval None
  */
void Set_IIC_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(IIC_SDA_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}
/**
  * @brief  ģ��IIC��ʼ��
  * @param  None
  * @retval None
  */
void IIC_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IIC_SDA_CLK | IIC_SCL_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);	
	IIC_stop();
}
/**
  * @brief  ģ��IIC��ʼ�ź�
  * @param  None
  * @retval None
  */
void IIC_start(void)
{
	Set_IIC_SDA_OUT();
	IIC_SCL_OUT_1;
	IIC_SDA_OUT_1;
	IIC_Delay();
	IIC_SDA_OUT_0;
	IIC_Delay();
	IIC_SCL_OUT_0;
	IIC_Delay();
}
/**
  * @brief  ģ��IICֹͣ�ź�
  * @param  None
  * @retval None
  */
void IIC_stop(void)
{
	Set_IIC_SDA_OUT();
	IIC_SCL_OUT_1;
	IIC_SDA_OUT_0;
	IIC_Delay();
	IIC_SDA_OUT_1;
}
/**
  * @brief  ģ��IIC����Ӧ��
  * @param  None
  * @retval None
  */
void IIC_ack(void)
{
	Set_IIC_SDA_OUT();
  IIC_SDA_OUT_0;
	IIC_Delay();
	IIC_SCL_OUT_1;
	IIC_Delay();
	IIC_SCL_OUT_0;
	IIC_Delay();	
	IIC_SDA_OUT_1;
}
/**
  * @brief  ģ��IIC������Ӧ��
  * @param  None
  * @retval None
  */
void IIC_noack(void)
{
	Set_IIC_SDA_OUT();
  IIC_SDA_OUT_1;
	IIC_Delay();
	IIC_SCL_OUT_1;
	IIC_Delay();
	IIC_SCL_OUT_0;
	IIC_Delay();
}
/**
  * @brief  ģ��IIC�ȴ��ӻ�Ӧ��
  * @param  None
	* @retval 1: ����Ӧ��ʧ�� 0: ����Ӧ��ɹ�
  */
uint8_t IIC_wait_ack(void)
{
	uint8_t rec = 0;
	Set_IIC_SDA_OUT();
	IIC_SDA_OUT_1;
	IIC_Delay();
	Set_IIC_SDA_IN();
	IIC_SCL_OUT_1;
	IIC_Delay();
	rec = IIC_SDA_IN();
	IIC_SCL_OUT_0;
	IIC_Delay();
	
	return rec;
}
/**
	* @brief  ģ��IIC����һ���ֽ�
  * @param  None
  * @retval None
  */
void IIC_send_byte(uint8_t txd)
{
	uint8_t i=0;
	Set_IIC_SDA_OUT();
	for(i=0;i<8;i++)
	{
		if(txd&0x80) IIC_SDA_OUT_1;
		else IIC_SDA_OUT_0;
		IIC_Delay();
		IIC_SCL_OUT_1;
		IIC_Delay(); // ��������
		IIC_SCL_OUT_0;
		if(i == 7) IIC_SDA_OUT_1; // ���һλ���ݷ�����Ҫ�ͷ�SDA����
		txd <<= 1;
		IIC_Delay();
	}
}
/**
	* @brief  ģ��IIC��ȡһ���ֽ�
	* @param  ack: 0,���겻����Ӧ�� 1,�������Ӧ��
  * @retval ���ض�ȡ�����ֽ�
  */
uint8_t IIC_read_byte(uint8_t ack)
{
	uint8_t i,receive=0;
	Set_IIC_SDA_IN();
	for(i=0;i<8;i++)
	{
		receive <<= 1;
		IIC_SCL_OUT_1;
		IIC_Delay();
		if(IIC_SDA_IN()) receive++; // ������ȡ��λ
		IIC_SCL_OUT_0;
		IIC_Delay();	
	}
  if(!ack) IIC_noack();
	else IIC_ack();
 
	return receive; // ���ض�ȡ�����ֽ�
}

