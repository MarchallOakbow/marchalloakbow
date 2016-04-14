/*********************************************************

												Smart Hair Resistance Read Program

																															made by Marchalloakbow 
																															ver.1				5th/Jan/2015

Target device: NUCLEO F401RE
Compiler:	 			 Keil ARMCC

	  This program is designed to read the change of the smart hair 
	actuator resistance. The main program has 3 state which consist 
	of 1. waitState, 2. measureState, and 3. transmitState. They are 
	controlled by the state machine named handleEvent. Each state 
	description is described below:
	
		waitState:					The wait state do nothing until the measure SW
														is pushed. Once the SW is pushed, the function
														set the next state as measureState to start 
														measuring actuator resistence  

		measureState:		The measure state do the measure routine with
														the AD converter and DA converter. This state 
														sets the next state as transmit state after the 
														measurement ends.

		transmitState:		The transmit state convert the data into .csv file.
														This state sets the next state as wait state after
														it finish converting the measured data.

		The hardware system is composed of Nucleo F401RE and I2C DA 
	converter, LT1167 opamp, and KSP13 transistor. The circuit and wiring
	is described in attached readResist.png. 

*********************************************************/

/*** Includes header files ***/
#include "mbed.h"
#include "MCP4726.h"

/*** Set ports and functions ***/
Timeout timer;
I2C i2c(D14,D15);
MCP4726 mcp4726(i2c);
//LocalFileSystem local("local");               // Create the local filesystem under the name "local"
//LocalFileSystem only works on the two original mbed boards (LPC1768 and LPC11u24).
 
DigitalOut my_led(LED2);
DigitalOut state1LED(D3); 
DigitalOut state2LED(D5); 
DigitalOut state3LED(D6); 
DigitalIn SW(D2);

AnalogIn Vsma(A0);
AnalogIn VshntR(A1);

/*** Declare state enumeration and define new type ***/
typedef enum State{ waitState, measureState, transmitState} State;
typedef enum Event{ noEvent, pushSW, measureEnd, transmitEnd} Event;

/*** Macros***/
#define HIGH 		1
#define LOW 		0
#define maxDataNum 600										//max data number 600 = 10sec measurement at 1/60 fps
#define dataVar 2 					 										//data variety is 2: Voltage and Current
#define Vout    45535								
//#define checkVout													//For checking Voltage of DA converter in the wait state


/*** Variable ***/
int on_delay 				= 0;
int off_delay 			= 0;
int i									= 0;
int Vshnt						= 0;
int Vsh							= 0;
unsigned short measureData[ maxDataNum ]  [dataVar ];        //unsigned short data array max 3600kB

/*** Function prototypes ***/
void toggleOn(void);
void toggleOff(void);
void measure(void);
Event receiveEvent(State s);
State handleEvent(State s, Event e);


/************************************
	               	Main Routine
************************************/
int main() {
	
	 /*** mcp4726 initialize ***/
  	i2c.frequency(100000);
	  mcp4726.put(0);
	
		/*** Initialize first state as waitState ***/
	  State s = waitState;
	
		/*** Initialize fist event as none ***/
		Event e = noEvent;
	
		/*** Run state machine ***/
		while(1) {
				e = receiveEvent(s);				//Check event
				s = handleEvent(s,e);				//Handle event to next state
		}
}


/***********************************
							Event Handler Function
************************************/
State handleEvent(State s, Event e)	{
				switch(s){
								/*** wait state routine***/
								case waitState:
													
													if(e == pushSW){
															s = measureState;
															wait(10); 																		//wait until vibration stops
															//state1LED = 0;
													}
													state1LED = !state1LED;
													
													#ifdef checkVout
																mcp4726.put(Vout);									//Check DA voltage amount
													
													#endif	
													break;
								
								/*** measure state routine ***/
								case measureState:
													measure();																						//Start measure

													s = transmitState;																//Set next state as transmit state

													state2LED = LOW;																//Turn off led
													break;
								
								/*** transmit state routine ***/
								case transmitState:

													/*** Open File to write CSV ***/
													FILE *fp;

													if ( NULL == (fp = fopen( "/local/measuredData.csv", "w" )) )
													error( "" );
								
													/*** Write CSV file  ***/
													fprintf(fp, "DataNum, Vol, Cur");

													for ( i = 0; i > maxDataNum; i++ ) {
																	fprintf( fp, "%d, %d, %d", i, measureData[i][0], measureData[i][1] );
																	wait( 0.1 );
													}

													/*** Close File ***/
													fclose( fp );
								
								
													s = waitState;
													break;
				}
				return s;
}

/**************************************
							Receive Event Function
**************************************/
Event receiveEvent(State s){
	
		switch(s){
				case waitState:
								
								if(SW == 0){								
									return pushSW;
								}else{
									return noEvent;
								}
								
								break;
								
				case measureState:
								
								
								break;
					
				case transmitState:
	
								
								break;
			 
			  default:
					return noEvent;

		}
}

void toggleOn(void) {
    state1LED = 1;
    timer.attach_us(toggleOff, on_delay);
}

void toggleOff(void) {
    state1LED = 0;
    timer.attach_us(toggleOn, off_delay);
}

void measure(void){
	
		/*** Turn on LED to show the measurement starts ***/
		state2LED = HIGH;

	/*** Set voltage on DA converter ***/
		mcp4726.put(Vout);
 
   	for(int cnt = 0; cnt < maxDataNum ; cnt++){ 
								
						/***  Read analog in value ***/
						Vsh		 = Vsma;																//Read Vsma to calc sma resistance
						Vshnt = VshntR;															//Read VshntR to sense current
												
						measureData[cnt][0] = Vsh;
						measureData[cnt][1] = Vshnt;

					wait(0.017);																		//Wait one frame rate

					/***	Toggle LED ***/
					state2LED = !state2LED;									//toggle LED2 to synchronize frame picture with array data
													
		}

}
