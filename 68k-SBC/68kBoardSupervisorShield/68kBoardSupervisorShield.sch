EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 1850 5150 0    50   Output ~ 0
DTACK
Text GLabel 1850 5750 0    50   Output ~ 0
BERR
Text GLabel 2850 5950 2    50   Input ~ 0
FC2
Text GLabel 2850 5850 2    50   Input ~ 0
FC1
Text GLabel 2850 5750 2    50   Input ~ 0
FC0
Text GLabel 2850 5550 2    50   Input ~ 0
RD
Text GLabel 2850 5650 2    50   Input ~ 0
WR
Text GLabel 3050 5450 2    50   Input ~ 0
R~W
Text GLabel 2850 5350 2    50   Input ~ 0
LDS
Text GLabel 2850 5250 2    50   Input ~ 0
UDS
Text GLabel 2850 5150 2    50   Input ~ 0
AS
$Comp
L power:GND #PWR0101
U 1 1 5E69B354
P 2000 7000
F 0 "#PWR0101" H 2000 6750 50  0001 C CNN
F 1 "GND" H 2005 6827 50  0000 C CNN
F 2 "" H 2000 7000 50  0001 C CNN
F 3 "" H 2000 7000 50  0001 C CNN
	1    2000 7000
	1    0    0    -1  
$EndComp
Text GLabel 2850 6050 2    50   Output ~ 0
IPL0
Text GLabel 2850 6150 2    50   Output ~ 0
IPL1
Text GLabel 2850 6250 2    50   Output ~ 0
IPL2
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 5E69B363
P 2300 2950
F 0 "J1" H 2350 4067 50  0000 C CNN
F 1 "ADBUS" H 2350 3976 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2300 2950 50  0001 C CNN
F 3 "~" H 2300 2950 50  0001 C CNN
	1    2300 2950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J2
U 1 1 5E69B369
P 2300 5950
F 0 "J2" H 2350 7067 50  0000 C CNN
F 1 "SIGBUS" H 2350 6976 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2300 5950 50  0001 C CNN
F 3 "~" H 2300 5950 50  0001 C CNN
	1    2300 5950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0103
U 1 1 5E69B36F
P 3550 4650
F 0 "#PWR0103" H 3550 4500 50  0001 C CNN
F 1 "+5V" H 3565 4823 50  0000 C CNN
F 2 "" H 3550 4650 50  0001 C CNN
F 3 "" H 3550 4650 50  0001 C CNN
	1    3550 4650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5E69B375
P 2700 7000
F 0 "#PWR0104" H 2700 6750 50  0001 C CNN
F 1 "GND" H 2705 6827 50  0000 C CNN
F 2 "" H 2700 7000 50  0001 C CNN
F 3 "" H 2700 7000 50  0001 C CNN
	1    2700 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 6950 2000 6950
Wire Wire Line
	2000 6950 2000 7000
Wire Wire Line
	2600 6950 2700 6950
Wire Wire Line
	2700 6950 2700 7000
Entry Wire Line
	2850 2250 2950 2150
Entry Wire Line
	2850 2150 2950 2050
Entry Wire Line
	2850 2050 2950 1950
Wire Wire Line
	2850 2050 2600 2050
Wire Wire Line
	2600 2150 2850 2150
Wire Wire Line
	2850 2250 2600 2250
Entry Wire Line
	1550 2950 1450 2850
Entry Wire Line
	1550 2850 1450 2750
Wire Wire Line
	2100 2850 1550 2850
Wire Wire Line
	2100 2950 1550 2950
Entry Wire Line
	3150 3050 3250 2950
Entry Wire Line
	3150 2950 3250 2850
Entry Wire Line
	3150 2850 3250 2750
Wire Wire Line
	3150 2850 2600 2850
Wire Wire Line
	2600 2950 3150 2950
Wire Wire Line
	3150 3050 2600 3050
NoConn ~ 2100 3950
Entry Wire Line
	2850 2550 2950 2450
Entry Wire Line
	2850 2450 2950 2350
Entry Wire Line
	2850 2350 2950 2250
Wire Wire Line
	2850 2350 2600 2350
Wire Wire Line
	2600 2450 2850 2450
Wire Wire Line
	2850 2550 2600 2550
Entry Wire Line
	2850 2750 2950 2650
Entry Wire Line
	2850 2650 2950 2550
Wire Wire Line
	2600 2650 2850 2650
Wire Wire Line
	2850 2750 2600 2750
Entry Wire Line
	1550 3150 1450 3050
Entry Wire Line
	1550 3050 1450 2950
