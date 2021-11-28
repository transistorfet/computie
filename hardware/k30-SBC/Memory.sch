EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 5
Title "k30 SBC"
Date ""
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3500 3450 3950 3450
Text GLabel 4200 3350 2    50   Input ~ 0
~DS2
Wire Wire Line
	3500 3350 4200 3350
$Comp
L ComputieParts:AS6C8016 U14
U 1 1 613F02A2
P 3000 2300
F 0 "U14" H 3000 3581 50  0000 C CNN
F 1 "AS6C8016" H 3000 3490 50  0000 C CNN
F 2 "Package_SO:TSOP-II-44_10.16x18.41mm_P0.8mm" H 3000 2300 50  0001 C CNN
F 3 "http://www.futurlec.com/Datasheet/Memory/628528.pdf" H 3000 2300 50  0001 C CNN
	1    3000 2300
	1    0    0    -1  
$EndComp
$Comp
L ComputieParts:AS6C8016 U15
U 1 1 613F1A83
P 3000 5450
F 0 "U15" H 3000 6731 50  0000 C CNN
F 1 "AS6C8016" H 3000 6640 50  0000 C CNN
F 2 "Package_SO:TSOP-II-44_10.16x18.41mm_P0.8mm" H 3000 5450 50  0001 C CNN
F 3 "http://www.futurlec.com/Datasheet/Memory/628528.pdf" H 3000 5450 50  0001 C CNN
	1    3000 5450
	1    0    0    -1  
$EndComp
Entry Wire Line
	2150 1300 2250 1400
Wire Bus Line
	2150 950  1250 950 
Wire Bus Line
	3850 800  1250 800 
Text GLabel 1250 950  0    50   Input ~ 0
A[0..31]
Text GLabel 1250 800  0    50   BiDi ~ 0
D[0..31]
Wire Wire Line
	2250 1400 2500 1400
Text Label 2250 1400 0    50   ~ 0
A2
Entry Wire Line
	2150 1400 2250 1500
Wire Wire Line
	2250 1500 2500 1500
Text Label 2250 1500 0    50   ~ 0
A3
Entry Wire Line
	2150 1500 2250 1600
Wire Wire Line
	2250 1600 2500 1600
Text Label 2250 1600 0    50   ~ 0
A4
Entry Wire Line
	2150 1600 2250 1700
Wire Wire Line
	2250 1700 2500 1700
Text Label 2250 1700 0    50   ~ 0
A5
Entry Wire Line
	2150 1700 2250 1800
Wire Wire Line
	2250 1800 2500 1800
Text Label 2250 1800 0    50   ~ 0
A6
Entry Wire Line
	2150 1800 2250 1900
Wire Wire Line
	2250 1900 2500 1900
Text Label 2250 1900 0    50   ~ 0
A7
Entry Wire Line
	2150 1900 2250 2000
Wire Wire Line
	2250 2000 2500 2000
Text Label 2250 2000 0    50   ~ 0
A8
Entry Wire Line
	2150 2000 2250 2100
Wire Wire Line
	2250 2100 2500 2100
Text Label 2250 2100 0    50   ~ 0
A9
Entry Wire Line
	2150 2100 2250 2200
Wire Wire Line
	2250 2200 2500 2200
Text Label 2250 2200 0    50   ~ 0
A10
Entry Wire Line
	2150 2200 2250 2300
Wire Wire Line
	2250 2300 2500 2300
Text Label 2250 2300 0    50   ~ 0
A11
Entry Wire Line
	2150 2300 2250 2400
Wire Wire Line
	2250 2400 2500 2400
Text Label 2250 2400 0    50   ~ 0
A12
Entry Wire Line
	2150 2400 2250 2500
Wire Wire Line
	2250 2500 2500 2500
Text Label 2250 2500 0    50   ~ 0
A13
Entry Wire Line
	2150 2500 2250 2600
Wire Wire Line
	2250 2600 2500 2600
Text Label 2250 2600 0    50   ~ 0
A14
Entry Wire Line
	2150 2600 2250 2700
Wire Wire Line
	2250 2700 2500 2700
