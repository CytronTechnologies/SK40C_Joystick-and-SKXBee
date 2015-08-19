//==========================================================================
//	Author				: 	Cytron Technologies
//	Project				: 	PR19- Flexibot-Using Transwheel
//	Project description	: 	Cytron Flexibot which can move in any 
//			           		direction without having to turn relative to the robot base
//****************************************************
//ENSURE THE XBEE1 address has been matched with XBEE2 
//****************************************************
//==========================================================================
//
//	include
//==========================================================================
#include <pic.h> 						//include the PIC microchip header file

//	configuration
//==========================================================================
__CONFIG(0x3FA2);						//configuration word register 1 for the  microcontroller
__CONFIG(0x3FBC);						//configuration word register 2 for the  microcontroller

//	assign global variable
//==========================================================================v
		
									
//	define
//========================================================================== 
#define _XTAL_FREQ 20000000UL
#define	rs			RB7					//RS pin of the LCD display
#define e			RB6					//E pin of the LCD display
#define button1		RA0					//button (active low)
#define button2		RA1					//button (active low)
#define	lcd_light	RA2					//Background light of LCD display
#define	buzzer		RA3					//Buzzer control pin
#define	lcd_data	PORTD				//LCD 8-bit data PORT
#define lmspeed		CCPR1L				//left motor driver speed control pin(L298N enable pin)
#define lmotor1		RC0					//left motor driver input1
#define lmotor2		RC3					//left motor driver input2
#define	rmspeed		CCPR2L				//right motor driver speed control pin(L298N enable pin)
#define rmotor1		RC4					//right motor driver input1
#define rmotor2		RC5					//right motor driver input2
#define	bmspeed		CCPR3L				//back motor driver speed control pin(L298N enable pin)
#define bmotor1		RB3					//back motor driver input1
#define bmotor2		RB4					//back motor driver input2
#define	exspeed		RB2					//extra motor driver speed control pin(L298N enable pin)
#define emotor1		RB1					//extra motor driver input1
#define emotor2		RB0					//extra motor driver input2

//	function prototype					(every function must have a function prototype)
//==========================================================================

void m_stop(void);
void lm_run(unsigned char dir);
void rm_run(unsigned char dir);
void bm_run(unsigned char dir);
void clockwise(void);
void anticlockwise(void);
void delay(unsigned long data);		
void lcd_init(void);
void send_config(unsigned char data);
void send_char(unsigned char data);
void lcd_goto(unsigned char data);
void lcd_clr(void);
void send_string(const char *s);
void mode_display(const char *x,const char *y);
void PWM_setup (void);

void forward (void);
void backward (void);
void left (void);
void right (void);

void uart_init(void);
unsigned char uart_rec(void);
void uart_send(unsigned char data);
void uart_str(const char *s);

void xbee_init(void);
void delay_ms(unsigned int ui_value);
//	main function						(main fucntion of the program)


//global variable
unsigned char i=0,shift=0;	
unsigned int rec_data,a;

//==========================================================================
void main(void)
{
//setup ADC
ADCON1 = 0b00001111;					//set ADx pin digital I/O

//set I/O input output
TRISA = 0b00000011;						//configure PORTA I/O direction
TRISB = 0b00000000;						//configure PORTB I/O direction
TRISC = 0b10000000;						//configure PORTC I/O direction
TRISD = 0b00000000;						//configure PORTD I/O direction
PORTB = 0x00;							//Clear port B all output pin
PORTC = 0x00;							//Clear port C all output pin	
PORTD = 0x00;							//Clear port D all output pin
	
lcd_init();
uart_init();
PWM_setup();

buzzer=0;
lcd_clr();
lcd_goto(0);
send_string("Xbee Communicate");
lcd_goto(20);
send_string("    Flexibot");

//program start
	while(1)							//Infinity Loop
	{

			a=uart_rec();
			if(a=='8')
			{
				forward();
				uart_str("Flexibot running forward");
				delay_ms(100);
			}
			else if(a=='2')
			{
				backward();
				uart_str("Flexibot running backward");
				delay_ms(100);
			}
			else if (a=='4')
			{
				left();
				uart_str("Flexibot running left");
				delay_ms(100);
			}
			else if (a=='6')
			{
				right();
				uart_str("Flexibot running right");
				delay_ms(100);
			}	
			else if (a=='5')
			{
				clockwise();
				uart_str("Flexibot rotating clockwise");
				delay_ms(100);
			}	
			else if (a=='3')
			{
				anticlockwise();	
				uart_str("Flexibot rotating anticlockwise");
				delay_ms(100);
			}
			else if (a=='0')	
			{
				m_stop();
				uart_str("Flexibot stop");
				delay_ms(100);
			}
		

	}
}
//function

void PWM_setup (void)
{
//Setup up PWM operation
PR2=255;								//Set PWM period
CCP1CON = 0b00001100;					//Configure CCP1CON to on the PWM1 operation
CCP2CON = 0b00001100;					//Configure CCP2CON to on the PWM2 operation
CCP3CON = 0b00001100;					//Configure CCP3CON to on the PWM3 operation
T2CON   = 0b00000100;					//On timer 2 for PWM & set prescale 1
lmspeed = 0;							//Clear left motor speed
rmspeed = 0;							//Clear right motor speed
bmspeed = 0;							//Clear back motor speed
}

