This is an accurate 6 digitals clock (HH.MM.SS) for the STM8S003.
Details: https://hw-by-design.blogspot.com/2019/11/stm8-led-clock-part-1.html

The RTC is implemented in firmware. It uses a 24-bit numerically-controlled 
oscillator (NCO) in formware for very fine accuracy adjustment.

Mine has been calibrated to within +/- 1 sec/month 
(room temperature is regulated by heaters.)

Hardware/ - Eagle CAD schematic, PCB
Firmware/ - STM8S003 firmware source code

Firmware written by me are licensed under GPL 3.0.
It can be compiled using the free STM8 Cosmic C compiler.
Additional incorporated code are under their own licenses.

Hardware is licensed under CC BY-4.0

https://creativecommons.org/licenses/by/4.0/

Initial release:
- bare minimal functionality, clock trim value is hard coded.
