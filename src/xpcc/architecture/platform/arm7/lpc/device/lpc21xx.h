/**
 * @file	lpc21xx.h
 * @brief	Header file for Philips LPC2114 / LPC2119 / LPC2124 / LPC2129 / LPC2194
 * 
 */

#ifndef LPC21xx_REGISTERS_H
#define LPC21xx_REGISTERS_H

/* Vectored Interrupt Controller (VIC) */
#define VICIRQStatus   (*((volatile unsigned long *) 0xFFFFF000))
#define VICFIQStatus   (*((volatile unsigned long *) 0xFFFFF004))
#define VICRawIntr     (*((volatile unsigned long *) 0xFFFFF008))
#define VICIntSelect   (*((volatile unsigned long *) 0xFFFFF00C))
#define VICIntEnable   (*((volatile unsigned long *) 0xFFFFF010))
#define VICIntEnClr    (*((volatile unsigned long *) 0xFFFFF014))
#define VICSoftInt     (*((volatile unsigned long *) 0xFFFFF018))
#define VICSoftIntClr  (*((volatile unsigned long *) 0xFFFFF01C))
#define VICProtection  (*((volatile unsigned long *) 0xFFFFF020))
#define VICVectAddr    (*((volatile unsigned long *) 0xFFFFF030))
#define VICDefVectAddr (*((volatile unsigned long *) 0xFFFFF034))

#define VICVectAddr0   (*((volatile unsigned long *) 0xFFFFF100))
#define VICVectAddr1   (*((volatile unsigned long *) 0xFFFFF104))
#define VICVectAddr2   (*((volatile unsigned long *) 0xFFFFF108))
#define VICVectAddr3   (*((volatile unsigned long *) 0xFFFFF10C))
#define VICVectAddr4   (*((volatile unsigned long *) 0xFFFFF110))
#define VICVectAddr5   (*((volatile unsigned long *) 0xFFFFF114))
#define VICVectAddr6   (*((volatile unsigned long *) 0xFFFFF118))
#define VICVectAddr7   (*((volatile unsigned long *) 0xFFFFF11C))
#define VICVectAddr8   (*((volatile unsigned long *) 0xFFFFF120))
#define VICVectAddr9   (*((volatile unsigned long *) 0xFFFFF124))
#define VICVectAddr10  (*((volatile unsigned long *) 0xFFFFF128))
#define VICVectAddr11  (*((volatile unsigned long *) 0xFFFFF12C))
#define VICVectAddr12  (*((volatile unsigned long *) 0xFFFFF130))
#define VICVectAddr13  (*((volatile unsigned long *) 0xFFFFF134))
#define VICVectAddr14  (*((volatile unsigned long *) 0xFFFFF138))
#define VICVectAddr15  (*((volatile unsigned long *) 0xFFFFF13C))

#define VICVectCntl0   (*((volatile unsigned long *) 0xFFFFF200))
#define VICVectCntl1   (*((volatile unsigned long *) 0xFFFFF204))
#define VICVectCntl2   (*((volatile unsigned long *) 0xFFFFF208))
#define VICVectCntl3   (*((volatile unsigned long *) 0xFFFFF20C))
#define VICVectCntl4   (*((volatile unsigned long *) 0xFFFFF210))
#define VICVectCntl5   (*((volatile unsigned long *) 0xFFFFF214))
#define VICVectCntl6   (*((volatile unsigned long *) 0xFFFFF218))
#define VICVectCntl7   (*((volatile unsigned long *) 0xFFFFF21C))
#define VICVectCntl8   (*((volatile unsigned long *) 0xFFFFF220))
#define VICVectCntl9   (*((volatile unsigned long *) 0xFFFFF224))
#define VICVectCntl10  (*((volatile unsigned long *) 0xFFFFF228))
#define VICVectCntl11  (*((volatile unsigned long *) 0xFFFFF22C))
#define VICVectCntl12  (*((volatile unsigned long *) 0xFFFFF230))
#define VICVectCntl13  (*((volatile unsigned long *) 0xFFFFF234))
#define VICVectCntl14  (*((volatile unsigned long *) 0xFFFFF238))
#define VICVectCntl15  (*((volatile unsigned long *) 0xFFFFF23C))

