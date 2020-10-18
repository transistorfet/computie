EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "68k-SBC Supervisor Shield"
Date ""
Rev "2"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 1850 4150 0    50   Output ~ 0
DTACK
Text GLabel 1850 4750 0    50   Output ~ 0
BERR
Text GLabel 2850 4950 2    50   Input ~ 0
FC2
Text GLabel 2850 4850 2    50   Input ~ 0
FC1
Text GLabel 2850 4750 2    50   Input ~ 0
FC0
Text GLabel 2850 4550 2    50   Input ~ 0
RD
Text GLabel 2850 4650 2    50   Input ~ 0
WR
Text GLabel 3050 4450 2    50   Input ~ 0
R~W
Text GLabel 2850 4350 2    50   Input ~ 0
LDS
Text GLabel 2850 4250 2    50   Input ~ 0
UDS
Text GLabel 2850 4150 2    50   Input ~ 0
AS
$Comp
L power:GND #PWR0101
U 1 1 5E69B354
P 2000 6000
F 0 "#PWR0101" H 2000 5750 50  0001 C CNN
F 1 "GND" H 2005 5827 50  0000 C CNN
F 2 "" H 2000 6000 50  0001 C CNN
F 3 "" H 2000 6000 50  0001 C CNN
	1    2000 6000
	1    0    0    -1  
$EndComp
Text GLabel 2850 5050 2    50   Output ~ 0
IPL0
Text GLabel 2850 5150 2    50   Output ~ 0
IPL1
Text GLabel 2850 5250 2    50   Output ~ 0
IPL2
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 5E69B363
P 2300 2300
F 0 "J1" H 2350 3417 50  0000 C CNN
F 1 "ADBUS" H 2350 3326 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2300 2300 50  0001 C CNN
F 3 "~" H 2300 2300 50  0001 C CNN
	1    2300 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J2
U 1 1 5E69B369
P 2300 4950
F 0 "J2" H 2350 6067 50  0000 C CNN
F 1 "SIGBUS" H 2350 5976 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 2300 4950 50  0001 C CNN
F 3 "~" H 2300 4950 50  0001 C CNN
	1    2300 4950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0103
U 1 1 5E69B36F
P 3550 3650
F 0 "#PWR0103" H 3550 3500 50  0001 C CNN
F 1 "+5V" H 3565 3823 50  0000 C CNN
F 2 "" H 3550 3650 50  0001 C CNN
F 3 "" H 3550 3650 50  0001 C CNN
	1    3550 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5E69B375
P 2700 6000
F 0 "#PWR0104" H 2700 5750 50  0001 C CNN
F 1 "GND" H 2705 5827 50  0000 C CNN
F 2 "" H 2700 6000 50  0001 C CNN
F 3 "" H 2700 6000 50  0001 C CNN
	1    2700 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 5950 2000 5950
Wire Wire Line
	2000 5950 2000 6000
Wire Wire Line
	2600 5950 2700 5950
Wire Wire Line
	2700 5950 2700 6000
Entry Wire Line
	2850 1600 2950 1500
Entry Wire Line
	2850 1500 2950 1400
Entry Wire Line
	2850 1400 2950 1300
Wire Wire Line
	2850 1400 2600 1400
Wire Wire Line
	2600 1500 2850 1500
Wire Wire Line
	2850 1600 2600 1600
Entry Wire Line
	1550 2300 1450 2200
Entry Wire Line
	1550 2200 1450 2100
Wire Wire Line
	2100 2200 1550 2200
Wire Wire Line
	2100 2300 1550 2300
Entry Wire Line
	3150 2400 3250 2300
Entry Wire Line
	3150 2300 3250 2200
Entry Wire Line
	3150 2200 3250 2100
Wire Wire Line
	3150 2200 2600 2200
Wire Wire Line
	2600 2300 3150 2300
Wire Wire Line
	3150 2400 2600 2400
NoConn ~ 2100 3300
Entry Wire Line
	2850 1900 2950 1800
Entry Wire Line
	2850 1800 2950 1700
Entry Wire Line
	2850 1700 2950 1600
Wire Wire Line
	2850 1700 2600 1700