//uart function
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
	while(*s)uart_send(*s++);
}

//========================================================================
void forward (void)											//Function to run Flexibot 0 degree
{
	lmspeed=rmspeed=255;									//assign speed to respective motor pwm pin
	bmspeed=0;
	lm_run(1);												//assign the direction to respective motor
	rm_run(0);
	mode_display("    Flexibot","  Run forward");					    		//display the current situation
}
void backward (void)											//Function to run Flexibot 180 degree
{
	lmspeed=rmspeed=255;
	bmspeed=0;
	lm_run(0);
	rm_run(1);
	mode_display("    Flexibot","  Run backward");	
}
void left (void)
{
	lmspeed=190;
	rmspeed=190;
	bmspeed=255;
	lm_run(0);
	rm_run(0);
	bm_run(1);
	mode_display("    Flexibot","   Run to left");

}
void right(void)
{
	lmspeed=190;
	rmspeed=190;
	bmspeed=255;
	lm_run(1);
	rm_run(1);
	bm_run(0);
	mode_display("    Flexibot","   Run to right");

}
// Motor Control function
//==========================================================================
void m_stop(void)											//Function to stop the motor
{
	lmotor1=0;
	lmotor2=0;
	rmotor1=0;
	rmotor2=0;
	bmotor1=0;
	bmotor2=0;		
    mode_display("    Flexibot","    Stop Run");						//display the current situation
}

void lm_run(unsigned char dir)								//Function to run the left motor
{
	lmotor1=dir;											//assign variable "dir" to left motor pin 1
	lmotor2=!dir;											//assign the oppesite value of "dir" variable to left motor pin 2
} 

void rm_run(unsigned char dir)								//Function to run the right motor
{
	rmotor1=dir;											//assign variable "dir" to right motor pin 1
	rmotor2=!dir;											//assign the oppesite value of "dir" variable to right motor pin 2
} 

void bm_run(unsigned char dir)								//Function to run the back motor
{
	bmotor1=!dir;											//assign variable "dir" to back motor pin 1
	bmotor2=dir;											//assign the oppesite value of "dir" variable to back motor pin 2	
} 

void clockwise(void)										//Function to run Flexibot clockwise
{
	lmspeed=rmspeed=bmspeed=255;
	lm_run(1);
	rm_run(1);
	bm_run(1);
	mode_display("  Flexibot","    Clockwise");
}

void anticlockwise(void)									//Function to run Flexibot anticlockwise
{
	lmspeed=rmspeed=bmspeed=255;
	lm_run(0);
	rm_run(0);
	bm_run(0);
	mode_display("  Flexibot"," Anticlockwise");
}

//	delay functions
//==========================================================================
void delay(unsigned long data)								//delay function, the delay time
{															//depend on the given value
	for( ;data>0;data-=1);
}

//	lcd functions
//==========================================================================
void lcd_init(void)
{
//configure lcd
send_config(0b00000001);				//clear display at lcd
send_config(0b00000010);				//lcd return to home 
send_config(0b00000110);				//entry mode-cursor increase 1
send_config(0b00001100);				//display on, cursor off and cursor blink off
send_config(0b00111000);				//function set
lcd_light=1;							//switch off background light

}
void send_config(unsigned char data)						//send lcd configuration 
{
	rs=0;													//set lcd to configuration mode
	lcd_data=data;											//lcd data port = data
	e=1;													//pulse e to confirm the data
	delay(50);
	e=0;
	delay(50);
}

void send_char(unsigned char data)							//send lcd character
{
	rs=1;													//set lcd to display mode
	lcd_data=data;											//lcd data port = data
	e=1;													//pulse e to confirm the data
	delay(10);
	e=0;
	delay(10);
}


void lcd_goto(unsigned char data)								//set the location of the lcd cursor
{																//if the given value is (0-15) the 
 	if(data<16)													//cursor will be at the upper line
	{															//if the given value is (20-35) the 
	 	send_config(0x80+data);									//cursor will be at the lower line
	}															//location of the lcd cursor(2X16):
	else														// -----------------------------------------------------
	{															// | |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15| |
	 	data=data-20;											// | |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35| |
		send_config(0xc0+data);									// -----------------------------------------------------	
	}
}

void lcd_clr(void)												//clear the lcd
{
 	send_config(0x01);
	delay(600);	
}

void send_string(const char *s)									//send a string to display in the lcd
{          
  	while (s && *s)send_char (*s++);
}


//	Mode display function
//==========================================================================
void mode_display(const char *x,const char *y)
{
    buzzer=0;		                       						//clear buzzor
	if(button1 == 0)return;										//condition to exit loop when button is pushed
	lcd_clr();													//clear lcd
	lcd_goto(0);												//set the lcd cursor to first line and first cursor 	
	send_string(x);												//display character in line 1
	lcd_goto(20);												//seet the lcd cursor to second line and first cursor 	
	send_string(y);												//display character in line 2
	delay(10000);                     							  //delay for character display
}		
void delay_ms(unsigned int ui_value)
{
   while (ui_value-- > 0) 
   {
      __delay_ms(1);   // must not over 39ms
   }   
}  