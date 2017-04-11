#include "key.h"
#include "delay.h"

//KEY����	   

//������ʼ������
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP

u8 KEY_Scan(u8 mode)
{
    static u8  key_up=1;     //�����ɿ���־
	
    if(mode==1)key_up=1;     //֧������
    if( key_up && (WK_UP==1) )
    {
        delay_ms(10);
        key_up=0;
        if(WK_UP==1) return WKUP_PRES;          
    }
	else if(WK_UP==0)key_up=1;
	
    return 0;   //�ް�������
}
