# NunKbd - A Nunchuck Based Bluetooth Keyboard for Android Phones 


# Objectives

Provide a single hand, ligthweight, ultra portable and fun bluetooth keyboard for phones and tablets, using a Wii Nunchuck

![Nunchuck as a Weapon or as a Keyboard](images/IMG_20210307_122031.jpg) 


# Benefits

 * Lightweight - Ultra portable - fun - reaasonably efficient
 * Single hand operation
 * Can even be used "inside the pocket", totally unnoticed.
 * Cross platform support. Natural target is Android phones, but works just as well with tablets, Windows and Linux PCs, Chromebooks, Mac, and even iPhones
 * Natively supported on all these platforms, nothing to install. NunKbd behaves as a standard Bluetooth Keyboard
 * Does not consume any spcace on the display. Compared to soft on-screen keyboards, screen space is completely free
 * An octogon limits the movement of the joystick. This octogon shape also provides natural feedback about the movements
 * Keyboard can easily be curstomised for a given subset of keys for a professional usage (data collection, control)
 * Could be useful to handicapped people. Alphabetic characters are accessible with a single finger. 
 * With some experience, it can be used when walking. For taking quick notes, if you accept a reasonable amount of typing errors, it can be used without watching any screen. 

# Original Keys Arrangement - 8Pen Project   

