//a program that outputs the temperature of a ds18b20 sensor to LEDs on PORTA

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT        // Oscillator Selection bits (LP oscillator: Low-power crystal on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = OFF         // Data Code Protection bit (Data memory code protection is enabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF       // Flash Program Memory Self Write Enable bits (0000h to 0FFFh write protected, 1000h to 1FFFh may be modified by EECON control)

#include <xc.h>
#define _XTAL_FREQ 4000000L

//function prototypes
void init(void);
void write_byte(int);
void write_zero(void);
void write_one(void);
int read_bit(void);
int read_byte(void);
int transform_data(int, int);

void main(void) {
    int lsbyte = 0;
    int msbyte = 0;
    int resultbyte = 0;

    init();
    write_byte(0xCC);
    write_byte(0x44);
    __delay_ms(1000);
    init();
    write_byte(0xCC);
    write_byte(0xBE);
    lsbyte = read_byte();
    msbyte = read_byte();
    resultbyte = transform_data(lsbyte, msbyte);
    TRISA = 0x00;
    PORTA = (char) ~resultbyte;
    __delay_ms(2000);
    return;
}

void init(void) {
    TRISC = 0xEF;
    PORTC = 0xEF;
    __delay_us(480);
    TRISC = 0xFF;
    __delay_us(480);
    return;
}

void write_byte(int byte) {
    int bitvalue = 0;
    for (int i = 8; i > 0; i--) {
        bitvalue = byte & 0x01;  //bitvalue takes the value of the lsb
        if (bitvalue == 0) {
            write_zero();
        } else {
            write_one();
        }
        byte = byte >> 1;  //shift to the next bit of lsb
    }
}

void write_zero(void) {
    TRISC = 0x00;
    PORTCbits.RC4 = 0;
    __delay_us(60);
    TRISC = 0xFF;
    __delay_us(1);
}

void write_one(void) {
    TRISC = 0x00;
    PORTCbits.RC4 = 0;
    __delay_us(1);
    TRISC = 0xFF;
    __delay_us(60);
}

int read_bit(void) {
    TRISC = 0x00;
    PORTCbits.RC4 = 0;
    __delay_us(5);
    TRISC = 0xFF;
    __delay_us(5);
    if (PORTCbits.RC4 == 0) {
        __delay_us(55);
        return 0;
    } else {
        __delay_us(55);
        return 1;
    }
}

int read_byte(void) {
    int result = 0;
    int bit = 0;
    for (int i = 8; i > 0; i--) {
        bit = read_bit();
        if (bit == 0) {
            result = result | 0x00;
        } else {
            result = result | 0x80;  //put the 1 at the left end of result
            //0x80 is 10000000 in binary
        }
        if (i != 1) {  //don't shift after the last bit was read
            result = result >> 1;
        }
    }
    return result;
}

int transform_data(int lsbyte, int msbyte) {
    int result = 0;
    lsbyte = 0xF0 & lsbyte;
    lsbyte = lsbyte >> 4;
    msbyte = 0x0F & msbyte;
    msbyte = msbyte << 4;
    result = lsbyte | msbyte;
    return result;
}
