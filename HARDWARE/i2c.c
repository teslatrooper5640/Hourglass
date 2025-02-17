#include "i2c.h"
 
 
 
/**
  * @brief  IIC延时
  * @param  None
  * @retval None
  */
static void IIC_Delay(void)
{
	uint8_t i;
	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 10; i++);
}
/**
  * @brief  SDA输出方向配置
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
  * @brief  SDA输入方向配置
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
  * @brief  模拟IIC初始化
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
  * @brief  模拟IIC起始信号
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
  * @brief  模拟IIC停止信号
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
  * @brief  模拟IIC主机应答
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
  * @brief  模拟IIC主机不应答
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
  * @brief  模拟IIC等待从机应答
  * @param  None
	* @retval 1: 接收应答失败 0: 接收应答成功
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
	* @brief  模拟IIC发送一个字节
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
		IIC_Delay(); // 发送数据
		IIC_SCL_OUT_0;
		if(i == 7) IIC_SDA_OUT_1; // 最后一位数据发送完要释放SDA总线
		txd <<= 1;
		IIC_Delay();
	}
}
/**
	* @brief  模拟IIC读取一个字节
	* @param  ack: 0,读完不产生应答 1,读完产生应答
  * @retval 返回读取到的字节
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
		if(IIC_SDA_IN()) receive++; // 连续读取八位
		IIC_SCL_OUT_0;
		IIC_Delay();	
	}
  if(!ack) IIC_noack();
	else IIC_ack();
 
	return receive; // 返回读取到的字节
}