/* Pin Connect Block */
#define PINSEL0        (*((volatile unsigned long *) 0xE002C000))
#define PINSEL1        (*((volatile unsigned long *) 0xE002C004))
#define PINSEL2        (*((volatile unsigned long *) 0xE002C014))

/* General Purpose Input/Output (GPIO) */
#define GPIO_BASE_ADDR		0xE0028000
#define IOPIN0         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x00))
#define IOSET0         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x04))
#define IODIR0         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x08))
#define IOCLR0         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x0C))

#define IOPIN1         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x10))
#define IOSET1         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x14))
#define IODIR1         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x18))
#define IOCLR1         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x1C))

#define IOPIN2         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x20))
#define IOSET2         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x24))
#define IODIR2         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x28))
#define IOCLR2         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x2C))

#define IOPIN3         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x30))
#define IOSET3         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x34))
#define IODIR3         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x38))
#define IOCLR3         (*(volatile unsigned long *)(GPIO_BASE_ADDR + 0x3C))

/* Memory Accelerator Module (MAM) */
#define MAMCR          (*((volatile unsigned char *) 0xE01FC000))
#define MAMTIM         (*((volatile unsigned char *) 0xE01FC004))
#define MEMMAP         (*((volatile unsigned char *) 0xE01FC040))

/* Phase Locked Loop (PLL) */
#define PLLCON         (*((volatile unsigned char *) 0xE01FC080))
#define PLLCFG         (*((volatile unsigned char *) 0xE01FC084))
#define PLLSTAT        (*((volatile unsigned short*) 0xE01FC088))
#define PLLFEED        (*((volatile unsigned char *) 0xE01FC08C))

/* VPB Divider */
#define VPBDIV         (*((volatile unsigned char *) 0xE01FC100))

/* Power Control */
#define PCON           (*((volatile unsigned char *) 0xE01FC0C0))
#define PCONP          (*((volatile unsigned long *) 0xE01FC0C4))

/* External Interrupts */
#define EXTINT         (*((volatile unsigned char *) 0xE01FC140))
#define EXTWAKE        (*((volatile unsigned char *) 0xE01FC144))	// LPC213x: INTWAKE
#define EXTMODE        (*((volatile unsigned char *) 0xE01FC148))
#define EXTPOLAR       (*((volatile unsigned char *) 0xE01FC14C))

/* LPC213x: Reset */
#define RSID           (*((volatile unsigned char *) 0xE01FC180))

/* LPC213x:Code Security / Debugging */
#define CSPR           (*((volatile unsigned char *) 0xE01FC184))

/* Timer 0 */
#define T0IR           (*((volatile unsigned long *) 0xE0004000))
#define T0TCR          (*((volatile unsigned long *) 0xE0004004))
#define T0TC           (*((volatile unsigned long *) 0xE0004008))
#define T0PR           (*((volatile unsigned long *) 0xE000400C))
#define T0PC           (*((volatile unsigned long *) 0xE0004010))
#define T0MCR          (*((volatile unsigned long *) 0xE0004014))
#define T0MR0          (*((volatile unsigned long *) 0xE0004018))
#define T0MR1          (*((volatile unsigned long *) 0xE000401C))
#define T0MR2          (*((volatile unsigned long *) 0xE0004020))
#define T0MR3          (*((volatile unsigned long *) 0xE0004024))
#define T0CCR          (*((volatile unsigned long *) 0xE0004028))
#define T0CR0          (*((volatile unsigned long *) 0xE000402C))
#define T0CR1          (*((volatile unsigned long *) 0xE0004030))
#define T0CR2          (*((volatile unsigned long *) 0xE0004034))
#define T0CR3          (*((volatile unsigned long *) 0xE0004038))
#define T0EMR          (*((volatile unsigned long *) 0xE000403C))
// LPC213x: T0CTCR

/* Timer 1 */
#define T1IR           (*((volatile unsigned long *) 0xE0008000))
#define T1TCR          (*((volatile unsigned long *) 0xE0008004))
#define T1TC           (*((volatile unsigned long *) 0xE0008008))
#define T1PR           (*((volatile unsigned long *) 0xE000800C))
#define T1PC           (*((volatile unsigned long *) 0xE0008010))
#define T1MCR          (*((volatile unsigned long *) 0xE0008014))
#define T1MR0          (*((volatile unsigned long *) 0xE0008018))
#define T1MR1          (*((volatile unsigned long *) 0xE000801C))
#define T1MR2          (*((volatile unsigned long *) 0xE0008020))
#define T1MR3          (*((volatile unsigned long *) 0xE0008024))
#define T1CCR          (*((volatile unsigned long *) 0xE0008028))
#define T1CR0          (*((volatile unsigned long *) 0xE000802C))
#define T1CR1          (*((volatile unsigned long *) 0xE0008030))
#define T1CR2          (*((volatile unsigned long *) 0xE0008034))
#define T1CR3          (*((volatile unsigned long *) 0xE0008038))
#define T1EMR          (*((volatile unsigned long *) 0xE000803C))
// LPC213x: T1CTCR

