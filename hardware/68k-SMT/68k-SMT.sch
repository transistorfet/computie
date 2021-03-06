EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title "68k-SMT"
Date ""
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CPU_NXP_68000:MC68000FN U2
U 1 1 5DD0E2E5
P 6050 3450
F 0 "U2" H 6050 6031 50  0000 C CNN
F 1 "MC68010" H 6050 5940 50  0000 C CNN
F 2 "Package_LCC:PLCC-68_THT-Socket" H 6050 3450 50  0001 C CNN
F 3 "https://www.nxp.com/docs/en/reference-manual/MC68000UM.pdf" H 6050 3450 50  0001 C CNN
	1    6050 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5DD14FE8
P 6050 6050
F 0 "#PWR017" H 6050 5800 50  0001 C CNN
F 1 "GND" H 6055 5877 50  0000 C CNN
F 2 "" H 6050 6050 50  0001 C CNN
F 3 "" H 6050 6050 50  0001 C CNN
	1    6050 6050
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5DD17080
P 5600 1000
F 0 "#PWR01" H 5600 850 50  0001 C CNN
F 1 "+5V" H 5615 1173 50  0000 C CNN
F 2 "" H 5600 1000 50  0001 C CNN
F 3 "" H 5600 1000 50  0001 C CNN
	1    5600 1000
	1    0    0    -1  
$EndComp
Text GLabel 4900 950  0    50   Input ~ 0
CLK
Entry Wire Line
	7150 3450 7250 3350
Entry Wire Line
	7150 3350 7250 3250
Entry Wire Line
	7150 3250 7250 3150
Entry Wire Line
	7150 3150 7250 3050
Entry Wire Line
	7150 3050 7250 2950
Entry Wire Line
	7150 2950 7250 2850
Entry Wire Line
	7150 2850 7250 2750
Entry Wire Line
	7150 2750 7250 2650
Entry Wire Line
	7150 2650 7250 2550
Entry Wire Line
	7150 2550 7250 2450
Entry Wire Line
	7150 2450 7250 2350
Entry Wire Line
	7150 2350 7250 2250
Entry Wire Line
	7150 2250 7250 2150
Entry Wire Line
	7150 2150 7250 2050
Entry Wire Line
	7150 2050 7250 1950
Entry Wire Line
	7150 1950 7250 1850
Entry Wire Line
	7150 1850 7250 1750
Entry Wire Line
	7150 1750 7250 1650
Entry Wire Line
	7150 1650 7250 1550
Entry Wire Line
	7150 1550 7250 1450
Entry Wire Line
	7150 1450 7250 1350
Entry Wire Line
	7150 1350 7250 1250
Entry Wire Line
	7150 1250 7250 1150
Wire Wire Line
	7050 1250 7150 1250
Wire Wire Line
	7150 1350 7050 1350
Wire Wire Line
	7050 1450 7150 1450
Wire Wire Line
	7150 1550 7050 1550
Wire Wire Line
	7050 1650 7150 1650
Wire Wire Line
	7150 1750 7050 1750
Wire Wire Line
	7150 1850 7050 1850
Wire Wire Line
	7050 1950 7150 1950
Wire Wire Line
	7150 2050 7050 2050
Wire Wire Line
	7050 2150 7150 2150
Wire Wire Line
	7150 2250 7050 2250
Wire Wire Line
	7050 2350 7150 2350
Wire Wire Line
	7150 2450 7050 2450
Wire Wire Line
	7050 2550 7150 2550
Wire Wire Line
	7150 2650 7050 2650
Wire Wire Line
	7050 2750 7150 2750
Wire Wire Line
	7150 2850 7050 2850
Wire Wire Line
	7050 2950 7150 2950
Wire Wire Line
	7150 3050 7050 3050
Wire Wire Line
	7050 3150 7150 3150
Wire Wire Line
	7150 3250 7050 3250
Wire Wire Line
	7050 3350 7150 3350
Wire Wire Line
	7150 3450 7050 3450
Entry Wire Line
	7250 5150 7350 5050
Entry Wire Line
	7250 5050 7350 4950
Entry Wire Line
	7250 4950 7350 4850
Entry Wire Line
	7250 4850 7350 4750
Entry Wire Line
	7250 4750 7350 4650
Entry Wire Line
	7250 4650 7350 4550
Entry Wire Line
	7250 4550 7350 4450
Entry Wire Line
	7250 4450 7350 4350
Entry Wire Line
	7250 4350 7350 4250
Entry Wire Line
	7250 4250 7350 4150
Entry Wire Line
	7250 4150 7350 4050
Entry Wire Line
	7250 4050 7350 3950
Entry Wire Line
	7250 3950 7350 3850
Entry Wire Line
	7250 3850 7350 3750
Entry Wire Line
	7250 3750 7350 3650
Entry Wire Line
	7250 3650 7350 3550
Wire Wire Line
	7250 3650 7050 3650
Wire Wire Line
	7050 3750 7250 3750
Wire Wire Line
	7250 3850 7050 3850
Wire Wire Line
	7050 3950 7250 3950
Wire Wire Line
	7250 4050 7050 4050
Wire Wire Line
	7050 4150 7250 4150
Wire Wire Line
	7250 4250 7050 4250
Wire Wire Line
	7050 4350 7250 4350
Wire Wire Line
	7250 4450 7050 4450
Wire Wire Line
	7050 4550 7250 4550
Wire Wire Line
	7250 4650 7050 4650
Wire Wire Line
	7050 4750 7250 4750
Wire Wire Line
	7250 4850 7050 4850
Wire Wire Line
	7050 4950 7250 4950
Wire Wire Line
	7250 5050 7050 5050
Wire Wire Line
	7050 5150 7250 5150
