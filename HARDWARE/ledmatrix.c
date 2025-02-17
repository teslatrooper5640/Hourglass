#include "ledmatrix.h"


static void MatrixWriteByte(uint8_t data)
{
	spi_read_write_byte(data);
}


//��ʼ��IO�ڣ���ʼ��SPI�ӿ�
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
	
	Write_Maxtrix(0x09,0x00);//���뷽ʽ�Ĵ��� 0 ������ 1 BCD-B����ģʽ
	Write_Maxtrix(0x0a,0x00);//���ȼĴ��� 0xx0� 0xxf����
	Write_Maxtrix(0x0b,0x07);//ɨ����޼Ĵ��� ��̬ɨ��λ�� 0~7H
	Write_Maxtrix(0x0c,0x01);//ͣ���Ĵ��� 0ͣ�� 1����
	Write_Maxtrix(0x0f,0x00);//��ʾ���ԼĴ��� 0����������1����led�����������ʾ
}



void Write_Maxtrix(uint8_t addr, uint8_t data)
{
	MAX7219_CS(0);
	MatrixWriteByte(addr);
	MatrixWriteByte(data);
	MAX7219_CS(1);
}

//0x8040201008040201 б�Խ�
//��λ����
//0x81c3663c183c66c3
void MatrixDrawOneFrame(uint64_t data)
{
	uint8_t i;
	
	for(i = 1; i < 9; i++)//digit0 ��Ӧ 0xx1 ��ʼ��ַΪ1
	{
		Write_Maxtrix(i,data);
		data>>=8;
	}
	
}


/*�����Ͻ�Ϊԭ�㣬 ԭ������һ����1����һλ��ԭ������һ����1���ư�λ ����(2,3)�������������������� 0x01<<2������3*8λ*/
//����һ����
uint64_t GetPixelData(uint8_t x, uint8_t y)
{
	uint64_t data = 0x01;
	if(x > 8 || y > 8)
	return 0;
	data<<= x+y*8;
	return data;
}




// ����ƽ�����ɸ���
void grounder(float pitch, float roll)
{
    // ���㵱ǰλ�ú�Ŀ��λ��֮��Ĳ��
	static uint8_t current_row = 0;
	static uint8_t current_col = 0;
	uint8_t target_row = (int)(((pitch + 90) * (LED_SIZE - 1) / 120)*1.1);
	uint8_t target_col = (int)(((roll + 90) * (LED_SIZE - 1) / 120)*1.1);
	float transition_speed = 1.0f;
	//// ����Ч���Ĳ���
	float elasticity_factor = 0.2f;  // ����ǿ��
	float damping_factor = 0.5f;    // ����ǿ�ȣ���ֵԽС����Խ��
	
    int row_diff = target_row - current_row;
    int col_diff = target_col - current_col;

//    // ����ƽ�������ٶȽ��и���
//    current_row += (row_diff > 0 ? 1 : (row_diff < 0 ? -1 : 0)) * transition_speed;
//    current_col += (col_diff > 0 ? 1 : (col_diff < 0 ? -1 : 0)) * transition_speed;
	
	    // ����������Ч��������Ƿ񳬳��߽磬ģ�ⷴ��
    if (current_row < 0 || current_row >= LED_SIZE) {
        row_diff = -row_diff * elasticity_factor;  // ������Χʱ����
    }
    if (current_col < 0 || current_col >= LED_SIZE) {
        col_diff = -col_diff * elasticity_factor;  // ������Χʱ����
    }

    // ����ƽ�����ɺ͵���Ч�����и���
    if (fabs(row_diff) > 0.01 || fabs(col_diff) > 0.01) {
        current_row += row_diff * transition_speed;
        current_col += col_diff * transition_speed;
    }

    // �����ᣬ���������ٶ�
    row_diff *= (1 - damping_factor);
    col_diff *= (1 - damping_factor);

    // ȷ������λ�ò��ᳬ����Χ
    if (current_row < 0) current_row = 0;
    if (current_row >= LED_SIZE) current_row = LED_SIZE - 1;
    if (current_col < 0) current_col = 0;
    if (current_col >= LED_SIZE) current_col = LED_SIZE - 1;
	
	MatrixDrawOneFrame(GetPixelData(current_row,current_col));
}
/***********************************************************************************/



//���Ի�ȡÿ���������λ��
//ÿ�����淽��仯������ȷ�仯λ��
//Ҫ���Ե�������һ����

//�涨ֻ���ĸ������仯����























