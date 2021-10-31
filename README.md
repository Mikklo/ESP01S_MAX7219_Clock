# ES8P266 MAX7219 Matrix Clock

This project realizes a NTP controlled matrix clock (4 elements)

## Code

The Code is intended to be used in the the Arduino Code Environment. However, you have to install 
* the ESP822 Board Package
* MD_MAX72XX Library


### Configuration

There are some Basic configuration parts rigth at the beginning of the code:

* Blink: Constant blinking of the HH and MM separator sign ":" can be defined
* Leading 0: Define if a leading "0" will be shown if the hour is < 10. 
* WiFi parameters: SSID and Password of the local WiFi


### Matrix Display configuration

The PIn configuration for the matrix display is shown in the code and per default:

| PIN      | Function           | 
| ------------- |:-------------:| 
| D5     | Clock   | 
| D7      | Data   |   
| D8 | Chip Select |    


### NTP configuration

The NTP server and time zone is configured in the function "initNTP()" and per default set to "Berlin, Paris, Amsterdam" with automatic daylight saving switch. 

### Font

The used local font is a modified MD_MAX72XX library font. Changes are made to:

* "1": to use the same length as all other digits.
* Character 124: for representing one empty column



## Case

As casing I printed the code from [this](https://www.thingiverse.com/thing:4383145) Thingiverse Project.


## Parts

The required parts are listed in the Thingiverse project linked above. However, I figured out that there are some MAX7219 matrix displays on the market which have a slithy to big gap between the individual elements and thus do not fit into the grid layout of the case. For dimming the display intensity beyond the configureable value i placed an extra sheet sold by an big electronic market place and which is designed specifically to lower the display brighnes of DVD player or other devices. 


