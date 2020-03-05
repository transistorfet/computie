EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CPU_NXP_68000:68000D U2
U 1 1 5DD0E2E5
P 6050 3450
F 0 "U2" H 6050 6031 50  0000 C CNN
F 1 "68000D" H 6050 5940 50  0000 C CNN
F 2 "Package_DIP:DIP-64_W22.86mm_Socket_LongPads" H 6050 3450 50  0001 C CNN
F 3 "https://www.nxp.com/docs/en/reference-manual/MC68000UM.pdf" H 6050 3450 50  0001 C CNN
	1    6050 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5DD14FE8
P 6150 5950
F 0 "#PWR010" H 6150 5700 50  0001 C CNN
F 1 "GND" H 6155 5777 50  0000 C CNN
F 2 "" H 6150 5950 50  0001 C CNN
F 3 "" H 6150 5950 50  0001 C CNN
	1    6150 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 5850 6150 5850
Wire Wire Line
	6150 5850 6150 5950
Connection ~ 6150 5850
$Comp
L power:+5V #PWR09
U 1 1 5DD17080
P 5750 1050
F 0 "#PWR09" H 5750 900 50  0001 C CNN
F 1 "+5V" H 5765 1223 50  0000 C CNN
F 2 "" H 5750 1050 50  0001 C CNN
F 3 "" H 5750 1050 50  0001 C CNN
	1    5750 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 1050 6050 1050
Wire Wire Line
	6150 1050 6050 1050
Connection ~ 6050 1050
Text GLabel 4900 1250 0    50   Input ~ 0
CLK
Wire Wire Line
	4900 1250 5050 1250
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
L 74xx:74HC14 U1
U 6 1 5DEECAAF
P 7450 5950
F 0 "U1" H 7450 6267 50  0000 C CNN
F 1 "74HC14" H 7450 6176 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 7450 5950 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 7450 5950 50  0001 C CNN
	6    7450 5950
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
Text GLabel 4150 4250 0    50   Input ~ 0
DTACK
Text GLabel 4150 4050 0    50   Input ~ 0
BERR
Text GLabel 4150 4750 0    50   BiDi ~ 0
RESET
$Sheet
S 8400 1100 500  500 
U 5E081AED
F0 "Memory" 50
F1 "Memory.sch" 50
$EndSheet
Wire Bus Line
	7250 650  7800 650 
Wire Bus Line
	7350 750  7800 750 
$Sheet
S 9850 1100 500  500 
U 5E114B4D
F0 "UART" 50
F1 "UART.sch" 50
$EndSheet
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
Text GLabel 4150 4650 0    50   BiDi ~ 0
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
Wire Wire Line
	3350 2050 5050 2050
$Comp
L Device:R_Small R12
U 1 1 5DFA3382
P 4750 3750
F 0 "R12" H 4809 3796 50  0000 L CNN
F 1 "1k" H 4809 3705 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4750 3750 50  0001 C CNN
F 3 "~" H 4750 3750 50  0001 C CNN
	1    4750 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R13
U 1 1 5DFA3AEF
P 5000 3750
F 0 "R13" H 5059 3796 50  0000 L CNN
F 1 "10k" H 5059 3705 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 5000 3750 50  0001 C CNN
F 3 "~" H 5000 3750 50  0001 C CNN
	1    5000 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 5DFA40A2
P 4250 3550
F 0 "#PWR06" H 4250 3400 50  0001 C CNN
F 1 "+5V" H 4265 3723 50  0000 C CNN
F 2 "" H 4250 3550 50  0001 C CNN
F 3 "" H 4250 3550 50  0001 C CNN
	1    4250 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3850 4750 4650
Wire Wire Line
	5000 3850 5000 4750
$Comp
L Device:R_Small R10
U 1 1 5DFB897A
P 4450 2950
F 0 "R10" H 4509 2996 50  0000 L CNN
F 1 "10k" H 4509 2905 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4450 2950 50  0001 C CNN
F 3 "~" H 4450 2950 50  0001 C CNN
	1    4450 2950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 5DFB8984
P 4450 2850
F 0 "#PWR07" H 4450 2700 50  0001 C CNN
F 1 "+5V" H 4465 3023 50  0000 C CNN
F 2 "" H 4450 2850 50  0001 C CNN
F 3 "" H 4450 2850 50  0001 C CNN
	1    4450 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3150 5050 3150
$Comp
L Device:R_Small R3
U 1 1 5DFE4CFF
P 3700 1350
F 0 "R3" H 3759 1396 50  0000 L CNN
F 1 "10k" H 3759 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 3700 1350 50  0001 C CNN
F 3 "~" H 3700 1350 50  0001 C CNN
	1    3700 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5DFE4D13
