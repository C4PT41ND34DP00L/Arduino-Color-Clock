# Arduino-Color-Clock

This uses a modified RTClib library by Adafruit forked from JeeLab to add the day of the year function so the Stardate is in the correct format (2118.195) (Adafruit https://github.com/adafruit/RTClib) (jeelabs https://jeelabs.org/pub/docs/rtclib/), and the UCGLib library by olikraus (https://github.com/olikraus/ucglib) for the Color OLED and the Battrey Snese library  by rlogiacco (https://github.com/rlogiacco/BatterySense) for the battery monitoring.
Clock contains battery percentage, Tempature, Time in 12 hour format and Stardate based off the new Star Trek movies (Century, Current Year of century, Current day of Year in decimal format) (http://mentalfloss.com/article/68741/how-do-star-trek-stardates-work).
This is a work in progress.

ToDo:
1. Add menu system
2. Add button control for menu
3. Add alarm saved to eprom
3. Add light sensor to change color when in lighted area

Wish List:
1. Add Bluetooth for setting alarm and time
2. Add set time function that saves to RTC
3. Add WiFi to set Alarm and Time
4. Create Android App for bluetooth settings

Extras Included:
1. PCB for table top clock
2. BOM for all parts with links.