Text Label 2250 2700 0    50   ~ 0
A15
Entry Wire Line
	2150 2700 2250 2800
Wire Wire Line
	2250 2800 2500 2800
Text Label 2250 2800 0    50   ~ 0
A16
Entry Wire Line
	2150 2800 2250 2900
Wire Wire Line
	2250 2900 2500 2900
Text Label 2250 2900 0    50   ~ 0
A17
Entry Wire Line
	2150 2900 2250 3000
Wire Wire Line
	2250 3000 2500 3000
Text Label 2250 3000 0    50   ~ 0
A18
Text Label 2250 3100 0    50   ~ 0
A19
Text Label 2250 3200 0    50   ~ 0
A20
$Comp
L power:+5V #PWR?
U 1 1 61408A2C
P 3350 1200
AR Path="/61408A2C" Ref="#PWR?"  Part="1" 
AR Path="/613DFFC0/61408A2C" Ref="#PWR034"  Part="1" 
F 0 "#PWR034" H 3350 1050 50  0001 C CNN
F 1 "+5V" H 3365 1373 50  0000 C CNN
F 2 "" H 3350 1200 50  0001 C CNN
F 3 "" H 3350 1200 50  0001 C CNN
	1    3350 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6140D40E
P 3000 3650
AR Path="/6140D40E" Ref="#PWR?"  Part="1" 
AR Path="/613DFFC0/6140D40E" Ref="#PWR032"  Part="1" 
F 0 "#PWR032" H 3000 3400 50  0001 C CNN
F 1 "GND" H 3005 3477 50  0000 C CNN
F 2 "" H 3000 3650 50  0001 C CNN
F 3 "" H 3000 3650 50  0001 C CNN
	1    3000 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 3600 2950 3650
Wire Wire Line
	2950 3650 3000 3650
Wire Wire Line
	3000 3650 3050 3650
Wire Wire Line
	3050 3650 3050 3600
Connection ~ 3000 3650
$Comp
L power:GND #PWR033
U 1 1 61413060
P 3000 6800
F 0 "#PWR033" H 3000 6550 50  0001 C CNN
F 1 "GND" H 3005 6627 50  0000 C CNN
F 2 "" H 3000 6800 50  0001 C CNN
F 3 "" H 3000 6800 50  0001 C CNN
	1    3000 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 6750 2950 6800
Wire Wire Line
	2950 6800 3000 6800
Wire Wire Line
	3000 6800 3050 6800
Wire Wire Line
	3050 6800 3050 6750
Connection ~ 3000 6800
Wire Wire Line
	2950 1200 3050 1200
Connection ~ 3050 1200
Wire Wire Line
	3050 1200 3350 1200
$Comp
L power:+5V #PWR?
U 1 1 614168BA
P 3350 4350
AR Path="/614168BA" Ref="#PWR?"  Part="1" 
AR Path="/613DFFC0/614168BA" Ref="#PWR035"  Part="1" 
F 0 "#PWR035" H 3350 4200 50  0001 C CNN
F 1 "+5V" H 3365 4523 50  0000 C CNN
F 2 "" H 3350 4350 50  0001 C CNN
F 3 "" H 3350 4350 50  0001 C CNN
	1    3350 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 4350 3050 4350
Connection ~ 3050 4350
Wire Wire Line
	3050 4350 3350 4350
Entry Wire Line
	2150 4450 2250 4550
Wire Wire Line
	2250 4550 2500 4550
Text Label 2250 4550 0    50   ~ 0
A2
Entry Wire Line
	2150 4550 2250 4650
Wire Wire Line
	2250 4650 2500 4650
Text Label 2250 4650 0    50   ~ 0
A3
Entry Wire Line
	2150 4650 2250 4750
Wire Wire Line
	2250 4750 2500 4750
Text Label 2250 4750 0    50   ~ 0
A4
Entry Wire Line
	2150 4750 2250 4850
Wire Wire Line
	2250 4850 2500 4850
Text Label 2250 4850 0    50   ~ 0
A5
Entry Wire Line
	2150 4850 2250 4950
