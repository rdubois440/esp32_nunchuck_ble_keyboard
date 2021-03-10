//Todo
//
// - Find a name
// - Use the nunchuck library - clean up the code
//
// - Add Alt Tab - Review the logic of key press / key release - or use the3 Android buttons
//	 Notice that App Switch is just Alt Tab - Alt is mainained for some extra time between presses
// - 2 special Android buttons - Home - Back

// - Something strange occurs on first nunchuck. When xValue = 255 AND yValue = 255. Radius becomes negative. Root cause is identified (potentiometers), but why is radius negative ?
// - Implement an Off line - dictaphone mode - If not connected, then store all text entered, and dump later
//
// - Rewrite using freertos directly without the arduino ide - requires porting hid over gatt to freertos
// - Multi device support (like K380 F1 F2 F3)

// Done
//	Implemented the Application Key - Menu Key
// - Esc  Tab | / ~
// - direction keys
// - Power off on dedicated joystick movement
// - Something strange occurs on first nunchuck. When xValue = 255 AND yValue = 255. Radius becomes negative.
// 	  	Work around is to truncate xValue and yvalue at 254. But why does this happen ? Why does radius become negative ?
//      Root cause is the bad quality of the potentiometers in the joystick - Different nunchucks send 255 at different "edge positions"
// - Power off - after 5 minutes  
// - Leds - show connection status

/* 	Nunchuck Connector

                      	             +--------------------------------+
                                    |     Transparent Part on Top    |
                    				 +--------------------------------+
                                    |  SDA         DD          VCC   |
                                    |                                |
                                    |  GND         NC          SCL   |
                                    |          +----------+          |
                     	             +----------+          +----------+

	Olimex Breakout Board

				+--------------------------------------------------------+
                |  NC          NC          SDA          NC         GND   |
                |                                                        |
                |  NC          NC          SCL          NC         VCC   |
				+-----------------------+      +-------------------------+
                                        +------+

	Nunchuck on board connector

				Pink	Brown 	White	Yellow	Green


*/

//#include <prismino.h>
#include <Wire.h>
//#include <Keyboard.h>
//#include <Mouse.h>
#define BTSTACK_FILE__ "hid_keyboard_demo.c"
#include <BleKeyboard.h>
#include "esp32-hal-log.h"


//#define SDA_PIN	16
//#define SCL_PIN	4




// Done
// - add digits and punctuation 




#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

BleKeyboard bleKeyboard;

//uint8_t keyLookup[64];
long sleepTimerStart = 0;
long blinkTimerStart = 0;
int blinkerStatus = 0;

#define BLINK_ON 	50
#define BLINK_OFF 	950


const int ledPin = 22; // Lolin32 Lite - onboard LED


/*
   /opt/arduino-1.5.8/arduino --board arduino:avr:leonardo --port /dev/ttyACM3 --upload  nunchuck.ino
   /opt/arduino-1.8.13/arduino --board arduino:avr:leonardo --port /dev/ttyACM3 --upload  nunchuck.ino

 */

uint8_t outbuf[6];
int cnt = 0;
int joy_x_axis, joy_y_axis, dirY, dirX, speedLeft, speedRight;
//uint16_t accX, accY, accZ;
//uint8_t button_c, button_z;


uint8_t zone;
uint8_t previousZone;
uint8_t zoneCount;
uint8_t zoneHistory[5];

long composite;

bool cButton;  
bool zButton;  
bool mousePressed;

#include <Wire.h>
/*
void setup()
{
  Wire.begin(4,16);
  Serial.begin(115200);
  while (!Serial);
}

void loop()
{
  byte error, address;
  int I2CDevices;

  Serial.println("Scanning for I2C Devices…");

  I2CDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");

      I2CDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (I2CDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("****\n");
  }
  delay(5000);
}

*/


