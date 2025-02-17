/*
ADXL345������б��ģ��
*/
#include "adxl345.h"
 
/**
  * @brief  ADXL345��ʼ��
  * @param  None
  * @retval None
  */
uint8_t ADXL345_init(void)
{
	IIC_init();
	if(ADXL345_read_reg(DEVICE_ID) == 0xE5)
	{
		ADXL345_write_reg(DATA_FORMAT,0X0B); // �͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
		ADXL345_write_reg(BW_RATE,0x0E); // ��������ٶ�Ϊ100Hz
		ADXL345_write_reg(POWER_CTL,0x08); // ����ʹ��,����ģʽ,ʡ������
		ADXL345_write_reg(INT_ENABLE,0x80); // ��ʹ���ж�		 
	 	ADXL345_write_reg(OFSX,0x00);
		ADXL345_write_reg(OFSY,0x00);
		ADXL345_write_reg(OFSZ,0x08);
		return 0;
	}
	return 1;
}
/**
  * @brief  ADXL345д�Ĵ���
  * @param  None
  * @retval None
  */
uint8_t ADXL345_write_reg(u8 addr,u8 val) 
{
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // ����д����ָ��	 
	if(IIC_wait_ack())
	{
		return 1;
	}    
  IIC_send_byte(addr); // ���ͼĴ�����ַ
	if(IIC_wait_ack())
	{
		return 2;
	} 	 										  		   
	IIC_send_byte(val); // ����ֵ					   
	if(IIC_wait_ack())
	{
		return 3;
	}   		    	   
  IIC_stop(); // ����һ��ֹͣ���� 	   
	return 0;
}
/**
  * @brief  ADXL345���Ĵ���
  * @param  None
  * @retval None
  */
u8 ADXL345_read_reg(u8 addr)
{
	u8 temp=0;		 
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // ����д����ָ��	 
	if(IIC_wait_ack())
	{
		return 1;
	}   
  IIC_send_byte(addr); // ���ͼĴ�����ַ
	if(IIC_wait_ack())
	{
		return 2;
	}  										  		   
	IIC_start(); // ��������
	IIC_send_byte(regaddress); // ���Ͷ�����ָ��	 
	if(IIC_wait_ack())
	{
		return 3;
	} 	   
  temp=IIC_read_byte(0); // ��ȡһ���ֽ�,�������ٶ�,����NAK
  IIC_stop(); // ����һ��ֹͣ���� 	    
	return temp;
}
/**
  * @brief  ADXL345��ȡ����
  * @param  None
  * @retval None
  */
void ADXL345_read_data(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	IIC_start();  				 
	IIC_send_byte(slaveaddress); // ����д����ָ��	 
	IIC_wait_ack();	   
  IIC_send_byte(0x32); // ���ͼĴ�����ַ(���ݻ������ʼ��ַΪ0X32)
	IIC_wait_ack(); 	 										  		   
 
 	IIC_start(); // ��������
	IIC_send_byte(regaddress); // ���Ͷ�����ָ��
	IIC_wait_ack();
	for(i=0;i<6;i++)
	{
		if(i==5)buf[i]=IIC_read_byte(0); // ��ȡһ���ֽ�,�������ٶ�,����NACK  
		else buf[i]=IIC_read_byte(1);	// ��ȡһ���ֽ�,������,����ACK 
 	}	        	   
  IIC_stop();	// ����һ��ֹͣ����
	*x=(short)(((u16)buf[1]<<8)+buf[0]); // �ϳ�����    
	*y=(short)(((u16)buf[3]<<8)+buf[2]); 	    
	*z=(short)(((u16)buf[5]<<8)+buf[4]); 
}
/**
  * @brief  ADXL345������ȡ����ȡƽ��ֵ
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
	if(times)//��ȡ������Ϊ0
	{
		for(i=0;i<times;i++)//������ȡtimes��
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
  * @brief  ADXL345����Ƕ�
  * @param  None
  * @retval None
  */
//void get_angle(float *x_angle,float *y_angle,float *z_angle)
//{
//	short ax,ay,az;
//	ADXL345_read_average(&ax,&ay,&az,10);
//	*x_angle=atan(ax/sqrt((az*az+ay*ay)))*180/3.14;
//	*y_angle=atan(ay/sqrt((ax*ax+az*az)))*180/3.14;
//	//*z_angle=atan(sqrt((ax*ax+ay*ay)/az))*180/3.14;//������ٶȼƺ���ǻ�ȡ����
//}


void get_angle(float *pitch,float *roll)
{
	short ax,ay,az;
	
	ADXL345_read_average(&ax,&ay,&az,10);
	*pitch=atan(ax/sqrt((az*az+ay*ay)))*180/3.14;
	*roll=atan(ay/sqrt((ax*ax+az*az)))*180/3.14;
}








 