Wire Wire Line
	2100 3050 1550 3050
Wire Wire Line
	2100 3150 1550 3150
Entry Wire Line
	1550 3350 1450 3250
Entry Wire Line
	1550 3250 1450 3150
Wire Wire Line
	2100 3250 1550 3250
Wire Wire Line
	2100 3350 1550 3350
Entry Wire Line
	1550 3550 1450 3450
Entry Wire Line
	1550 3450 1450 3350
Wire Wire Line
	2100 3450 1550 3450
Wire Wire Line
	2100 3550 1550 3550
Entry Wire Line
	1550 3650 1450 3550
Wire Wire Line
	2100 3650 1550 3650
Entry Wire Line
	1550 3850 1450 3750
Entry Wire Line
	1550 3750 1450 3650
Wire Wire Line
	2100 3750 1550 3750
Wire Wire Line
	2100 3850 1550 3850
Entry Wire Line
	3150 3350 3250 3250
Entry Wire Line
	3150 3250 3250 3150
Entry Wire Line
	3150 3150 3250 3050
Wire Wire Line
	3150 3150 2600 3150
Wire Wire Line
	2600 3250 3150 3250
Wire Wire Line
	3150 3350 2600 3350
Entry Wire Line
	3150 3650 3250 3550
Entry Wire Line
	3150 3550 3250 3450
Entry Wire Line
	3150 3450 3250 3350
Wire Wire Line
	3150 3450 2600 3450
Wire Wire Line
	2600 3550 3150 3550
Wire Wire Line
	3150 3650 2600 3650
Entry Wire Line
	3150 3950 3250 3850
Entry Wire Line
	3150 3850 3250 3750
Entry Wire Line
	3150 3750 3250 3650
Wire Wire Line
	3150 3750 2600 3750
Wire Wire Line
	2600 3850 3150 3850
Wire Wire Line
	3150 3950 2600 3950
Wire Wire Line
	1850 5150 2100 5150
Wire Wire Line
	2100 5750 1850 5750
Wire Wire Line
	2600 5550 2850 5550
Wire Wire Line
	2850 5650 2600 5650
Wire Wire Line
	2850 6050 2600 6050
Wire Wire Line
	2850 6150 2600 6150
Wire Wire Line
	2850 6250 2600 6250
Wire Wire Line
	2850 5750 2600 5750
Wire Wire Line
	2850 5850 2600 5850
Wire Wire Line
	2600 5950 2850 5950
Wire Wire Line
	2850 6850 2600 6850
Wire Wire Line
	3050 5450 2600 5450
Wire Wire Line
	2600 5350 2850 5350
Wire Wire Line
	2600 5250 2850 5250
Wire Wire Line
	2600 5150 2850 5150
Text GLabel 2850 6550 2    50   Input ~ 0
OP2
Text GLabel 2850 6650 2    50   Input ~ 0
OP3
Text GLabel 2850 6750 2    50   Output ~ 0
IP2
Text GLabel 2850 6850 2    50   Output ~ 0
IP3
Wire Wire Line
	2850 6450 2600 6450
Wire Wire Line
	2850 6550 2600 6550
Wire Wire Line
	2850 6650 2600 6650
Wire Wire Line
	2600 6750 2850 6750
Text GLabel 2850 6350 2    50   Input ~ 0
SIACK
Wire Wire Line
	2850 6350 2600 6350
Wire Wire Line
	2100 5250 1850 5250
Wire Wire Line
	1850 5350 2100 5350
Wire Wire Line
	2100 5450 1850 5450
Text GLabel 1850 5350 0    50   Output ~ 0
BGACK
Text GLabel 1850 5250 0    50   Input ~ 0
BG
Text GLabel 1850 5450 0    50   Output ~ 0
BR
Text GLabel 1850 5650 0    50   BiDi ~ 0
RESET
Text GLabel 1850 5550 0    50   BiDi ~ 0
HALT
Wire Wire Line
	1850 5550 2100 5550
Wire Wire Line
	1850 5650 2100 5650
NoConn ~ 2100 5850
Text GLabel 1850 6850 0    50   Output ~ 0
CLK
Wire Wire Line
	1850 6050 2100 6050
Text GLabel 1850 6050 0    50   Input ~ 0
0SEL
Wire Wire Line
	1850 5950 2100 5950
Wire Wire Line
	2100 6150 1850 6150
Wire Wire Line
	1850 6250 2100 6250
Wire Wire Line
	2100 6350 1850 6350
Wire Wire Line
	1850 6450 2100 6450