Text Label 7250 3650 2    50   ~ 0
D0
Text Label 7250 3750 2    50   ~ 0
D1
Text Label 7250 3850 2    50   ~ 0
D2
Text Label 7250 3950 2    50   ~ 0
D3
Text Label 7250 4050 2    50   ~ 0
D4
Text Label 7250 4150 2    50   ~ 0
D5
Text Label 7250 4250 2    50   ~ 0
D6
Text Label 7250 4350 2    50   ~ 0
D7
Text Label 7250 4450 2    50   ~ 0
D8
Text Label 7250 4550 2    50   ~ 0
D9
Text Label 7250 4650 2    50   ~ 0
D10
Text Label 7250 4750 2    50   ~ 0
D11
Text Label 7250 4850 2    50   ~ 0
D12
Text Label 7250 4950 2    50   ~ 0
D13
Text Label 7250 5050 2    50   ~ 0
D14
Text Label 7250 5150 2    50   ~ 0
D15
Text Label 7150 1250 2    50   ~ 0
A1
Text Label 7150 1350 2    50   ~ 0
A2
Text Label 7150 1450 2    50   ~ 0
A3
Text Label 7150 1550 2    50   ~ 0
A4
Text Label 7150 1650 2    50   ~ 0
A5
Text Label 7150 1750 2    50   ~ 0
A6
Text Label 7150 1850 2    50   ~ 0
A7
Text Label 7150 1950 2    50   ~ 0
A8
Text Label 7150 2050 2    50   ~ 0
A9
Text Label 7150 2150 2    50   ~ 0
A10
Text Label 7150 2250 2    50   ~ 0
A11
Text Label 7150 2350 2    50   ~ 0
A12
Text Label 7150 2450 2    50   ~ 0
A13
Text Label 7150 2550 2    50   ~ 0
A14
Text Label 7150 2650 2    50   ~ 0
A15
Text Label 7150 2750 2    50   ~ 0
A16
Text Label 7150 2850 2    50   ~ 0
A17
Text Label 7150 2950 2    50   ~ 0
A18
Text Label 7150 3050 2    50   ~ 0
A19
Text Label 7150 3150 2    50   ~ 0
A20
Text Label 7150 3250 2    50   ~ 0
A21
Text Label 7150 3350 2    50   ~ 0
A22
Text Label 7150 3450 2    50   ~ 0
A23
$Comp
L 74xx:74HC14 U3
U 1 1 5DEECAAF
P 7450 5950
F 0 "U3" H 7450 6267 50  0000 C CNN
F 1 "74HC14" H 7450 6176 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 7450 5950 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 7450 5950 50  0001 C CNN
	1    7450 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 5950 7150 5950
Wire Wire Line
	7750 5950 7800 5950
Wire Wire Line
	7050 5650 7050 5950
Wire Wire Line
	7050 6150 7050 6300
Wire Wire Line
	7050 6150 7800 6150
Text GLabel 7800 5950 2    50   Output ~ 0
RD
Text GLabel 7800 6150 2    50   Output ~ 0
WR
Text GLabel 7800 6300 2    50   Output ~ 0
R~W
Wire Wire Line
	7800 6300 7050 6300
Text GLabel 3350 4150 0    50   Input ~ 0
DTACK
Text GLabel 3350 3950 0    50   Input ~ 0
BERR
Text GLabel 3350 4850 0    50   BiDi ~ 0
RESET
Wire Bus Line
	7250 650  7800 650 
Wire Bus Line
	7350 750  7800 750 
Text GLabel 7800 650  2    50   3State ~ 0
A[0..23]
Text GLabel 7800 750  2    50   3State ~ 0
D[0..15]
Wire Wire Line
	7050 5350 7800 5350
Wire Wire Line
	7050 5450 7800 5450
Wire Wire Line
	7050 5550 7800 5550
Connection ~ 7050 5950
Wire Wire Line
	7050 5950 7050 6150
Wire Bus Line
	7350 750  2200 750 
Connection ~ 7350 750 
Wire Bus Line
	7250 650  2500 650 
Connection ~ 7250 650 
Connection ~ 7050 6150
Text GLabel 3350 4650 0    50   BiDi ~ 0
HALT
Text GLabel 4900 2650 0    50   Output ~ 0
FC2
Wire Wire Line
	4900 2650 5050 2650
Text GLabel 4900 2550 0    50   Output ~ 0
FC1
Text GLabel 4900 2450 0    50   Output ~ 0
FC0
Wire Wire Line
	4900 2550 5050 2550
Wire Wire Line
	5050 2450 4900 2450
Text GLabel 3350 2150 0    50   Input ~ 0
BR
Text GLabel 3350 2050 0    50   Output ~ 0
BG
Text GLabel 3350 1950 0    50   Input ~ 0
BGACK
$Comp
L Device:R_Small R6
U 1 1 5DFB897A
P 4800 3000
F 0 "R6" H 4859 3046 50  0000 L CNN
F 1 "10k" H 4859 2955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" H 4800 3000 50  0001 C CNN
F 3 "~" H 4800 3000 50  0001 C CNN
	1    4800 3000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5DFB8984
P 4800 2900
F 0 "#PWR05" H 4800 2750 50  0001 C CNN
F 1 "+5V" H 4815 3073 50  0000 C CNN
F 2 "" H 4800 2900 50  0001 C CNN
F 3 "" H 4800 2900 50  0001 C CNN
	1    4800 2900
	1    0    0    -1  
$EndComp
NoConn ~ 5050 3050
NoConn ~ 5050 2950
Text GLabel 7800 5550 2    50   Output ~ 0
LDS
Text GLabel 7800 5450 2    50   Output ~ 0
UDS
Text GLabel 7800 5350 2    50   Output ~ 0
AS
Text GLabel 1100 3750 0    50   Output ~ 0
DTACK
Text GLabel 1100 4350 0    50   Output ~ 0
BERR
Text GLabel 2100 4550 2    50   Input ~ 0
FC2
Text GLabel 2100 4450 2    50   Input ~ 0
FC1
Text GLabel 2100 4350 2    50   Input ~ 0
FC0
Text GLabel 2100 3950 2    50   Input ~ 0
LDS
Text GLabel 2100 3850 2    50   Input ~ 0
UDS
Text GLabel 2100 3750 2    50   Input ~ 0
AS
$Comp
L power:GND #PWR015
U 1 1 5E412D92
P 1250 5600
F 0 "#PWR015" H 1250 5350 50  0001 C CNN
F 1 "GND" H 1255 5427 50  0000 C CNN
F 2 "" H 1250 5600 50  0001 C CNN
F 3 "" H 1250 5600 50  0001 C CNN
	1    1250 5600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR09
U 1 1 5E414403
P 1250 3600
F 0 "#PWR09" H 1250 3450 50  0001 C CNN
F 1 "+5V" H 1265 3773 50  0000 C CNN
F 2 "" H 1250 3600 50  0001 C CNN
F 3 "" H 1250 3600 50  0001 C CNN
	1    1250 3600
	1    0    0    -1  