P 3950 1200
F 0 "#PWR05" H 3950 1050 50  0001 C CNN
F 1 "+5V" H 3965 1373 50  0000 C CNN
F 2 "" H 3950 1200 50  0001 C CNN
F 3 "" H 3950 1200 50  0001 C CNN
	1    3950 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 1200 3950 1250
Wire Wire Line
	4200 1450 4200 1950
Wire Wire Line
	4450 1450 4450 2150
NoConn ~ 5050 3050
NoConn ~ 5050 2950
Text GLabel 7800 5550 2    50   Output ~ 0
LDS
Text GLabel 7800 5450 2    50   Output ~ 0
UDS
Text GLabel 7800 5350 2    50   Output ~ 0
AS
Text GLabel 1100 5000 0    50   Output ~ 0
DTACK
Text GLabel 1100 5600 0    50   Output ~ 0
BERR
Text GLabel 2100 5800 2    50   Input ~ 0
FC2
Text GLabel 2100 5700 2    50   Input ~ 0
FC1
Text GLabel 2100 5600 2    50   Input ~ 0
FC0
Text GLabel 2100 5400 2    50   Input ~ 0
RD
Text GLabel 2100 5500 2    50   Input ~ 0
WR
Text GLabel 2300 5300 2    50   Input ~ 0
R~W
Text GLabel 2100 5200 2    50   Input ~ 0
LDS
Text GLabel 2100 5100 2    50   Input ~ 0
UDS
Text GLabel 2100 5000 2    50   Input ~ 0
AS
$Comp
L power:GND #PWR02
U 1 1 5E412D92
P 1250 6850
F 0 "#PWR02" H 1250 6600 50  0001 C CNN
F 1 "GND" H 1255 6677 50  0000 C CNN
F 2 "" H 1250 6850 50  0001 C CNN
F 3 "" H 1250 6850 50  0001 C CNN
	1    1250 6850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5E414403
P 1250 4850
F 0 "#PWR01" H 1250 4700 50  0001 C CNN
F 1 "+5V" H 1265 5023 50  0000 C CNN
F 2 "" H 1250 4850 50  0001 C CNN
F 3 "" H 1250 4850 50  0001 C CNN
	1    1250 4850
	1    0    0    -1  
$EndComp
Text GLabel 3250 1750 0    50   Output ~ 0
IPL2
Text GLabel 3250 1650 0    50   Output ~ 0
IPL1
Text GLabel 3250 1550 0    50   Output ~ 0
IPL0
Wire Wire Line
	3250 1550 3450 1550
Wire Wire Line
	3250 1650 3700 1650
Text GLabel 2100 5900 2    50   Output ~ 0
IPL0
Text GLabel 2100 6000 2    50   Output ~ 0
IPL1
Text GLabel 2100 6100 2    50   Output ~ 0
IPL2
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 5E4E078F
P 1550 2800
F 0 "J1" H 1600 3917 50  0000 C CNN
F 1 "ADBUS" H 1600 3826 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 1550 2800 50  0001 C CNN
F 3 "~" H 1550 2800 50  0001 C CNN
	1    1550 2800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J2
U 1 1 5E4E7079
P 1550 5800
F 0 "J2" H 1600 6917 50  0000 C CNN
F 1 "SIGBUS" H 1600 6826 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 1550 5800 50  0001 C CNN
F 3 "~" H 1550 5800 50  0001 C CNN
	1    1550 5800
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R6
U 1 1 5DFF3D5C
P 4250 3750
F 0 "R6" H 4309 3796 50  0000 L CNN
F 1 "10k" H 4309 3705 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4250 3750 50  0001 C CNN
F 3 "~" H 4250 3750 50  0001 C CNN
	1    4250 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R11
U 1 1 5DFF41F7
P 4500 3750
F 0 "R11" H 4559 3796 50  0000 L CNN
F 1 "10k" H 4559 3705 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4500 3750 50  0001 C CNN
F 3 "~" H 4500 3750 50  0001 C CNN
	1    4500 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3600 4250 3650
Wire Wire Line
	4250 3600 4500 3600
Wire Wire Line
	4250 3600 4250 3550
Connection ~ 4250 3600
Wire Wire Line
	4500 3600 4500 3650
Wire Wire Line
	4500 3600 4750 3600
Wire Wire Line
	4750 3600 4750 3650
Wire Wire Line
	4750 3600 5000 3600
Wire Wire Line
	5000 3600 5000 3650
Connection ~ 4750 3600
Wire Wire Line
	4250 3850 4250 4050
Wire Wire Line
	4500 3850 4500 4250