Wire Wire Line
	2100 6550 1850 6550
Wire Wire Line
	1850 6650 2100 6650
Wire Wire Line
	2100 6750 1850 6750
Text GLabel 1850 6650 0    50   Input ~ 0
6SEL
Text GLabel 1850 6550 0    50   Input ~ 0
5SEL
Text GLabel 1850 6450 0    50   Input ~ 0
4SEL
Text GLabel 1850 6350 0    50   Input ~ 0
3SEL
Text GLabel 1850 6250 0    50   Input ~ 0
2SEL
Text GLabel 1850 5950 0    50   Input ~ 0
ROMSEL
Text GLabel 1850 6150 0    50   Input ~ 0
RAMSEL
Text GLabel 1850 6750 0    50   Input ~ 0
IOSEL
Wire Wire Line
	1850 6850 2100 6850
Text GLabel 2850 6450 2    50   Output ~ 0
SIRQ
$Comp
L ArduinoShields:Arduino_Mega2560_Shield XA1
U 1 1 5E781D1C
P 6250 3850
F 0 "XA1" H 6250 1469 60  0000 C CNN
F 1 "Arduino_Mega2560_Shield" H 6250 1363 60  0000 C CNN
F 2 "ArduinoShields:Arduino_Mega2560_Shield" H 6950 6600 60  0001 C CNN
F 3 "https://store.arduino.cc/arduino-mega-2560-rev3" H 6950 6600 60  0001 C CNN
	1    6250 3850
	1    0    0    -1  
$EndComp
Wire Bus Line
	2950 1250 4550 1250
Wire Bus Line
	1750 850  8150 850 
Wire Bus Line
	1450 650  8650 650 
Wire Bus Line
	3250 1050 8350 1050
Entry Wire Line
	4650 4000 4550 3900
Entry Wire Line
	4650 3900 4550 3800
Entry Wire Line
	4650 3800 4550 3700
Entry Wire Line
	4650 4300 4550 4200
Entry Wire Line
	4650 4200 4550 4100
Entry Wire Line
	4650 4100 4550 4000
Entry Wire Line
	4650 4500 4550 4400
Entry Wire Line
	4650 4400 4550 4300
Wire Wire Line
	4650 3800 4950 3800
Wire Wire Line
	4950 3900 4650 3900
Wire Wire Line
	4650 4000 4950 4000
Wire Wire Line
	4950 4100 4650 4100
Wire Wire Line
	4650 4200 4950 4200
Wire Wire Line
	4950 4300 4650 4300
Wire Wire Line
	4650 4400 4950 4400
Wire Wire Line
	4950 4500 4650 4500
Text Label 4650 3800 0    50   ~ 0
D8
Text Label 4650 3900 0    50   ~ 0
D9
Text Label 4650 4000 0    50   ~ 0
D10
Text Label 4650 4100 0    50   ~ 0
D11
Text Label 4650 4200 0    50   ~ 0
D12
Text Label 4650 4300 0    50   ~ 0
D13
Text Label 4650 4400 0    50   ~ 0
D14
Text Label 4650 4500 0    50   ~ 0
D15
Entry Wire Line
	8550 5000 8650 4900
Entry Wire Line
	8550 4900 8650 4800
Entry Wire Line
	8550 5300 8650 5200
Entry Wire Line
	8550 5200 8650 5100
Entry Wire Line
	8550 5100 8650 5000
Entry Wire Line
	8550 5600 8650 5500
Entry Wire Line
	8550 5500 8650 5400
Entry Wire Line
	8550 5400 8650 5300
Entry Wire Line
	8050 3000 8150 2900
Entry Wire Line
	8050 2900 8150 2800
Entry Wire Line
	8050 3300 8150 3200
Entry Wire Line
	8050 3200 8150 3100
Entry Wire Line
	8050 3100 8150 3000
Entry Wire Line
	8050 3600 8150 3500
Entry Wire Line
	8050 3500 8150 3400
Entry Wire Line
	8050 3400 8150 3300
Entry Wire Line
	8250 3800 8350 3700
Entry Wire Line
	8250 4100 8350 4000
Entry Wire Line
	8250 4000 8350 3900
Entry Wire Line
	8250 3900 8350 3800
Entry Wire Line
	8550 4400 8650 4300
Entry Wire Line
	8550 4300 8650 4200
Entry Wire Line
	8550 4200 8650 4100
Entry Wire Line
	8250 3700 8350 3600
Wire Wire Line
	8250 3700 7550 3700
Wire Wire Line
	7550 3800 8250 3800
