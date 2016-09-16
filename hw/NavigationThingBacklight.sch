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
L LED_RGB D1
U 1 1 57D1C253
P 4450 3000
F 0 "D1" H 4450 3425 50  0000 C CNN
F 1 "5050_White" H 4450 3350 50  0000 C CNN
F 2 "LEDs:LED_RGB_PLLC-6" H 4450 2950 50  0001 C CNN
F 3 "" H 4450 2950 50  0000 C CNN
	1    4450 3000
	-1   0    0    1   
$EndComp
$Comp
L LED_RGB D2
U 1 1 57D1C2FF
P 5350 3000
F 0 "D2" H 5350 3425 50  0000 C CNN
F 1 "5050_White" H 5350 3350 50  0000 C CNN
F 2 "LEDs:LED_RGB_PLLC-6" H 5350 2950 50  0001 C CNN
F 3 "" H 5350 2950 50  0000 C CNN
	1    5350 3000
	-1   0    0    1   
$EndComp
$Comp
L R R1
U 1 1 57D1C363
P 4050 2650
F 0 "R1" V 4130 2650 50  0000 C CNN
F 1 "330R" V 4050 2650 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 3980 2650 50  0001 C CNN
F 3 "" H 4050 2650 50  0000 C CNN
	1    4050 2650
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 57D1C3E0
P 5000 2650
F 0 "R2" V 5080 2650 50  0000 C CNN
F 1 "330R" V 5000 2650 50  0000 C CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" V 4930 2650 50  0001 C CNN
F 3 "" H 5000 2650 50  0000 C CNN
	1    5000 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2800 4250 2800
Wire Wire Line
	4200 2800 4200 3200
Wire Wire Line
	4200 3000 4250 3000
Connection ~ 4200 2800
Wire Wire Line
	4200 3200 4250 3200
Connection ~ 4200 3000
Wire Wire Line
	5000 2800 5150 2800
Wire Wire Line
	5100 2800 5100 3200
Wire Wire Line
	5100 3000 5150 3000
Connection ~ 5100 2800
Wire Wire Line
	5100 3200 5150 3200
Connection ~ 5100 3000
Wire Wire Line
	4650 3200 4800 3200
Wire Wire Line
	5550 3200 5700 3200
Wire Wire Line
	5700 3200 5700 2350
Wire Wire Line
	5700 2350 4800 2350
Connection ~ 4800 2350
Wire Wire Line
	5550 3000 5700 3000
Connection ~ 5700 3000
Wire Wire Line
	5550 2800 5700 2800
Wire Wire Line
	5700 2800 5700 2750
Connection ~ 5700 2800
Wire Wire Line
	4650 2800 4800 2800
Connection ~ 4800 2800
Wire Wire Line
	4650 3000 4800 3000
Connection ~ 4800 3000
Wire Wire Line
	4050 2500 5000 2500
Wire Wire Line
	4800 3200 4800 2000
Wire Wire Line
	4050 2500 4050 2000
$Comp
L CONN_01X01 P2
U 1 1 57D1C756
P 4800 1800
F 0 "P2" H 4800 1900 50  0000 C CNN
F 1 "CONN_01X01" V 4900 1800 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 4800 1800 50  0001 C CNN
F 3 "" H 4800 1800 50  0000 C CNN
	1    4800 1800
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 57D1C7C0
P 4050 1800
F 0 "P1" H 4050 1900 50  0000 C CNN
F 1 "CONN_01X01" V 4150 1800 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 4050 1800 50  0001 C CNN
F 3 "" H 4050 1800 50  0000 C CNN
	1    4050 1800
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
