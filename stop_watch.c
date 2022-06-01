/****************************************************************/
/***********     Author     : Tarek Elmenshawy       ************/
/***********     File Name  : stop_watch.c		     ************/
/***********     Date       : 03-05-2022             ************/
/***********     Description: Digital Stop Watch	 ************/
/****************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Macros to enable/disable Timer 1 */
#define TIM1_ENABLE		(TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12))
#define TIM1_DISABLE	(TCCR1B = 0x00)

/* Function Prototypes */
void Display_Init(void);
void displayTime(void);
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void TIM1_Init(void);

/* Global Variables to contain time */
volatile unsigned char sec, min, hr;

int main(void)
{
	sei();									/* Enable All Interrupt */

	Display_Init();							/* 7-segments pins configurations */

	INT0_Init();							/* INT0 Initialization */
	INT1_Init();							/* INT1 Initialization */
	INT2_Init();							/* INT2 Initialization */

	TIM1_Init();							/* TIMER1 Initialization */

	while(1)
	{
		displayTime();						/* Display time on 7-segment display */
	}

	return 0;
}

void Display_Init(void)
{
	DDRC  |= 0x0F;							/* Configure PC[0:3] (decoder pins) as output */
	PORTC &= 0xF0;							/* Set PC[0:3] Low */
	DDRA  |= 0x3F;							/* Configure PA[0:5] (7-segments enable pins) as output */
	PORTA &= 0xC0;							/* Set PA[0:5] Low */
}

void displayTime(void)
{
	PORTA = (PORTA & 0xC0) | (0x01);		/* Enable first 7-segment */
	PORTC = (PORTC & 0xF0) | (sec % 10);	/* Write first digit in seconds variable */
	_delay_ms(2);							/* delay for some time to appear for user */
	PORTA = (PORTA & 0xC0) | (0x02);		/* Enable second 7-segment */
	PORTC = (PORTC & 0xF0) | (sec / 10);	/* Write second digit in seconds variable */
	_delay_ms(2);							/* delay for some time to appear for user */

	PORTA = (PORTA & 0xC0) | (0x04);		/* Enable third 7-segment */
	PORTC = (PORTC & 0xF0) | (min % 10);	/* Write first digit in minutes variable */
	_delay_ms(2);							/* delay for some time to appear for user */
	PORTA = (PORTA & 0xC0) | (0x08);		/* Enable fourth 7-segment */
	PORTC = (PORTC & 0xF0) | (min / 10);	/* Write second digit in minutes variable */
	_delay_ms(2);							/* delay for some time to appear for user */

	PORTA = (PORTA & 0xC0) | (0x10);		/* Enable fifth 7-segment */
	PORTC = (PORTC & 0xF0) | (hr % 10);		/* Write first digit in hours variable */
	_delay_ms(2);							/* delay for some time to appear for user */
	PORTA = (PORTA & 0xC0) | (0x20);		/* Enable sixth 7-segment */
	PORTC = (PORTC & 0xF0) | (hr / 10);		/* Write second digit in hours variable */
	_delay_ms(2);							/* delay for some time to appear for user */
}

void INT0_Init(void)
{
	DDRD   &= ~(1 << PD2);					/* Configure PD2 as input */
	PORTD  |=  (1 << PD2);					/* Enable internal pull-up on PD2 */
	MCUCR  |=  (1 << ISC01);				/* Configure INT0 in failing edge */
	GICR   |=  (1 << INT0);					/* Enable external interrupt INT0 */
}

void INT1_Init(void)
{
	DDRD   &= ~(1 << PD3);					/* Configure PD3 as input */
	MCUCR  |= (1 << ISC10) | (1 << ISC11);	/* Configure INT1 in rising edge */
	GICR   |= (1 << INT1);					/* Enable external interrupt INT1 */
}

void INT2_Init(void)
{
	DDRB   &= ~(1 << PB2);					/* Configure PB2 as input */
	PORTB  |=  (1 << PB2);					/* Enable internal pull-up on PB2 */
	MCUCSR &= ~(1 << ISC2);					/* Configure INT2 in failing edge */
	GICR   |= (1 << INT2);					/* Enable external interrupt INT2 */
}

void TIM1_Init(void)
{
	TCNT1  = 0;								/* Reset TCNT1 register */
	OCR1A  = 975;							/* Load OCR register with required_ticks - 1 */
	TIMSK |= (1 << OCIE1A);					/* Enable Output Compare Interrupt */
	TCCR1A = (1 << FOC1A);					/* Enable Force output Compare */
	/* Configure Timer 1 in CTC Mode with 1024 pre-scaler */
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
}

/******************************************************************************************/
/**********************						ISRs					***********************/
/******************************************************************************************/
ISR(TIMER1_COMPA_vect)
{
	sec++;									/* Increment seconds variable every 1 S */
	/* Reset seconds variable if it reaches its max then increment minutes variable */
	if(sec == 60)
	{
		sec = 0;
		min++;
	}

	/* Reset minutes variable if it reaches its max then increment hours variable */
	if(min == 60)
	{
		min = 0;
		hr++;
	}
}

ISR(INT0_vect)
{
	sec = min = hr = 0;						/* Reset all stop watch variables */
}


ISR(INT1_vect)
{
	TIM1_DISABLE;							/* Disable Timer to pause stop watch */
}

ISR(INT2_vect)
{
	TIM1_ENABLE;							/* Enable Timer to resume stop watch */
}


