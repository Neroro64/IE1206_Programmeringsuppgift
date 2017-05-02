/* ADvolt.c 16F690 1 value AD-logg            */
/* Sends A2 value to UART-tool on key-press   */

/* B Knudsen Cc5x C-compiler - not ANSI-C */
#include "16F690.h"
#pragma config |= 0x00D4 

/* This value is correct for unipolar AD values                      */ 
#define OFFSET 512
/* The SCALE_FACTOR value is wrong - you should correct it!          */
#define SCALE_FACTOR 49
/* The DECIMALS value is wrong - you should correct it!              */
#define DECIMALS 4
/* This value is correct the AD-value is unsigned (allways positive) */
#define UN_SIGNED 1

/* Decimal mark: point or comma - what do you like?                   */
#define DECIMAL_MARK ','

void initserial( void );
void ADinit( void );
void putchar( char );
void string_out( const char * ); 
void longDecimal_out(long number, char decimalPlaces, bit un_signed); 
void delay10( char );

void main(void)
{
  unsigned long advalue;
  TRISC.0 = 0; // lightdiode at RC0 is output
  PORTC.0 = 0; // no light
  TRISA.3 = 1; // switch SW is input

  initserial();
  ADinit();
  delay10(100); 

  // Header text
  string_out("U [V]\r\n");

  while(1)
   {
     while(PORTA.3) ; // wait for key pressed - new measurement 
     PORTC.0=1;       // LED Sampling indicator
	
      /* Now measure the Voltage [V]  */
      GO=1;         // start AD
      while(GO);    // wait for done
      advalue = ADRESH*256;    // read result 10 bit
      advalue += ADRESL;
	  advalue -= OFFSET;  // no offset needed
      // 1024 -> 5,0000 [V]
      // multiply with integer scalefactor
      // and place the decimal mark correct
      // the supplied scalefactor is wrong please correct it!
	  advalue *= SCALE_FACTOR ;  

	  // the supplied number of decimals is wrong please correct it!
      longDecimal_out(advalue, DECIMALS, UN_SIGNED); 
      putchar('\r'); putchar('\n');

      delay10(1);         // Debounce
      PORTC.0=0;          // LED off measurement done 
      while (!PORTA.3) ;  // wait for key released
      delay10(1);         // Debounce
     }
}




/* *********************************** */
/*            FUNCTIONS                */
/* *********************************** */


/* **** ADconverter function ************** */

void ADinit( void )
{
  // AD setup 
  ANSEL.2 = 1; // RA2 AN2 analog configurated
  TRISA.2=1;   // AN2 input

  ADCON1 = 0b0.101.0000; // AD conversion clock 'fosc/16'

  /* 
     1.x.xxxx.x.x  ADRESH:ADRESL is 10 bit right justified
     x.0.xxxx.x.x  Vref is Vdd
     x.x.0010.x.x  Channel (AN2) 
     x.x.xxxx.0.x  Go/!Done - start from program later
     x.x.xxxx.x.1  Enable AD-converter
  */
  ADCON0 = 0b1.0.0010.0.1; 
}


/* **** bitbanging serial communication **** */

void initserial( void )  /* initialise PIC16F690 bbCom */
{
   ANSEL.0 = 0; // No AD on RA0
   PORTA.0 = 1; // marking line
   TRISA.0 = 0; // output to PK2 UART-tool
   return;      
}


void putchar( char ch )  // sends one char bitbanging
{
  char bitCount, ti;
  PORTA.0 = 0; // set startbit
  for ( bitCount = 10; bitCount > 0 ; bitCount-- )
   {
     // delay one bit 104 usec at 4 MHz
     // 5+18*5-1+1+9=104 without optimization 
     ti = 18; do ; while( --ti > 0); nop(); 
     Carry = 1;     // stopbit
     ch = rr( ch ); // Rotate Right through Carry
     PORTA.0 = Carry;
   }
  return;
}

void string_out(const char * string)
{
  char i, k;
  for(i = 0 ; ; i++)
   {
     k = string[i];
     if( k == '\0') return;   // found end of string
     putchar(k); 
   }
}


/* **** print decimal number **** */

void longDecimal_out(long number, char decimalPlaces, bit un_signed)
{
   char string[7]; // temporary buffer for reordering characters
   char i,temp;
   string[6] = '\0';
   string[0] = '+'; 
 if(!un_signed)
  {
    if (number < 0 )
     {
       string[0] = '-'; 
       number = -number;
     }
  } 
  
   for (i = 5; ;i--)
     {
       temp = (uns16)number % 10;
       temp += '0';
       string[i]=temp;
       if (i==1) break;
       (uns16)number /= 10;
     }
   for(i = 0 ; ; i++)
     {
        if(i==6-decimalPlaces) putchar( DECIMAL_MARK ); 
        temp = string[i];
        if( temp == '\0') return;   // found end of string
        putchar(temp); 
     }
} 


/* **** delay function **** */

void delay10( char n)
{
    char i;
    OPTION = 7;
    do  {  i = TMR0 + 39; /* 256 microsec * 39 = 10 ms */
        while ( i != TMR0) ;
    } while ( --n > 0);
}


/* *********************************** */
/*            HARDWARE                 */
/* *********************************** */


/*
   Use "PICkit2 UART Tool" as a 9600 Baud terminal to save data.
   with BitBanging routines.
           _____________  ____________    
          |             \/            | 
    +5V---|Vdd        16F690       Vss|---GND
          |RA5               RA0/(PGD)|bbTx ->- PK2Rx/PGD
          |RA4/AN3   AN1/REF/RA1/(PGC)|------<- PGC
          |RA3/!MCLR/(Vpp) RA2/AN2/INT|-<- U
          |RC5/CCP                 RC0|->- LED
          |RC4                     RC1|
          |RC3                     RC2|
          |RC6                     RB4|
          |RC7                  RB5/Rx|
          |RB7/Tx                  RB6|-<- SW
          |___________________________|
*/