$EndComp
Text GLabel 3250 1750 0    50   Output ~ 0
IPL2
Text GLabel 3250 1650 0    50   Output ~ 0
IPL1
Text GLabel 3250 1550 0    50   Output ~ 0
IPL0
Text GLabel 2100 4650 2    50   Output ~ 0
IPL0
Text GLabel 2100 4750 2    50   Output ~ 0
IPL1
Text GLabel 2100 4850 2    50   Output ~ 0
IPL2
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 5E4E078F
P 1550 2200
F 0 "J1" H 1600 3317 50  0000 C CNN
F 1 "ADBUS" H 1600 3226 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 1550 2200 50  0001 C CNN
F 3 "~" H 1550 2200 50  0001 C CNN
	1    1550 2200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J3
U 1 1 5E4E7079
P 1550 4550
F 0 "J3" H 1600 5667 50  0000 C CNN
F 1 "SIGBUS" H 1600 5576 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 1550 4550 50  0001 C CNN
F 3 "~" H 1550 4550 50  0001 C CNN
	1    1550 4550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR010
U 1 1 5E473AD7
P 1950 3600
F 0 "#PWR010" H 1950 3450 50  0001 C CNN
F 1 "+5V" H 1965 3773 50  0000 C CNN
F 2 "" H 1950 3600 50  0001 C CNN
F 3 "" H 1950 3600 50  0001 C CNN
	1    1950 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5E496B92
P 1950 5600
F 0 "#PWR016" H 1950 5350 50  0001 C CNN
F 1 "GND" H 1955 5427 50  0000 C CNN
F 2 "" H 1950 5600 50  0001 C CNN
F 3 "" H 1950 5600 50  0001 C CNN
	1    1950 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 3600 1250 3650
Wire Wire Line
	1250 3650 1350 3650
Wire Wire Line
	1850 3650 1950 3650
Wire Wire Line
	1950 3650 1950 3600
Wire Wire Line
	1350 5550 1250 5550
Wire Wire Line
	1250 5550 1250 5600
Wire Wire Line
	1850 5550 1950 5550
Wire Wire Line
	1950 5550 1950 5600
Wire Bus Line
	2200 750  1000 750 
Connection ~ 2200 750 
Entry Wire Line
	2100 1500 2200 1400
Entry Wire Line
	2100 1400 2200 1300
Entry Wire Line
	2100 1300 2200 1200
Wire Wire Line
	2100 1300 1850 1300
Wire Wire Line
	1850 1400 2100 1400
Text Label 2100 1300 2    50   ~ 0
D1
Text Label 2100 1400 2    50   ~ 0
D3
Text Label 2100 1500 2    50   ~ 0
D5
Entry Wire Line
	1100 1500 1000 1400
Entry Wire Line
	1100 1400 1000 1300
Entry Wire Line
	1100 1300 1000 1200
Wire Wire Line
	1100 1300 1350 1300
Wire Wire Line
	1350 1400 1100 1400
Text Label 1100 1300 0    50   ~ 0
D0
Text Label 1100 1400 0    50   ~ 0
D2
Text Label 1100 1500 0    50   ~ 0
D4
Wire Wire Line
	1350 1500 1100 1500
Wire Wire Line
	2100 1500 1850 1500
Entry Wire Line
	800  2200 700  2100
Entry Wire Line
	800  2100 700  2000
Wire Wire Line
	1350 2100 800  2100
Text Label 800  2100 0    50   ~ 0
A22
Text Label 800  2200 0    50   ~ 0
A20
Wire Wire Line
	1350 2200 800  2200
Connection ~ 2500 650 
Wire Bus Line
	2500 650  700  650 
Entry Wire Line
	2400 2300 2500 2200
Entry Wire Line
	2400 2200 2500 2100
Entry Wire Line
	2400 2100 2500 2000
Wire Wire Line
	2400 2100 1850 2100
Wire Wire Line
	1850 2200 2400 2200
Text Label 2400 2100 2    50   ~ 0
A23
Text Label 2400 2200 2    50   ~ 0
A21
Text Label 2400 2300 2    50   ~ 0
A19
Wire Wire Line
	2400 2300 1850 2300
NoConn ~ 1350 3200
Entry Wire Line
	1100 1800 1000 1700
Entry Wire Line
	1100 1700 1000 1600
Entry Wire Line
	1100 1600 1000 1500
Wire Wire Line
	1100 1600 1350 1600
Wire Wire Line
	1350 1700 1100 1700
Text Label 1100 1600 0    50   ~ 0
D6
Text Label 1100 1700 0    50   ~ 0
D8
Text Label 1100 1800 0    50   ~ 0
D10
Wire Wire Line
	1350 1800 1100 1800
Entry Wire Line
	1100 2000 1000 1900
Entry Wire Line
	1100 1900 1000 1800
Wire Wire Line
	1350 1900 1100 1900
Text Label 1100 1900 0    50   ~ 0
D12
Text Label 1100 2000 0    50   ~ 0
D14
Wire Wire Line
	1350 2000 1100 2000
Entry Wire Line
	2100 1800 2200 1700
Entry Wire Line
	2100 1700 2200 1600
Entry Wire Line
	2100 1600 2200 1500
Wire Wire Line
	2100 1600 1850 1600
Wire Wire Line
	1850 1700 2100 1700
Text Label 2100 1600 2    50   ~ 0
D7
Text Label 2100 1700 2    50   ~ 0
D9
Text Label 2100 1800 2    50   ~ 0
D11
Wire Wire Line
	2100 1800 1850 1800
Entry Wire Line
	2100 2000 2200 1900
Entry Wire Line
	2100 1900 2200 1800
Wire Wire Line
	1850 1900 2100 1900
Text Label 2100 1900 2    50   ~ 0
D13
Text Label 2100 2000 2    50   ~ 0
D15
Wire Wire Line
	2100 2000 1850 2000
Entry Wire Line
	800  2400 700  2300
Entry Wire Line
	800  2300 700  2200
Wire Wire Line
	1350 2300 800  2300
Text Label 800  2300 0    50   ~ 0
A18
Text Label 800  2400 0    50   ~ 0
A16
Wire Wire Line
	1350 2400 800  2400
Entry Wire Line
	800  2600 700  2500
Entry Wire Line
	800  2500 700  2400
Wire Wire Line
	1350 2500 800  2500