Wire Wire Line
	2600 1800 2850 1800
Wire Wire Line
	2850 1900 2600 1900
Entry Wire Line
	2850 2100 2950 2000
Entry Wire Line
	2850 2000 2950 1900
Wire Wire Line
	2600 2000 2850 2000
Wire Wire Line
	2850 2100 2600 2100
Entry Wire Line
	1550 2500 1450 2400
Entry Wire Line
	1550 2400 1450 2300
Wire Wire Line
	2100 2400 1550 2400
Wire Wire Line
	2100 2500 1550 2500
Entry Wire Line
	1550 2700 1450 2600
Entry Wire Line
	1550 2600 1450 2500
Wire Wire Line
	2100 2600 1550 2600
Wire Wire Line
	2100 2700 1550 2700
Entry Wire Line
	1550 2900 1450 2800
Entry Wire Line
	1550 2800 1450 2700
Wire Wire Line
	2100 2800 1550 2800
Wire Wire Line
	2100 2900 1550 2900
Entry Wire Line
	1550 3000 1450 2900
Wire Wire Line
	2100 3000 1550 3000
Entry Wire Line
	1550 3200 1450 3100
Entry Wire Line
	1550 3100 1450 3000
Wire Wire Line
	2100 3100 1550 3100
Wire Wire Line
	2100 3200 1550 3200
Entry Wire Line
	3150 2700 3250 2600
Entry Wire Line
	3150 2600 3250 2500
Entry Wire Line
	3150 2500 3250 2400
Wire Wire Line
	3150 2500 2600 2500
Wire Wire Line
	2600 2600 3150 2600
Wire Wire Line
	3150 2700 2600 2700
Entry Wire Line
	3150 3000 3250 2900
Entry Wire Line
	3150 2900 3250 2800
Entry Wire Line
	3150 2800 3250 2700
Wire Wire Line
	3150 2800 2600 2800
Wire Wire Line
	2600 2900 3150 2900
Wire Wire Line
	3150 3000 2600 3000
Entry Wire Line
	3150 3300 3250 3200
Entry Wire Line
	3150 3200 3250 3100
Entry Wire Line
	3150 3100 3250 3000
Wire Wire Line
	3150 3100 2600 3100
Wire Wire Line
	2600 3200 3150 3200
Wire Wire Line
	3150 3300 2600 3300
Wire Wire Line
	1850 4150 2100 4150
Wire Wire Line
	2100 4750 1850 4750
Wire Wire Line
	2600 4550 2850 4550
Wire Wire Line
	2850 4650 2600 4650
Wire Wire Line
	2850 5050 2600 5050
Wire Wire Line
	2850 5150 2600 5150
Wire Wire Line
	2850 5250 2600 5250
Wire Wire Line
	2850 4750 2600 4750
Wire Wire Line
	2850 4850 2600 4850
Wire Wire Line
	2600 4950 2850 4950
Wire Wire Line
	2850 5850 2600 5850
Wire Wire Line
	3050 4450 2600 4450
Wire Wire Line
	2600 4350 2850 4350
Wire Wire Line
	2600 4250 2850 4250
Wire Wire Line
	2600 4150 2850 4150
Text GLabel 2850 5550 2    50   Input ~ 0
OP2
Text GLabel 2850 5650 2    50   Input ~ 0
OP3
Text GLabel 2850 5750 2    50   Output ~ 0
IP4
Text GLabel 2850 5850 2    50   Output ~ 0
IP5
Wire Wire Line
	2850 5550 2600 5550
Wire Wire Line
	2850 5650 2600 5650
Wire Wire Line
	2600 5750 2850 5750
Text GLabel 2850 5350 2    50   Input ~ 0
IACK
Wire Wire Line
	2850 5350 2600 5350
Wire Wire Line
	2100 4250 1850 4250
Wire Wire Line
	1850 4350 2100 4350
Wire Wire Line
	2100 4450 1850 4450
Text GLabel 1850 4350 0    50   Output ~ 0
BGACK
Text GLabel 1850 4250 0    50   Input ~ 0
BG
Text GLabel 1850 4450 0    50   Output ~ 0
BR
Text GLabel 1850 4650 0    50   BiDi ~ 0
RESET
Text GLabel 1850 4550 0    50   BiDi ~ 0
HALT
Wire Wire Line
	1850 4550 2100 4550