Wire Wire Line
	2250 4950 2500 4950
Text Label 2250 4950 0    50   ~ 0
A6
Entry Wire Line
	2150 4950 2250 5050
Wire Wire Line
	2250 5050 2500 5050
Text Label 2250 5050 0    50   ~ 0
A7
Entry Wire Line
	2150 5050 2250 5150
Wire Wire Line
	2250 5150 2500 5150
Text Label 2250 5150 0    50   ~ 0
A8
Entry Wire Line
	2150 5150 2250 5250
Wire Wire Line
	2250 5250 2500 5250
Text Label 2250 5250 0    50   ~ 0
A9
Entry Wire Line
	2150 5250 2250 5350
Wire Wire Line
	2250 5350 2500 5350
Text Label 2250 5350 0    50   ~ 0
A10
Entry Wire Line
	2150 5350 2250 5450
Wire Wire Line
	2250 5450 2500 5450
Text Label 2250 5450 0    50   ~ 0
A11
Entry Wire Line
	2150 5450 2250 5550
Wire Wire Line
	2250 5550 2500 5550
Text Label 2250 5550 0    50   ~ 0
A12
Entry Wire Line
	2150 5550 2250 5650
Wire Wire Line
	2250 5650 2500 5650
Text Label 2250 5650 0    50   ~ 0
A13
Entry Wire Line
	2150 5650 2250 5750
Wire Wire Line
	2250 5750 2500 5750
Text Label 2250 5750 0    50   ~ 0
A14
Entry Wire Line
	2150 5750 2250 5850
Wire Wire Line
	2250 5850 2500 5850
Text Label 2250 5850 0    50   ~ 0
A15
Entry Wire Line
	2150 5850 2250 5950
Wire Wire Line
	2250 5950 2500 5950
Text Label 2250 5950 0    50   ~ 0
A16
Entry Wire Line
	2150 5950 2250 6050
Wire Wire Line
	2250 6050 2500 6050
Text Label 2250 6050 0    50   ~ 0
A17
Entry Wire Line
	2150 6050 2250 6150
Wire Wire Line
	2250 6150 2500 6150
Text Label 2250 6150 0    50   ~ 0
A18
Entry Wire Line
	2150 6150 2250 6250
Wire Wire Line
	2250 6250 2500 6250
Text Label 2250 6250 0    50   ~ 0
A19
Entry Wire Line
	2150 6250 2250 6350
Wire Wire Line
	2250 6350 2500 6350
Text Label 2250 6350 0    50   ~ 0
A20
Wire Wire Line
	2250 3200 2500 3200
Wire Wire Line
	2250 3100 2500 3100
Entry Wire Line
	2150 3100 2250 3200
Entry Wire Line
	2150 3000 2250 3100
Entry Wire Line
	3750 1400 3850 1300
Wire Wire Line
	3500 1400 3750 1400
Text Label 3750 1400 2    50   ~ 0
D16
Entry Wire Line
	3750 1500 3850 1400
Wire Wire Line
	3500 1500 3750 1500
Text Label 3750 1500 2    50   ~ 0
D17
Entry Wire Line
	3750 1600 3850 1500
Wire Wire Line
	3500 1600 3750 1600
Text Label 3750 1600 2    50   ~ 0
D18
Entry Wire Line
	3750 1700 3850 1600
Wire Wire Line
	3500 1700 3750 1700
Text Label 3750 1700 2    50   ~ 0
D19
Entry Wire Line
	3750 1800 3850 1700
Wire Wire Line
	3500 1800 3750 1800
Text Label 3750 1800 2    50   ~ 0
D20
Entry Wire Line
	3750 1900 3850 1800
Wire Wire Line
	3500 1900 3750 1900
Text Label 3750 1900 2    50   ~ 0
D21
Entry Wire Line
	3750 2000 3850 1900
Wire Wire Line
	3500 2000 3750 2000
Text Label 3750 2000 2    50   ~ 0
D22
Entry Wire Line
	3750 2100 3850 2000
Wire Wire Line
	3500 2100 3750 2100