Text Label 800  2500 0    50   ~ 0
A14
Text Label 800  2600 0    50   ~ 0
A12
Wire Wire Line
	1350 2600 800  2600
Entry Wire Line
	800  2800 700  2700
Entry Wire Line
	800  2700 700  2600
Wire Wire Line
	1350 2700 800  2700
Text Label 800  2700 0    50   ~ 0
A10
Text Label 800  2800 0    50   ~ 0
A8
Wire Wire Line
	1350 2800 800  2800
Entry Wire Line
	800  2900 700  2800
Text Label 800  2900 0    50   ~ 0
A6
Wire Wire Line
	1350 2900 800  2900
Entry Wire Line
	800  3100 700  3000
Entry Wire Line
	800  3000 700  2900
Wire Wire Line
	1350 3000 800  3000
Text Label 800  3000 0    50   ~ 0
A4
Text Label 800  3100 0    50   ~ 0
A2
Wire Wire Line
	1350 3100 800  3100
Entry Wire Line
	2400 2600 2500 2500
Entry Wire Line
	2400 2500 2500 2400
Entry Wire Line
	2400 2400 2500 2300
Wire Wire Line
	2400 2400 1850 2400
Wire Wire Line
	1850 2500 2400 2500
Text Label 2400 2400 2    50   ~ 0
A17
Text Label 2400 2500 2    50   ~ 0
A15
Text Label 2400 2600 2    50   ~ 0
A13
Wire Wire Line
	2400 2600 1850 2600
Entry Wire Line
	2400 2900 2500 2800
Entry Wire Line
	2400 2800 2500 2700
Entry Wire Line
	2400 2700 2500 2600
Wire Wire Line
	2400 2700 1850 2700
Wire Wire Line
	1850 2800 2400 2800
Text Label 2400 2700 2    50   ~ 0
A11
Text Label 2400 2800 2    50   ~ 0
A9
Text Label 2400 2900 2    50   ~ 0
A7
Wire Wire Line
	2400 2900 1850 2900
Entry Wire Line
	2400 3200 2500 3100
Entry Wire Line
	2400 3100 2500 3000
Entry Wire Line
	2400 3000 2500 2900
Wire Wire Line
	2400 3000 1850 3000
Wire Wire Line
	1850 3100 2400 3100
Text Label 2400 3000 2    50   ~ 0
A5
Text Label 2400 3100 2    50   ~ 0
A3
Text Label 2400 3200 2    50   ~ 0
A1
Wire Wire Line
	2400 3200 1850 3200
Wire Wire Line
	1100 3750 1350 3750
Wire Wire Line
	1350 4350 1100 4350
Wire Wire Line
	2100 4650 1850 4650
Wire Wire Line
	2100 4750 1850 4750
Wire Wire Line
	2100 4850 1850 4850
Wire Wire Line
	2100 4350 1850 4350
Wire Wire Line
	2100 4450 1850 4450
Wire Wire Line
	1850 4550 2100 4550
Wire Wire Line
	2100 5450 1850 5450
Wire Wire Line
	1850 3950 2100 3950
Wire Wire Line
	1850 3850 2100 3850
Wire Wire Line
	1850 3750 2100 3750
$Comp
L 74xx:74HC14 U3
U 2 1 5E061F57
P 4750 6700
F 0 "U3" H 4750 7017 50  0000 C CNN
F 1 "74HC14" H 4750 6926 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 4750 6700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 4750 6700 50  0001 C CNN
	2    4750 6700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC14 U3
U 3 1 5E061F5D
P 5350 6700
F 0 "U3" H 5350 7017 50  0000 C CNN
F 1 "74HC14" H 5350 6926 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 5350 6700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 5350 6700 50  0001 C CNN
	3    5350 6700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R13
U 1 1 5E061F63
P 4700 7100
F 0 "R13" V 4493 7100 50  0000 C CNN
F 1 "5.1k" V 4584 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 4630 7100 50  0001 C CNN
F 3 "~" H 4700 7100 50  0001 C CNN
	1    4700 7100
	0    1    1    0   
$EndComp
$Comp
L Device:R R14
U 1 1 5E061F69
P 5350 7100
F 0 "R14" V 5143 7100 50  0000 C CNN
F 1 "5.1k" V 5234 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 5280 7100 50  0001 C CNN
F 3 "~" H 5350 7100 50  0001 C CNN
	1    5350 7100
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5E061F6F
P 5050 6300
F 0 "Y1" H 5050 6568 50  0000 C CNN
F 1 "Crystal" H 5050 6477 50  0000 C CNN
F 2 "Crystal:Crystal_HC49-4H_Vertical" H 5050 6300 50  0001 C CNN
F 3 "~" H 5050 6300 50  0001 C CNN
	1    5050 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 6300 4450 6300
Wire Wire Line
	4450 6300 4450 6700
Wire Wire Line
	4450 6700 4450 7100
Wire Wire Line
	4450 7100 4550 7100
Connection ~ 4450 6700
Wire Wire Line
	4850 7100 5050 7100
Wire Wire Line
	5050 7100 5050 6700
Connection ~ 5050 6700
Wire Wire Line
	5050 7100 5200 7100
Connection ~ 5050 7100
Wire Wire Line
	5500 7100 5650 7100
Wire Wire Line
	5650 7100 5650 6700
Wire Wire Line
	5650 6700 5650 6300
Wire Wire Line
	5650 6300 5200 6300
Connection ~ 5650 6700
Text GLabel 5900 6700 2    50   Output ~ 0
CLK
Text GLabel 2100 5150 2    50   Input ~ 0
OP2
Text GLabel 2100 5250 2    50   Input ~ 0
OP3
Text GLabel 2100 5350 2    50   Output ~ 0
IP4
Text GLabel 2100 5450 2    50   Output ~ 0
IP5
Wire Wire Line
	2100 5150 1850 5150
Wire Wire Line
	2100 5250 1850 5250
Wire Wire Line
	1850 5350 2100 5350
Wire Wire Line
	1350 3850 1100 3850
Wire Wire Line
	1100 3950 1350 3950
Wire Wire Line
	1350 4050 1100 4050
Text GLabel 1100 3950 0    50   Output ~ 0
BGACK
Text GLabel 1100 3850 0    50   Input ~ 0
BG
Text GLabel 1100 4050 0    50   Output ~ 0
BR
Wire Wire Line
	5650 6700 5900 6700