Wire Wire Line
	8250 3900 7550 3900
Wire Wire Line
	7550 4000 8250 4000
Wire Wire Line
	8250 4100 7550 4100
Wire Wire Line
	7550 4200 8550 4200
Wire Wire Line
	8550 4300 7550 4300
Wire Wire Line
	7550 4400 8550 4400
Wire Wire Line
	8050 2900 7550 2900
Wire Wire Line
	7550 3000 8050 3000
Wire Wire Line
	8050 3100 7550 3100
Wire Wire Line
	7550 3200 8050 3200
Wire Wire Line
	8050 3300 7550 3300
Wire Wire Line
	7550 3400 8050 3400
Wire Wire Line
	8050 3500 7550 3500
Wire Wire Line
	7550 3600 8050 3600
Wire Wire Line
	2100 2650 1850 2650
Entry Wire Line
	1850 2650 1750 2550
Entry Wire Line
	1850 2750 1750 2650
Wire Wire Line
	2100 2550 1850 2550
Wire Wire Line
	2100 2450 1850 2450
Wire Wire Line
	1850 2350 2100 2350
Entry Wire Line
	1850 2350 1750 2250
Entry Wire Line
	1850 2450 1750 2350
Entry Wire Line
	1850 2550 1750 2450
Wire Wire Line
	2100 2250 1850 2250
Wire Wire Line
	2100 2150 1850 2150
Wire Wire Line
	1850 2050 2100 2050
Entry Wire Line
	1850 2050 1750 1950
Entry Wire Line
	1850 2150 1750 2050
Entry Wire Line
	1850 2250 1750 2150
Text Label 8050 3600 2    50   ~ 0
D7
Text Label 8050 3500 2    50   ~ 0
D6
Text Label 8050 3400 2    50   ~ 0
D5
Text Label 8050 3300 2    50   ~ 0
D4
Text Label 8050 3200 2    50   ~ 0
D3
Text Label 8050 3100 2    50   ~ 0
D2
Text Label 8050 3000 2    50   ~ 0
D1
Text Label 8050 2900 2    50   ~ 0
D0
Text Label 8550 4400 2    50   ~ 0
A9
Text Label 8550 4300 2    50   ~ 0
A10
Text Label 8550 4200 2    50   ~ 0
A11
Text Label 8250 4100 2    50   ~ 0
A12
Text Label 8250 4000 2    50   ~ 0
A13
Text Label 8250 3900 2    50   ~ 0
A14
Text Label 8250 3800 2    50   ~ 0
A15
Text Label 8250 3700 2    50   ~ 0
A16
Wire Wire Line
	8550 4900 7550 4900
Wire Wire Line
	7550 5000 8550 5000
Wire Wire Line
	8550 5100 7550 5100
Wire Wire Line
	7550 5200 8550 5200
Wire Wire Line
	8550 5300 7550 5300
Wire Wire Line
	7550 5400 8550 5400
Wire Wire Line
	8550 5500 7550 5500
Wire Wire Line
	7550 5600 8550 5600
Text Label 8550 4900 2    50   ~ 0
A8
Text Label 8550 5000 2    50   ~ 0
A7
Text Label 8550 5100 2    50   ~ 0
A6
Text Label 8550 5200 2    50   ~ 0
A5
Text Label 8550 5300 2    50   ~ 0
A4
Text Label 8550 5400 2    50   ~ 0
A3
Text Label 8550 5500 2    50   ~ 0
A2
Text Label 8550 5600 2    50   ~ 0
A1
Text GLabel 8050 6000 2    50   Input ~ 0
R~W
Text GLabel 7850 5900 2    50   Input ~ 0
LDS
Text GLabel 7850 5800 2    50   Input ~ 0
UDS
Text GLabel 7850 5700 2    50   Input ~ 0
AS
Text GLabel 7850 4800 2    50   Output ~ 0
BR
Text GLabel 7850 4700 2    50   Output ~ 0
BGACK
Text GLabel 7850 4600 2    50   Input ~ 0
BG
Text GLabel 7850 4500 2    50   Output ~ 0
DTACK
Wire Wire Line
	8050 6000 7550 6000
Wire Wire Line
	7550 5900 7850 5900
Wire Wire Line
	7850 5800 7550 5800
Wire Wire Line
	7550 5700 7850 5700
Wire Wire Line
	7850 4500 7550 4500
Wire Wire Line
	7550 4600 7850 4600
Wire Wire Line
	7850 4700 7550 4700
Wire Wire Line
	7550 4800 7850 4800
