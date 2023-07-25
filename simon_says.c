#include <mega164a.h>
#include <alcd.h>
#include <stdio.h>
#include <delay.h>
#include <stdbool.h>
#include <stdlib.h>


#define DATA_REGISTER_EMPTY (1<<UDRE0)
#define RX_COMPLETE (1<<RXC0)
#define FRAMING_ERROR (1<<FE0)
#define PARITY_ERROR (1<<UPE0)
#define DATA_OVERRUN (1<<DOR0)



#pragma used+
char getchar1(void)
{
char status,data;
while (1)
      {
      while (((status=UCSR1A) & RX_COMPLETE)==0);
      data=UDR1;
      if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
         return data;
      }
}
#pragma used-

#pragma used+
void putchar1(char c)
{
while ((UCSR1A & DATA_REGISTER_EMPTY)==0);
UDR1=c;
}
#pragma used-
 
eeprom int hs = 0; //highscore (eeprom so that it doesn't reset in case of power failure/reset/unplugging)

void main(void)
{

bool g_over;
int score = 0;
char data[2];
int dly; //delay between LEDs
int nr_leds;
int i;
int spin = 0;
int values[8];


#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (1<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (1<<PORTA3) | (1<<PORTA2) | (1<<PORTA1) | (1<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=P Bit2=P Bit1=P Bit0=P 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (1<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;


ASSR=(0<<EXCLK) | (0<<AS2);
TCCR2A=(0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (0<<WGM21) | (0<<WGM20);
TCCR2B=(0<<WGM22) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (0<<TOIE0);

TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);


TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (0<<TOIE2);


EICRA=(0<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EIMSK=(0<<INT2) | (0<<INT1) | (0<<INT0);
PCICR=(0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);


UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x81;


UCSR1A=(0<<RXC1) | (0<<TXC1) | (0<<UDRE1) | (0<<FE1) | (0<<DOR1) | (0<<UPE1) | (0<<U2X1) | (0<<MPCM1);
UCSR1B=(0<<RXCIE1) | (0<<TXCIE1) | (0<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1) | (0<<UCSZ12) | (0<<RXB81) | (0<<TXB81);
UCSR1C=(0<<UMSEL11) | (0<<UMSEL10) | (0<<UPM11) | (0<<UPM10) | (0<<USBS1) | (1<<UCSZ11) | (1<<UCSZ10) | (0<<UCPOL1);
UBRR1H=0x00;
UBRR1L=0x81;


ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
ADCSRB=(0<<ACME);

DIDR1=(0<<AIN0D) | (0<<AIN1D);

ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);


SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);


lcd_init(16);




     
while (1)
      {
        bool round_over;
        int checker;
        bool lost = false;
        dly = 1000;
        PORTA = 1;
        
        //Starting Procedure 
        while(PIND.5 == 1)
        {   
            
            PORTA = PORTA << 1;
            delay_ms(50);
            if(PORTA == 16)
                PORTA = 1;
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts("Press USR: play");
            spin ++;        
        }
        
        PORTA = 0;
        srand(spin);
        //Randomizing the patterns
        for(i = 0 ; i < 8 ; i++)
        {   
            
            values[i] = 1 << rand() % 3;
        }
        
        g_over = false;
        score = 0; 
        nr_leds = 1;
        
        //Start game
        while(g_over == false)
        {   
            //LCD interface
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts("High Score:");
            itoa(hs , data);
            lcd_puts(data);
            itoa(score , data);
            lcd_gotoxy(0,1);
            lcd_puts("Score:");
            lcd_puts(data);
            
            delay_ms(500);
            PORTA = 0;
            
        //----Showing the sequence----    
        for(i = 0 ; i < nr_leds ; i++)
        {
            PORTA = values[i];
            delay_ms(dly);
            PORTA = 0;
            delay_ms(dly);
        }
        //---------------------------
        
        
        
        round_over = false;
        
        
        while(round_over == false)
        {   
            for(i = 0 ; i < nr_leds ; i++)
            {   
                checker = 0; 
                
                //--------Inputting answer------
                while(checker == 0)
                {   
                    //LED0
                    if(!PINB.0)
                    {
                        PORTA.0 = 1;
                        checker = 0b0001; 
                    }
                        
                    else
                    
                    {    
                        PORTA.0 = 0;
                    }
                    
                    //LED1         
                    if(!PINB.1)
                    {
                        PORTA.1 = 1;
                        checker = 0b0010; 
                    }
                        
                    else
                    
                    {    
                        PORTA.1 = 0;
                    }
                    
                    //LED2                
                    if(!PINB.2)
                    {
                        PORTA.2 = 1;
                        checker = 0b0100; 
                    }
                        
                    else
                    
                    {    
                        PORTA.2 = 0;
                    }
                    
                    //LED3            
                    if(!PINB.3)
                    {
                        PORTA.3 = 1;
                        checker = 0b1000; 
                    }
                        
                    else
                    
                    {    
                        PORTA.3 = 0;
                    }
                    
                }
                //------------------------------
                
                delay_ms(20);
                
                if(checker != values[i]) //Check if answer is correct
                {
                    lost = true;
                    g_over = true;
                    
                    //FLASH LIGHTS
                    PORTA = 0b00001111;
                    delay_ms(200);
                    PORTA = 0;
                    delay_ms(200);
                    PORTA = 0b00001111;
                    delay_ms(200);
                    PORTA = 0;
                    delay_ms(200);
                    //END FLASH LIGHTS
                    
                    break;     
                }
                
                delay_ms(1000);    
            }
            
            round_over = true;
            
            if(lost == false)
            {
                score++;
            }             
        }   
        
        
         
        nr_leds++;
        dly-=100; //Raise difficulty
        
        if(score == 8) //Game ends at 8 points
        {    
            g_over = true;           
        }
        
        delay_ms(1000);
        
       
        
                           
      }
      
      //--------Check Outcome------
      if(lost == true)
        {
            lcd_clear();
            lcd_gotoxy(0,0);
            if(score > hs)
            {
                hs = score;
                lcd_puts("New High Score:");
            }
            else
            { 
                lcd_puts("Final Score:");
            }
                   
            itoa(score , data);
            lcd_puts(data);
            delay_ms(3000);
              
        }
      else
        {
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts("Congratulations!");
            lcd_gotoxy(0,1);
            lcd_puts("Final Score:");
            itoa(score , data);
            lcd_puts(data);
        
        }
        //-------------------------
}
}
