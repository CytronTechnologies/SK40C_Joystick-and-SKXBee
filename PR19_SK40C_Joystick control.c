//==========================================================================
//	Author					: CYTRON
//	Project					: SK40C UART
//	Project description		: Test Functionality of SK40C UART using
//				  			  UC00A.This sample source code is valid for
//							  20MHz crystal.
//
//==========================================================================

//	include
//==========================================================================
#include <pic.h>

//	configuration
//==========================================================================
__CONFIG ( 0x3F32 );				//configuration for the  microcontroller

//	define
//==========================================================================
#define _XTAL_FREQ 20000000UL
#define	rs			RB4				//RS pin of the LCD display
#define	e			RB5				//E pin of the LCD display

#define	lcd_data	PORTD			//LCD 8-bit data PORT

#define	SW1			RB0
#define	SW2			RB1
#define joy_sw      RC5
#define	LED1		RB6
#define	LED2		RB7

//Wireless communicate
#define forward 56 //decimal value for ascii code char'8'
#define backward 50 //'2'
#define left 52 //'4'
#define right 54//'3'
#define clockwise 53//'5'
#define stop 48//'0'
//	function prototype				(every function must have a function prototype)
//==========================================================================

void delay(unsigned long data);

void send_config(unsigned char data);
void send_char(unsigned char data);
void lcd_goto(unsigned char data);
void lcd_clr(void);
void lcd_init(void);
void send_string(const char *s);
void lcd_bcd(unsigned char uc_digit, unsigned int ui_number);
void uart_init();
unsigned char uart_rec(void);			//receive uart value
void uart_send(unsigned char data);
void uart_str(const char *s);
void send_cmd(unsigned char num, unsigned int data, unsigned int ramp);
void delay_ms(unsigned int ui_value);

void setup_adc ( void );
int ADC_read(unsigned char ch);
void xbee_init(void);
//	global variable
//==========================================================================
unsigned int result,x_axis, y_axis,a;
unsigned int rec_data;
unsigned int *rec;
unsigned *rec1;
//	main function					(main fucntion of the program)
//==========================================================================
void main()
{
	unsigned long delay_time=5000;
	unsigned int a=0;

	//set I/O input output
	TRISB = 0b00000011;					//configure PORTB I/O direction
	TRISD = 0b00000000;					//configure PORTD I/O direction
	TRISA = 0b00001111;	//AN0,AN1,AN2=adc input,AN3=Vref	//configure PORTA I/O direction
	TRISC = 0b10100000;
	//Configure UART
	uart_init();
	//setup ADC
	setup_adc();
	xbee_init();
	lcd_init();
	lcd_clr();							//clear lcd
	lcd_goto(0);						//set the lcd cursor to location 0
	send_string("Cytron_Joy_Stick_");		//
	lcd_goto(20);						//set the lcd cursor to location 0
	send_string("SK40C controller");		//
	LED1=0;								// OFF LED1
	LED2=0;

	while(1)
	{

		x_axis=ADC_read(1);
		y_axis=ADC_read(2);
		if (x_axis<500)//center value is 2.296V decimal=508
			{
			lcd_clr();
			lcd_goto(0);
			send_string("  Joy_forward");
			lcd_goto(20);
			send_string("  X_axis:");
			lcd_goto(30);
			lcd_bcd(4,x_axis);
			uart_send(forward);
			delay_ms(200);
			}
		else	if (x_axis>508)//center value is 2.348V decimal=520
			{
			lcd_clr();
			lcd_goto(0);
			send_string("  Joy_backward");
			lcd_goto(20);
			send_string("  X_axis:");
			lcd_goto(30);
			lcd_bcd(4,x_axis);
			uart_send(backward);
			delay_ms(200);
			}
		else if (y_axis<500)
			{
			lcd_clr();
			lcd_goto(0);
			send_string("    Joy_left");
			lcd_goto(20);
			send_string("  X_axis:");
			lcd_goto(30);
			lcd_bcd(4,y_axis);
			uart_send(left);
			delay_ms(200);

			}
		else if (y_axis>520)
			{
			lcd_clr();
			lcd_goto(0);
			send_string("    Joy_right");
			lcd_goto(20);
			send_string("  Y_axis:");
			lcd_goto(30);
			lcd_bcd(4,y_axis);
			uart_send(right);
			delay_ms(200);
			}
		else if (joy_sw==0)
			{
			lcd_clr();
			lcd_goto(0);
			send_string("SW is pressed");
			lcd_goto(20);
			send_string("robot:clockwise");
			uart_send(clockwise);
			delay_ms(200);
			}
		else
			{
			lcd_clr();
			lcd_goto(0);
			send_string("   Joy_center");
			lcd_goto(20);
			send_string("zero_axis:");
			lcd_goto(31);
			lcd_bcd(4,x_axis);
			uart_send(stop);
			delay_ms(200);
			}
	}
}

