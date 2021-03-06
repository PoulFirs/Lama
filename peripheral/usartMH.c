#include "usartMH.h"

void USART3SendCMD(const uint8_t *pucBuffer, uint8_t size)
{
    for (uint8_t i=0;i<size;i++)
    {
        USART_SendData(USART3, pucBuffer[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {
        }
    }
}


void GetCO2(void){
	uint8_t RXi_MH = 0;

	while(RXi_MH < BUF_SIZE){
		for(uint16_t i = 10000; i; i--){
			if((USART3->SR & USART_FLAG_RXNE) != (u16)RESET){
				uint8_t rx_byte = USART_ReceiveData(USART3);
				if((rx_byte == 0xff)&&(RXi_MH < 2))
					RXi_MH = 0;
				sensors.co[RXi_MH] = rx_byte;

				/*if(RXi_MH == 3){
					sensors.co_h = BUF[RXi_MH - 1]; //���������� �������� ������� �������� ���������� �����
					sensors.co_l = BUF[RXi_MH]; //���������� �������� ������� �������� ���������� �����
				}*/
				break;
			}
		}
		RXi_MH++;//������� � ���������� �������� �������.
	}
}