/* Pulse Width Modulator (PWM) */
#define PWMIR          (*((volatile unsigned long *) 0xE0014000))
#define PWMTCR         (*((volatile unsigned long *) 0xE0014004))
#define PWMTC          (*((volatile unsigned long *) 0xE0014008))
#define PWMPR          (*((volatile unsigned long *) 0xE001400C))
#define PWMPC          (*((volatile unsigned long *) 0xE0014010))
#define PWMMCR         (*((volatile unsigned long *) 0xE0014014))
#define PWMMR0         (*((volatile unsigned long *) 0xE0014018))
#define PWMMR1         (*((volatile unsigned long *) 0xE001401C))
#define PWMMR2         (*((volatile unsigned long *) 0xE0014020))
#define PWMMR3         (*((volatile unsigned long *) 0xE0014024))
#define PWMMR4         (*((volatile unsigned long *) 0xE0014040))
#define PWMMR5         (*((volatile unsigned long *) 0xE0014044))
#define PWMMR6         (*((volatile unsigned long *) 0xE0014048))
#define PWMCCR         (*((volatile unsigned long *) 0xE0014028))	// LPC213x: not available
#define PWMCR0         (*((volatile unsigned long *) 0xE001402C))	// LPC213x: not available
#define PWMCR1         (*((volatile unsigned long *) 0xE0014030))	// LPC213x: not available
#define PWMCR2         (*((volatile unsigned long *) 0xE0014034))	// LPC213x: not available
#define PWMCR3         (*((volatile unsigned long *) 0xE0014038))	// LPC213x: not available
#define PWMEMR         (*((volatile unsigned long *) 0xE001403C))
#define PWMPCR         (*((volatile unsigned long *) 0xE001404C))
#define PWMLER         (*((volatile unsigned long *) 0xE0014050))

/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define U0RBR          (*((volatile unsigned char *) 0xE000C000))
#define U0THR          (*((volatile unsigned char *) 0xE000C000))
#define U0IER          (*((volatile unsigned char *) 0xE000C004))
#define U0IIR          (*((volatile unsigned char *) 0xE000C008))
#define U0FCR          (*((volatile unsigned char *) 0xE000C008))
#define U0LCR          (*((volatile unsigned char *) 0xE000C00C))
#define U0MCR          (*((volatile unsigned char *) 0xE000C010))	// LPC213x: not available
#define U0LSR          (*((volatile unsigned char *) 0xE000C014))
#define U0MSR          (*((volatile unsigned char *) 0xE000C018))
#define U0SCR          (*((volatile unsigned char *) 0xE000C01C))
#define U0DLL          (*((volatile unsigned char *) 0xE000C000))
#define U0DLM          (*((volatile unsigned char *) 0xE000C004))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define U1RBR          (*((volatile unsigned char *) 0xE0010000))
#define U1THR          (*((volatile unsigned char *) 0xE0010000))
#define U1IER          (*((volatile unsigned char *) 0xE0010004))
#define U1IIR          (*((volatile unsigned char *) 0xE0010008))
#define U1FCR          (*((volatile unsigned char *) 0xE0010008))
#define U1LCR          (*((volatile unsigned char *) 0xE001000C))
#define U1MCR          (*((volatile unsigned char *) 0xE0010010))
#define U1LSR          (*((volatile unsigned char *) 0xE0010014))
#define U1MSR          (*((volatile unsigned char *) 0xE0010018))
#define U1SCR          (*((volatile unsigned char *) 0xE001001C))
#define U1DLL          (*((volatile unsigned char *) 0xE0010000))
#define U1DLM          (*((volatile unsigned char *) 0xE0010004))
// LPC213x: #define U1TER          (*((volatile unsigned char *) 0xE0010030))