$Comp
L Connector:Barrel_Jack J3
U 1 1 5DFC7827
P 8650 2300
F 0 "J3" H 8707 2625 50  0000 C CNN
F 1 "Barrel_Jack" H 8707 2534 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 8700 2260 50  0001 C CNN
F 3 "~" H 8700 2260 50  0001 C CNN
	1    8650 2300
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:L7805 U3
U 1 1 5DFC860D
P 9700 2200
F 0 "U3" H 9700 2442 50  0000 C CNN
F 1 "L7805" H 9700 2351 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 9725 2050 50  0001 L CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/41/4f/b3/b0/12/d4/47/88/CD00000444.pdf/files/CD00000444.pdf/jcr:content/translations/en.CD00000444.pdf" H 9700 2150 50  0001 C CNN
	1    9700 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C3
U 1 1 5DFC979F
P 9150 2350
F 0 "C3" H 9265 2396 50  0000 L CNN
F 1 "47uF" H 9265 2305 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D12.5mm_P5.00mm" H 9150 2350 50  0001 C CNN
F 3 "~" H 9150 2350 50  0001 C CNN
	1    9150 2350
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C10
U 1 1 5DFCA67C
P 10200 2350
F 0 "C10" H 10315 2396 50  0000 L CNN
F 1 "47uF" H 10315 2305 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D12.5mm_P5.00mm" H 10200 2350 50  0001 C CNN
F 3 "~" H 10200 2350 50  0001 C CNN
	1    10200 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 2200 9150 2200
Connection ~ 9150 2200
Wire Wire Line
	9150 2200 9400 2200
Wire Wire Line
	8950 2400 8950 2500
Wire Wire Line
	8950 2500 9150 2500
Connection ~ 9150 2500
Wire Wire Line
	9150 2500 9700 2500
Connection ~ 9700 2500
Wire Wire Line
	9700 2500 10200 2500
Wire Wire Line
	10000 2200 10200 2200
$Comp
L power:GND #PWR011
U 1 1 5DFD65E5
P 9700 2600
F 0 "#PWR011" H 9700 2350 50  0001 C CNN
F 1 "GND" H 9705 2427 50  0000 C CNN
F 2 "" H 9700 2600 50  0001 C CNN
F 3 "" H 9700 2600 50  0001 C CNN
	1    9700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 2600 9700 2500
$Comp
L Device:C C13
U 1 1 5DFDEAC5
P 10600 2350
F 0 "C13" H 10715 2396 50  0000 L CNN
F 1 "0.1uF" H 10715 2305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10638 2200 50  0001 C CNN
F 3 "~" H 10600 2350 50  0001 C CNN
	1    10600 2350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR018
U 1 1 5DFE16FD
P 10900 2200
F 0 "#PWR018" H 10900 2050 50  0001 C CNN
F 1 "+5V" H 10915 2373 50  0000 C CNN
F 2 "" H 10900 2200 50  0001 C CNN
F 3 "" H 10900 2200 50  0001 C CNN
	1    10900 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 2200 10600 2200
Connection ~ 10200 2200
Connection ~ 10600 2200
Wire Wire Line
	10600 2200 10900 2200
Wire Wire Line
	10600 2500 10200 2500
Connection ~ 10200 2500
$Comp
L Switch:SW_Push SW1
U 1 1 5E00A64E
P 4800 5400
F 0 "SW1" H 4800 5685 50  0000 C CNN
F 1 "RESET" H 4800 5594 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4800 5600 50  0001 C CNN
F 3 "~" H 4800 5600 50  0001 C CNN
	1    4800 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4750 5000 5400
$Comp
L power:GND #PWR08
U 1 1 5E010946
P 4600 5500
F 0 "#PWR08" H 4600 5250 50  0001 C CNN
F 1 "GND" H 4605 5327 50  0000 C CNN
F 2 "" H 4600 5500 50  0001 C CNN
F 3 "" H 4600 5500 50  0001 C CNN
	1    4600 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 5500 4600 5400
$Comp
L Device:R_Small R1
U 1 1 5E0305DA
P 3450 1350
F 0 "R1" H 3509 1396 50  0000 L CNN
F 1 "10k" H 3509 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 3450 1350 50  0001 C CNN
F 3 "~" H 3450 1350 50  0001 C CNN
	1    3450 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R9
U 1 1 5E030A4A
P 4450 1350
F 0 "R9" H 4509 1396 50  0000 L CNN
F 1 "10k" H 4509 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4450 1350 50  0001 C CNN
F 3 "~" H 4450 1350 50  0001 C CNN
	1    4450 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R5
U 1 1 5E030E00
P 4200 1350
F 0 "R5" H 4259 1396 50  0000 L CNN
F 1 "10k" H 4259 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4200 1350 50  0001 C CNN
F 3 "~" H 4200 1350 50  0001 C CNN
	1    4200 1350
	1    0    0    -1  
$EndComp
Connection ~ 3950 1250
Wire Wire Line
	3450 1450 3450 1550
Wire Wire Line
	3700 1450 3700 1650
Wire Wire Line
	4150 4050 4250 4050
Wire Wire Line
	4150 4250 4500 4250
Connection ~ 4750 4650
Wire Wire Line
	4750 4650 5050 4650