void setup()
{
	Serial.begin(115200);

	Serial.println("Inside setup()");
	//delay(1000);

	pinMode(ledPin, OUTPUT);
	Wire.begin(4,16);

	Serial.println("After Wire.begin()");

	nunchuckInit();
	Serial.println("After nunchuckInit()");

	//  play(440, 300);
	//  play(880, 300);
	//Keyboard.begin();
	//Mouse.begin();
	mousePressed = 0;

	zoneHistory[0] = 0;
	zoneHistory[1] = 0;
	zoneHistory[2] = 0;
	zoneHistory[3] = 0;
	zoneHistory[4] = 0;

	//delay(10000);

	//Serial.println("zone - cosAngle - radius - xCoord - yCoord");

	bleKeyboard.begin();
	bleKeyboard.deviceName = "esp32 - nunchuck";
	


	// Will now somehow blink - indication that BLE setup fails at boot time
	digitalWrite(ledPin, LOW);
	sleepTimerStart = millis();

	Serial.setDebugOutput(true);
	log_printf("Calling log_printf\n");
	esp_log_level_set("*", ESP_LOG_VERBOSE);
	ESP_LOGD("EXAMPLE", "This doesn't show");


	bleKeyboard.begin();

	Serial.println("Done with BLE setup");

	// Another slow blink - indication that BLE setup fails at boot time
	//delay( 100 ); 	
	digitalWrite(ledPin, HIGH);
	Serial.println("After 1 more second");

	sleepTimerStart = millis();

}

void nunchuckInit()
{
	Serial.println("Inside nunchuckInit()");
	// transmit to device 0x52
	Wire.beginTransmission(0x52);

	// sends memory address
	//  Wire.write(0x40);
	Wire.write(0xf0);

	// sends sent a zero
	Wire.write(0x55);

	//delay(1000);

	// sends memory address
	Wire.write(0xfb);

	// sends sent a zero
	Wire.write(0x00);

	// sends another  zero
	Wire.write(0x00);

	// stop transmitting
	Wire.endTransmission();
}

void sendZero()
{
	// transmit to device 0x52
	Wire.beginTransmission(0x52);
	// sends one byte
	Wire.write(0x00);
	// stop transmitting
	Wire.endTransmission();
}

/*void showZoneHistory()
{
	uint8_t index = 0;

	while (index < zoneCount)
	{
		// Serial.print("   ");
		//Serial.print(zoneHistory[index]);
		index++;
	}
	//Serial.println("");
}
*/