/* I2C Interface */
#define I2CONSET       (*((volatile unsigned char *) 0xE001C000))
#define I2STAT         (*((volatile unsigned char *) 0xE001C004))
#define I2DAT          (*((volatile unsigned char *) 0xE001C008))
#define I2ADR          (*((volatile unsigned char *) 0xE001C00C))
#define I2SCLH         (*((volatile unsigned short*) 0xE001C010))
#define I2SCLL         (*((volatile unsigned short*) 0xE001C014))
#define I2CONCLR       (*((volatile unsigned char *) 0xE001C018))

/* SPI0 (Serial Peripheral Interface 0) */
#define S0SPCR         (*((volatile unsigned char *) 0xE0020000))
#define S0SPSR         (*((volatile unsigned char *) 0xE0020004))
#define S0SPDR         (*((volatile unsigned char *) 0xE0020008))
#define S0SPCCR        (*((volatile unsigned char *) 0xE002000C))
#define S0SPTCR        (*((volatile unsigned char *) 0xE0020010))
#define S0SPTSR        (*((volatile unsigned char *) 0xE0020014))
#define S0SPTOR        (*((volatile unsigned char *) 0xE0020018))
#define S0SPINT        (*((volatile unsigned char *) 0xE002001C))

/* SPI1 (Serial Peripheral Interface 1) */
#define S1SPCR         (*((volatile unsigned char *) 0xE0030000))
#define S1SPSR         (*((volatile unsigned char *) 0xE0030004))
#define S1SPDR         (*((volatile unsigned char *) 0xE0030008))
#define S1SPCCR        (*((volatile unsigned char *) 0xE003000C))
#define S1SPTCR        (*((volatile unsigned char *) 0xE0030010))
#define S1SPTSR        (*((volatile unsigned char *) 0xE0030014))
#define S1SPTOR        (*((volatile unsigned char *) 0xE0030018))
#define S1SPINT        (*((volatile unsigned char *) 0xE003001C))

/* Real Time Clock */
#define ILR            (*((volatile unsigned char *) 0xE0024000))
#define CTC            (*((volatile unsigned short*) 0xE0024004))
#define CCR            (*((volatile unsigned char *) 0xE0024008))
#define CIIR           (*((volatile unsigned char *) 0xE002400C))
#define AMR            (*((volatile unsigned char *) 0xE0024010))
#define CTIME0         (*((volatile unsigned long *) 0xE0024014))
#define CTIME1         (*((volatile unsigned long *) 0xE0024018))
#define CTIME2         (*((volatile unsigned long *) 0xE002401C))
#define SEC            (*((volatile unsigned char *) 0xE0024020))
#define MIN            (*((volatile unsigned char *) 0xE0024024))
#define HOUR           (*((volatile unsigned char *) 0xE0024028))
#define DOM            (*((volatile unsigned char *) 0xE002402C))
#define DOW            (*((volatile unsigned char *) 0xE0024030))
#define DOY            (*((volatile unsigned short*) 0xE0024034))
#define MONTH          (*((volatile unsigned char *) 0xE0024038))
#define YEAR           (*((volatile unsigned short*) 0xE002403C))
#define ALSEC          (*((volatile unsigned char *) 0xE0024060))
#define ALMIN          (*((volatile unsigned char *) 0xE0024064))
#define ALHOUR         (*((volatile unsigned char *) 0xE0024068))
#define ALDOM          (*((volatile unsigned char *) 0xE002406C))
#define ALDOW          (*((volatile unsigned char *) 0xE0024070))
#define ALDOY          (*((volatile unsigned short*) 0xE0024074))
#define ALMON          (*((volatile unsigned char *) 0xE0024078))
#define ALYEAR         (*((volatile unsigned short*) 0xE002407C))
#define PREINT         (*((volatile unsigned short*) 0xE0024080))
#define PREFRAC        (*((volatile unsigned short*) 0xE0024084))

/* A/D Converter */
#define ADCR           (*((volatile unsigned long *) 0xE0034000))
#define ADDR           (*((volatile unsigned long *) 0xE0034004))

/* CAN Acceptance Filter RAM */
#define AFRAM          (*((volatile unsigned long *) 0xE0038000))