Connection ~ 5000 4750
Wire Wire Line
	5000 4750 5050 4750
Wire Wire Line
	4150 4650 4300 4650
Connection ~ 4500 3600
Connection ~ 4250 4050
Wire Wire Line
	4250 4050 5050 4050
Connection ~ 4500 4250
Wire Wire Line
	4500 4250 5050 4250
Wire Wire Line
	4150 4750 5000 4750
$Comp
L Device:LED D1
U 1 1 5E14D5B7
P 4300 4950
F 0 "D1" V 4339 4833 50  0000 R CNN
F 1 "HALT" V 4248 4833 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm" H 4300 4950 50  0001 C CNN
F 3 "~" H 4300 4950 50  0001 C CNN
	1    4300 4950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4300 4650 4300 4800
Connection ~ 4300 4650
Wire Wire Line
	4300 4650 4750 4650
Wire Wire Line
	4300 5350 4300 5400
Wire Wire Line
	4300 5400 4600 5400
$Comp
L Device:R_Small R7
U 1 1 5E16B4C0
P 4300 5250
F 0 "R7" H 4359 5296 50  0000 L CNN
F 1 "1k" H 4359 5205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 4300 5250 50  0001 C CNN
F 3 "~" H 4300 5250 50  0001 C CNN
	1    4300 5250
	1    0    0    -1  
$EndComp
Connection ~ 4600 5400
Wire Wire Line
	4300 5150 4300 5100
Wire Wire Line
	3350 2150 4450 2150
Wire Wire Line
	3350 1950 4200 1950
Wire Wire Line
	3450 1250 3700 1250
Connection ~ 3450 1550
Connection ~ 3700 1250
Wire Wire Line
	3700 1250 3950 1250
Connection ~ 3700 1650
Wire Wire Line
	3950 1250 4200 1250
Wire Wire Line
	3700 1650 5050 1650
Wire Wire Line
	3250 1750 3950 1750
Wire Wire Line
	3450 1550 5050 1550
$Comp
L Device:R_Small R4
U 1 1 5DFE4D09
P 3950 1350
F 0 "R4" H 4009 1396 50  0000 L CNN
F 1 "10k" H 4009 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" H 3950 1350 50  0001 C CNN
F 3 "~" H 3950 1350 50  0001 C CNN
	1    3950 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 1450 3950 1750
Connection ~ 3950 1750
Wire Wire Line
	3950 1750 5050 1750
Connection ~ 4200 1250
Wire Wire Line
	4200 1250 4450 1250
Connection ~ 4200 1950
Wire Wire Line
	4200 1950 5050 1950
Connection ~ 4450 2150
Wire Wire Line
	4450 2150 5050 2150
Wire Wire Line
	4450 3050 4450 3150
$Comp
L Device:C_Small C2
U 1 1 5E2883BA
P 8900 4300
F 0 "C2" H 8992 4346 50  0000 L CNN
F 1 "0.1uF" H 8992 4255 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8900 4300 50  0001 C CNN
F 3 "~" H 8900 4300 50  0001 C CNN
	1    8900 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 5E28E56D
P 9300 4300
F 0 "C5" H 9392 4346 50  0000 L CNN
F 1 "0.1uF" H 9392 4255 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9300 4300 50  0001 C CNN
F 3 "~" H 9300 4300 50  0001 C CNN
	1    9300 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C7
U 1 1 5E28E898
P 9700 4300
F 0 "C7" H 9792 4346 50  0000 L CNN
F 1 "0.1uF" H 9792 4255 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9700 4300 50  0001 C CNN
F 3 "~" H 9700 4300 50  0001 C CNN
	1    9700 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C9
U 1 1 5E28EB9E
P 10100 4300
F 0 "C9" H 10192 4346 50  0000 L CNN
F 1 "0.1uF" H 10192 4255 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10100 4300 50  0001 C CNN
F 3 "~" H 10100 4300 50  0001 C CNN
	1    10100 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C12
U 1 1 5E28EE63
P 10500 4300
F 0 "C12" H 10592 4346 50  0000 L CNN
F 1 "0.1uF" H 10592 4255 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10500 4300 50  0001 C CNN
F 3 "~" H 10500 4300 50  0001 C CNN
	1    10500 4300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 5E28F0FE
P 10500 3350
F 0 "C11" H 10592 3396 50  0000 L CNN
F 1 "0.1uF" H 10592 3305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10500 3350 50  0001 C CNN
F 3 "~" H 10500 3350 50  0001 C CNN
	1    10500 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5E28F415
P 8900 3350
F 0 "C1" H 8992 3396 50  0000 L CNN
F 1 "0.1uF" H 8992 3305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 8900 3350 50  0001 C CNN
F 3 "~" H 8900 3350 50  0001 C CNN
	1    8900 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 3250 9300 3250
Wire Wire Line
	8900 4200 9300 4200