Text GLabel 1100 4250 0    50   BiDi ~ 0
RESET
Text GLabel 1100 4150 0    50   BiDi ~ 0
HALT
Wire Wire Line
	1100 4150 1350 4150
Wire Wire Line
	1100 4250 1350 4250
NoConn ~ 1350 4450
Text GLabel 1100 5450 0    50   Output ~ 0
CLK
Wire Wire Line
	1100 4650 1350 4650
Text GLabel 1100 4650 0    50   Input ~ 0
0SEL
Wire Wire Line
	1100 4550 1350 4550
Wire Wire Line
	1350 4750 1100 4750
Wire Wire Line
	1100 4850 1350 4850
Wire Wire Line
	1350 4950 1100 4950
Wire Wire Line
	1100 5050 1350 5050
Wire Wire Line
	1350 5150 1100 5150
Wire Wire Line
	1100 5250 1350 5250
Wire Wire Line
	1350 5350 1100 5350
Text GLabel 1100 5250 0    50   Input ~ 0
6SEL
Text GLabel 1100 5150 0    50   Input ~ 0
5SEL
Text GLabel 1100 5050 0    50   Input ~ 0
4SEL
Text GLabel 1100 4950 0    50   Input ~ 0
3SEL
Text GLabel 1100 4850 0    50   Input ~ 0
2SEL
Text GLabel 1100 4550 0    50   Input ~ 0
ROMSEL
Text GLabel 1100 4750 0    50   Input ~ 0
RAMSEL
Text GLabel 1100 5350 0    50   Input ~ 0
IOSEL
Wire Wire Line
	1100 5450 1350 5450
Wire Wire Line
	2100 4250 1850 4250
Wire Wire Line
	1850 4150 2100 4150
Text GLabel 2100 4250 2    50   Input ~ 0
WR
Text GLabel 2100 4150 2    50   Input ~ 0
RD
Wire Wire Line
	2300 4050 1850 4050
Text GLabel 2300 4050 2    50   Input ~ 0
R~W
Text GLabel 2100 4950 2    50   Input ~ 0
IACK
Wire Wire Line
	2100 4950 1850 4950
Wire Wire Line
	4900 950  5000 950 
Wire Wire Line
	5000 950  5000 1250
Wire Wire Line
	5000 1250 5050 1250
Wire Wire Line
	2650 6700 2450 6700
Wire Wire Line
	2650 6450 2650 6700
Connection ~ 2650 6700
Wire Wire Line
	2650 6700 2650 6950
Text GLabel 3350 6950 2    50   BiDi ~ 0
RESET
Text GLabel 3350 6450 2    50   BiDi ~ 0
HALT
Wire Wire Line
	3250 6950 3350 6950
Wire Wire Line
	3350 6450 3250 6450
$Comp
L Switch:SW_Push SW1
U 1 1 5F9018F9
P 1400 6950
F 0 "SW1" V 1450 7150 50  0000 C CNN
F 1 "RESET" V 1350 7150 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 1400 7150 50  0001 C CNN
F 3 "~" H 1400 7150 50  0001 C CNN
	1    1400 6950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1850 6700 1650 6700
Wire Wire Line
	1650 6700 1650 6850
$Comp
L Device:R_Small R12
U 1 1 5F901901
P 1650 6500
F 0 "R12" H 1709 6546 50  0000 L CNN
F 1 "10k" H 1709 6455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" H 1650 6500 50  0001 C CNN
F 3 "~" H 1650 6500 50  0001 C CNN
	1    1650 6500
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C15
U 1 1 5F901907
P 1650 6950
F 0 "C15" H 1742 6996 50  0000 L CNN
F 1 "0.47uF" H 1742 6905 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 1650 6950 50  0001 C CNN
F 3 "~" H 1650 6950 50  0001 C CNN
	1    1650 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 6700 1650 6600
Connection ~ 1650 6700
Wire Wire Line
	1650 7050 1650 7200
Wire Wire Line
	1650 6300 1650 6400
Wire Wire Line
	1400 6750 1400 6700
Wire Wire Line
	1400 6700 1650 6700
Wire Wire Line
	1400 7150 1400 7200
Wire Wire Line
	1400 7200 1650 7200
$Comp
L power:+5V #PWR019
U 1 1 5F94ADB6
P 1650 6300
F 0 "#PWR019" H 1650 6150 50  0001 C CNN
F 1 "+5V" H 1665 6473 50  0000 C CNN
F 2 "" H 1650 6300 50  0001 C CNN
F 3 "" H 1650 6300 50  0001 C CNN
	1    1650 6300
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC14 U3
U 6 1 5F9716D8
P 2150 6700
F 0 "U3" H 2150 7017 50  0000 C CNN
F 1 "74HC14" H 2150 6926 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 2150 6700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 2150 6700 50  0001 C CNN
	6    2150 6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5F92861D
P 1650 7300
F 0 "#PWR020" H 1650 7050 50  0001 C CNN
F 1 "GND" H 1655 7127 50  0000 C CNN
F 2 "" H 1650 7300 50  0001 C CNN
F 3 "" H 1650 7300 50  0001 C CNN
	1    1650 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 7300 1650 7200
Connection ~ 1650 7200
$Comp
L 74xx:74LS05 U?
U 2 1 5F8D1FA5
P 2950 6950
AR Path="/5E081AED/5F8D1FA5" Ref="U?"  Part="1" 
AR Path="/5F8D1FA5" Ref="U4"  Part="2" 
F 0 "U4" H 2950 7267 50  0000 C CNN
F 1 "74HC05" H 2950 7176 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 2950 6950 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 2950 6950 50  0001 C CNN
	2    2950 6950
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS05 U?
U 1 1 5F8D1FAB
P 2950 6450
AR Path="/5E081AED/5F8D1FAB" Ref="U?"  Part="1" 
AR Path="/5F8D1FAB" Ref="U4"  Part="1" 
F 0 "U4" H 2950 6767 50  0000 C CNN
F 1 "74HC05" H 2950 6676 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 2950 6450 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 2950 6450 50  0001 C CNN
	1    2950 6450
	1    0    0    -1  
$EndComp
NoConn ~ 1850 5050
Wire Wire Line
	9600 4500 9400 4500
Connection ~ 9600 4500
Wire Wire Line
	9600 4300 9800 4300
