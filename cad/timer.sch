EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:pspice
LIBS:motor_drivers
LIBS:motors
LIBS:Oscillators
LIBS:timer-cache
EELAYER 25 0
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
$Comp
L LM393 U2
U 1 1 59BEE76C
P 3850 1400
F 0 "U2" H 4000 1550 50  0000 C CNN
F 1 "LM393" H 4100 1250 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 3850 1400 50  0001 C CNN
F 3 "" H 3850 1400 50  0001 C CNN
	1    3850 1400
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 59BEE84B
P 2400 1300
F 0 "R1" V 2480 1300 50  0000 C CNN
F 1 "1MΩ" V 2400 1300 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2330 1300 50  0001 C CNN
F 3 "" H 2400 1300 50  0001 C CNN
	1    2400 1300
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 59BEE8DB
P 2850 1550
F 0 "R2" V 2930 1550 50  0000 C CNN
F 1 "68kΩ" V 2850 1550 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2780 1550 50  0001 C CNN
F 3 "" H 2850 1550 50  0001 C CNN
	1    2850 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 59BEEB46
P 3750 1700
F 0 "#PWR01" H 3750 1450 50  0001 C CNN
F 1 "GND" H 3750 1550 50  0000 C CNN
F 2 "" H 3750 1700 50  0001 C CNN
F 3 "" H 3750 1700 50  0001 C CNN
	1    3750 1700
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR02
U 1 1 59BEEC80
P 3750 1100
F 0 "#PWR02" H 3750 950 50  0001 C CNN
F 1 "VCC" H 3750 1250 50  0000 C CNN
F 2 "" H 3750 1100 50  0001 C CNN
F 3 "" H 3750 1100 50  0001 C CNN
	1    3750 1100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 59BEECA3
P 2050 1100
F 0 "#PWR03" H 2050 950 50  0001 C CNN
F 1 "VCC" H 2050 1250 50  0000 C CNN
F 2 "" H 2050 1100 50  0001 C CNN
F 3 "" H 2050 1100 50  0001 C CNN
	1    2050 1100
	1    0    0    -1  
$EndComp
Text Notes 2500 1050 0    60   ~ 0
Soft Power Switch
$Comp
L L293D U3
U 1 1 59BEF0C3
P 6900 4300
F 0 "U3" H 6700 5325 50  0000 R CNN
F 1 "L293D" H 6700 5250 50  0000 R CNN
F 2 "Housings_DIP:DIP-16_W7.62mm" H 7150 3550 50  0001 L CNN
F 3 "" H 6600 5000 50  0001 C CNN
	1    6900 4300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 59BEF4FF
P 6900 5250
F 0 "#PWR04" H 6900 5000 50  0001 C CNN
F 1 "GND" H 6900 5100 50  0000 C CNN
F 2 "" H 6900 5250 50  0001 C CNN
F 3 "" H 6900 5250 50  0001 C CNN
	1    6900 5250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR05
U 1 1 59BEF642
P 7000 2800
F 0 "#PWR05" H 7000 2650 50  0001 C CNN
F 1 "VCC" H 7000 2950 50  0000 C CNN
F 2 "" H 7000 2800 50  0001 C CNN
F 3 "" H 7000 2800 50  0001 C CNN
	1    7000 2800
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA328P-PU U1
U 1 1 59BF0585
P 3500 4050
F 0 "U1" H 2750 5300 50  0000 L BNN
F 1 "ATMEGA328P-PU" H 3900 2650 50  0000 L BNN
F 2 "Housings_DIP:DIP-28_W7.62mm" H 3500 4050 50  0001 C CIN
F 3 "" H 3500 4050 50  0001 C CNN
	1    3500 4050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 59BF0888
