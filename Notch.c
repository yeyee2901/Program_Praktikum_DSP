/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Evaluation
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 4/16/2019
Author  : Freeware, for evaluation and non-commercial use only
Company : 
Comments: 


Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32A.h>
#include <delay.h>
#include <lcd.h>	// library bawaan 

// cek daftar alamat register di datasheet mikro-nya, klo disini 0x18 = address PORTB
// ada di Datasheet bagian paling bawah
#asm
   .equ __lcd_port=0x18
#endasm

#define FREQUENCY_SAMPLING 2800
#define ADC_VREF_TYPE 0x60
bit flag = 0;
unsigned char freq_smpl;
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
      flag = 1;
}
unsigned char read_adc(unsigned char adc_input)
{
        ADMUX = adc_input | (ADC_VREF_TYPE & 0xff);
        ADCSRA|=0x40;
        while ((ADCSRA & 0x10)==0);
        ADCSRA|=0x10;
        return ADCH;                         
}
unsigned char sampling_rate(unsigned long Hz)
{
    unsigned long temp;
    temp=255-(999900/(Hz*4));
    return temp;
}
void Init_Hardware()
{
    PORTA=0x00;
    DDRA=0x00;
    PORTB=0x00;
    DDRB=0x00;
    PORTC=0x00;
    DDRC=0xFF;
    PORTD=0x00;
    DDRD=0xFF;
    TCCR0=0x03;
    TCNT0=0x00;
    OCR0=0x00;
    TIMSK=0x01;
    ACSR=0x80;
    SFIOR=0x00;
    ADMUX=ADC_VREF_TYPE & 0xff;
    ADCSRA=0x84;
    SFIOR&=0xEF;
    lcd_init(16);
}
void main(void)
{
    int m0;
    int Y0;
    float x0=0, x1=0,x2=0,y0= 0.0, y1=0,y2=0;      
    Init_Hardware();
    freq_smpl=sampling_rate(FREQUENCY_SAMPLING); // Set the Sampling Rate 
    lcd_gotoxy(0,1);
    lcd_putsf("FS : ");
    lcd_putchar(FREQUENCY_SAMPLING/10000%10  +0x30);
    lcd_putchar(FREQUENCY_SAMPLING/1000%10  +0x30);
    lcd_putchar(FREQUENCY_SAMPLING/100%10  +0x30);
    lcd_putchar(FREQUENCY_SAMPLING/10%10  +0x30);
    lcd_putchar(FREQUENCY_SAMPLING%10  +0x30);
    lcd_putsf(" Hz-char");
    #asm("sei")
    while (1)                        
    {   
        TCNT0 = freq_smpl;    // Start Timer
        m0 = read_adc(0);
        x0 = (float)m0;
        y0 = (1*x0)-(0.223929* x1)+(1*x2)+(0.198804 *y1)-(0.788189*y2); 
        Y0 = (int)y0;
        PORTC = m0;                                                                              
        PORTD = Y0;
        x2=x1;
        x1=x0;
        y2=y1;
        y1=y0;
        while ( flag == 0 );    // Wait Interrupt Flag Overflow
        flag = 0;  
    }
}
