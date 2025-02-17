/*
ADXL345三轴倾斜度模块
*/
#include "adxl345.h"
 
/**
  * @brief  ADXL345初始化
  * @param  None
  * @retval None
  */
uint8_t ADXL345_init(void)
{
	IIC_init();
	if(ADXL345_read_reg(DEVICE_ID) == 0xE5)
	{
		ADXL345_write_reg(DATA_FORMAT,0X0B); // 低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
		ADXL345_write_reg(BW_RATE,0x0E); // 数据输出速度为100Hz
		ADXL345_write_reg(POWER_CTL,0x08); // 链接使能,测量模式,省电特性
		ADXL345_write_reg(INT_ENABLE,0x80); // 不使用中断		 
	 	ADXL345_write_reg(OFSX,0x00);
		ADXL345_write_reg(OFSY,0x00);
		ADXL345_write_reg(OFSZ,0x08);
		return 0;
	}
	return 1;
}
/**
  * @brief  ADXL345写寄存器
  * @param  None
  * @retval None
  */
uint8_t ADXL345_write_reg(u8 addr,u8 val) 
{
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // 发送写器件指令	 
	if(IIC_wait_ack())
	{
		return 1;
	}    
  IIC_send_byte(addr); // 发送寄存器地址
	if(IIC_wait_ack())
	{
		return 2;
	} 	 										  		   
	IIC_send_byte(val); // 发送值					   
	if(IIC_wait_ack())
	{
		return 3;
	}   		    	   
  IIC_stop(); // 产生一个停止条件 	   
	return 0;
}
/**
  * @brief  ADXL345读寄存器
  * @param  None
  * @retval None
  */
u8 ADXL345_read_reg(u8 addr)
{
	u8 temp=0;		 
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // 发送写器件指令	 
	if(IIC_wait_ack())
	{
		return 1;
	}   
  IIC_send_byte(addr); // 发送寄存器地址
	if(IIC_wait_ack())
	{
		return 2;
	}  										  		   
	IIC_start(); // 重新启动
	IIC_send_byte(regaddress); // 发送读器件指令	 
	if(IIC_wait_ack())
	{
		return 3;
	} 	   
  temp=IIC_read_byte(0); // 读取一个字节,不继续再读,发送NAK
  IIC_stop(); // 产生一个停止条件 	    
	return temp;
}
/**
  * @brief  ADXL345读取数据
  * @param  None
  * @retval None
  */
void ADXL345_read_data(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // 发送写器件指令	 
	IIC_wait_ack();	   
  IIC_send_byte(0x32); // 发送寄存器地址(数据缓存的起始地址为0X32)
	IIC_wait_ack(); 	 										  		   
 
 	IIC_start(); // 重新启动
	IIC_send_byte(regaddress); // 发送读器件指令
	IIC_wait_ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_read_byte(0); // 读取一个字节,不继续再读,发送NACK  
		else buf[i]=IIC_read_byte(1);	// 读取一个字节,继续读,发送ACK 
 	}	        	   
  IIC_stop();	// 产生一个停止条件
	*x=(short)(((u16)buf[1]<<8)+buf[0]); // 合成数据    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 
}
/**
  * @brief  ADXL345连读读取几次取平均值
  * @param  None
  * @retval None
  */
void ADXL345_read_average(short *x,short *y,short *z,u8 times)
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)//读取次数不为0
	{
		for(i=0;i<times;i++)//连续读取times次
		{
			ADXL345_read_data(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			delay_ms(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}
/**
  * @brief  ADXL345计算角度
  * @param  None
  * @retval None
  */
//void get_angle(float *x_angle,float *y_angle,float *z_angle)
//{
//	short ax,ay,az;
//	ADXL345_read_average(&ax,&ay,&az,10);
//	*x_angle=atan(ax/sqrt((az*az+ay*ay)))*180/3.14;
//	*y_angle=atan(ay/sqrt((ax*ax+az*az)))*180/3.14;
//	//*z_angle=atan(sqrt((ax*ax+ay*ay)/az))*180/3.14;//三轴加速度计航向角获取不了
//}


void get_angle(float *pitch,float *roll)
{
	short ax,ay,az;
	
	ADXL345_read_average(&ax,&ay,&az,10);
	*pitch=atan(ax/sqrt((az*az+ay*ay)))*180/3.14;
	*roll=atan(ay/sqrt((ax*ax+az*az)))*180/3.14;
}








 