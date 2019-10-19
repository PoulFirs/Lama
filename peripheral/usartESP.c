#include "usartESP.h"

inline void clear_Buffer(uint8_t pucBuffer[BUF_SIZE], uint8_t size) {

    for (uint8_t i=0;i<size;i++){
    	pucBuffer[i] = '\0';
    }
	RXi = 0;
	FLAG_REPLY = 0;
}

void USARTSendSTR(const char *pucBuffer)
{
	while (*pucBuffer)
	{
		USART_SendData(USART1, *pucBuffer++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		{
		}
	}
}
void USARTSendCMD(uint8_t pucBuffer[BUF_SIZE], uint8_t size)
{
    for (uint8_t i=0;i<size;i++)
    {
        USART_SendData(USART1, pucBuffer[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
    clear_Buffer(TX_BUF, BUF_SIZE);//������� ������ TX
}
void USARTSendCHAR(uint8_t pucBuffer)
{
	USART_SendData(USART1, pucBuffer);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
}

void USART1_IRQHandler(void)
{
	if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		RXc = USART_ReceiveData(USART1);

		if(init)
		{
			if ((RXc != 255) && (RXc != 0) && (RXc != 10))
			{
				if (RXc != 13)
				{
					RX_BUF[RXi] = RXc;
					RXi++;
					if (RXi > RX_BUF_SIZE-1)
					{
						clear_Buffer(RX_BUF, BUF_SIZE);
					}

				}
				else
				{
					FLAG_REPLY = 1;
				}
			}
		}
		else {//'dslgoh
			RX_BUF[RXi] = RXc;
			RXi++;
			switch(way_cmd){
				case(ID):
					if(RXc == ':'){
						id = RX_BUF[0];
						id_rx = atoi (&RX_BUF[2]);
						clear_Buffer(RX_BUF, BUF_SIZE);
						way_cmd = RX_MODE;
					}
					break;
				case(RX_MODE):
						if(RXi == id_rx){
							way_cmd = DECODE;
						}
					break;

				case(INVITATION):
					if(RXc == '>'){
						way_cmd = SENDMES;
					}
					break;
				case(WAIT):
					if(strstr(RX_BUF, "+IPD,")){
						way_cmd = ID;
						clear_Buffer(RX_BUF, BUF_SIZE);

					}
					if(strstr(RX_BUF, "0,CLOSED")){
						clear_Buffer(RX_BUF, BUF_SIZE);
						init = 1;
						way_cmd = INIT_ESP;
					}
					break;
			}
		}
	}
}

void uart_wite_for(char *str) {
	char result = 0;

	while (result == 0){
		FLAG_REPLY = 0;
		clear_Buffer(RX_BUF, BUF_SIZE);
		while (FLAG_REPLY == 0) {

		}
		result = strstr(RX_BUF, str);
	}
}

void init_ESP(void)
{
	USARTSendCHAR(0xff);
	USARTSendSTR("AT\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CIPMUX=1\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CIPSERVER=1,48778\r\n");
	uart_wite_for("OK");

	uart_wite_for("0,CONNECT");
}