P 2500 5350
F 0 "#PWR06" H 2500 5100 50  0001 C CNN
F 1 "GND" H 2500 5200 50  0000 C CNN
F 2 "" H 2500 5350 50  0001 C CNN
F 3 "" H 2500 5350 50  0001 C CNN
	1    2500 5350
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR07
U 1 1 59BF09B4
P 2450 2850
F 0 "#PWR07" H 2450 2700 50  0001 C CNN
F 1 "VCC" H 2450 3000 50  0000 C CNN
F 2 "" H 2450 2850 50  0001 C CNN
F 3 "" H 2450 2850 50  0001 C CNN
	1    2450 2850
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 59BF0B26
P 2450 3800
F 0 "C1" H 2475 3900 50  0000 L CNN
F 1 "47pF" H 2475 3700 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 2488 3650 50  0001 C CNN
F 3 "" H 2450 3800 50  0001 C CNN
	1    2450 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 59BF0C0B
P 2450 3950
F 0 "#PWR08" H 2450 3700 50  0001 C CNN
F 1 "GND" H 2450 3800 50  0000 C CNN
F 2 "" H 2450 3950 50  0001 C CNN
F 3 "" H 2450 3950 50  0001 C CNN
	1    2450 3950
	1    0    0    -1  
$EndComp
Text Notes 6600 2550 0    60   ~ 0
Motor Control
$Comp
L Crystal Y1
U 1 1 59BF0DF2
P 5300 5600
F 0 "Y1" H 5300 5750 50  0000 C CNN
F 1 "32.768kHz" H 5300 5450 50  0000 C CNN
F 2 "Crystals:Crystal_C38-LF_d3.0mm_l8.0mm_Vertical" H 5300 5600 50  0001 C CNN
F 3 "http://www.abracon.com/Resonators/AB38T.pdf" H 5300 5600 50  0001 C CNN
	1    5300 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 1300 3550 1300
Wire Wire Line
	2850 1300 2850 1400
Connection ~ 2850 1300
Wire Wire Line
	2050 1100 2050 1300
Wire Wire Line
	2050 1300 2250 1300
Wire Wire Line
	6700 5100 6700 5250
Wire Wire Line
	6350 5250 7100 5250
Wire Wire Line
	7100 5250 7100 5100
Wire Wire Line
	7000 5100 7000 5250
Connection ~ 7000 5250
Wire Wire Line
	6800 5100 6800 5250
Connection ~ 6800 5250
Connection ~ 6900 5250
Wire Wire Line
	6400 4700 6350 4700
Wire Wire Line
	6350 4700 6350 5250
Connection ~ 6700 5250
Wire Wire Line
	7000 2800 7000 3300
Wire Wire Line
	4500 5250 6100 5250
Wire Wire Line
	6100 5250 6100 3700
Wire Wire Line
	6100 3700 6400 3700
Wire Wire Line
	5700 2950 5700 3900
Wire Wire Line
	5850 5050 4500 5050
Wire Wire Line
	5850 3150 5850 5050
Wire Wire Line
	5850 4100 6400 4100
Wire Wire Line
	2450 2850 2450 3250
Wire Wire Line
	2450 2950 2600 2950
Wire Wire Line
	2600 5150 2500 5150
Wire Wire Line
	2500 5150 2500 5350
Wire Wire Line
	2600 5250 2500 5250
Connection ~ 2500 5250
Wire Wire Line
	2450 3250 2600 3250
Connection ~ 2450 2950
Wire Wire Line
	2600 3550 2450 3550
Wire Wire Line
	2450 3550 2450 3650
Wire Wire Line
	4800 4750 4500 4750
Wire Wire Line
	4800 1400 4800 4750
Wire Wire Line
	4150 1400 4800 1400
Wire Wire Line
	4500 3550 5550 3550
Wire Wire Line
	5550 3550 5550 5850
Wire Wire Line
	5550 5600 5450 5600
Wire Wire Line
	5150 5600 5050 5600
Wire Wire Line
	5050 3650 5050 5850
Wire Wire Line
	5050 3650 4500 3650
