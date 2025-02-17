#include "ledmatrix.h"


static void MatrixWriteByte(uint8_t data)
{
	spi_read_write_byte(data);
}


//初始化IO口，初始化SPI接口
void Matrix_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_6);

	spi_init();
	
	Write_Maxtrix(0x09,0x00);//译码方式寄存器 0 不处理 1 BCD-B译码模式
	Write_Maxtrix(0x0a,0x00);//亮度寄存器 0xx0最暗 0xxf最亮
	Write_Maxtrix(0x0b,0x07);//扫描界限寄存器 动态扫描位数 0~7H
	Write_Maxtrix(0x0c,0x01);//停机寄存器 0停机 1工作
	Write_Maxtrix(0x0f,0x00);//显示测试寄存器 0正常工作，1所有led按最大亮度显示
}



void Write_Maxtrix(uint8_t addr, uint8_t data)
{
	MAX7219_CS(0);
	MatrixWriteByte(addr);
	MatrixWriteByte(data);
	MAX7219_CS(1);
}

//0x8040201008040201 斜对角
//高位优先
//0x81c3663c183c66c3
void MatrixDrawOneFrame(uint64_t data)
{
	uint8_t i;
	
	for(i = 1; i < 9; i++)//digit0 对应 0xx1 起始地址为1
	{
		Write_Maxtrix(i,data);
		data>>=8;
	}
	
}


/*以左上角为原点， 原点往左一格则1左移一位，原点往下一格则1左移八位 例如(2,3)代表左移两格下移三格 0x01<<2再左移3*8位*/
//点亮一个点
uint64_t GetPixelData(uint8_t x, uint8_t y)
{
	uint64_t data = 0x01;
	if(x > 8 || y > 8)
	return 0;
	data<<= x+y*8;
	return data;
}




// 进行平滑过渡更新
void grounder(float pitch, float roll)
{
    // 计算当前位置和目标位置之间的差距
	static uint8_t current_row = 0;
	static uint8_t current_col = 0;
	uint8_t target_row = (int)(((pitch + 90) * (LED_SIZE - 1) / 120)*1.1);
	uint8_t target_col = (int)(((roll + 90) * (LED_SIZE - 1) / 120)*1.1);
	float transition_speed = 1.0f;
	//// 弹力效果的参数
	float elasticity_factor = 0.2f;  // 弹力强度
	float damping_factor = 0.5f;    // 阻尼强度，数值越小反弹越长
	
    int row_diff = target_row - current_row;
    int col_diff = target_col - current_col;

//    // 按照平滑过渡速度进行更新
//    current_row += (row_diff > 0 ? 1 : (row_diff < 0 ? -1 : 0)) * transition_speed;
//    current_col += (col_diff > 0 ? 1 : (col_diff < 0 ? -1 : 0)) * transition_speed;
	
	    // 弹力和阻尼效果：检查是否超出边界，模拟反弹
    if (current_row < 0 || current_row >= LED_SIZE) {
        row_diff = -row_diff * elasticity_factor;  // 超出范围时反弹
    }
    if (current_col < 0 || current_col >= LED_SIZE) {
        col_diff = -col_diff * elasticity_factor;  // 超出范围时反弹
    }

    // 按照平滑过渡和弹力效果进行更新
    if (fabs(row_diff) > 0.01 || fabs(col_diff) > 0.01) {
        current_row += row_diff * transition_speed;
        current_col += col_diff * transition_speed;
    }

    // 逐渐阻尼，减弱反弹速度
    row_diff *= (1 - damping_factor);
    col_diff *= (1 - damping_factor);

    // 确保最终位置不会超出范围
    if (current_row < 0) current_row = 0;
    if (current_row >= LED_SIZE) current_row = LED_SIZE - 1;
    if (current_col < 0) current_col = 0;
    if (current_col >= LED_SIZE) current_col = LED_SIZE - 1;
	
	MatrixDrawOneFrame(GetPixelData(current_row,current_col));
}
/***********************************************************************************/



//可以获取每个点的坐标位置
//每个点随方向变化可以正确变化位置
//要可以单独控制一个点

//规定只有四个方向会变化坐标