Connection ~ 9300 4200
Wire Wire Line
	9300 4200 9700 4200
Connection ~ 9700 4200
Wire Wire Line
	9700 4200 10100 4200
Connection ~ 10100 4200
Wire Wire Line
	10100 4200 10500 4200
Wire Wire Line
	10100 3250 10500 3250
Wire Wire Line
	10500 3450 10100 3450
Wire Wire Line
	9300 3450 8900 3450
Connection ~ 9300 4400
Wire Wire Line
	9300 4400 8900 4400
Connection ~ 9700 4400
Wire Wire Line
	9700 4400 9300 4400
Connection ~ 10100 4400
Wire Wire Line
	10100 4400 9700 4400
Wire Wire Line
	10500 4400 10100 4400
$Comp
L power:GND #PWR015
U 1 1 5E29BA33
P 9700 4500
F 0 "#PWR015" H 9700 4250 50  0001 C CNN
F 1 "GND" H 9705 4327 50  0000 C CNN
F 2 "" H 9700 4500 50  0001 C CNN
F 3 "" H 9700 4500 50  0001 C CNN
	1    9700 4500
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR014
U 1 1 5E29DD32
P 9700 4100
F 0 "#PWR014" H 9700 3950 50  0001 C CNN
F 1 "+5V" H 9715 4273 50  0000 C CNN
F 2 "" H 9700 4100 50  0001 C CNN
F 3 "" H 9700 4100 50  0001 C CNN
	1    9700 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 4100 9700 4200
Wire Wire Line
	9700 4400 9700 4500
$Comp
L Device:C_Small C4
U 1 1 5E2CACCA
P 9300 3350
F 0 "C4" H 9392 3396 50  0000 L CNN
F 1 "0.1uF" H 9392 3305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9300 3350 50  0001 C CNN
F 3 "~" H 9300 3350 50  0001 C CNN
	1    9300 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C6
U 1 1 5E2CACD4
P 9700 3350
F 0 "C6" H 9792 3396 50  0000 L CNN
F 1 "0.1uF" H 9792 3305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 9700 3350 50  0001 C CNN
F 3 "~" H 9700 3350 50  0001 C CNN
	1    9700 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C8
U 1 1 5E2CACDE
P 10100 3350
F 0 "C8" H 10192 3396 50  0000 L CNN
F 1 "0.1uF" H 10192 3305 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 10100 3350 50  0001 C CNN
F 3 "~" H 10100 3350 50  0001 C CNN
	1    10100 3350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR012
U 1 1 5E2D3F35
P 9700 3150
F 0 "#PWR012" H 9700 3000 50  0001 C CNN
F 1 "+5V" H 9715 3323 50  0000 C CNN
F 2 "" H 9700 3150 50  0001 C CNN
F 3 "" H 9700 3150 50  0001 C CNN
	1    9700 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3150 9700 3250
$Comp
L power:GND #PWR013
U 1 1 5E2DDA60
P 9700 3550
F 0 "#PWR013" H 9700 3300 50  0001 C CNN
F 1 "GND" H 9705 3377 50  0000 C CNN
F 2 "" H 9700 3550 50  0001 C CNN
F 3 "" H 9700 3550 50  0001 C CNN
	1    9700 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3450 9700 3550
Wire Wire Line
	9300 3250 9700 3250
Connection ~ 9300 3250
Connection ~ 10100 3250
Connection ~ 9700 3250
Wire Wire Line
	9700 3250 10100 3250
Wire Wire Line
	10100 3450 9700 3450
Connection ~ 10100 3450
Connection ~ 9300 3450
Connection ~ 9700 3450
Wire Wire Line
	9700 3450 9300 3450
$Comp
L power:+5V #PWR016
U 1 1 5E364C1A
P 9700 5050
F 0 "#PWR016" H 9700 4900 50  0001 C CNN
F 1 "+5V" H 9715 5223 50  0000 C CNN
F 2 "" H 9700 5050 50  0001 C CNN
F 3 "" H 9700 5050 50  0001 C CNN
	1    9700 5050
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC14 U1
U 7 1 5E377C41
P 9200 5650
F 0 "U1" H 9200 5967 50  0000 C CNN
F 1 "74HC14" H 9200 5876 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 9200 5650 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 9200 5650 50  0001 C CNN
	7    9200 5650
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS32 U4
U 5 1 5E3CE265
P 9700 5650
AR Path="/5E3CE265" Ref="U4"  Part="5" 
AR Path="/5E081AED/5E3CE265" Ref="U?"  Part="2" 
F 0 "U4" H 9700 5975 50  0000 C CNN
F 1 "74LS32" H 9700 5884 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 9700 5650 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 9700 5650 50  0001 C CNN
	5    9700 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5E36DFFC