Wire Wire Line
	1850 4650 2100 4650
NoConn ~ 2100 4850
Text GLabel 1850 5850 0    50   Output ~ 0
CLK
Wire Wire Line
	1850 5050 2100 5050
Text GLabel 1850 5050 0    50   Input ~ 0
0SEL
Wire Wire Line
	1850 4950 2100 4950
Wire Wire Line
	2100 5150 1850 5150
Wire Wire Line
	1850 5250 2100 5250
Wire Wire Line
	2100 5350 1850 5350
Wire Wire Line
	1850 5450 2100 5450
Wire Wire Line
	2100 5550 1850 5550
Wire Wire Line
	1850 5650 2100 5650
Wire Wire Line
	2100 5750 1850 5750
Text GLabel 1850 5650 0    50   Input ~ 0
6SEL
Text GLabel 1850 5550 0    50   Input ~ 0
5SEL
Text GLabel 1850 5450 0    50   Input ~ 0
4SEL
Text GLabel 1850 5350 0    50   Input ~ 0
3SEL
Text GLabel 1850 5250 0    50   Input ~ 0
2SEL
Text GLabel 1850 4950 0    50   Input ~ 0
ROMSEL
Text GLabel 1850 5150 0    50   Input ~ 0
RAMSEL
Text GLabel 1850 5750 0    50   Input ~ 0
IOSEL
Wire Wire Line
	1850 5850 2100 5850
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
	1750 850  2950 850 
Wire Bus Line
	1450 650  3250 650 
Entry Wire Line
	4450 4000 4350 3900
Entry Wire Line
	4450 3900 4350 3800
Entry Wire Line
	4450 3800 4350 3700
Entry Wire Line
	4450 4300 4350 4200
Entry Wire Line
	4450 4200 4350 4100
Entry Wire Line
	4450 4100 4350 4000
Entry Wire Line
	4450 4500 4350 4400
Entry Wire Line
	4450 4400 4350 4300
Wire Wire Line
	4450 3800 4950 3800
Wire Wire Line
	4950 3900 4450 3900
Wire Wire Line
	4450 4000 4950 4000
Wire Wire Line
	4950 4100 4450 4100
Wire Wire Line
	4450 4200 4950 4200
Wire Wire Line
	4950 4300 4450 4300
Wire Wire Line
	4450 4400 4950 4400
Wire Wire Line
	4950 4500 4450 4500
Text Label 4450 3800 0    50   ~ 0
D8
Text Label 4450 3900 0    50   ~ 0
D9
Text Label 4450 4000 0    50   ~ 0
D10
Text Label 4450 4100 0    50   ~ 0
D11
Text Label 4450 4200 0    50   ~ 0
D12
Text Label 4450 4300 0    50   ~ 0
D13
Text Label 4450 4400 0    50   ~ 0
D14
Text Label 4450 4500 0    50   ~ 0
D15
Entry Wire Line
	8250 3000 8350 2900
Entry Wire Line
	8250 2900 8350 2800
Entry Wire Line
	8250 3300 8350 3200
Entry Wire Line
	8250 3200 8350 3100
Entry Wire Line
	8250 3100 8350 3000
Entry Wire Line
	8250 3600 8350 3500
Entry Wire Line
	8250 3500 8350 3400
Entry Wire Line
	8250 3400 8350 3300
Wire Wire Line
	7550 3200 8250 3200
Wire Wire Line
	8250 3300 7550 3300
Wire Wire Line
	7550 3400 8250 3400
Wire Wire Line
	8250 3500 7550 3500
Wire Wire Line
	7550 3600 8250 3600
Wire Wire Line
	2100 2000 1850 2000
Entry Wire Line
	1850 2000 1750 1900
Entry Wire Line
	1850 2100 1750 2000
Wire Wire Line
	2100 1900 1850 1900
Wire Wire Line
	2100 1800 1850 1800
Wire Wire Line
	1850 1700 2100 1700
Entry Wire Line
	1850 1700 1750 1600