//	functions
//==========================================================================
void xbee_init(void)
{
			uart_str("+++");//send command to enter XBee Pro command mode
			delay_ms(200);// waiting for finish sending and XBee respond

			uart_str("atmy11");//send command to setting Source address
			uart_send(0xD);// 0XD is hexa code of "Enter"key
			delay_ms(200);

			uart_str("atwr");// send "WR" (write)command to SKXBee
			uart_send(0xD);//0XD is hexa code of "Enter"key
			delay_ms(200);

			uart_str("atdl22");// send command to setting Destination address
			uart_send(0xD);//0XD is hexa code of "Enter"key
			delay_ms(200);

			uart_str("atwr");//send "WR" (write)command to SKXBee
			uart_send(0xD);//0XD is hexa code of "Enter"key
			delay_ms(200);

			uart_str("atcn");// send command for Exit AT Command Mode
			uart_send(0xD);//0XD is hexa code of "Enter"key
			delay_ms(200);

}
void setup_adc ( void )
{
   ADCON0 = 0b10000000;   //ADC off
   ADCON1 = 0b11000011;   //right justified,ADCS=0 FOSC32,AN0,AN1,AN2,AN4=analog IN AN3=Vref
}
int ADC_read(unsigned char ch)
{

   switch (ch)
   {
      case 1://x axis
      ADCON0 = 0b10000001; //AN0
      break;
      case 2://y axis
      ADCON0 = 0b10001001;//AN1
      break;
      case 3:
      ADCON0 = 0b10010001;//AN2
      break;
      default:
      ADCON0 = 0b10000000;
   }

   delay_ms(20);
   GODONE=1;      //cytron com=GODONE my laptop=GO        //start to covert
   while(GODONE) continue;//waiting ADC finish convert value
   ADON=0;              //switch off ADC
   result=(ADRESL+(ADRESH<<8));//for 10bit adc
   return result;//return 10bit of result value
}
void lcd_init(void)
{
	//configure lcd
	send_config(0b00000001);			//clear display at lcd
	send_config(0b00000010);			//lcd return to home
	send_config(0b00000110);			//entry mode-cursor increase 1
	send_config(0b00001100);			//display on, cursor off and cursor blink off
	send_config(0b00111000);			//function set
}
void uart_init(void)
{
	SPBRG=129;			//set baud rate as 9600 baud
	BRGH=1;				//baud rate high speed option
	TXEN=1;				//enable transmission
	TX9 =0;				//8-bit transmission
	RX9 =0;				//8-bit reception
	CREN=1;				//enable reception
	SPEN=1;				//enable serial port
}
void delay(unsigned long data)			//delay function, the delay time
{										//depend on the given value
	for( ;data>0;data--);
}

void delay_ms(unsigned int ui_value)
{
   while (ui_value-- > 0)
   {
      __delay_ms(1);   // must not over 39ms
   }
}
void send_config(unsigned char data)	//send lcd configuration
{
	rs=0;								//set lcd to configuration mode
	lcd_data=data;						//lcd data port = data
	e=1;								//pulse e to confirm the data
	delay(50);
	e=0;
	delay(50);
}

void send_char(unsigned char data)		//send lcd character
{
 	rs=1;								//set lcd to display mode
	lcd_data=data;						//lcd data port = data
	e=1;								//pulse e to confirm the data
	delay(10);
	e=0;
	delay(10);
}

void lcd_goto(unsigned char data)		//set the location of the lcd cursor
{										//if the given value is (0-15) the
 	if(data<16)							//cursor will be at the upper line
	{									//if the given value is (20-35) the
	 	send_config(0x80+data);			//cursor will be at the lower line
	}									//location of the lcd cursor(2X16):
	else								// -----------------------------------------------------
	{									// | |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15| |
	 	data=data-20;					// | |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35| |
		send_config(0xc0+data);			// -----------------------------------------------------
	}
}

void lcd_clr(void)						//clear the lcd
{
 	send_config(0x01);
	delay(600);
}

void send_string(const char *s)			//send a string to display in the lcd
{
  	while (s && *s)send_char (*s++);
}

unsigned char uart_rec(void)			//receive uart value
{
	unsigned int rec_data;
	while(RCIF==0);						//wait for data
	rec_data = RCREG;
	return rec_data;					//return the data received
}

void uart_send(unsigned char data)
{
	while(TXIF==0);				//only send the new data after
	TXREG=data;					//the previous data finish sent
}

void uart_str(const char *s)
{
	while(*s)uart_send(*s++);  // UART sending string
}

void lcd_bcd(unsigned char uc_digit, unsigned int ui_number)
{
        unsigned int ui_decimal[5] ={ 0 };
        //extract 5 single digit from ui_number
                ui_decimal[4] = ui_number/10000;        // obtain the largest single digit, digit4
                ui_decimal[3] = ui_number%10000;        // obtain the remainder
                ui_decimal[2] = ui_decimal[3]%1000;
                ui_decimal[3] = ui_decimal[3]/1000;                        // obtain the 2nd largest single digit, digit3
                ui_decimal[1] = ui_decimal[2]%100;
                ui_decimal[2] = ui_decimal[2]/100;                        // obtain the 3rd largest single digit, digit2
                ui_decimal[0] = ui_decimal[1]%10;                        // obtain the smallest single digit, digit0
                ui_decimal[1] = ui_decimal[1]/10;                        // obtain the 4th largest single digit, digit1

                if (uc_digit > 5) uc_digit = 5;                        // limit to 5 digits only
                for( ; uc_digit > 0; uc_digit--)
                {
                        send_char(ui_decimal[uc_digit - 1] + 0x30);
                }
}