The NunKbd project was initially inspired by the 8Pen project. 
See the links to the 8Pen project at the end of this article, and a description of the concept below (Copied from http://www.8pen.com/concept):

 * To enter a letter, start by placing the finger/pointer in the central region
 * Without lifting, move out in the sector that contains the letter
 * Turn clockwise or anti-clockwise, according to the side on which the letter lies
 * Cross 1 to 4 branches, according to the order of the letter on the boundary, and return to the center to place it

![8Pen Layout](images/8pen_color.jpeg)  


# Entering Characters with NunKbd

See the zones identified as zone0 to zone4 below, and the branches separating the zones (the diagonal lines).    
At rest, the nunchuck is in zone0. If moved to the direction of west, this is zone1. North is zone2 etc.    
The code tracks the position of the nunchuck. and records it in a sequence.    
A sequence starts from zone 0, and ends when back to zone 0.    
When recording the sequence, the starting and ending zones, which are both 'zone0' are omitted.    
The longest sequence required to encode a complete keyboard crosses 4 branches in a complete turn. The longest sequence to record is therefore 5 zones long, starting and ending in 
the same zone. 
   
Long sequence example: zone0 - zone4 - zone3 - zone2 - zone1 - zone4 - zone0 :	letter 'q'   

## Entering The Letter 'e'

In the example below, the zone history is zone0 - zone4 - zone3 - zone0. and is encoded as 

![Entering the Letter 'e'](images/letter_e.png)

```
			case 0x43000:		inKey = 'e';	break;
```

## Entering The Letter 'k'

In this other example, the sequence is zone0 - zone1 - zone2 - zone3 - zone4 - zone 0, and is encoded as 

![Entering the Letter 'k'](images/letter_k.png)

```
			case 0x12340:		inKey = 'k';	break;
```



# Full Keyboard Arrangement


The original letter arrangement of the 8Pen project is not used for NunKbd, the letters are re-organized according to their frequency in the English language.
To the best of my knowledge, this is the alphabet organized by frequency

```
E T A O I N S H R D L C U M W F G Y P B V K J X Q Z
```

## cButton and zButton

There are two buttons on the nunchuck. 
The big square button is the Z button, the small round button is the C button.

 * None of these buttons is pressed when entering alphabetic characters. 
 * Pressing Z button only (large square button) enters numeric characters and punctuation.
 * Pressing both Z and C buttons enters special characters.
 * Pressing C button only is not used at the moment. This mode could be used to enter more characters, or move the mouse with the joystick, similar to joysticks on laptops



## Alphabetic Characters Entry

This is the chosen layout 

![Full Keyboard Layout](images/keyboard_layout.png)


Most frequent keys (Enter, Backspace, Space) are entered with a single return from zone0 to one of zone4, zone3 or zone1 and back.
Sequence examples


 * A quick trip from zone0 to zone1 and back is a space
 * A quick trip from zone0 to zone2 and back is a one-time press of the left shift key.Produces one Upper case character
 * A quick trip from zone0 to zone3 and back is a backspace
 * A quick trip from zone0 to zone4 and back is an Enter



 * Starting from zone4, turning clockwise, crossing 1 branch is letter  'e'
 * Starting from zone4, turning clockwise, crossing 2 branches is letter  'r'
 * Starting from zone4, turning clockwise, crossing 3 branches is letter  'g'
 * Starting from zone4, turning clockwise, crossing 4 branches is letter  'q'

 * Starting from zone4, turning counter clockwise, crossing 1 branch is letter  'i'
 * Starting from zone4, turning counter clockwise, crossing 2 branches is letter  'u'
 * Starting from zone4, turning clockwise, crossing 3 branches is letter  'v'


## Numeric and Puncutations - zButton pressed

This is the chosen layout 

![Full Keyboard Layout - Digits](images/keyboard_layout_digits.png)

### Direction keys

 * A quick trip from zone0 to zone1 and back is a RIGHT Arrow
 * A quick trip from zone0 to zone2 and back is an UP Arrow 
 * A quick trip from zone0 to zone3 and back is a LEFT Arrow
 * A quick trip from zone0 to zone4 and back is a DOWN Arrow


### Puntuation and Special Characters:

 * Starting from zone2, turning clockwise, crossing 2 branches prints a hyphen '-'
 * Starting from zone1, turning clockwise, crossing 2 branches prints a comma ','
 
 * Starting from zone4, turning counter clockwise, crossing 2 branches prints a single dot '.'
 * Starting from zone1, turning counter clockwise, crossing 2 branches prints a slash '/'
 * Starting from zone2, turning counter clockwise, crossing 2 branches prints a vertical bar '|'
 * Starting from zone3, turning counter clockwise, crossing 2 branches prints a tilde '~'

 * Starting from zone4, a full turn clockwise is a power off mode (in fact deep sleep)
 * Pressing the Z button, a full turn, counter clockwise, starting and ending in zone4. is the Menu key.





## Special Actions - Z and C Buttons Pressed 

 * A quick trip from zone0 to zone1 and back is a TAB
 * A quick trip from zone0 to zone2 and back, is the Application Switch key.
 * A quick trip from zone0 to zone3 and back is an ESCAPE
 * A quick trip from zone0 to zone4 and back is special key combination Alt + Ctrl + k. This is used in termux to show / hide the soft keyboard

## Upper Case Characters

As mentionned above, a quick trip from zone0 to zone2 and back is a one-time press of the left shift key. This will produce one Upper case character with the next key.

No action is produced immediately in this case, but a variable is set to 1, and will be reset AFTER the next key is produced. This gives access to the full keyboard 
in Shift (or Caps Lock) mode, but for one character only. Ex: on Querty keyboards, this gives access to ! @ # $ etc.  
Of course this can be repeated before the next character is desired.


## Android Special Keys

Bluetooth keyboards designed for Android devices provide 4 extra buttons, Home, Application Switch, Menu and Back. One of the problems with these buttons is that 
their names are not fully standardized, sometimes used ambiguously


### Application Switch key

Internally, this button is implemented as 2 buttons: Alt and Tab plus some extra logic.

A closer look at the behaviour of this key with evtest shows some extra logic (connect one bluetooth keyboard to a linux PC)

```
Event: time 1615023308.816492, type 4 (EV_MSC), code 4 (MSC_SCAN), value 700e2
Event: time 1615023308.816492, type 1 (EV_KEY), code 56 (KEY_LEFTALT), value 1
Event: time 1615023308.816492, type 4 (EV_MSC), code 4 (MSC_SCAN), value 7002b
Event: time 1615023308.816492, type 1 (EV_KEY), code 15 (KEY_TAB), value 1
Event: time 1615023308.816492, -------------- SYN_REPORT ------------
Event: time 1615023308.828973, type 4 (EV_MSC), code 4 (MSC_SCAN), value 7002b
Event: time 1615023308.828973, type 1 (EV_KEY), code 15 (KEY_TAB), value 0
Event: time 1615023308.828973, -------------- SYN_REPORT ------------
Event: time 1615023309.815427, type 4 (EV_MSC), code 4 (MSC_SCAN), value 700e2
Event: time 1615023309.815427, type 1 (EV_KEY), code 56 (KEY_LEFTALT), value 0
Event: time 1615023309.815427, -------------- SYN_REPORT ------------
```

When pressing this Application Switch key (F5 on my keyboard), 2 virtual keys are pressed, LEFTALT and TAB.   
When releasing the key, only TAB is released. LEFTALT remains pressed for another 500 milliseconds. then released automaticlally.    
The result of this logic is that pressing F5 repetitively within less than 500 milliseconds will cycle through all open windows. Pressing F5 after more than 500 milliseconds will 
only toggle the last 2 open applications.   
In addition, after 500 milliseconds, the selected application will open in full screen when LEFTALT is released

Nunchuck implements this a bit differently.

A timer would be inefficient, NunKbd cannot be as fast as a regular keyboard. A One-key Alt-Tab is available with both C and Z buttons pressed, and a quick trip from zone0 to zone2 and back. 
This triggers both KEY_LEFT_ALT and KEY_TAB, but only KEY_TAB is released.   
 * If the next key is again a One-key Alt-Tab, then KEY_TAB is pressed again, cycling through all open windows
 * If the next key is a different key, LEFT_ALT is released, and the next key sent normally.


### Menu Key

The menu key is present on most keyboards, at the right of the Space bar. It usually looks like an open menu, with multiple lines. Do not confuse with the right Alt or right Windows key.
It opens a context related menu on certain applications, equivalent to pressing the 3 dots menu on the top right of the display.

The Menu key is available with Z buttons pressed, and a full turn, counter clockwise, starting and ending in zone4. 

### Home and Back Keys

Home and Back keys are of very little use on a single hand keyboard, and are not implemented at the moment.



# Learning Required    

Learning is not trivial, but reasonable, 2 weeks of 20 minutes daily practice should get you started.   

At the time of the original proect, I recommended purchasing 8Pen for android for 99 cents for learning. 

I personally use command line gtypist for practice. See nunchuck.typ for a ready made set of lessons customized for NunKbd. Start the attached lesson on any linux prompt with:
```
gtypist nunchuck.typ
```

# Limitations    
Multiple keys is not possible. Example Control – C.   
BUT it is possible to create a special key combinations. Alt Tab is implemented as a single key, as it is very useful for switching between open applications.   
This is also how the Application Swith button is implemented on commercial Bluetooth Keyboards (Ex Logitech K380)

# Future Extensions

 * Support multiple devices   

# Wemos Lolin32 Lite

This is the perfect microcontroller for this project.   


 * Small size
 * powerful
 * Suppported by Arduino IDE
 * Contains the hardware for bluetooth and BLE and the support library for Keyboard / HID 
 * Connector for a LiPo battery, charging and discharging circuitry

# Wiring   

Wires colours will vary across manufacturers, signal position on the connector will not vary. It is necessary to identify the wires by their position, 
and then refer to their colors.


## Front view of the nunchuck connector


In my case, the colour wires were as follows    

Signal   | Colour  | Connected to       | Signal Name       
---------|---------|--------------------|---------------       
SDA      | Yellow  | Arduino SDA pin 4  | Data           
DD       | Brown   | Not Connected      | Device Detect               
Vcc      | Pink    | Arduino +3.3v      | VCC            
Gnd      | Green   | Arduino GND        | GND            
NC       |         | Not Connected      | Not Connected               
SCL      | White   | Arduino SCL pin 16 | Clock       
       
![Nunchuck Connector](images/20171011-160313-Rene.jpg)

After carefully identifying the colour of each signal, cut the cable about 5cm from the connector, and connect the wires to the arduino according to the table above.    

Program is configured to expect SDA on pin 4, and SCL on pin 16
```
  Wire.begin(4,16);
```



## Olimex Adapter

For quick prototyping, without destroying the original nunchuck by cutting the cable, Olimex makes this nice breakout board

www.olimex.com
MOD-WII-UEXT
MOD-Wii-UEXT-NUNCHUCk



![Olimex breakout board](images/IMG_20210308_093404.jpg)
![Olimex breakout board - pinout](images/2021-03-08-104958_1366x768_scrot.png)



# Assembly


![Assembly ](images/IMG_20210308_102711.jpg)

The housing of a mini usb hub was a perfect candidate for this.    
The 500maH battery is taped under the board 
Notice the foam used to keep everything in place, and the small lever, making it easier to press the reset button through one of the existing holes

## Tip for Makers

The nunchuck enclosure is pretty empty. It just contains a small board, horizontally mounted below the joystick and behind the 2 buttons. 
The handle part is empty, and the Lolin32 could nearly fit in this space. It must be possible to create a 3D printed handle (just the bottom part), slightly bigger
than the origial one, and have the microcontroller and battery inside this handle.

# Arduino IDI vs ESP-IDF FreeRTOS 

There are 2 main development environments for the Lolin32, the native ESP-IDF FreeRTOS and the more friendly Arduino IDE.

I would normally prefer using the ESP-IDF FreeRTOS, which provides a better control of the environment, but unfortunately this environment only provided 
HID library for the Bluetooth mode 3, In bluetooth 3 mode, the reconnection to the phone was not as smooth at in bluetooth 4 mode, as provided by the Arduino IDE environment.


# Arduino IDE

Install support for board "Wemos Lolin32 Lite".    

The tool menu should look like this when connected to the esp32 lolin32 lite board:

```
IDE Setting for Wemos Lolin32 Lite - Dec 2020     
Board           Wemos Lolin 32
Upload speed    115200
CPU Freq        240Mhz (Wifi / BT)
Flash Freq      80Mhz            
Part Scheme     Default         
Port            /dev/ttyUSB0   
```
                                                                                                                                       
                      

# Arduino Code Structure

Use the I2C Wire Library for communication with nunchuck.    
Use the BleKeyboard ESP32 Library for the implementation of Hid over Gatt 



## Setup function
Initialize Wire library for i2c connections, as well as BleKeyboard 
BleKeyboard.init()
Mouse.init()

## Loop Function

Read xValue an yValue coordinates (0 - 255)    
Convert to center based coordinates (-128 - +127)    

Convert to polar coordinates (Radius, angle)    
In fact, the program does not calculate the angle itself, but only the cosinus of the angle.    
Similarly, it does not calculate the radius, but radius * radius    

Radius is used to determine if within the central area (radius * radius < 1000)   
Cosine of Angle Plus X and Y polararity determines the zone    

Identify the visited zones:    

 * Remains in central area: Do nothing    
 * Outside of central area: Keep record of quadrants visited    
 * Back in central area: Record the zones visited into and format into a sequence, then call the parseSequence() function    

## ParseSequence Function

 * Compares the sequence with one of the expected sequences    
 * Uses a different set of predefined sequences for combinations of cButton and zButton.    
 * Send the corresponding character if the sequences match. Do nothing if there is no predefined matching sequence


## Debouncing   
There is no debouncing between zone0 and the other zones, the peripheral zones. This does not seem to cause any problem.    

Between the peripheral zones, the debouncing works as follows:    
When moving from zone[n] to zone[n+1], if zone[n+1] is the same as zone[n-1], then decrement the zone counter. In other words, forget about visiting zone[n], the current zone.   
Where n is the current zone, n-1 is the previous zone, and n+1 is the potential future zone



## Blinking Led

One timer is used to blink the built-in Blue Led when active, and provides feedback, one quick 50 millisecond blink every second.    
When using the "Power off" command, the Led is turned ON for one second before the microcontroller is set to deep sleep. This provides a nice confirmation.    
Timers are not real hardware implemented timers. Timers are just variables which record the starting time of certain events. In the loop function, called every 10 milliseconds, 
the time passed since this event is calculated using the current time. 


## Power Modes

There is no power switch, the esp32 board is always powered.
The microcontroller is put to deep sleep mode in either of 2 conditions:

 * The joystick remainded untouched for the last 5 minutes
 * The special "Power Off" sequence is manually entered.   

The reset switch takes the microcontroller out of deep sleep when pressed, the keyboard reconnects in 5 seconds

# Note on the Quality of the Nunchucks

I own 3 nunchucks, from different origins.
One of them works very smoothly, the other 2 have sudden jumps in the reported xValue or yValue.    
It looks like the cursor of the potentiometer inside the nunchuck suddenly disconnects, and the potentiometer reports either Gnd or Vcc. xValue or yValue reports 255 in these areas.   
I am curious to know how these devices would behave when connected to a Wii box. At the moment, I just selected the smoothest nunchuck to use.    
A possible work around could be to
remember the previous value of either xValue or yValue, and use that previous value when reading 255.

# Termux on Android

Why do I need a keyboard so much on my phone?   
I am a big fan of using termux on Android. Termux provides a pretty complete linux environment on any recent Android device, without special requirement. No root required.   
Many standard linux packages can be installed (Ex: pkg install gtypist taskwarrior ...).    
X11 Gui programs can be installed as well, XServer XSDL provides an Android native X11 server that termux programs can use.   
Whenever possible, I use command line programs under termux. At a desk, I use a normal bluetooth keyboard, on the move, I take my nunchuck.   


# Links

1. http://www.8pen.com/concept   
2. https://lifehacker.com/8pen-is-a-speedy-gesture-based-keyboard-for-android-pho-5678488
3. Very good article. Contains a detailled description of the wiring.   
https://create.arduino.cc/projecthub/infusion/using-a-wii-nunchuk-with-arduino-597254
4. Same article, nicer to read 
https://www.xarg.org/2016/12/using-a-wii-nunchuk-with-arduino/
5. The termux project    
https://wiki.termux.com/wiki/Main_Page
6. The gtypist manual
https://www.gnu.org/software/gtypist/doc/gtypist.html



# Document Generation
```
pandoc -s -o Nunchuck_Ble_Keyboard.pdf  README.md
pandoc -s -o Nunchuck_Ble_Keyboard.pdf  -V geometry:"top=2cm, bottom=2cm, left=2cm, right=2cm" README.md
```