Entry Wire Line
	1850 1800 1750 1700
Entry Wire Line
	1850 1900 1750 1800
Wire Wire Line
	2100 1600 1850 1600
Wire Wire Line
	2100 1500 1850 1500
Wire Wire Line
	1850 1400 2100 1400
Entry Wire Line
	1850 1400 1750 1300
Entry Wire Line
	1850 1500 1750 1400
Entry Wire Line
	1850 1600 1750 1500
Text Label 8250 3600 2    50   ~ 0
D7
Text Label 8250 3500 2    50   ~ 0
D6
Text Label 8250 3400 2    50   ~ 0
D5
Text Label 8250 3300 2    50   ~ 0
D4
Text Label 8250 3200 2    50   ~ 0
D3
Text Label 8250 3100 2    50   ~ 0
D2
Text Label 8250 3000 2    50   ~ 0
D1
Text Label 8250 2900 2    50   ~ 0
D0
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
Text GLabel 7850 2500 2    50   BiDi ~ 0
RESET
Wire Wire Line
	7550 2500 7850 2500
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
Text Label 2850 1400 2    50   ~ 0
D1
Text Label 2850 1500 2    50   ~ 0
D3
Text Label 2850 1600 2    50   ~ 0
D5
Text Label 1850 1400 0    50   ~ 0
D0
Text Label 1850 1500 0    50   ~ 0
D2
Text Label 1850 1600 0    50   ~ 0
D4
Text Label 1550 2200 0    50   ~ 0
A22
Text Label 1550 2300 0    50   ~ 0
A20
Text Label 3150 2200 2    50   ~ 0
A23
Text Label 3150 2300 2    50   ~ 0
A21
Text Label 3150 2400 2    50   ~ 0
A19
Text Label 1850 1700 0    50   ~ 0
D6
Text Label 1850 1800 0    50   ~ 0
D8
Text Label 1850 1900 0    50   ~ 0
D10
Text Label 1850 2000 0    50   ~ 0
D12
Text Label 1850 2100 0    50   ~ 0
D14
Text Label 2850 1700 2    50   ~ 0
D7
Text Label 2850 1800 2    50   ~ 0
D9
Text Label 2850 1900 2    50   ~ 0
D11
Text Label 2850 2000 2    50   ~ 0
D13
Text Label 2850 2100 2    50   ~ 0
D15
Text Label 1550 2400 0    50   ~ 0
A18
Text Label 1550 2500 0    50   ~ 0
A16
Text Label 1550 2600 0    50   ~ 0
A14
Text Label 1550 2700 0    50   ~ 0
A12
Text Label 1550 2800 0    50   ~ 0
A10
Text Label 1550 2900 0    50   ~ 0
A8
Text Label 1550 3000 0    50   ~ 0
A6
Text Label 1550 3100 0    50   ~ 0
A4
Text Label 1550 3200 0    50   ~ 0
A2
Text Label 3150 2500 2    50   ~ 0
A17
Text Label 3150 2600 2    50   ~ 0
A15
Text Label 3150 2700 2    50   ~ 0
A13
Text Label 3150 2800 2    50   ~ 0
A11
Text Label 3150 2900 2    50   ~ 0
A9
Text Label 3150 3000 2    50   ~ 0
A7
Text Label 3150 3100 2    50   ~ 0
A5
Text Label 3150 3200 2    50   ~ 0
A3
Text Label 3150 3300 2    50   ~ 0
A1
Wire Wire Line
	2100 2100 1850 2100
Wire Wire Line
	2100 4050 2100 3750
Wire Wire Line
	2100 3750 2600 3750
Wire Wire Line
	2600 3750 2600 4050
$Comp
L Device:Jumper JP1
U 1 1 5E88070B
P 3100 3750
F 0 "JP1" H 3100 4014 50  0000 C CNN
F 1 "Jumper" H 3100 3923 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3100 3750 50  0001 C CNN
F 3 "~" H 3100 3750 50  0001 C CNN
	1    3100 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3750 3550 3750
Wire Wire Line
	3550 3750 3550 3650
Wire Wire Line
	2600 3750 2800 3750