void loop()
{

	uint8_t data;
	uint8_t xValue, yValue;
	char msg[256];

	//int xMove;
	//int yMove;

	int xCoord, yCoord;

	long radius;
	long cosAngle;

	//long composite = 0;

	// request data from nunchuck
	Wire.requestFrom(0x52, 6);


	while(Wire.available())
	{
		// receive byte as an integer
		data = Wire.read();
		outbuf[cnt] = data;
		//digitalWrite(LED, HIGH);
		cnt++;
	}

	xValue = outbuf[0];
	yValue = outbuf[1];
	cButton = outbuf[5] & 0x2;
	zButton = outbuf[5] & 0x1;

	// Something strange happens when xValue AND yValue are at 255. let's truncate
	if (xValue == 255)
		xValue = 254;

	if (yValue == 255)
		yValue = 254;



	xCoord = xValue - 127;
	yCoord = yValue - 127;

	radius = xCoord * xCoord + yCoord * yCoord;
	cosAngle = ((long) xCoord * (long) xCoord * (long) 100) / radius;

	//if (radius < 404)
	if (radius < 1000)
	{
		zone = 0;
	}

	else if (cosAngle > 50)
	{
		if (xCoord > 0)
		{
			zone = 1;
		} 	
		else
		{
			zone = 3;
		}
	}

	else 
	{
		if (yCoord > 0)
		{
			zone = 2;
		} 	
		else
		{
			zone = 4;
		}
	}

	cnt = 0;
	// send the request for next bytes

/*
	if (!cButton) 			// Mouse action
	{

		xMove = 0;
		if (xCoord < -20) xMove = -1;
		if (xCoord > 20) xMove = 1;

		yMove = 0;
		if (yCoord < -20) yMove = 1;
		if (yCoord > 20) yMove = -1;

		//Serial.print(" cButton is pressed - ");
		  //Serial.print(xCoord);
		  //Serial.print(" - ");
		  //Serial.print(yCoord);
		  //Serial.print(" - ");
		  //Serial.print(xMove);
		  //Serial.print(" - ");
		  //Serial.print(yMove);
		  //Serial.print("\r");

		//if ((xMove != 0) || (yMove != 0))
		//{
			//Mouse.move(xMove, yMove);
		//}

		//if ((!zButton) && (!Mouse.isPressed()))
		//{
			//Mouse.press();
		//}
		//if (zButton)
		//{
			//Mouse.release();
//
		//}
	}
*/

	if ((zone != previousZone) && (zone == zoneHistory[zoneCount -2]) && (zoneCount > 1))		// Keyboard action - debounce
	{
		//Serial.println("Debouncing");
		//Serial.print("Back to previous zone after short move to some other zone - Just get back to this previous zone\n");
		//Serial.print("Previous Previous zone   ");
		//Serial.print(zoneHistory[zoneCount - 2]);
		//Serial.print(" ");
		zoneCount--;
		zoneHistory[zoneCount] = 0;
		zoneCount--;
		zoneHistory[zoneCount] = 0;
	}

	if (zone != previousZone)		// Keyboard action - Crossed a zone 
	{

		//Serial.print("New zone\n");
		//Serial.print("Previous zone   ");
		//Serial.print(previousZone);

		//Serial.print("New zone   ");
		//Serial.println(zone);

		composite =	zoneHistory[4]
				+ 16 * zoneHistory[3] 
				+ 256 * zoneHistory[2] 
				+ 4096 * zoneHistory[1] 
				+ 16L * 4096L  * zoneHistory[0] ;

		if (zone != 0)
		{

			zoneHistory[zoneCount++] = zone;
		}
		else
		{
			//Serial.println("Back to zone 0");

			
		

			//showZoneHistory();
			//sprintf( msg, "Before calling parseSequence zoneCount is %x composite is %lx ", zoneCount, (long) composite);	
			//Serial.println(msg);
			if (zoneCount > 0)
			{
				parseSequence(composite, zoneCount);
				Serial.println("");
				sprintf( msg, "xVal-%04d yVal-%04d zone-%d cosAngle-%04ld radius-%06ld xCoord-%04d yCoord-%04d composite-%06lx ", xValue, yValue, zone, cosAngle, radius, xCoord, yCoord, composite);	
				Serial.println(msg);
			}
			zoneCount = 0;
			zoneHistory[0] = 0;
			zoneHistory[1] = 0;
			zoneHistory[2] = 0;
			zoneHistory[3] = 0;
			zoneHistory[4] = 0;
			sleepTimerStart = millis();				// Restart the timeout timer
		}

		previousZone = zone;

	}

	sprintf( msg, "\rxVal-%04d yVal-%04d zone-%d cosAngle-%04ld radius-%06ld xCoord-%04d yCoord-%04d composite-%06lx ", xValue, yValue, zone, cosAngle, radius, xCoord, yCoord, composite);	
	Serial.print(msg);

	//Serial.print(xValue);
	//Serial.print("   ");
	//Serial.print(yValue);
	//Serial.print("   ");
	//Serial.print(zone);
	//Serial.print("   ");
	//Serial.print(cosAngle);
	//Serial.print("   ");
	//Serial.print(radius);
	//Serial.print("   ");
	//Serial.print(xCoord);
	//Serial.print("   ");
	//Serial.print(yCoord);
	//Serial.print("   ");
	//Serial.print(composite);
	//Serial.print("               ");
	//Serial.print(cButton);
	//Serial.print("   ");
	//Serial.print(zButton);
	//Serial.print("   ");
	//Serial.print("\r");
	//Serial.print("\r");


	sendZero();


	long timer = millis() - sleepTimerStart;
	printf("timer is now %ld\n", timer);
	if (timer > 300000L)
	{
		gotosleep();
		//digitalWrite(ledPin, HIGH);
		//printf("Going to sleep\n");
		//Serial.println(getCpuFrequencyMhz());
		//vTaskDelay(500 / portTICK_RATE_MS);
		//vTaskDelay(500 / portTICK_RATE_MS);
		////Enter sleep mode 
		//esp_deep_sleep_start();
	}
	long blinkTimer = millis() - blinkTimerStart;

	if ((blinkerStatus == 0) && (blinkTimer > BLINK_ON))
	{
		//printf("Blinker OFF\n");
		blinkerStatus = 1;
		digitalWrite(ledPin, blinkerStatus);
		blinkTimerStart = millis();
	}
	else if ((blinkerStatus == 1) && (blinkTimer > BLINK_OFF))
	{
		//printf("Blinker ON\n");
		blinkerStatus = 0;
		digitalWrite(ledPin, blinkerStatus);
		blinkTimerStart = millis();
	}

	delay(10);
}