Text GLabel 7750 2100 2    50   BiDi ~ 0
RESET
Wire Wire Line
	7550 2100 7750 2100
$Comp
L power:GND #PWR0105
U 1 1 5EB32A5A
P 4550 5200
F 0 "#PWR0105" H 4550 4950 50  0001 C CNN
F 1 "GND" H 4555 5027 50  0000 C CNN
F 2 "" H 4550 5200 50  0001 C CNN
F 3 "" H 4550 5200 50  0001 C CNN
	1    4550 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 5100 4550 5100
Text Label 2850 2050 2    50   ~ 0
D1
Text Label 2850 2150 2    50   ~ 0
D3
Text Label 2850 2250 2    50   ~ 0
D5
Text Label 1850 2050 0    50   ~ 0
D0
Text Label 1850 2150 0    50   ~ 0
D2
Text Label 1850 2250 0    50   ~ 0
D4
Text Label 1550 2850 0    50   ~ 0
A22
Text Label 1550 2950 0    50   ~ 0
A20
Text Label 3150 2850 2    50   ~ 0
A23
Text Label 3150 2950 2    50   ~ 0
A21
Text Label 3150 3050 2    50   ~ 0
A19
Text Label 1850 2350 0    50   ~ 0
D6
Text Label 1850 2450 0    50   ~ 0
D8
Text Label 1850 2550 0    50   ~ 0
D10
Text Label 1850 2650 0    50   ~ 0
D12
Text Label 1850 2750 0    50   ~ 0
D14
Text Label 2850 2350 2    50   ~ 0
D7
Text Label 2850 2450 2    50   ~ 0
D9
Text Label 2850 2550 2    50   ~ 0
D11
Text Label 2850 2650 2    50   ~ 0
D13
Text Label 2850 2750 2    50   ~ 0
D15
Text Label 1550 3050 0    50   ~ 0
A18
Text Label 1550 3150 0    50   ~ 0
A16
Text Label 1550 3250 0    50   ~ 0
A14
Text Label 1550 3350 0    50   ~ 0
A12
Text Label 1550 3450 0    50   ~ 0
A10
Text Label 1550 3550 0    50   ~ 0
A8
Text Label 1550 3650 0    50   ~ 0
A6
Text Label 1550 3750 0    50   ~ 0
A4
Text Label 1550 3850 0    50   ~ 0
A2
Text Label 3150 3150 2    50   ~ 0
A17
Text Label 3150 3250 2    50   ~ 0
A15
Text Label 3150 3350 2    50   ~ 0
A13
Text Label 3150 3450 2    50   ~ 0
A11
Text Label 3150 3550 2    50   ~ 0
A9
Text Label 3150 3650 2    50   ~ 0
A7
Text Label 3150 3750 2    50   ~ 0
A5
Text Label 3150 3850 2    50   ~ 0
A3
Text Label 3150 3950 2    50   ~ 0
A1
Wire Wire Line
	2100 2750 1850 2750
Wire Wire Line
	2100 5050 2100 4750
Wire Wire Line
	2100 4750 2600 4750
Wire Wire Line
	2600 4750 2600 5050
$Comp
L Device:Jumper JP1
U 1 1 5E88070B
P 3100 4750
F 0 "JP1" H 3100 5014 50  0000 C CNN
F 1 "Jumper" H 3100 4923 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3100 4750 50  0001 C CNN
F 3 "~" H 3100 4750 50  0001 C CNN
	1    3100 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 4750 3550 4750
Wire Wire Line
	3550 4750 3550 4650
Wire Wire Line
	2600 4750 2800 4750
Connection ~ 2600 4750
$Comp
L power:+5V #PWR0102
U 1 1 5E8B09C2
P 4550 5650
F 0 "#PWR0102" H 4550 5500 50  0001 C CNN
F 1 "+5V" H 4565 5823 50  0000 C CNN
F 2 "" H 4550 5650 50  0001 C CNN
F 3 "" H 4550 5650 50  0001 C CNN
	1    4550 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 5700 4950 5700
Wire Wire Line
	4550 5100 4550 5200
Wire Wire Line
	4550 5700 4550 5650
Wire Bus Line
	8350 1050 8350 4000
Wire Bus Line
	2950 1250 2950 2650
Wire Bus Line
	4550 1250 4550 4400
Wire Bus Line
	1750 850  1750 2650
Wire Bus Line
	8150 850  8150 3500
Wire Bus Line
	8650 650  8650 5500
Wire Bus Line
	3250 1050 3250 3850
Wire Bus Line
	1450 650  1450 3750
$EndSCHEMATC
