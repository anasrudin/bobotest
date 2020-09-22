// bagian A
// ------------------------------------------------
// pseudo code
// Init ports and variables
// Read Temp from the analog sensor
// InitReference Temp
// COMPARE REF WITH CURRENT
// if current Temp is greater than ref Temp, Switch ON Actuator
// If current Temp is less than ref Temp, Switch Off Actuator





//bagian B
// _______________________________

// Where devices are connected
// led bit 0 port a
// actuator bit 1 port a
// port A 0x1000

unsigned int* PortA = 0x1000;

// 12 bit ADC, range 0 - 3.3 V, connected to sensor with sensitivity 1 deg C / 100 mV
unsigned int* Adc = 0x2000;

// 32 bit timer counter register, unit in ticks, configured as 1 millisecond per tick, decrementing.
unsigned long int* TimerCount = 0x3000;


//hardware initialization to configure PortA, Adc, and TimerCount
// 16MHz clock
#define F_CPU 16000000UL 

//Calculate value for CTC match value in OCR1A
#define CTC_MATCH_OVERFLOW((F_CPU/1000)/8)

void init()
{
    //PortA init output
    PORTA = ((1<<PORTA3) | (1<<PORTA2) | (1<<PORTA1) | (1<<PORTA0));


    //ADC init
    ADCSRA=((1<<ADEN) | (1<<ADPS0); //Enable ADC, sampling)
    ADMUX=0x00;


    //Timer1 Init for 16 bit 
    //CTC mode/ clock /8
    TCCR1B |= (1 << WGM12) | (1 << CS11);
    //compare  high and low byte
    OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
    OCR1AL = CTC_MATCH_OVERFLOW;
    //Enable to compare match interrupt
    TIMSK1 |= (1 << OCIE1A);

}

uint16_t GetAdcValue(uint8_t v_adcChannel_u8)
{
    ADMUX = v_adcChannel_u8;
    DELAY_us(10);
    
    // conversion
    ADCSRA |= (1 << ADSC);

    while(ADCSRA & (1 <<ADSC));

        return(ADCW); //return 10 bit result

}


double readTemperature()
{
	//read temperature from ADC
	int adcVal;
	float mV;
	double temp;

	adcVal = GetAdcValue(0);	  //read from channel 0
	mV = (adcVal * 3300) / 1023; //10 bit resolution 3.3 ref

	temp = adcVal / 10; //lm35 sensor calibration return to degree celcius

	return temp;
}

volatile unsigned long timer1_millis;
long milliseconds_since;
unsigned long getTime()
{
	//get time
	unsigned long getTime_return;
	//cannot interrupt
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		getTime_return = timer1_millis;
	}
	return getTime_return;
}


void setActuator(bool on)
{
	//set Actuator on/off
if (on == true)
	{
		PORTA.1 = 1;
	}
	else
	{
		PORTA.1 = 0;
	}
}

void setHeart(bool on)
{
	//set heart beat on/off
	unsigned long currentTime = getTime();
	if (on == true)
	{
		PORTA.0 = 1;
		if(currentTime - milliseconds_since > 1) {
			milliseconds_since = currentTime;
		}
	}
	else
	{
		PORTA.0 = 0;
	}
}

void main(void)
{
	init();

	double refTemp = 20;
	double currentTemp = readTemperature();
	// loop endlessly
	while (true)
	{
		// logic to control temperature belong here
		if(currentTemp >= refTemp){
			setActuator(true);
			
		}
		else{
			setActuator(false);
			
		}

		setHeart(true);
		delay_ms(1000);
		setHeart(false);
	}
}
