#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

struct zhc
{
	float num;
	char delim;
};

void delay(uint32_t count)
{
	while(count--);
}

void usart3_init(void)
{
	/* RCC initialization */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* GPIO initialization */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USART initialization */
	USART_InitTypeDef USART_InitStruct = {
		.USART_BaudRate = 9600,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No
	};
	USART_Init(USART3, &USART_InitStruct);

	USART_Cmd(USART3, ENABLE);

	USART_ClearFlag(USART3, USART_FLAG_TC);
}

char usart_getc(void)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != SET);
	return USART_ReceiveData(USART3);
}

void usart_putc(char data)
{
	/* wait until TXE (Transmit Data Register Empty) flag is set */
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, data);
	/* wait until TC (Tranmission Complete) flag is set */
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

void usart_puts(struct zhc *ptr)
{
	char *ch =  ( ( ( char * )( & ( ptr->num ) ) ) );
	for(int i =0;i<4;i++)
	{
		usart_putc( *ch );
		ch++;
	}
	usart_putc( ptr->delim );
}

float convert_to_number()
{	
	int i;
	float n;
    char *ch = (char *) &n;
	
	for(i =0;i<4;i++)
	{
		ch[i] = usart_getc();
	//	ch++;
	}
	return n;
}

/*void deliver_number(char *ch)
{
	for(int j=0;j<4;j++)
	{
		usart_putc( *ch );
		ch++;
	}
}*/

void deliver_number(float f)
{
	char* ch = (char*)&f;

	for(int j=0;j<4;j++)
	{
		usart_putc( *ch );
		ch++;
	}
}

int main()
{
	int i;
	char receieved_ch;
	float temp;
	float receieved_number[100];

	for(i=0;i<100;i++)
		receieved_number[i] = 1.0*i;

	usart3_init();
	struct zhc z = {26.4000,165};

	while(1) 
	{
		receieved_ch =usart_getc();

		if(receieved_ch == '\n')
			for(i=0;i<100;i++)
				receieved_number[i] = 2*convert_to_number(); //convert one number for one time
				

		//deliver data to rpi
		usart_putc('\n');
		for(i=0;i<100;i++)
		{
			temp = receieved_number[i];
		//	deliver_number( (char*)&temp ); 
			deliver_number( temp ); 
		}
	}
	return 0;
}




	/*float temp = 1.5;
			char* c = (char*)&temp;
			usart_putc('\n');
			for(int k=0;k<100;k++)
			{
				for(int r=0;r<4;r++)
				{
					usart_putc( *c );
					c++;
				}
				c = (char*)&temp;
			}*/