Connection ~ 9600 4300
Wire Wire Line
	9600 6200 10100 6200
Connection ~ 9600 6200
Wire Wire Line
	9600 5200 10100 5200
Connection ~ 9600 5200
Connection ~ 9100 6200
Connection ~ 9100 5200
Wire Wire Line
	8600 6200 9100 6200
Wire Wire Line
	8600 5200 9100 5200
Connection ~ 9000 4500
Wire Wire Line
	8600 4500 9000 4500
Connection ~ 9000 4300
Wire Wire Line
	8600 4300 9000 4300
$Comp
L Device:C_Small C9
U 1 1 608FEF9B
P 8600 4400
F 0 "C9" H 8692 4446 50  0000 L CNN
F 1 "0.1uF" H 8692 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8600 4400 50  0001 C CNN
F 3 "~" H 8600 4400 50  0001 C CNN
	1    8600 4400
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS20 U?
U 3 1 60527AC9
P 10600 5700
AR Path="/5E081AED/60527AC9" Ref="U?"  Part="1" 
AR Path="/60527AC9" Ref="U6"  Part="3" 
F 0 "U6" H 10700 6050 50  0000 C CNN
F 1 "74HC20" H 10600 5700 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 10600 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS20" H 10600 5700 50  0001 C CNN
	3    10600 5700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS05 U?
U 7 1 604FF595
P 9100 5700
AR Path="/5E081AED/604FF595" Ref="U?"  Part="1" 
AR Path="/604FF595" Ref="U4"  Part="7" 
F 0 "U4" H 9150 6050 50  0000 L CNN
F 1 "74HC05" H 8950 5700 50  0000 L CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 9100 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 9100 5700 50  0001 C CNN
	7    9100 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 5200 9600 5100
Wire Wire Line
	9600 6200 9600 6300
$Comp
L power:GND #PWR018
U 1 1 5E36DFFC
P 9600 6300
F 0 "#PWR018" H 9600 6050 50  0001 C CNN
F 1 "GND" H 9605 6127 50  0000 C CNN
F 2 "" H 9600 6300 50  0001 C CNN
F 3 "" H 9600 6300 50  0001 C CNN
	1    9600 6300
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS32 U5
U 5 1 5E3CE265
P 9600 5700
AR Path="/5E3CE265" Ref="U5"  Part="5" 
AR Path="/5E081AED/5E3CE265" Ref="U?"  Part="2" 
F 0 "U5" H 9750 6050 50  0000 C CNN
F 1 "74HC32" H 9600 5700 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 9600 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 9600 5700 50  0001 C CNN
	5    9600 5700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC14 U3
U 7 1 5E377C41
P 8600 5700
F 0 "U3" H 8750 6050 50  0000 C CNN
F 1 "74HC14" H 8600 5700 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 8600 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 8600 5700 50  0001 C CNN
	7    8600 5700
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR013
U 1 1 5E364C1A
P 9600 5100
F 0 "#PWR013" H 9600 4950 50  0001 C CNN
F 1 "+5V" H 9615 5273 50  0000 C CNN
F 2 "" H 9600 5100 50  0001 C CNN
F 3 "" H 9600 5100 50  0001 C CNN
	1    9600 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 3550 9000 3550
Connection ~ 9400 3550
Connection ~ 9000 3550
Connection ~ 9800 3550
Wire Wire Line
	9800 3550 9600 3550
Wire Wire Line
	9400 3350 9600 3350
Connection ~ 9400 3350
Connection ~ 9800 3350
Connection ~ 9000 3350
Wire Wire Line
	9000 3350 9400 3350
Wire Wire Line
	9600 3550 9600 3650
$Comp
L power:GND #PWR08
U 1 1 5E2DDA60
P 9600 3650
F 0 "#PWR08" H 9600 3400 50  0001 C CNN
F 1 "GND" H 9605 3477 50  0000 C CNN
F 2 "" H 9600 3650 50  0001 C CNN
F 3 "" H 9600 3650 50  0001 C CNN
	1    9600 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 3250 9600 3350
$Comp
L power:+5V #PWR06
U 1 1 5E2D3F35
P 9600 3250
F 0 "#PWR06" H 9600 3100 50  0001 C CNN
F 1 "+5V" H 9615 3423 50  0000 C CNN
F 2 "" H 9600 3250 50  0001 C CNN
F 3 "" H 9600 3250 50  0001 C CNN
	1    9600 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C7
U 1 1 5E2CACDE
P 9800 3450
F 0 "C7" H 9892 3496 50  0000 L CNN
F 1 "0.1uF" H 9892 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9800 3450 50  0001 C CNN
F 3 "~" H 9800 3450 50  0001 C CNN
	1    9800 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C6
U 1 1 5E2CACD4
P 9400 3450
F 0 "C6" H 9492 3496 50  0000 L CNN
F 1 "0.1uF" H 9492 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9400 3450 50  0001 C CNN
F 3 "~" H 9400 3450 50  0001 C CNN
	1    9400 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 5E2CACCA
P 9000 3450
F 0 "C5" H 9092 3496 50  0000 L CNN
F 1 "0.1uF" H 9092 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9000 3450 50  0001 C CNN
F 3 "~" H 9000 3450 50  0001 C CNN
	1    9000 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 4500 9600 4600
Wire Wire Line
	9600 4200 9600 4300
$Comp
L power:+5V #PWR011
U 1 1 5E29DD32
P 9600 4200
F 0 "#PWR011" H 9600 4050 50  0001 C CNN
F 1 "+5V" H 9615 4373 50  0000 C CNN
F 2 "" H 9600 4200 50  0001 C CNN
F 3 "" H 9600 4200 50  0001 C CNN
	1    9600 4200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5E29BA33
P 9600 4600
F 0 "#PWR012" H 9600 4350 50  0001 C CNN
F 1 "GND" H 9605 4427 50  0000 C CNN
F 2 "" H 9600 4600 50  0001 C CNN
F 3 "" H 9600 4600 50  0001 C CNN
	1    9600 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 4500 10200 4500
Wire Wire Line
	10200 4500 9800 4500
Connection ~ 10200 4500
Wire Wire Line
	9800 4500 9600 4500
Connection ~ 9800 4500
Wire Wire Line
	9400 4500 9000 4500
Connection ~ 9400 4500
Wire Wire Line
	9000 3550 8600 3550