Connection ~ 2600 3750
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
Connection ~ 3250 650 
Wire Wire Line
	8250 3100 7550 3100
Wire Wire Line
	7550 3000 8250 3000
Wire Wire Line
	8250 2900 7550 2900
Wire Bus Line
	3250 650  4500 650 
Text Label 8550 5600 2    50   ~ 0
A1
Text Label 8550 5500 2    50   ~ 0
A2
Text Label 8550 5400 2    50   ~ 0
A3
Text Label 8550 5300 2    50   ~ 0
A4
Text Label 8550 5200 2    50   ~ 0
A5
Text Label 8550 5100 2    50   ~ 0
A6
Text Label 8550 5000 2    50   ~ 0
A7
Text Label 8550 4900 2    50   ~ 0
A8
Wire Wire Line
	7550 5600 8550 5600
Wire Wire Line
	8550 5500 7550 5500
Wire Wire Line
	7550 5400 8550 5400
Wire Wire Line
	8550 5300 7550 5300
Wire Wire Line
	7550 5200 8550 5200
Wire Wire Line
	8550 5100 7550 5100
Wire Wire Line
	7550 5000 8550 5000
Wire Wire Line
	8550 4900 7550 4900
Text Label 8550 4200 2    50   ~ 0
A11
Text Label 8550 4300 2    50   ~ 0
A10
Text Label 8550 4400 2    50   ~ 0
A9
Wire Wire Line
	7550 4400 8550 4400
Wire Wire Line
	8550 4300 7550 4300
Wire Wire Line
	7550 4200 8550 4200
Entry Wire Line
	8550 4200 8650 4100
Entry Wire Line
	8550 4300 8650 4200
Entry Wire Line
	8550 4400 8650 4300
Entry Wire Line
	8550 5400 8650 5300
Entry Wire Line
	8550 5500 8650 5400
Entry Wire Line
	8550 5600 8650 5500
Entry Wire Line
	8550 5100 8650 5000
Entry Wire Line
	8550 5200 8650 5100
Entry Wire Line
	8550 5300 8650 5200
Entry Wire Line
	8550 4900 8650 4800
Entry Wire Line
	8550 5000 8650 4900
Entry Wire Line
	8550 3800 8650 3700
Entry Wire Line
	8550 4100 8650 4000
Entry Wire Line
	8550 4000 8650 3900
Entry Wire Line
	8550 3900 8650 3800
Wire Wire Line
	7550 3800 8550 3800
Wire Wire Line
	8550 3900 7550 3900
Wire Wire Line
	7550 4000 8550 4000
Wire Wire Line
	8550 4100 7550 4100
Text Label 8550 4100 2    50   ~ 0
A12
Text Label 8550 4000 2    50   ~ 0
A13
Text Label 8550 3900 2    50   ~ 0
A14
Text Label 8550 3800 2    50   ~ 0
A15
Text GLabel 4900 3700 0    50   Input ~ 0
ROMSEL
Connection ~ 2950 850 
Wire Bus Line
	2950 850  4350 850 
Connection ~ 4350 850 
Wire Bus Line
	4350 850  8350 850 
Text GLabel 7850 2600 2    50   Output ~ 0
BERR
Wire Wire Line
	7550 2600 7850 2600
Wire Wire Line
	4950 5500 4950 5400
Connection ~ 4950 5100
Connection ~ 4950 5200
Wire Wire Line
	4950 5200 4950 5100
Connection ~ 4950 5300
Wire Wire Line
	4950 5300 4950 5200
Connection ~ 4950 5400
Wire Wire Line
	4950 5400 4950 5300
Entry Wire Line
	4600 3200 4500 3100
Entry Wire Line
	4600 3100 4500 3000
Entry Wire Line
	4600 3000 4500 2900
Entry Wire Line
	4600 3500 4500 3400
Entry Wire Line
	4600 3400 4500 3300
Entry Wire Line
	4600 3300 4500 3200
Entry Wire Line
	4600 3600 4500 3500
Wire Wire Line
	4600 3000 4950 3000
Wire Wire Line
	4600 3200 4950 3200
Wire Wire Line
	4600 3400 4950 3400