void gotosleep()
	{
		digitalWrite(ledPin, LOW);
		printf("Going to sleep\n");
		Serial.println(getCpuFrequencyMhz());
		vTaskDelay(500 / portTICK_RATE_MS);
		vTaskDelay(500 / portTICK_RATE_MS);
		/* Enter sleep mode */
		esp_deep_sleep_start();
	}


void parseSequence(long composite, uint8_t zoneCount)
{
	char inKey;

	//Serial.println("Inside parseSequence");

	if (zoneCount > 5)

	{
		Serial.println("Not supported yet");
		return;
	}

	//sprintf( msg, "Inside parseSequence zoneCount is %x composite is %lx ", zoneCount, (long) composite);	
	//Serial.println(msg);


	if (zButton && cButton)					//Alphabet Mode - cButton and zButtons are active low
	{
		switch (composite)
		{
			// Cross 1
			case 0x10000:		inKey = ' ';	break;
			case 0x20000:		inKey = ' ';	break;
			case 0x30000:		inKey = '\b';	break;
			case 0x40000:		inKey = '\n';	break;


			case 0x43000:		inKey = 'e';	break;
			case 0x43200:		inKey = 'r';	break;
			case 0x43210:		inKey = 'g';	break;
			case 0x43214:		inKey = 'q';	break;

			case 0x32000:		inKey = 't';	break;
			case 0x32100:		inKey = 'd';	break;
			case 0x32140:		inKey = 'y';	break;
			case 0x32143:		inKey = 'z';	break;

			case 0x21000:		inKey = 'a';	break;
			case 0x21400:		inKey = 'l';	break;
			case 0x21430:		inKey = 'p';	break;

			case 0x14000:		inKey = 'o';	break;
			case 0x14300:		inKey = 'c';	break;
			case 0x14320:		inKey = 'b';	break;

			case 0x41000:		inKey = 'i';	break;
			case 0x41200:		inKey = 'u';	break;
			case 0x41230:		inKey = 'v';	break;

			case 0x12000:		inKey = 'n';	break;
			case 0x12300:		inKey = 'm';	break;
			case 0x12340:		inKey = 'k';	break;

			case 0x23000:		inKey = 's';	break;
			case 0x23400:		inKey = 'w';	break;
			case 0x23410:		inKey = 'j';	break;

			case 0x34000:		inKey = 'h';	break;
			case 0x34100:		inKey = 'f';	break;
			case 0x34120:		inKey = 'x';	break;

			default:								break;
		}
	}

	else if (!zButton && cButton)					// Numeric and Punctuation mode - cButton pressed
	{
		switch (composite)
		{
			// Cross 1
			case 0x10000:		inKey = KEY_RIGHT_ARROW;	break;
			case 0x20000:		inKey = KEY_UP_ARROW;	break;
			case 0x30000:		inKey = KEY_LEFT_ARROW;	break;
			case 0x40000:		inKey = KEY_DOWN_ARROW;	break;

			case 0x43000:		inKey = '0';	break;
			case 0x43200:		inKey = '8';	break;
			case 0x43214:		gotosleep();		break;

			case 0x32000:		inKey = '1';	break;
			case 0x32100:		inKey = '9';	break;

			case 0x21000:		inKey = '2';	break;
			case 0x21400:		inKey = '-';	break;

			case 0x14000:		inKey = '3';	break;
			case 0x14300:		inKey = ',';	break;

			case 0x41000:		inKey = '4';	break;
			case 0x41200:		inKey = '.';	break;
			case 0x41234:		inKey = 0xed;	break;			// This is the Windows Application Key - Menu key - equivalent to mouse right click

			case 0x12000:		inKey = '5';	break;
			case 0x12300:		inKey = '/';	break;

			case 0x23000:		inKey = '6';	break;
			case 0x23400:		inKey = '|';	break;

			case 0x34000:		inKey = '7';	break;
			case 0x34100:		inKey = '~';	break;

			default:								break;
		}
	}

	/*else if (zButton && !cButton)					// Numeric and Punctuation mode - zButton pressed
	{
		switch (composite)
		{
			// Cross 1
			case 0x10000:		inKey = KEY_RIGHT_ARROW;	break;
			case 0x20000:		inKey = KEY_UP_ARROW;	break;
			case 0x30000:		inKey = KEY_LEFT_ARROW;	break;
			case 0x40000:		inKey = KEY_DOWN_ARROW;	break;


			case 0x43000:		inKey = '0';	break;
			case 0x43200:		inKey = '8';	break;
			case 0x43214:		gotosleep();		break;

			case 0x32000:		inKey = '1';	break;
			case 0x32100:		inKey = '9';	break;

			case 0x21000:		inKey = '2';	break;
			case 0x21400:		inKey = '-';	break;


			case 0x14000:		inKey = '3';	break;
			case 0x14300:		inKey = ',';	break;


			case 0x41000:		inKey = '4';	break;
			case 0x41200:		inKey = '.';	break;

			case 0x12000:		inKey = '5';	break;

			case 0x23000:		inKey = '6';	break;


			case 0x34000:		inKey = '7';	break;


			default:								break;


		}
	}*/

	else if (!zButton && !cButton)					// Arrow keys, power off 
	{
		switch (composite)
		{
			// Cross 1
			case 0x10000:		inKey = KEY_TAB;	break;
			case 0x20000:		inKey = KEY_TAB;	break;
			case 0x30000:		inKey = KEY_ESC;	break;
			case 0x40000:		inKey = KEY_TAB;	break;


			default:								break;


		}
	}

	bleKeyboard.press(inKey);
	bleKeyboard.release(inKey);
	//Keyboard.write(inKey);	
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////







/*

void setup()
{
		Serial.begin(115200);
	//	Serial1.begin(9600);

		pinMode(row1, OUTPUT);
		pinMode(row2, OUTPUT);
		pinMode(row3, OUTPUT);
		pinMode(row4, OUTPUT);
		pinMode(row5, OUTPUT);
		pinMode(row6, OUTPUT);
		pinMode(row7, OUTPUT);
		pinMode(row8, OUTPUT);

		pinMode(colA, INPUT_PULLUP);
		pinMode(colB, INPUT_PULLUP);
		pinMode(colC, INPUT_PULLUP);
		pinMode(colD, INPUT_PULLUP);
		pinMode(colE, INPUT_PULLUP);
		pinMode(colF, INPUT_PULLUP);
		pinMode(colG, INPUT_PULLUP);
		pinMode(colH, INPUT_PULLUP);
		pinMode(colI, INPUT_PULLUP);

		pinMode(ledPin, OUTPUT);

		bleKeyboard.begin();
		bleKeyboard.deviceName = "esp32 - g66";

		keyValue = 0;
		previousKey = 0;

		for(int i=0; i<256; i++)
		{
				isKeyPressed[i] = 0;
				wasKeyPressed[i] = 0;
		}

	//mode = MODE_USB;


	// Will now somehow blink - indication that BLE setup fails at boot time
	digitalWrite(ledPin, LOW);
	sleepTimerStart = millis();

	Serial.setDebugOutput(true);
	log_printf("Calling log_printf\n");
	esp_log_level_set("*", ESP_LOG_VERBOSE);
	ESP_LOGD("EXAMPLE", "This doesn't show");


	bleKeyboard.begin();

	Serial.println("Done with BLE setup");

	// Another slow blink - indication that BLE setup fails at boot time
	//delay( 100 ); 	
	digitalWrite(ledPin, HIGH);
	Serial.println("After 1 more second");

	initLookup();
	sleepTimerStart = millis();

}
*/



/*
void loop()
{

		int keyCount = 0;
		int index = 0;

		previousKey = keyValue;
		isLWinDown = 0;

		//Serial.println("Looping");

		for(int i=0; i<256; i++)
		{
				wasKeyPressed[i] = isKeyPressed[i];
				isKeyPressed[i] = 0;
		}
		//   isKeyPressed = 0;
		keyValue = 0;


		if(noKey == 16)
		{ // no keys were pressed
				readKey = true; // keyboard is ready to accept a new keypress
		}
		noKey = 0;




	for(int rIndex=0; rIndex < (sizeof(arRows) / sizeof(int)); rIndex++)
	{
		pullDownRow(arRows[rIndex]); // switch on one row at a time
		//Serial.print("Looping on rows: ");
		//Serial.println(arRows[rIndex]);

		for(int colIndex = 0; colIndex < (sizeof(arCols)/sizeof(int)); colIndex++)
		{
			//delay(100); 
			//Serial.print("Looping on columns: ");
			//Serial.println(arCols[colIndex]);

			if (digitalRead(arCols[colIndex]) == 0)
			{
				Serial.print("Pressed row ");
				Serial.print(arRows[rIndex]);
				Serial.print(" Col " );
				Serial.println(arCols[colIndex]);

				index = rIndex * 10 + colIndex;
				isKeyPressed[index] = 1;
				Serial.print("index is  " );
				Serial.println(index);

				sleepTimerStart = millis();

			}
		}
	}

	delay(debounceDelay);

	if ((isKeyPressed[50] == 1) && (isKeyPressed[60] == 1) && (isKeyPressed[75] == 1))			// Alt Ctrl BackSpace
	{
		gotosleep();
	}

	for(int i=0; i<256; i++)
	{
		if ((isKeyPressed[i] == 1) && (wasKeyPressed[i] == 0))
		{

			// Esc, Back, Space and Enter, send immediately
			if (keyLookup[i] != 0)
			{
				Serial.print("Pressed ");
				Serial.println(i);
				bleKeyboard.press(keyLookup[i]);
			}

		}
		if ((isKeyPressed[i] == 0) && (wasKeyPressed[i] == 1))
		{
			Serial.println("Released ");
			Serial.println(i);
			//bleKeyboard.release(keyLookup[i]);
			//bleKeyboard.releaseAll();
			bleKeyboard.release(keyLookup[i]);
		}
	}



	long timer = millis() - sleepTimerStart;
	printf("timer is now %ld\n", timer);
	if (timer > 300000L)
	{
		gotosleep();
		//digitalWrite(ledPin, HIGH);
		//printf("Going to sleep\n");
		//Serial.println(getCpuFrequencyMhz());
		//vTaskDelay(500 / portTICK_RATE_MS);
		//vTaskDelay(500 / portTICK_RATE_MS);
		////Enter sleep mode 
		//esp_deep_sleep_start();
	}

	long blinkTimer = millis() - blinkTimerStart;

	if ((blinkerStatus == 0) && (blinkTimer > BLINK_ON))
	{
		//printf("Blinker OFF\n");
		blinkerStatus = 1;
		digitalWrite(ledPin, blinkerStatus);
		blinkTimerStart = millis();
	}
	else if ((blinkerStatus == 1) && (blinkTimer > BLINK_OFF))
	{
		//printf("Blinker ON\n");
		blinkerStatus = 0;
		digitalWrite(ledPin, blinkerStatus);
		blinkTimerStart = millis();
	}




}
*/