Wire Wire Line
	10200 3550 9800 3550
Wire Wire Line
	9800 3350 10200 3350
Wire Wire Line
	10200 4300 10600 4300
Connection ~ 10200 4300
Wire Wire Line
	9800 4300 10200 4300
Connection ~ 9800 4300
Wire Wire Line
	9400 4300 9600 4300
Connection ~ 9400 4300
Wire Wire Line
	9000 4300 9400 4300
Wire Wire Line
	8600 3350 9000 3350
$Comp
L Device:C_Small C4
U 1 1 5E28F415
P 8600 3450
F 0 "C4" H 8692 3496 50  0000 L CNN
F 1 "0.1uF" H 8692 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8600 3450 50  0001 C CNN
F 3 "~" H 8600 3450 50  0001 C CNN
	1    8600 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C8
U 1 1 5E28F0FE
P 10200 3450
F 0 "C8" H 10292 3496 50  0000 L CNN
F 1 "0.1uF" H 10292 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10200 3450 50  0001 C CNN
F 3 "~" H 10200 3450 50  0001 C CNN
	1    10200 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C14
U 1 1 5E28EE63
P 10600 4400
F 0 "C14" H 10692 4446 50  0000 L CNN
F 1 "0.1uF" H 10692 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10600 4400 50  0001 C CNN
F 3 "~" H 10600 4400 50  0001 C CNN
	1    10600 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C13
U 1 1 5E28EB9E
P 10200 4400
F 0 "C13" H 10292 4446 50  0000 L CNN
F 1 "0.1uF" H 10292 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10200 4400 50  0001 C CNN
F 3 "~" H 10200 4400 50  0001 C CNN
	1    10200 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C12
U 1 1 5E28E898
P 9800 4400
F 0 "C12" H 9892 4446 50  0000 L CNN
F 1 "0.1uF" H 9892 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9800 4400 50  0001 C CNN
F 3 "~" H 9800 4400 50  0001 C CNN
	1    9800 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 5E28E56D
P 9400 4400
F 0 "C11" H 9492 4446 50  0000 L CNN
F 1 "0.1uF" H 9492 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9400 4400 50  0001 C CNN
F 3 "~" H 9400 4400 50  0001 C CNN
	1    9400 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C10
U 1 1 5E2883BA
P 9000 4400
F 0 "C10" H 9092 4446 50  0000 L CNN
F 1 "0.1uF" H 9092 4355 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9000 4400 50  0001 C CNN
F 3 "~" H 9000 4400 50  0001 C CNN
	1    9000 4400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR03
U 1 1 5DFE16FD
P 9600 2200
F 0 "#PWR03" H 9600 2050 50  0001 C CNN
F 1 "+5V" H 9615 2373 50  0000 C CNN
F 2 "" H 9600 2200 50  0001 C CNN
F 3 "" H 9600 2200 50  0001 C CNN
	1    9600 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 2650 9600 2550
$Comp
L power:GND #PWR04
U 1 1 5DFD65E5
P 9600 2650
F 0 "#PWR04" H 9600 2400 50  0001 C CNN
F 1 "GND" H 9605 2477 50  0000 C CNN
F 2 "" H 9600 2650 50  0001 C CNN
F 3 "" H 9600 2650 50  0001 C CNN
	1    9600 2650
	1    0    0    -1  
$EndComp
$Sheet
S 8650 750  500  500 
U 5E081AED
F0 "Memory" 50
F1 "Memory.sch" 50
$EndSheet
$Sheet
S 9450 750  500  500 
U 5E114B4D
F0 "UART" 50
F1 "UART.sch" 50
$EndSheet
$Comp
L Mechanical:MountingHole H1
U 1 1 5F398330
P 9050 1650
F 0 "H1" H 9150 1696 50  0000 L CNN
F 1 "." H 9150 1605 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_Pad_Via" H 9050 1650 50  0001 C CNN
F 3 "~" H 9050 1650 50  0001 C CNN
	1    9050 1650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5F3B492E
P 9400 1650
F 0 "H2" H 9500 1696 50  0000 L CNN
F 1 "." H 9500 1605 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_Pad_Via" H 9400 1650 50  0001 C CNN
F 3 "~" H 9400 1650 50  0001 C CNN
	1    9400 1650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5F3B4B6A
P 9750 1650
F 0 "H3" H 9850 1696 50  0000 L CNN
F 1 "." H 9850 1605 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_Pad_Via" H 9750 1650 50  0001 C CNN
F 3 "~" H 9750 1650 50  0001 C CNN
	1    9750 1650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5F3B4D93
P 10100 1650
F 0 "H4" H 10200 1696 50  0000 L CNN
F 1 "." H 10200 1605 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_Pad_Via" H 10100 1650 50  0001 C CNN
F 3 "~" H 10100 1650 50  0001 C CNN
	1    10100 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 6050 5950 6050
Connection ~ 5950 6050
Wire Wire Line
	5950 6050 6050 6050
Connection ~ 6050 6050
Wire Wire Line
	6050 6050 6150 6050
Connection ~ 6150 6050
Wire Wire Line
	6150 6050 6250 6050
Wire Wire Line
	5600 1000 5800 1000
Wire Wire Line
	5800 1000 5800 850 
Wire Wire Line
	5800 850  5950 850 
Connection ~ 5950 850 
Wire Wire Line
	5950 850  6150 850 
$Sheet
S 10300 750  500  500 
U 5FAF94FC
F0 "CompactFlash" 50
F1 "CompactFlash.sch" 50
$EndSheet
$Comp
L Device:CP1 C2
U 1 1 5DFCA67C
P 9600 2400
F 0 "C2" H 9715 2446 50  0000 L CNN
F 1 "470uF" H 9715 2355 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 9600 2400 50  0001 C CNN
F 3 "~" H 9600 2400 50  0001 C CNN
	1    9600 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 2250 9600 2200
Wire Wire Line
	3350 3950 3900 3950
Wire Wire Line
	5000 5550 5000 5600
Wire Wire Line
	5000 5350 5000 5300
$Comp
L Device:R_Small R11
U 1 1 5E16B4C0
P 5000 5450
F 0 "R11" H 5059 5496 50  0000 L CNN
F 1 "1k" H 5059 5405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" H 5000 5450 50  0001 C CNN
F 3 "~" H 5000 5450 50  0001 C CNN
	1    5000 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4650 5000 5000