$Comp
L C C2
U 1 1 59BF12C4
P 5050 6000
F 0 "C2" H 5075 6100 50  0000 L CNN
F 1 "15pF" H 5075 5900 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 5088 5850 50  0001 C CNN
F 3 "" H 5050 6000 50  0001 C CNN
	1    5050 6000
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 59BF148B
P 5550 6000
F 0 "C3" H 5575 6100 50  0000 L CNN
F 1 "15pF" H 5575 5900 50  0000 L CNN
F 2 "Capacitors_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 5588 5850 50  0001 C CNN
F 3 "" H 5550 6000 50  0001 C CNN
	1    5550 6000
	1    0    0    -1  
$EndComp
Connection ~ 5050 5600
Connection ~ 5550 5600
$Comp
L GND #PWR09
U 1 1 59BF1604
P 5300 6250
F 0 "#PWR09" H 5300 6000 50  0001 C CNN
F 1 "GND" H 5300 6100 50  0000 C CNN
F 2 "" H 5300 6250 50  0001 C CNN
F 3 "" H 5300 6250 50  0001 C CNN
	1    5300 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 6150 5550 6150
Wire Wire Line
	5300 6150 5300 6250
Connection ~ 5300 6150
Text Notes 6050 950  0    60   ~ 0
Audio Amplifier
Wire Wire Line
	5750 1350 5750 1600
$Comp
L BC327 Q3
U 1 1 59BF1DFA
P 5650 1800
F 0 "Q3" H 5850 1875 50  0000 L CNN
F 1 "S9012" H 5850 1800 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Narrow_Oval" H 5850 1725 50  0001 L CIN
F 3 "" H 5650 1800 50  0001 L CNN
	1    5650 1800
	1    0    0    -1  
$EndComp
$Comp
L BC337 Q2
U 1 1 59BF2669
P 5650 1150
F 0 "Q2" H 5850 1225 50  0000 L CNN
F 1 "S9013" H 5850 1150 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Narrow_Oval" H 5850 1075 50  0001 L CIN
F 3 "" H 5650 1150 50  0001 L CNN
	1    5650 1150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 59BF27C2
P 6200 2050
F 0 "#PWR010" H 6200 1800 50  0001 C CNN
F 1 "GND" H 6200 1900 50  0000 C CNN
F 2 "" H 6200 2050 50  0001 C CNN
F 3 "" H 6200 2050 50  0001 C CNN
	1    6200 2050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR011
U 1 1 59BF2800
P 5750 950
F 0 "#PWR011" H 5750 800 50  0001 C CNN
F 1 "VCC" H 5750 1100 50  0000 C CNN
F 2 "" H 5750 950 50  0001 C CNN
F 3 "" H 5750 950 50  0001 C CNN
	1    5750 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 1150 5450 1800
Wire Wire Line
	4500 5150 4950 5150
Wire Wire Line
	4950 5150 4950 1500
Wire Wire Line
	4950 1500 5450 1500
Connection ~ 5450 1500
Wire Wire Line
	5750 1500 6500 1500
Connection ~ 5750 1500
Wire Wire Line
	5750 2000 6500 2000
Wire Wire Line
	6500 2000 6500 1600
Wire Wire Line
	6200 2000 6200 2050
Connection ~ 6200 2000
$Comp
L Speaker LS1
U 1 1 59BF2E68
P 6700 1500
F 0 "LS1" H 6750 1725 50  0000 R CNN
F 1 "Speaker" H 6750 1650 50  0000 R CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 6700 1300 50  0001 C CNN
F 3 "" H 6690 1450 50  0001 C CNN
	1    6700 1500
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J1
U 1 1 59BF3CFE
P 7750 3600
F 0 "J1" H 7750 3700 50  0000 C CNN
F 1 "Motor" H 7750 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 7750 3600 50  0001 C CNN
F 3 "" H 7750 3600 50  0001 C CNN
	1    7750 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 3900 7500 3900
Wire Wire Line
	7500 3900 7500 3600
