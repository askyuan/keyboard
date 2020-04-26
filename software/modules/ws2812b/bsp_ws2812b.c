#include "bsp_ws2812b.h"
#include "ws2812b_task.h"
#include "tim.h"
/*
   PB7     ------> TIM4_CH2 ------> L_DIN_1 ------> 1-27--------27
   PB8     ------> TIM4_CH3 ------> L_DIN_2 ------> 28-54-------27
   PB9     ------> TIM4_CH4 ------> L_DIN_3 ------> 55-87-------33
*/
/*IRQ��ʹ���ݴ棬IRQ�в��ܰ���������룬�ʷ�װ��ʾ*/
static ws2812b_led ws2812b_irq_cfg[3][MAX_WS2812B_NUM];
void mx_ws2812b_init(keyboard_rgb *board_rgb_led)
{
    uint8_t i;
    for(i=0; i<87; i++)
        {
            board_rgb_led[i].id = i+1;
            /*ͨ������*/
            if(board_rgb_led[i].id<=27)
                {
                    board_rgb_led[i].id_ch = L_DIN_1;
                    board_rgb_led[i].id_ch_id = i+1;
                }
            else if(board_rgb_led[i].id<=54)
                {
                    board_rgb_led[i].id_ch = L_DIN_2;
                    board_rgb_led[i].id_ch_id = i+1-27;
                }
            else if(board_rgb_led[i].id<=87)
                {
                    board_rgb_led[i].id_ch = L_DIN_3;
                    board_rgb_led[i].id_ch_id = i+1-54;
                }
            /*��������*/
            if(board_rgb_led[i].id<=13)
                {
                    board_rgb_led[i].id_line = 1;//������
                    board_rgb_led[i].id_column = i+1;//������
                }
            else if(board_rgb_led[i].id<=27)
                {
                    board_rgb_led[i].id_line = 2;//������
                    board_rgb_led[i].id_column = i+1-13;//������
                }
            else if(board_rgb_led[i].id<=41)
                {
                    board_rgb_led[i].id_line = 3;//������
                    board_rgb_led[i].id_column = i-26;//������
                }
            else if(board_rgb_led[i].id<=54)
                {
                    board_rgb_led[i].id_line = 4;//������
                    board_rgb_led[i].id_column = i-40;//������
                }
            else if(board_rgb_led[i].id<=66)
                {
                    board_rgb_led[i].id_line = 5;//������
                    board_rgb_led[i].id_column = i-53;//������
                }
            else if(board_rgb_led[i].id<=74)
                {
                    board_rgb_led[i].id_line = 6;//������
                    board_rgb_led[i].id_column = i-65;//������
                }
            else if(board_rgb_led[i].id<=87)
                {
                    board_rgb_led[i].id_line = 7;//������
                    board_rgb_led[i].id_column = i-73;//������
                }
        }
}
void mx_ws2812b_reset(void)
{
    TIM4->ARR  = 1199;
    TIM4->CCR2 = 0;
    TIM4->CCR3 = 0;
    TIM4->CCR4 = 0;
}
void mx_ws2812b_config(uint8_t ch,uint8_t set)
{
    TIM4->ARR = 95;
    switch(ch)
        {
        case 2:
            TIM4->CCR2 = set;
            break;
        case 3:
            TIM4->CCR3 = set;
            break;
        case 4:
            TIM4->CCR4 = set;
            break;
        }
}
void mx_ws2812b_low(uint8_t ch)
{
    mx_ws2812b_config(ch,4);
}
void mx_ws2812b_high(uint8_t ch)
{
    mx_ws2812b_config(ch,9);
}
void ws2812b_column_cfg(keyboard_rgb *board_rgb_led)
{
	


}
/*��ID����*/
void ws2812b_id_cfg(keyboard_rgb *board_rgb_led)
{
	uint8_t i;
	for(i=0;i<87;i++)
	{
		/*
			PB7     ------> TIM4_CH2 ------> L_DIN_1 ------> 1-27--------27
			PB8     ------> TIM4_CH3 ------> L_DIN_2 ------> 28-54-------27
			PB9     ------> TIM4_CH4 ------> L_DIN_3 ------> 55-87-------33
		*/
		switch(board_rgb_led[i].id_ch)
		{
			case L_DIN_1:ws2812b_irq_cfg[0][i].WS2812B_RGB = board_rgb_led[i].rgb_set.WS2812B_RGB;break;
			case L_DIN_2:ws2812b_irq_cfg[1][i-27].WS2812B_RGB = board_rgb_led[i].rgb_set.WS2812B_RGB;break;
			case L_DIN_3:ws2812b_irq_cfg[2][i-54].WS2812B_RGB = board_rgb_led[i].rgb_set.WS2812B_RGB;break;
		}
	}
	
}
void ws2812b_IRQ(void)
{
    static uint16_t ws2812b_num = 0;
    static uint8_t ws2812b_size = 0;
    if(ws2812b_num==MAX_WS2812B_NUM)
        {
            ws2812b_num=0;
            mx_ws2812b_reset();
        }
    else
        {
            if((ws2812b_irq_cfg[0][ws2812b_num].WS2812B_RGB>>(23-ws2812b_size)&0x01)==1)
                {
                    mx_ws2812b_high(2);
                }
            else
                {
                    mx_ws2812b_low(2);
                }
            if((ws2812b_irq_cfg[1][ws2812b_num].WS2812B_RGB>>(23-ws2812b_size)&0x01)==1)
                {
                    mx_ws2812b_high(3);
                }
            else
                {
                    mx_ws2812b_low(3);
                }
            if((ws2812b_irq_cfg[2][ws2812b_num].WS2812B_RGB>>(23-ws2812b_size)&0x01)==1)
                {
                    mx_ws2812b_high(4);
                }
            else
                {
                    mx_ws2812b_low(4);
                }
            ws2812b_size++;
            if(ws2812b_size==24)
                {
                    ws2812b_num++;
                    ws2812b_size = 0;
                }
        }
}