P 9700 6250
F 0 "#PWR017" H 9700 6000 50  0001 C CNN
F 1 "GND" H 9705 6077 50  0000 C CNN
F 2 "" H 9700 6250 50  0001 C CNN
F 3 "" H 9700 6250 50  0001 C CNN
	1    9700 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 6150 9700 6250
Wire Wire Line
	9200 5150 9700 5150
Connection ~ 9700 5150
Wire Wire Line
	9700 5150 10200 5150
Wire Wire Line
	9700 5150 9700 5050
Wire Wire Line
	9200 6150 9700 6150
Connection ~ 9700 6150
Wire Wire Line
	9700 6150 10200 6150
$Comp
L power:+5V #PWR03
U 1 1 5E473AD7
P 1950 4850
F 0 "#PWR03" H 1950 4700 50  0001 C CNN
F 1 "+5V" H 1965 5023 50  0000 C CNN
F 2 "" H 1950 4850 50  0001 C CNN
F 3 "" H 1950 4850 50  0001 C CNN
	1    1950 4850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5E496B92
P 1950 6850
F 0 "#PWR04" H 1950 6600 50  0001 C CNN
F 1 "GND" H 1955 6677 50  0000 C CNN
F 2 "" H 1950 6850 50  0001 C CNN
F 3 "" H 1950 6850 50  0001 C CNN
	1    1950 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 4850 1250 4900
Wire Wire Line
	1250 4900 1350 4900
Wire Wire Line
	1850 4900 1950 4900
Wire Wire Line
	1950 4900 1950 4850
Wire Wire Line
	1350 6800 1250 6800
Wire Wire Line
	1250 6800 1250 6850
Wire Wire Line
	1850 6800 1950 6800
Wire Wire Line
	1950 6800 1950 6850
Wire Bus Line
	2200 750  1000 750 
Connection ~ 2200 750 
Entry Wire Line
	2100 2100 2200 2000
Entry Wire Line
	2100 2000 2200 1900
Entry Wire Line
	2100 1900 2200 1800
Wire Wire Line
	2100 1900 1850 1900
Wire Wire Line
	1850 2000 2100 2000
Text Label 2100 1900 2    50   ~ 0
D8
Text Label 2100 2000 2    50   ~ 0
D9
Text Label 2100 2100 2    50   ~ 0
D10
Entry Wire Line
	1100 2100 1000 2000
Entry Wire Line
	1100 2000 1000 1900
Entry Wire Line
	1100 1900 1000 1800
Wire Wire Line
	1100 1900 1350 1900
Wire Wire Line
	1350 2000 1100 2000
Text Label 1100 1900 0    50   ~ 0
D0
Text Label 1100 2000 0    50   ~ 0
D1
Text Label 1100 2100 0    50   ~ 0
D2
Wire Wire Line
	1350 2100 1100 2100
Wire Wire Line
	2100 2100 1850 2100
Entry Wire Line
	800  2800 700  2700
Entry Wire Line
	800  2700 700  2600
Wire Wire Line
	1350 2700 800  2700
Text Label 800  2700 0    50   ~ 0
A11
Text Label 800  2800 0    50   ~ 0
A10
Wire Wire Line
	1350 2800 800  2800
Connection ~ 2500 650 
Wire Bus Line
	2500 650  700  650 
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
A23
Text Label 2400 2800 2    50   ~ 0
A22
Text Label 2400 2900 2    50   ~ 0
A21
Wire Wire Line
	2400 2900 1850 2900
NoConn ~ 1350 3800
Entry Wire Line
	1100 2400 1000 2300
Entry Wire Line
	1100 2300 1000 2200
Entry Wire Line
	1100 2200 1000 2100
Wire Wire Line
	1100 2200 1350 2200
Wire Wire Line
	1350 2300 1100 2300
Text Label 1100 2200 0    50   ~ 0
D3
Text Label 1100 2300 0    50   ~ 0
D4
Text Label 1100 2400 0    50   ~ 0
D5
Wire Wire Line
	1350 2400 1100 2400
Entry Wire Line
	1100 2600 1000 2500
Entry Wire Line
	1100 2500 1000 2400
Wire Wire Line
	1350 2500 1100 2500
Text Label 1100 2500 0    50   ~ 0
D6
Text Label 1100 2600 0    50   ~ 0
D7
Wire Wire Line
	1350 2600 1100 2600
Entry Wire Line
	2100 2400 2200 2300
Entry Wire Line
	2100 2300 2200 2200
Entry Wire Line
	2100 2200 2200 2100
Wire Wire Line
	2100 2200 1850 2200
Wire Wire Line
	1850 2300 2100 2300
Text Label 2100 2200 2    50   ~ 0
D11
Text Label 2100 2300 2    50   ~ 0
D12
Text Label 2100 2400 2    50   ~ 0
D13
Wire Wire Line
	2100 2400 1850 2400
Entry Wire Line
	2100 2600 2200 2500