Text Label 3750 2100 2    50   ~ 0
D23
Entry Wire Line
	3750 2200 3850 2100
Wire Wire Line
	3500 2200 3750 2200
Text Label 3750 2200 2    50   ~ 0
D24
Entry Wire Line
	3750 2300 3850 2200
Wire Wire Line
	3500 2300 3750 2300
Text Label 3750 2300 2    50   ~ 0
D25
Entry Wire Line
	3750 2400 3850 2300
Wire Wire Line
	3500 2400 3750 2400
Text Label 3750 2400 2    50   ~ 0
D26
Entry Wire Line
	3750 2500 3850 2400
Wire Wire Line
	3500 2500 3750 2500
Text Label 3750 2500 2    50   ~ 0
D27
Entry Wire Line
	3750 2600 3850 2500
Wire Wire Line
	3500 2600 3750 2600
Text Label 3750 2600 2    50   ~ 0
D28
Entry Wire Line
	3750 2700 3850 2600
Wire Wire Line
	3500 2700 3750 2700
Text Label 3750 2700 2    50   ~ 0
D29
Entry Wire Line
	3750 2800 3850 2700
Wire Wire Line
	3500 2800 3750 2800
Text Label 3750 2800 2    50   ~ 0
D30
Entry Wire Line
	3750 2900 3850 2800
Wire Wire Line
	3500 2900 3750 2900
Text Label 3750 2900 2    50   ~ 0
D31
Entry Wire Line
	3750 4550 3850 4450
Wire Wire Line
	3500 4550 3750 4550
Text Label 3750 4550 2    50   ~ 0
D0
Entry Wire Line
	3750 4650 3850 4550
Wire Wire Line
	3500 4650 3750 4650
Text Label 3750 4650 2    50   ~ 0
D1
Entry Wire Line
	3750 4750 3850 4650
Wire Wire Line
	3500 4750 3750 4750
Text Label 3750 4750 2    50   ~ 0
D2
Entry Wire Line
	3750 4850 3850 4750
Wire Wire Line
	3500 4850 3750 4850
Text Label 3750 4850 2    50   ~ 0
D3
Entry Wire Line
	3750 4950 3850 4850
Wire Wire Line
	3500 4950 3750 4950
Text Label 3750 4950 2    50   ~ 0
D4
Entry Wire Line
	3750 5050 3850 4950
Wire Wire Line
	3500 5050 3750 5050
Text Label 3750 5050 2    50   ~ 0
D5
Entry Wire Line
	3750 5150 3850 5050
Wire Wire Line
	3500 5150 3750 5150
Text Label 3750 5150 2    50   ~ 0
D6
Entry Wire Line
	3750 5250 3850 5150
Wire Wire Line
	3500 5250 3750 5250
Text Label 3750 5250 2    50   ~ 0
D7
Entry Wire Line
	3750 5350 3850 5250
Wire Wire Line
	3500 5350 3750 5350
Text Label 3750 5350 2    50   ~ 0
D8
Entry Wire Line
	3750 5450 3850 5350
Wire Wire Line
	3500 5450 3750 5450
Text Label 3750 5450 2    50   ~ 0
D9
Entry Wire Line
	3750 5550 3850 5450
Wire Wire Line
	3500 5550 3750 5550
Text Label 3750 5550 2    50   ~ 0
D10
Entry Wire Line
	3750 5650 3850 5550
Wire Wire Line
	3500 5650 3750 5650
Text Label 3750 5650 2    50   ~ 0
D11
Entry Wire Line
	3750 5750 3850 5650
Wire Wire Line
	3500 5750 3750 5750
Text Label 3750 5750 2    50   ~ 0
D12
Entry Wire Line
	3750 5850 3850 5750
Wire Wire Line
	3500 5850 3750 5850
Text Label 3750 5850 2    50   ~ 0
D13
Entry Wire Line
	3750 5950 3850 5850
Wire Wire Line
	3500 5950 3750 5950
Text Label 3750 5950 2    50   ~ 0
D14
Entry Wire Line
	3750 6050 3850 5950
