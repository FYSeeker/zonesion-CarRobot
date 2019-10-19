#include <stdio.h>
#include "stm32f4xx.h"
#include "user.h"
#include "SteeringEngine.h"



void uart1_init(unsigned int bpr)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
    
    /* Configure USARTx_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度10MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    USART_InitStructure.USART_BaudRate = bpr;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStructure);
    
    /* Enable the USARTz Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;     //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;            //子优先级   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Enable USARTy Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    /* Enable the USARTy */
    USART_Cmd(USART1, ENABLE);
    
}

void uart1_putc( char x)
{
    USART_SendData(USART1, x);
    // Loop until the end of transmission 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*PUTCHAR_PROTOTYPE
{
uart1_putc(ch);
return ch;
}*/
//static int (*input_call)( char x) = NULL;

//void  uart1_set_input(int (*fun)( char ) )
//{
//    input_call = fun;
//}



//定义最大接收字节数 100
#define USART_REC_LEN 100  	

//接收缓冲,最大USART_REC_LEN个字节
u8 USART1_RX_BUF[USART_REC_LEN];  

//接收状态标记 
//bit15:	接收完成标志
//bit14:	接收到'{'
//bit13-0:	接收到的有效字节数目.   
u16 USART1_RX_STA=0;       				

void USART1_IRQHandler(void)
{
    unsigned char ch=0;  
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {          
		ch =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART1_RX_STA&0x4000)//接收到了'{'
            {
                USART1_RX_BUF[USART1_RX_STA&0X3FFF]=ch ;
                USART1_RX_STA++;
                
                if((USART1_RX_STA&0X3FFF)>(USART_REC_LEN-1)) USART1_RX_STA=0;//接收数据错误,重新开始接收	 
                
                if(ch=='}') USART1_RX_STA|=0x8000;//接收完成了 
            }
			else if(ch=='{') //收到'{'
            {	
                    USART1_RX_STA|=0x4000;
                    USART1_RX_BUF[USART1_RX_STA&0X3FFF]=ch ;
                    USART1_RX_STA++;
            }
		}   
    } 
}





/*m^n函数，返回值:m^n次方*/
double Usart_Pow(double m,u8 n)
{
	double result=1;	 
	while(n--) result*=m;    
	return result;
}



/*命令格式
"X1:"
"X2:"
"X3:"
...
*/
void uartData_handle()
{
    u8 i=0,n=0,data_count=0;;
    u8 command=0,point_info=0;
    double temp=0;
    
    if(USART1_RX_STA&0x8000)
    {
        if((USART1_RX_BUF[1]=='X') && (USART1_RX_BUF[3]==':'))
        {
            if((USART1_RX_BUF[2]>='0')&&(USART1_RX_BUF[2]<='9'))
            {
                /*判断命令类型*/
                switch(USART1_RX_BUF[2])
                {
                    case '1':
                        command = 1;
                        break;
                    case '2':
                        command = 2;
                        break;
                    case '3':
                        command = 3;
                        break;
                    case '4':
                        command = 4;
                        break;
                    case '5':
                        command = 5;
                        break;
                    case '6':
                        command = 6;
                        break;
                    case '7':
                        command = 7;
                        break;
                    case '8':
                        command = 8;
                        break;
                    default:                  
                        command = 0;
                }               
                printf("\r\n\r\ncommand = %d\r\n",command);
                
                /*判断数据有效性*/
                i = 4;
                while(USART1_RX_BUF[i]!='}')
                {
                    if((USART1_RX_BUF[i]<'0'||USART1_RX_BUF[i]>'9') && USART1_RX_BUF[i]!='.')//超出范围
                    {
                        command = 0;
                        data_count = 0;
                        printf("数据异常,请重新发送！！！\r\n");
                        break;
                    }
                    
                    if(USART1_RX_BUF[i]=='.')   //有小数点
                    {
                        if(point_info!=0)       //有小数点且个数大于1
                        {
                            command = 0;
                            point_info = 0;
                            data_count = 0;
                            printf("数据异常,请重新发送！！！\r\n");
                            break;
                        }
                        else
                        {
                            point_info = i;
                        }
                    }
                    i++;
                }
                
                if(command!=0)
                {
                    data_count = i;
                    
                    if(point_info!=0)
                    {          
                        /*计算整数部分*/  
                        temp = 0;
                        for(i=4,n=point_info-5; i<point_info; i++)	
                        {
                            temp += (USART1_RX_BUF[i]-'0')*Usart_Pow(10,n--);	
                        }                      
                        /*计算小数部分*/
                        for(i=point_info+1,n=1; i<data_count; i++)
                        {
                            temp += (USART1_RX_BUF[i]-'0') * Usart_Pow(0.1,n++);
                        }
                        printf("data = %lf\r\n",temp);
                    }
                    else
                    {                  
                        temp = 0;
                        for(i=4,n=data_count-5; i<data_count; i++)	
                        {
                            temp += (USART1_RX_BUF[i]-'0')*Usart_Pow(10,n--);	
                        }
                        printf("data = %lf\r\n",temp);	
                    }
                    
                    switch(command)
                    {
                        case 1:
                            break;
                            
                        case 2:
                            break;
                            
                        case 3:
                            break;
                            
                        case 4:
                            
                            break;
                        case 5:
                            break;
                            
                        case 6:
                            break;
                            
                        case 7:
                            break;
                            
                        case 8:
                            V3 = (uint8)(temp/100);
                            V4 = (uint8)((uint16)temp%100);
                            break;
                    }
                }
                
            }
        }
        
		/*清除接收BUF*/
		i=0;
		while(USART1_RX_BUF[i]!='\0')
		{
			USART1_RX_BUF[i] = 0;
			i++;
		}
		USART1_RX_STA = 0;		//接收状态和数据位数清0
    }
}