Entry Wire Line
	2100 2500 2200 2400
Wire Wire Line
	1850 2500 2100 2500
Text Label 2100 2500 2    50   ~ 0
D14
Text Label 2100 2600 2    50   ~ 0
D15
Wire Wire Line
	2100 2600 1850 2600
Entry Wire Line
	800  3000 700  2900
Entry Wire Line
	800  2900 700  2800
Wire Wire Line
	1350 2900 800  2900
Text Label 800  2900 0    50   ~ 0
A9
Text Label 800  3000 0    50   ~ 0
A8
Wire Wire Line
	1350 3000 800  3000
Entry Wire Line
	800  3200 700  3100
Entry Wire Line
	800  3100 700  3000
Wire Wire Line
	1350 3100 800  3100
Text Label 800  3100 0    50   ~ 0
A7
Text Label 800  3200 0    50   ~ 0
A6
Wire Wire Line
	1350 3200 800  3200
Entry Wire Line
	800  3400 700  3300
Entry Wire Line
	800  3300 700  3200
Wire Wire Line
	1350 3300 800  3300
Text Label 800  3300 0    50   ~ 0
A5
Text Label 800  3400 0    50   ~ 0
A4
Wire Wire Line
	1350 3400 800  3400
Entry Wire Line
	800  3500 700  3400
Text Label 800  3500 0    50   ~ 0
A3
Wire Wire Line
	1350 3500 800  3500
Entry Wire Line
	800  3700 700  3600
Entry Wire Line
	800  3600 700  3500
Wire Wire Line
	1350 3600 800  3600
Text Label 800  3600 0    50   ~ 0
A2
Text Label 800  3700 0    50   ~ 0
A1
Wire Wire Line
	1350 3700 800  3700
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
A20
Text Label 2400 3100 2    50   ~ 0
A19
Text Label 2400 3200 2    50   ~ 0
A18
Wire Wire Line
	2400 3200 1850 3200
Entry Wire Line
	2400 3500 2500 3400
Entry Wire Line
	2400 3400 2500 3300
Entry Wire Line
	2400 3300 2500 3200
Wire Wire Line
	2400 3300 1850 3300
Wire Wire Line
	1850 3400 2400 3400
Text Label 2400 3300 2    50   ~ 0
A17
Text Label 2400 3400 2    50   ~ 0
A16
Text Label 2400 3500 2    50   ~ 0
A15
Wire Wire Line
	2400 3500 1850 3500
Entry Wire Line
	2400 3800 2500 3700
Entry Wire Line
	2400 3700 2500 3600
Entry Wire Line
	2400 3600 2500 3500
Wire Wire Line
	2400 3600 1850 3600
Wire Wire Line
	1850 3700 2400 3700
Text Label 2400 3600 2    50   ~ 0
A14
Text Label 2400 3700 2    50   ~ 0
A13
Text Label 2400 3800 2    50   ~ 0
A12
Wire Wire Line
	2400 3800 1850 3800
Wire Wire Line
	1100 5000 1350 5000
Wire Wire Line
	1350 5600 1100 5600
Wire Wire Line
	1850 5400 2100 5400
Wire Wire Line
	2100 5500 1850 5500
Wire Wire Line
	2100 5900 1850 5900
Wire Wire Line
	2100 6000 1850 6000
Wire Wire Line
	2100 6100 1850 6100
Wire Wire Line
	2100 5600 1850 5600
Wire Wire Line
	2100 5700 1850 5700
Wire Wire Line
	1850 5800 2100 5800
Wire Wire Line
	2100 6700 1850 6700
Wire Wire Line
	2300 5300 1850 5300
Wire Wire Line
	1850 5200 2100 5200
Wire Wire Line
	1850 5100 2100 5100
Wire Wire Line
	1850 5000 2100 5000
$Comp
L 74xx:74HC14 U1
U 2 1 5E061F57
P 3700 6900
F 0 "U1" H 3700 7217 50  0000 C CNN
F 1 "74HC14" H 3700 7126 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 3700 6900 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 3700 6900 50  0001 C CNN
	2    3700 6900
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC14 U1
U 3 1 5E061F5D
P 4300 6900
F 0 "U1" H 4300 7217 50  0000 C CNN
F 1 "74HC14" H 4300 7126 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 4300 6900 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74HC14" H 4300 6900 50  0001 C CNN
	3    4300 6900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5E061F63