/* CAN Acceptance Filter */
#define AFMR           (*((volatile unsigned long *) 0xE003C000))
#define SFF_sa         (*((volatile unsigned long *) 0xE003C004))
#define SFF_GRP_sa     (*((volatile unsigned long *) 0xE003C008))
#define EFF_sa         (*((volatile unsigned long *) 0xE003C00C))
#define EFF_GRP_sa     (*((volatile unsigned long *) 0xE003C010))
#define ENDofTable     (*((volatile unsigned long *) 0xE003C014))
#define LUTerrAd       (*((volatile unsigned long *) 0xE003C018))
#define LUTerr         (*((volatile unsigned long *) 0xE003C01C))

/* CAN Central Registers */
#define CANTxSR        (*((volatile unsigned long *) 0xE0040000))
#define CANRxSR        (*((volatile unsigned long *) 0xE0040004))
#define CANMSR         (*((volatile unsigned long *) 0xE0040008))

/* CAN Controller 1 (CAN1) */
#define C1MOD          (*((volatile unsigned long *) 0xE0044000))
#define C1CMR          (*((volatile unsigned long *) 0xE0044004))
#define C1GSR          (*((volatile unsigned long *) 0xE0044008))
#define C1ICR          (*((volatile unsigned long *) 0xE004400C))
#define C1IER          (*((volatile unsigned long *) 0xE0044010))
#define C1BTR          (*((volatile unsigned long *) 0xE0044014))
#define C1EWL          (*((volatile unsigned long *) 0xE0044018))
#define C1SR           (*((volatile unsigned long *) 0xE004401C))
#define C1RFS          (*((volatile unsigned long *) 0xE0044020))
#define C1RID          (*((volatile unsigned long *) 0xE0044024))
#define C1RDA          (*((volatile unsigned long *) 0xE0044028))
#define C1RDB          (*((volatile unsigned long *) 0xE004402C))
#define C1TFI1         (*((volatile unsigned long *) 0xE0044030))
#define C1TID1         (*((volatile unsigned long *) 0xE0044034))
#define C1TDA1         (*((volatile unsigned long *) 0xE0044038))
#define C1TDB1         (*((volatile unsigned long *) 0xE004403C))
#define C1TFI2         (*((volatile unsigned long *) 0xE0044040))
#define C1TID2         (*((volatile unsigned long *) 0xE0044044))
#define C1TDA2         (*((volatile unsigned long *) 0xE0044048))
#define C1TDB2         (*((volatile unsigned long *) 0xE004404C))
#define C1TFI3         (*((volatile unsigned long *) 0xE0044050))
#define C1TID3         (*((volatile unsigned long *) 0xE0044054))
#define C1TDA3         (*((volatile unsigned long *) 0xE0044058))
#define C1TDB3         (*((volatile unsigned long *) 0xE004405C))

/* CAN Controller 2 (CAN2) */
#define C2MOD          (*((volatile unsigned long *) 0xE0048000))
#define C2CMR          (*((volatile unsigned long *) 0xE0048004))
#define C2GSR          (*((volatile unsigned long *) 0xE0048008))
#define C2ICR          (*((volatile unsigned long *) 0xE004800C))
#define C2IER          (*((volatile unsigned long *) 0xE0048010))
#define C2BTR          (*((volatile unsigned long *) 0xE0048014))
#define C2EWL          (*((volatile unsigned long *) 0xE0048018))
#define C2SR           (*((volatile unsigned long *) 0xE004801C))
#define C2RFS          (*((volatile unsigned long *) 0xE0048020))
#define C2RID          (*((volatile unsigned long *) 0xE0048024))
#define C2RDA          (*((volatile unsigned long *) 0xE0048028))
#define C2RDB          (*((volatile unsigned long *) 0xE004802C))
#define C2TFI1         (*((volatile unsigned long *) 0xE0048030))
#define C2TID1         (*((volatile unsigned long *) 0xE0048034))
#define C2TDA1         (*((volatile unsigned long *) 0xE0048038))
#define C2TDB1         (*((volatile unsigned long *) 0xE004803C))
#define C2TFI2         (*((volatile unsigned long *) 0xE0048040))
#define C2TID2         (*((volatile unsigned long *) 0xE0048044))
#define C2TDA2         (*((volatile unsigned long *) 0xE0048048))
#define C2TDB2         (*((volatile unsigned long *) 0xE004804C))
#define C2TFI3         (*((volatile unsigned long *) 0xE0048050))
#define C2TID3         (*((volatile unsigned long *) 0xE0048054))
#define C2TDA3         (*((volatile unsigned long *) 0xE0048058))
#define C2TDB3         (*((volatile unsigned long *) 0xE004805C))