Wire Wire Line
	4600 3600 4950 3600
Text Label 4600 3000 0    50   ~ 0
A17
Text Label 4600 3100 0    50   ~ 0
A18
Text Label 4600 3200 0    50   ~ 0
A19
Text Label 4600 3300 0    50   ~ 0
A20
Text Label 4600 3400 0    50   ~ 0
A21
Text Label 4600 3500 0    50   ~ 0
A22
Text Label 4600 3600 0    50   ~ 0
A23
Wire Wire Line
	4950 3100 4600 3100
Wire Wire Line
	4950 3300 4600 3300
Wire Wire Line
	4950 3500 4600 3500
Connection ~ 4500 650 
Wire Bus Line
	4500 650  8650 650 
$Comp
L Switch:SW_Push SW1
U 1 1 5F31611C
P 4250 4800
F 0 "SW1" H 4250 5085 50  0000 C CNN
F 1 "RESET" H 4250 4994 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4250 5000 50  0001 C CNN
F 3 "~" H 4250 5000 50  0001 C CNN
	1    4250 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 4800 4450 4800
Wire Wire Line
	4050 4800 4050 5100
Wire Wire Line
	4050 5100 4550 5100
Connection ~ 4550 5100
Entry Wire Line
	8550 3700 8650 3600
Wire Wire Line
	7550 3700 8550 3700
Text Label 8550 3700 2    50   ~ 0
A16
NoConn ~ 2600 5450
NoConn ~ 4950 4600
NoConn ~ 4950 4900
Wire Wire Line
	4900 3700 4950 3700
Text GLabel 7850 2700 2    50   BiDi ~ 0
HALT
Wire Wire Line
	7550 2700 7850 2700
$Comp
L Connector_Generic:Conn_02x06_Odd_Even J3
U 1 1 5F3489D0
P 2300 6950
F 0 "J3" H 2350 7367 50  0000 C CNN
F 1 "UNMAPPED" H 2350 7276 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x06_P2.54mm_Vertical" H 2300 6950 50  0001 C CNN
F 3 "~" H 2300 6950 50  0001 C CNN
	1    2300 6950
	1    0    0    -1  
$EndComp
Text GLabel 2850 6950 2    50   Input ~ 0
FC2
Text GLabel 2850 6850 2    50   Input ~ 0
FC1
Text GLabel 2850 6750 2    50   Input ~ 0
FC0
Text GLabel 2850 7050 2    50   Output ~ 0
IPL0
Text GLabel 2850 7150 2    50   Output ~ 0
IPL1
Text GLabel 2850 7250 2    50   Output ~ 0
IPL2
Wire Wire Line
	2850 7050 2600 7050
Wire Wire Line
	2850 7150 2600 7150
Wire Wire Line
	2850 7250 2600 7250
Wire Wire Line
	2850 6750 2600 6750
Wire Wire Line
	2850 6850 2600 6850
Wire Wire Line
	2600 6950 2850 6950
Text GLabel 1850 7250 0    50   Input ~ 0
IACK
Text GLabel 1850 7150 0    50   Input ~ 0
OP2
Text GLabel 1850 7050 0    50   Input ~ 0
OP3
Text GLabel 1850 6950 0    50   Output ~ 0
IP4
Text GLabel 1850 6850 0    50   Output ~ 0
IP5
Text GLabel 1850 6750 0    50   Output ~ 0
CLK
Wire Wire Line
	1850 6750 2100 6750
Wire Wire Line
	1850 7250 2100 7250
Wire Wire Line
	2100 7150 1850 7150
Wire Wire Line
	1850 7050 2100 7050
Wire Wire Line
	2100 6950 1850 6950
Wire Wire Line
	1850 6850 2100 6850
Wire Bus Line
	2950 850  2950 2000
Wire Bus Line
	4350 850  4350 4400
Wire Bus Line
	4500 650  4500 3600
Wire Bus Line
	8350 850  8350 3500
Wire Bus Line
	1450 650  1450 3100
Wire Bus Line
	1750 850  1750 2000
Wire Bus Line
	3250 650  3250 3200
Wire Bus Line
	8650 650  8650 5500
$EndSCHEMATC