P 3650 7300
F 0 "R2" V 3443 7300 50  0000 C CNN
F 1 "5.1k" V 3534 7300 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3580 7300 50  0001 C CNN
F 3 "~" H 3650 7300 50  0001 C CNN
	1    3650 7300
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5E061F69
P 4300 7300
F 0 "R8" V 4093 7300 50  0000 C CNN
F 1 "5.1k" V 4184 7300 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4230 7300 50  0001 C CNN
F 3 "~" H 4300 7300 50  0001 C CNN
	1    4300 7300
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5E061F6F
P 4000 6500
F 0 "Y1" H 4000 6768 50  0000 C CNN
F 1 "Crystal" H 4000 6677 50  0000 C CNN
F 2 "Crystal:Crystal_HC49-4H_Vertical" H 4000 6500 50  0001 C CNN
F 3 "~" H 4000 6500 50  0001 C CNN
	1    4000 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 6500 3400 6500
Wire Wire Line
	3400 6500 3400 6900
Wire Wire Line
	3400 6900 3400 7300
Wire Wire Line
	3400 7300 3500 7300
Connection ~ 3400 6900
Wire Wire Line
	3800 7300 4000 7300
Wire Wire Line
	4000 7300 4000 6900
Connection ~ 4000 6900
Wire Wire Line
	4000 7300 4150 7300
Connection ~ 4000 7300
Wire Wire Line
	4450 7300 4600 7300
Wire Wire Line
	4600 7300 4600 6900
Wire Wire Line
	4600 6900 4600 6500
Wire Wire Line
	4600 6500 4150 6500
Connection ~ 4600 6900
Text GLabel 4850 6900 2    50   Output ~ 0
CLK
Text GLabel 2100 6400 2    50   Input ~ 0
OP2
Text GLabel 2100 6500 2    50   Input ~ 0
OP3
Text GLabel 2100 6600 2    50   Output ~ 0
IP2
Text GLabel 2100 6700 2    50   Output ~ 0
IP3
Wire Wire Line
	2100 6300 1850 6300
Wire Wire Line
	2100 6400 1850 6400
Wire Wire Line
	2100 6500 1850 6500
Wire Wire Line
	1850 6600 2100 6600
Text GLabel 2100 6200 2    50   Input ~ 0
SIACK
Wire Wire Line
	2100 6200 1850 6200
$Comp
L 74xx:74LS393 U5
U 3 1 5EE26210
P 10200 5650
AR Path="/5EE26210" Ref="U5"  Part="3" 
AR Path="/5E081AED/5EE26210" Ref="U?"  Part="1" 
F 0 "U5" H 10200 6017 50  0000 C CNN
F 1 "74LS393" H 10200 5926 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 10200 5650 50  0001 C CNN
F 3 "74xx\\74LS393.pdf" H 10200 5650 50  0001 C CNN
	3    10200 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 5100 1100 5100
Wire Wire Line
	1100 5200 1350 5200
Wire Wire Line
	1350 5300 1100 5300
Text GLabel 1100 5200 0    50   Output ~ 0
BGACK
Text GLabel 1100 5100 0    50   Input ~ 0
BG
Text GLabel 1100 5300 0    50   Output ~ 0
BR
Wire Wire Line
	4600 6900 4850 6900
Text GLabel 1100 5500 0    50   BiDi ~ 0
RESET
Text GLabel 1100 5400 0    50   BiDi ~ 0
HALT
Wire Wire Line
	1100 5400 1350 5400
Wire Wire Line
	1100 5500 1350 5500
NoConn ~ 1350 5700
Text GLabel 1100 6700 0    50   Output ~ 0
CLK
Wire Wire Line
	1100 5900 1350 5900
Text GLabel 1100 5900 0    50   Input ~ 0
0SEL
Wire Wire Line
	1100 5800 1350 5800
Wire Wire Line
	1350 6000 1100 6000
Wire Wire Line
	1100 6100 1350 6100
Wire Wire Line
	1350 6200 1100 6200
Wire Wire Line
	1100 6300 1350 6300
Wire Wire Line
	1350 6400 1100 6400
Wire Wire Line
	1100 6500 1350 6500
Wire Wire Line
	1350 6600 1100 6600
Text GLabel 1100 6500 0    50   Input ~ 0
6SEL
Text GLabel 1100 6400 0    50   Input ~ 0
5SEL
Text GLabel 1100 6300 0    50   Input ~ 0
4SEL
Text GLabel 1100 6200 0    50   Input ~ 0
3SEL
Text GLabel 1100 6100 0    50   Input ~ 0
2SEL
Text GLabel 1100 5800 0    50   Input ~ 0
ROMSEL
Text GLabel 1100 6000 0    50   Input ~ 0
RAMSEL
Text GLabel 1100 6600 0    50   Input ~ 0
IOSEL
Wire Wire Line
	1100 6700 1350 6700
Wire Bus Line
	1000 750  1000 2500
Wire Bus Line
	2200 750  2200 2500
Wire Bus Line
	2500 650  2500 3700
Wire Bus Line
	700  650  700  3600
Wire Bus Line
	7350 750  7350 5050
Wire Bus Line
	7250 650  7250 3350
Text GLabel 2100 6300 2    50   Output ~ 0
SIRQ
$EndSCHEMATC