Wire Wire Line
	7500 3600 7550 3600
Wire Wire Line
	7400 3700 7550 3700
$Comp
L Conn_01x03 J2
U 1 1 59BF3FFB
P 5450 2500
F 0 "J2" H 5450 2700 50  0000 C CNN
F 1 "Pot" H 5450 2300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 5450 2500 50  0001 C CNN
F 3 "" H 5450 2500 50  0001 C CNN
	1    5450 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 1500 3350 1500
Wire Wire Line
	3350 1500 3350 2500
Wire Wire Line
	3350 2500 5250 2500
Wire Wire Line
	4500 3800 4700 3800
Wire Wire Line
	4700 3800 4700 2500
Connection ~ 4700 2500
$Comp
L GND #PWR012
U 1 1 59BF46FB
P 5150 2300
F 0 "#PWR012" H 5150 2050 50  0001 C CNN
F 1 "GND" H 5150 2150 50  0000 C CNN
F 2 "" H 5150 2300 50  0001 C CNN
F 3 "" H 5150 2300 50  0001 C CNN
	1    5150 2300
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR013
U 1 1 59BF473F
P 5150 2700
F 0 "#PWR013" H 5150 2550 50  0001 C CNN
F 1 "VCC" H 5150 2850 50  0000 C CNN
F 2 "" H 5150 2700 50  0001 C CNN
F 3 "" H 5150 2700 50  0001 C CNN
	1    5150 2700
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR014
U 1 1 59BF4AEC
P 3600 6400
F 0 "#PWR014" H 3600 6150 50  0001 C CNN
F 1 "GND" H 3600 6250 50  0000 C CNN
F 2 "" H 3600 6400 50  0001 C CNN
F 3 "" H 3600 6400 50  0001 C CNN
	1    3600 6400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR015
U 1 1 59BF4B33
P 3600 6000
F 0 "#PWR015" H 3600 5850 50  0001 C CNN
F 1 "VCC" H 3600 6150 50  0000 C CNN
F 2 "" H 3600 6000 50  0001 C CNN
F 3 "" H 3600 6000 50  0001 C CNN
	1    3600 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 2600 5150 2600
Wire Wire Line
	5150 2600 5150 2700
Wire Wire Line
	5250 2400 5150 2400
Wire Wire Line
	5150 2400 5150 2300
$Comp
L Battery BT1
U 1 1 59BF528A
P 3600 6200
F 0 "BT1" H 3700 6300 50  0000 L CNN
F 1 "Battery" H 3700 6200 50  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" V 3600 6260 50  0001 C CNN
F 3 "" V 3600 6260 50  0001 C CNN
	1    3600 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 59BF59E2
P 2850 1700
F 0 "#PWR016" H 2850 1450 50  0001 C CNN
F 1 "GND" H 2850 1550 50  0000 C CNN
F 2 "" H 2850 1700 50  0001 C CNN
F 3 "" H 2850 1700 50  0001 C CNN
	1    2850 1700
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x03 J4
U 1 1 59BFF28A
P 5300 4500
F 0 "J4" H 5300 4700 50  0000 C CNN
F 1 "Prog" H 5300 4300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 5300 4500 50  0001 C CNN
F 3 "" H 5300 4500 50  0001 C CNN
	1    5300 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4400 5100 4400
Wire Wire Line
	4500 4550 4700 4550
Wire Wire Line
	4700 4550 4700 4500
Wire Wire Line
	4700 4500 5100 4500
Wire Wire Line
	4500 4650 4700 4650
Wire Wire Line
	4700 4650 4700 4600
Wire Wire Line
	4700 4600 5100 4600
Wire Wire Line
	4500 2950 5700 2950
Wire Wire Line
	5700 3900 6400 3900
Wire Wire Line
	5850 3150 6800 3150
Wire Wire Line
	6800 3150 6800 3300
Connection ~ 5850 4100
$EndSCHEMATC