$Comp
L Device:LED D1
U 1 1 5E14D5B7
P 5000 5150
F 0 "D1" V 5039 5033 50  0000 R CNN
F 1 "HALT" V 4948 5033 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm" H 5000 5150 50  0001 C CNN
F 3 "~" H 5000 5150 50  0001 C CNN
	1    5000 5150
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5E010946
P 5000 5600
F 0 "#PWR014" H 5000 5350 50  0001 C CNN
F 1 "GND" H 5005 5427 50  0000 C CNN
F 2 "" H 5000 5600 50  0001 C CNN
F 3 "" H 5000 5600 50  0001 C CNN
	1    5000 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3450 5000 3550
$Comp
L power:+5V #PWR07
U 1 1 5DFA40A2
P 5000 3450
F 0 "#PWR07" H 5000 3300 50  0001 C CNN
F 1 "+5V" H 5015 3623 50  0000 C CNN
F 2 "" H 5000 3450 50  0001 C CNN
F 3 "" H 5000 3450 50  0001 C CNN
	1    5000 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3750 5000 4650
$Comp
L Device:R_Small R9
U 1 1 5DFA3382
P 5000 3650
F 0 "R9" H 5059 3696 50  0000 L CNN
F 1 "1k" H 5059 3605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" H 5000 3650 50  0001 C CNN
F 3 "~" H 5000 3650 50  0001 C CNN
	1    5000 3650
	1    0    0    -1  
$EndComp
Connection ~ 5000 4650
Wire Wire Line
	5000 4650 5050 4650
Wire Wire Line
	4800 3100 4800 3150
Wire Wire Line
	4800 3150 5050 3150
$Comp
L power:+5V #PWR045
U 1 1 5FCCE5B4
P 3500 1000
F 0 "#PWR045" H 3500 850 50  0001 C CNN
F 1 "+5V" H 3515 1173 50  0000 C CNN
F 2 "" H 3500 1000 50  0001 C CNN
F 3 "" H 3500 1000 50  0001 C CNN
	1    3500 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 4650 5000 4650
Wire Wire Line
	3250 1550 4200 1550
Wire Wire Line
	3250 1650 4100 1650
Wire Wire Line
	3250 1750 4000 1750
Wire Wire Line
	3350 4850 3800 4850
Wire Wire Line
	3350 1950 3700 1950
Wire Wire Line
	3350 2150 3600 2150
Wire Wire Line
	3350 2050 5050 2050
Wire Wire Line
	3350 4150 3500 4150
Wire Wire Line
	3500 1450 3500 4150
Connection ~ 3500 4150
Wire Wire Line
	3500 4150 5050 4150
Wire Wire Line
	3600 1450 3600 2150
Connection ~ 3600 2150
Wire Wire Line
	3600 2150 5050 2150
Wire Wire Line
	3700 1450 3700 1950
Connection ~ 3700 1950
Wire Wire Line
	3700 1950 5050 1950
Wire Wire Line
	3800 1450 3800 4850
Connection ~ 3800 4850
Wire Wire Line
	3800 4850 5050 4850
Wire Wire Line
	3900 1450 3900 3950
Connection ~ 3900 3950
Wire Wire Line
	3900 3950 5050 3950
Wire Wire Line
	4000 1450 4000 1750
Connection ~ 4000 1750
Wire Wire Line
	4000 1750 5050 1750
Wire Wire Line
	4100 1450 4100 1650
Connection ~ 4100 1650
Wire Wire Line
	4100 1650 5050 1650
Wire Wire Line
	4200 1450 4200 1550
Connection ~ 4200 1550
Wire Wire Line
	4200 1550 5050 1550
$Comp
L 74xx:74LS32 U1
U 5 1 5FCCA363
P 10100 5700
AR Path="/5FCCA363" Ref="U1"  Part="5" 
AR Path="/5E081AED/5FCCA363" Ref="U?"  Part="2" 
F 0 "U1" H 10250 6050 50  0000 C CNN
F 1 "74HC32" H 10100 5700 50  0000 C CNN
F 2 "Package_SO:TSSOP-14_4.4x5mm_P0.65mm" H 10100 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 10100 5700 50  0001 C CNN
	5    10100 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 5200 10600 5200
Connection ~ 10100 5200
Wire Wire Line
	10600 6200 10100 6200
Connection ~ 10100 6200
Wire Wire Line
	9100 6200 9600 6200
Wire Wire Line
	9100 5200 9600 5200
$Comp
L Device:C_Small C1
U 1 1 5FD65EC1
P 10600 3450
F 0 "C1" H 10692 3496 50  0000 L CNN
F 1 "0.1uF" H 10692 3405 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10600 3450 50  0001 C CNN
F 3 "~" H 10600 3450 50  0001 C CNN
	1    10600 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 3350 10600 3350
Connection ~ 10200 3350
Wire Wire Line
	10600 3550 10200 3550
Connection ~ 10200 3550
Connection ~ 9600 3350
Wire Wire Line
	9600 3350 9800 3350
Connection ~ 9600 3550
Wire Wire Line
	9600 3550 9400 3550
$Comp
L Device:R_Network08 RN1
U 1 1 5FAF66B2
P 3900 1250
F 0 "RN1" H 4288 1296 50  0000 L CNN
F 1 "10k" H 4288 1205 50  0000 L CNN
F 2 "Resistor_THT:R_Array_SIP9" V 4375 1250 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 3900 1250 50  0001 C CNN
	1    3900 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 1050 3500 1000
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5FC9A346
P 10300 2350
F 0 "J2" H 10380 2342 50  0000 L CNN
F 1 "POWER" H 10380 2251 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10300 2350 50  0001 C CNN
F 3 "~" H 10300 2350 50  0001 C CNN
	1    10300 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 2250 10100 2250
Wire Wire Line
	10100 2250 10100 2350
Connection ~ 9600 2250
Wire Wire Line
	9600 2550 10100 2550
Wire Wire Line
	10100 2550 10100 2450
Wire Bus Line
	1000 750  1000 1900
Wire Bus Line
	2200 750  2200 1900
Wire Bus Line
	2500 650  2500 3100
Wire Bus Line
	700  650  700  3000
Wire Bus Line
	7350 750  7350 5050
Wire Bus Line
	7250 650  7250 3350
Connection ~ 9600 2550
$EndSCHEMATC