Wire Wire Line
	3500 6050 3750 6050
Text Label 3750 6050 2    50   ~ 0
D15
$Comp
L ComputieParts:AM29F040 U16
U 1 1 6144FBE4
P 6950 2200
F 0 "U16" H 6950 3381 50  0000 C CNN
F 1 "AM29F040" H 6950 3290 50  0000 C CNN
F 2 "Package_LCC:PLCC-32_THT-Socket" H 6950 2200 50  0001 C CNN
F 3 "" H 6950 2200 50  0001 C CNN
	1    6950 2200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 61451196
P 7300 1200
AR Path="/61451196" Ref="#PWR?"  Part="1" 
AR Path="/613DFFC0/61451196" Ref="#PWR037"  Part="1" 
F 0 "#PWR037" H 7300 1050 50  0001 C CNN
F 1 "+5V" H 7315 1373 50  0000 C CNN
F 2 "" H 7300 1200 50  0001 C CNN
F 3 "" H 7300 1200 50  0001 C CNN
	1    7300 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 1200 7300 1200
$Comp
L power:GND #PWR?
U 1 1 61456225
P 6950 3400
AR Path="/61456225" Ref="#PWR?"  Part="1" 
AR Path="/613DFFC0/61456225" Ref="#PWR036"  Part="1" 
F 0 "#PWR036" H 6950 3150 50  0001 C CNN
F 1 "GND" H 6955 3227 50  0000 C CNN
F 2 "" H 6950 3400 50  0001 C CNN
F 3 "" H 6950 3400 50  0001 C CNN
	1    6950 3400
	1    0    0    -1  
$EndComp
Entry Wire Line
	6100 1500 6200 1600
Wire Wire Line
	6200 1600 6450 1600
Text Label 6200 1600 0    50   ~ 0
A2
Entry Wire Line
	6100 1600 6200 1700
Wire Wire Line
	6200 1700 6450 1700
Text Label 6200 1700 0    50   ~ 0
A3
Entry Wire Line
	6100 1700 6200 1800
Wire Wire Line
	6200 1800 6450 1800
Text Label 6200 1800 0    50   ~ 0
A4
Entry Wire Line
	6100 1800 6200 1900
Wire Wire Line
	6200 1900 6450 1900
Text Label 6200 1900 0    50   ~ 0
A5
Entry Wire Line
	6100 1900 6200 2000
Wire Wire Line
	6200 2000 6450 2000
Text Label 6200 2000 0    50   ~ 0
A6
Entry Wire Line
	6100 2000 6200 2100
Wire Wire Line
	6200 2100 6450 2100
Text Label 6200 2100 0    50   ~ 0
A7
Entry Wire Line
	6100 2100 6200 2200
Wire Wire Line
	6200 2200 6450 2200
Text Label 6200 2200 0    50   ~ 0
A8
Entry Wire Line
	6100 2200 6200 2300
Wire Wire Line
	6200 2300 6450 2300
Text Label 6200 2300 0    50   ~ 0
A9
Entry Wire Line
	6100 2300 6200 2400
Wire Wire Line
	6200 2400 6450 2400
Text Label 6200 2400 0    50   ~ 0
A10
Entry Wire Line
	6100 2400 6200 2500
Wire Wire Line
	6200 2500 6450 2500
Text Label 6200 2500 0    50   ~ 0
A11
Entry Wire Line
	6100 2500 6200 2600
Wire Wire Line
	6200 2600 6450 2600
Text Label 6200 2600 0    50   ~ 0
A12
Entry Wire Line
	6100 2600 6200 2700
Wire Wire Line
	6200 2700 6450 2700
Text Label 6200 2700 0    50   ~ 0
A13
Entry Wire Line
	6100 2700 6200 2800
Wire Wire Line
	6200 2800 6450 2800
Text Label 6200 2800 0    50   ~ 0
A14
Entry Wire Line
	6100 2800 6200 2900
Wire Wire Line
	6200 2900 6450 2900
Text Label 6200 2900 0    50   ~ 0
A15
Entry Wire Line
	6100 2900 6200 3000
