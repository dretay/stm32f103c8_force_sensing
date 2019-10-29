## A STM32F103C8-based 3D Printer Calibrator ##

### Overview ###
When using a delta 3d printer you often need to go through a laborous calibration routine to properly set the height of your printer nozel. This calibration is necessary because the delta arms may move slightly different amounts, resulting in some unevenness across the print surface. The normal approach to adjusting for this offset is to put a sheet of paper on the printer bed and then move the nozel down until it makes contact with the paper. While this works it can be hard to get precise calibrations because you are relying on a human to "feel" when the paper has made contact. 

This simple project helps create more consistent 3d printer calibrations by giving you a "probe" to take the place of the sheet of paper. The probe is actually a force sensing resistor which forms part of a resistor divider back to the uController's ADC. Thus the more pressure exherted on the sensor, the higher the voltage sensed on the uController. 

Thus using this approach allows a user to more quickly and consistently calibrate a printer. 

### Main Features ###
![](https://raw.githubusercontent.com/dretay/stm32f103c8_force_sensing/master/IMG_4449.jpg)
- 