/* CAN Controller 3 (CAN3) */
#define C3MOD          (*((volatile unsigned long *) 0xE004C000))
#define C3CMR          (*((volatile unsigned long *) 0xE004C004))
#define C3GSR          (*((volatile unsigned long *) 0xE004C008))
#define C3ICR          (*((volatile unsigned long *) 0xE004C00C))
#define C3IER          (*((volatile unsigned long *) 0xE004C010))
#define C3BTR          (*((volatile unsigned long *) 0xE004C014))
#define C3EWL          (*((volatile unsigned long *) 0xE004C018))
#define C3SR           (*((volatile unsigned long *) 0xE004C01C))
#define C3RFS          (*((volatile unsigned long *) 0xE004C020))
#define C3RID          (*((volatile unsigned long *) 0xE004C024))
#define C3RDA          (*((volatile unsigned long *) 0xE004C028))
#define C3RDB          (*((volatile unsigned long *) 0xE004C02C))
#define C3TFI1         (*((volatile unsigned long *) 0xE004C030))
#define C3TID1         (*((volatile unsigned long *) 0xE004C034))
#define C3TDA1         (*((volatile unsigned long *) 0xE004C038))
#define C3TDB1         (*((volatile unsigned long *) 0xE004C03C))
#define C3TFI2         (*((volatile unsigned long *) 0xE004C040))
#define C3TID2         (*((volatile unsigned long *) 0xE004C044))
#define C3TDA2         (*((volatile unsigned long *) 0xE004C048))
#define C3TDB2         (*((volatile unsigned long *) 0xE004C04C))
#define C3TFI3         (*((volatile unsigned long *) 0xE004C050))
#define C3TID3         (*((volatile unsigned long *) 0xE004C054))
#define C3TDA3         (*((volatile unsigned long *) 0xE004C058))
#define C3TDB3         (*((volatile unsigned long *) 0xE004C05C))

/* CAN Controller 4 (CAN4) */
#define C4MOD          (*((volatile unsigned long *) 0xE0050000))
#define C4CMR          (*((volatile unsigned long *) 0xE0050004))
#define C4GSR          (*((volatile unsigned long *) 0xE0050008))
#define C4ICR          (*((volatile unsigned long *) 0xE005000C))
#define C4IER          (*((volatile unsigned long *) 0xE0050010))
#define C4BTR          (*((volatile unsigned long *) 0xE0050014))
#define C4EWL          (*((volatile unsigned long *) 0xE0050018))
#define C4SR           (*((volatile unsigned long *) 0xE005001C))
#define C4RFS          (*((volatile unsigned long *) 0xE0050020))
#define C4RID          (*((volatile unsigned long *) 0xE0050024))
#define C4RDA          (*((volatile unsigned long *) 0xE0050028))
#define C4RDB          (*((volatile unsigned long *) 0xE005002C))
#define C4TFI1         (*((volatile unsigned long *) 0xE0050030))
#define C4TID1         (*((volatile unsigned long *) 0xE0050034))
#define C4TDA1         (*((volatile unsigned long *) 0xE0050038))
#define C4TDB1         (*((volatile unsigned long *) 0xE005003C))
#define C4TFI2         (*((volatile unsigned long *) 0xE0050040))
#define C4TID2         (*((volatile unsigned long *) 0xE0050044))
#define C4TDA2         (*((volatile unsigned long *) 0xE0050048))
#define C4TDB2         (*((volatile unsigned long *) 0xE005004C))
#define C4TFI3         (*((volatile unsigned long *) 0xE0050050))
#define C4TID3         (*((volatile unsigned long *) 0xE0050054))
#define C4TDA3         (*((volatile unsigned long *) 0xE0050058))
#define C4TDB3         (*((volatile unsigned long *) 0xE005005C))

/* Watchdog */
#define WDMOD          (*((volatile unsigned char *) 0xE0000000))
#define WDTC           (*((volatile unsigned long *) 0xE0000004))
#define WDFEED         (*((volatile unsigned char *) 0xE0000008))
#define WDTV           (*((volatile unsigned long *) 0xE000000C))

#endif  // LPC21xx_REGISTERS_H
