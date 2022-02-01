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

// #include <mega32A.h>    // SESUAIKAN DENGAN ATMEGA YANG DIGUNAKAN
// #include <delay.h>
// #include <lcd.h>

// GLOBAL VARIABLE DECLARATION
// SESUAIKAN DENGAN FREKUENSI SAMPLING YANG DIGUNAKAN;
const int FREQUENCY_SAMPLING = 2800;
const unsigned char ADC_VREF_TYPE = 0x60;

volatile int flag = 0;
unsigned char freq_smpl;

// ISR (Interrupt Service Routine handler)
/* Pakai sesuai code editor yang digunakan */

/* Versi Arduino IDE / compiler avr-gcc barebone : */
// ISR(TIMER0_OVF_vect)
// {
//     flag = 1;
// }

/* Versi CV-AVR */
interrupt[TIM0_OVF] void isr_TIM0_OVF(void)
{
    flag = 1;
}

/* FUNCTION DECLARATION */
unsigned char read_adc(unsigned char adc_input);
unsigned char sampling_rate(unsigned long Hz);
void init_hardware(void);

/* MAIN FUNCTION */
void main(void)
{
    int m0 = 0;
    int Y0 = 0;
    float x0 = 0.0, x1 = 0.0, x2 = 0.0, y0 = 0.0, y1 = 0.0, y2 = 0.0;

    // inisialisasi hardware
    init_hardware();

    // set nilai freq_sampl berdasarkan hasil konversi
    // menggunakan fungsi sampling_rate()
    freq_smpl = sampling_rate(FREQUENCY_SAMPLING);

    // enable all interrupt
    // sei = Set Enable Interrupt
    sei();

    while (1)
    {
        // set nilai TCNT0 sesuai hasil konversi pada fungsi
        // sampling_rate()
        TCNT0 = freq_smpl;

        // baca hasil mentah
        m0 = read_adc(0);

        // type cast ke tipe data float
        x0 = (float)m0;

        // aplikasikan persamaan y[n]
        y0 = ((1 * x0) - (0.223929 * x1) + (1 * x2) + (0.198804 * y1) - (0.788189 * y2));

        // typecast ke integer agar dapat dimasukkan sebagai nilai
        // PORT 8-bit
        Y0 = (int)y0;

        // keluarkan output
        // PORTC -> sinyal mentah m0
        // PORTC -> hasil DSP
        PORTC = m0;
        PORTD = Y0;

        // simpan hasil pembacaan
        x2 = x1;
        x1 = x0;
        y2 = y1;
        y1 = y0;

        // tunggu timer0 overflow, karena timer overflow
        // sesuai dengan sample rate
        // kemudian bersihkan flag, baru kembali ke loop while(1)
        while (flag == 0)
            ;
        flag = 0;
    }
}

/* FUNCTION DEFINITIONS */

/** init_hardware
 * @function
 * @brief
 * Melakukan inisialisasi hardware dari mikrokontroler.
 * Peripheral yang digunakan:
 *  1. PORTA - ADC input
 *  2. PORTB - LCD control
 *  3. PORTC - sinyal mentah hasil sampling
 *  4. PORTD - sinyal hasil filtering
 *  5. TIMER0 - Untuk sampling sinyal
 *  6. ADC - Channel 0, mode free running, VREF = 5V
 * */
void init_hardware()
{
    // SETTING PORT OUTPUT
    // PORTC = untuk sinyal mentah
    // PORTD = untuk sinyal hasil DSP filtering
    DDRC = 0xFF;
    PORTC = 0x00;
    DDRD = 0xFF;
    PORTD = 0x00;

    // SETTING TIMER
    // set timer 0 agar menggunakan clock external 16MHz
    // (bit CS01 s/d CS02 di SET 1)
    // enable overflow interrupt untuk timer-0
    TCCR0 = 0x03;
    TCNT0 = 0x00; // kosongkan counter

    TIMSK = 0x01;
    ACSR = 0x80;

    // ADC free running
    SFIOR = 0x00;

    // VREF ADC 5V
    DDRA = 0x00;
    ADMUX = ADC_VREF_TYPE & 0xff;
    ADCSRA = 0x84;
    SFIOR &= 0xEF;
}

/** @name read_adc
 * @function
 *
 * @brief
 * Membaca nilai hasil konversi dari ADC.
 * 1 kali pemanggilan fungsi = 1 hasil ADC (metode tanpa buffering)
 * */
unsigned char read_adc(unsigned char adc_input)
{
    ADMUX = adc_input | (ADC_VREF_TYPE & 0xFF);
    ADCSRA |= 0x40;

    while ((ADCSRA & 0x10) == 0)
        ;

    ADCSRA |= 0x10;

    return ADCH;
}

/** sampling_rate
* @function
*
* @brief
  Lakukan konversi dari satuan Hertz menjadi nilai
  yang dapat dimasukkan sebagai nilai awal register TCNT0.

  Nilai yang didapat dari fungsi ini akan menjadi nilai awal
  TCNT0 setiap kali Timer-0 overflow, agar event overflow Timer-0
  dapat diatur terjadi setiap T detik dengan akurat.
  -> T = 1 / FREQUENCY_SAMPLING
*/
unsigned char sampling_rate(unsigned long Hz)
{
    unsigned long temp = 255 - (999900 / (Hz * 4));
    return temp;
}
