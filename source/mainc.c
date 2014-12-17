#include "stm32f4xx.h"                  // Device header

#define PWM_PERIOD 0xFF
#define PWM_PRESCALER 0x2F
#define FADDING_TIME  0x1FFFF


void delay_ms(int delay_time);

void timer2_setDutyCycle(int dutyCycle);
void timer2_pwmConfig(int prescaler,int autorreload);


int main(){
	int dc=0;
	timer2_pwmConfig(PWM_PRESCALER,PWM_PERIOD);	
	timer2_setDutyCycle(0x80);
	while(1){
		timer2_setDutyCycle(dc);
		dc++;
		if(dc>(PWM_PERIOD/2)) dc=0;
		delay_ms(FADDING_TIME);
	} 
}

void timer2_pwmConfig(int prescaler,int autorreload){
	
	RCC->AHB1ENR|=(0x1);//Enable GPIOA
	GPIOA->MODER|=(0x2<<10);// Pin_A5 as alternate function
	GPIOA->AFR[0]|=(0x1<<20);//AFRL5 0001: AF1 = TIM2_CH1/TIM2_ETR 
	
	//select the output mode by writing the CCS bits in the CCMRx register
	RCC->APB1ENR|=(0x1);//Enable TIM2
	TIM2->CCMR1=0;
	TIM2->CCMR1|=((0x0<<7)// OC1CE 0:
							|(0x6<<4)// OC1M 110: PWM mode 1 in upcounting mode TIMx_CNT<TIMx_CCR1 = output active
							|(0x1<<3)// OC1PE 1: Preload register on TIMx_CCR1 enabled (shadow register updated at update event)
							|(0x0<<2)// OC1FE don't care 
							|(0x0<<0)//CC1S 00: CC1 channel is configured as output 
	);
	
	
	//Configure output polarity
	TIM2->CCER=0;
	TIM2->CCER|=(0x0<<1);//CC1P 0: OC1 active high
	//configure initial duty cycle
	TIM2->CCR1=0;
	//set time base values
	TIM2->PSC=prescaler;
	TIM2->ARR=autorreload;
	//configure time base
	TIM2->CR1=0;
	TIM2->CR1|=((0x0<<8)//CKD 00: tds=tcd_int/1
						|(0x1<<7)//ARPE 1: TIMxARR is buffered
						|(0x0<<5)//CMS Don't care
						|(0x0<<4)//DIR 0:Counter is used upcounter
						|(0x0<<3)//OPM 0:Counter is not stoped at update event
						|(0x1<<2)//URS 1:Only overflows/underflow generate an update interrupt
						|(0x0<<1)//UDIS 0:Update events enabled
						|(0x0<<0)//CEN 0:Counter disabled;
						);
	//enable CCP1					
	TIM2->CCER|=(0x1<<0);//CC1E 1: On-OC1 us output on the corresponding output pin
	//start Timer
	TIM2->CR1|=(0x1<<0);//CEN 1:Counter enabled;
}

void timer2_setDutyCycle(int dutyCycle){
	TIM2->CCR1=dutyCycle;
}

void delay_ms(int delay_time){
	int i;
	for(i=0;i<delay_time;i++){
	}
}