Wire Wire Line
	6200 3000 6450 3000
Text Label 6200 3000 0    50   ~ 0
A16
Entry Wire Line
	6100 3000 6200 3100
Wire Wire Line
	6200 3100 6450 3100
Text Label 6200 3100 0    50   ~ 0
A17
Entry Wire Line
	6100 3100 6200 3200
Wire Wire Line
	6200 3200 6450 3200
Text Label 6200 3200 0    50   ~ 0
A18
Wire Bus Line
	6100 950  2150 950 
Connection ~ 2150 950 
Entry Wire Line
	6100 1300 6200 1400
Wire Wire Line
	6200 1400 6450 1400
Text Label 6200 1400 0    50   ~ 0
A0
Entry Wire Line
	6100 1400 6200 1500
Text Label 6200 1500 0    50   ~ 0
A1
Wire Wire Line
	6200 1500 6450 1500
Entry Wire Line
	7700 1400 7800 1300
Wire Wire Line
	7450 1400 7700 1400
Text Label 7700 1400 2    50   ~ 0
D24
Entry Wire Line
	7700 1500 7800 1400
Wire Wire Line
	7450 1500 7700 1500
Text Label 7700 1500 2    50   ~ 0
D25
Entry Wire Line
	7700 1600 7800 1500
Wire Wire Line
	7450 1600 7700 1600
Text Label 7700 1600 2    50   ~ 0
D26
Entry Wire Line
	7700 1700 7800 1600
Wire Wire Line
	7450 1700 7700 1700
Text Label 7700 1700 2    50   ~ 0
D27
Entry Wire Line
	7700 1800 7800 1700
Wire Wire Line
	7450 1800 7700 1800
Text Label 7700 1800 2    50   ~ 0
D28
Entry Wire Line
	7700 1900 7800 1800
Wire Wire Line
	7450 1900 7700 1900
Text Label 7700 1900 2    50   ~ 0
D29
Entry Wire Line
	7700 2000 7800 1900
Wire Wire Line
	7450 2000 7700 2000
Text Label 7700 2000 2    50   ~ 0
D30
Entry Wire Line
	7700 2100 7800 2000
Wire Wire Line
	7450 2100 7700 2100
Text Label 7700 2100 2    50   ~ 0
D31
Wire Bus Line
	7800 800  3850 800 
Connection ~ 3850 800 
Text GLabel 3950 3450 2    50   Input ~ 0
~DS3
Wire Wire Line
	3500 6600 3950 6600
Text GLabel 4200 6500 2    50   Input ~ 0
~DS0
Wire Wire Line
	3500 6500 4200 6500
Text GLabel 3950 6600 2    50   Input ~ 0
~DS1
Wire Wire Line
	3500 3150 3950 3150
Text GLabel 3950 3150 2    50   Input ~ 0
RD
Wire Wire Line
	3500 3250 3950 3250
Text GLabel 3950 3250 2    50   Input ~ 0
WR
Wire Wire Line
	3500 6300 3950 6300
Text GLabel 3950 6300 2    50   Input ~ 0
RD
Wire Wire Line
	3500 6400 3950 6400
Text GLabel 3950 6400 2    50   Input ~ 0
WR
Wire Wire Line
	3500 3050 3950 3050
Text GLabel 3950 3050 2    50   Input ~ 0
RAMSEL
Wire Wire Line
	3500 6200 3950 6200
Text GLabel 3950 6200 2    50   Input ~ 0
RAMSEL
Wire Wire Line
	7450 2300 7900 2300
Text GLabel 7900 2300 2    50   Input ~ 0
ROMSEL
Wire Wire Line
	7450 2400 7900 2400
Text GLabel 7900 2400 2    50   Input ~ 0
RD
Wire Wire Line
	7450 2500 7900 2500
Text GLabel 7900 2500 2    50   Input ~ 0
WR
Wire Bus Line
	7800 800  7800 2000
Wire Bus Line
	6100 950  6100 3100
Wire Bus Line
	3850 800  3850 5950
Wire Bus Line
	2150 950  2150 6250
$EndSCHEMATC
