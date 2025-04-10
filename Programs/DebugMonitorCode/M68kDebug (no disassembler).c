#include "DebugMonitor.h"

// use 08030000 for a system running from sram or 0B000000 for system running from dram
#define StartOfExceptionVectorTable 0x08030000
//#define StartOfExceptionVectorTable 0x0B000000

// use 0C000000 for dram or hex 08040000 for sram
#define TopOfStack 0x08040000
//#define TopOfStack 0x0C000000

/* DO NOT INITIALISE GLOBAL VARIABLES - DO IT in MAIN() */
unsigned int i, x, y, z, PortA_Count;
int     Trace, GoFlag, Echo;                       // used in tracing/single stepping

// 68000 register dump and preintialise value (these can be changed by the user program when it is running, e.g. stack pointer, registers etc

unsigned int d0,d1,d2,d3,d4,d5,d6,d7 ;
unsigned int a0,a1,a2,a3,a4,a5,a6 ;
unsigned int PC, SSP, USP ;
unsigned short int SR;

// Breakpoint variables
unsigned int BreakPointAddress[8];                      //array of 8 breakpoint addresses
unsigned short int BreakPointInstruction[8] ;           // to hold the instruction opcode at the breakpoint
unsigned int BreakPointSetOrCleared[8] ;
unsigned int InstructionSize ;

// watchpoint variables
unsigned int WatchPointAddress[8];                      //array of 8 breakpoint addresses
unsigned int WatchPointSetOrCleared[8] ;
int clock_count_ms;

char    TempString[100] ;


void InstallExceptionHandler( void (*function_ptr)(), int level)
{
    volatile long int *RamVectorAddress = (volatile long int *)(StartOfExceptionVectorTable) ;   // pointer to the Ram based interrupt vector table created in Cstart in debug monitor

    RamVectorAddress[level] = (long int *)(function_ptr);
}
/*********************************************************************************************
*Subroutine to initialise the RS232 Port by writing some commands to the internal registers
*********************************************************************************************/
void Init_RS232(void)
{
    RS232_Control = (char)(0x15) ; //  %00010101    divide by 16 clock, set rts low, 8 bits no parity, 1 stop bit transmitter interrupt disabled
    RS232_Baud = (char)(0x1) ;      // program baud rate generator 000 = 230k, 001 = 115k, 010 = 57.6k, 011 = 38.4k, 100 = 19.2, all others = 9600
}

int kbhit(void)
{
    if(((char)(RS232_Status) & (char)(0x01)) == (char)(0x01))    // wait for Rx bit in status register to be '1'
        return 1 ;
    else
        return 0 ;
}

/*********************************************************************************************************
**  Subroutine to provide a low level output function to 6850 ACIA
**  This routine provides the basic functionality to output a single character to the serial Port
**  to allow the board to communicate with HyperTerminal Program
**
**  NOTE you do not call this function directly, instead you call the normal putchar() function
**  which in turn calls _putch() below). Other functions like puts(), printf() call putchar() so will
**  call _putch() also
*********************************************************************************************************/

int _putch( int c)
{
    while(((char)(RS232_Status) & (char)(0x02)) != (char)(0x02))    // wait for Tx bit in status register or 6850 serial comms chip to be '1'
        ;

    (char)(RS232_TxData) = ((char)(c) & (char)(0x7f));                      // write to the data register to output the character (mask off bit 8 to keep it 7 bit ASCII)
    return c ;                                              // putchar() expects the character to be returned
}

/*********************************************************************************************************
**  Subroutine to provide a low level input function to 6850 ACIA
**  This routine provides the basic functionality to input a single character from the serial Port
**  to allow the board to communicate with HyperTerminal Program Keyboard (your PC)
**
**  NOTE you do not call this function directly, instead you call the normal _getch() function
**  which in turn calls _getch() below). Other functions like gets(), scanf() call _getch() so will
**  call _getch() also
*********************************************************************************************************/

int _getch( void )
{
    int c ;
    while(((char)(RS232_Status) & (char)(0x01)) != (char)(0x01))    // wait for Rx bit in 6850 serial comms chip status register to be '1'
        ;

    c = (RS232_RxData & (char)(0x7f));                   // read received character, mask off top bit and return as 7 bit ASCII character

    // shall we echo the character? Echo is set to TRUE at reset, but for speed we don't want to echo when downloading code with the 'L' debugger command
    if(Echo)
        _putch(c);

    return c ;
}

// flush the input stream for any unread characters

void FlushKeyboard(void)
{
    char c ;

    while(1)    {
        if(((char)(RS232_Status) & (char)(0x01)) == (char)(0x01))    // if Rx bit in status register is '1'
            c = ((char)(RS232_RxData) & (char)(0x7f)) ;
        else
            return ;
     }
}

// converts hex char to 4 bit binary equiv in range 0000-1111 (0-F)
// char assumed to be a valid hex char 0-9, a-f, A-F

char xtod(int c)
{
    if ((char)(c) <= (char)('9'))
        return c - (char)(0x30);    // 0 - 9 = 0x30 - 0x39 so convert to number by sutracting 0x30
    else if((char)(c) > (char)('F'))    // assume lower case
        return c - (char)(0x57);    // a-f = 0x61-66 so needs to be converted to 0x0A - 0x0F so subtract 0x57
    else
        return c - (char)(0x37);    // A-F = 0x41-46 so needs to be converted to 0x0A - 0x0F so subtract 0x37
}

int Get2HexDigits(char *CheckSumPtr)
{
    register int i = (xtod(_getch()) << 4) | (xtod(_getch()));

    if(CheckSumPtr)
        *CheckSumPtr += i ;

    return i ;
}

int Get4HexDigits(char *CheckSumPtr)
{
    return (Get2HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
}

int Get6HexDigits(char *CheckSumPtr)
{
    return (Get4HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
}

int Get8HexDigits(char *CheckSumPtr)
{
    return (Get4HexDigits(CheckSumPtr) << 16) | (Get4HexDigits(CheckSumPtr));
}


void UnknownCommand()
{
    printf("\r\nUnknown Command.....\r\n") ;
    Help() ;
}

// system when the users program executes a TRAP #15 instruction to halt program and return to debug monitor

void CallDebugMonitor(void)
{
    printf("\r\nProgram Ended (TRAP #15)....") ;
    menu();
}

void Help(void)
{
    char *banner = "\r\n----------------------------------------------------------------" ;

    printf(banner) ;
    printf("\r\n  Final Project") ;
    printf(banner) ;
    printf("\r\n  T    - Run Cosmic Impala Game") ;
	printf(banner) ;
    printf("\r\n  G    - Run Graphics Test") ;
    printf(banner) ;
}


void menu(void)
{
    char c;
	int c1 ;

    while(1)    {
        FlushKeyboard() ;               // dump unread characters from keyboard
        printf("\r\n#") ;
        c = toupper(_getch());


        if ( c == (char)('T'))  {
               printf("\nRunning Cosmic Impalas Game\n");
		       cosmic_impalas_main();
			   continue;
		} 
		
		if ( c == (char)('G'))  {
               printf("\nRunning Graphics Test\n");
		       graphics_test_main();
			   continue;
		} 
		
        UnknownCommand() ;
    }
}

void PrintErrorMessageandAbort(char *string) {
    printf("\r\n\r\nProgram ABORT !!!!!!\r\n") ;
    printf("%s\r\n", string) ;
    menu() ;
}

void IRQMessage(int level) {
     printf("\r\n\r\nProgram ABORT !!!!!");
     printf("\r\nUnhandled Interrupt: IRQ%d !!!!!", level) ;
     menu() ;
}

void UnhandledIRQ1(void) {
     IRQMessage(1);
}

void UnhandledIRQ2(void) {
    IRQMessage(2);
}

void UnhandledIRQ3(void){
    IRQMessage(3);
}

void UnhandledIRQ4(void) {
     IRQMessage(4);
}

void UnhandledIRQ5(void) {
    IRQMessage(5);
}

void UnhandledIRQ6(void) {
    PrintErrorMessageandAbort("ADDRESS ERROR: 16 or 32 Bit Transfer to/from an ODD Address....") ;
    menu() ;
}

void UnhandledIRQ7(void) {
    IRQMessage(7);
}

void UnhandledTrap(void) {
    PrintErrorMessageandAbort("Unhandled Trap !!!!!") ;
}

void BusError() {
   PrintErrorMessageandAbort("BUS Error!") ;
}

void AddressError() {
   PrintErrorMessageandAbort("ADDRESS Error!") ;
}

void IllegalInstruction() {
    PrintErrorMessageandAbort("ILLEGAL INSTRUCTION") ;
}

void Dividebyzero() {
    PrintErrorMessageandAbort("DIVIDE BY ZERO") ;
}

void Check() {
   PrintErrorMessageandAbort("'CHK' INSTRUCTION") ;
}

void Trapv() {
   PrintErrorMessageandAbort("TRAPV INSTRUCTION") ;
}

void PrivError() {
    PrintErrorMessageandAbort("PRIVILEGE VIOLATION") ;
}

void UnitIRQ() {
    PrintErrorMessageandAbort("UNINITIALISED IRQ") ;
}

void Spurious() {
    PrintErrorMessageandAbort("SPURIOUS IRQ") ;
}

/*********************************************************************************
** Timer ISR
**********************************************************************************/
void Timer_ISR(void)
{
   	if(Timer1Status == 1) {       // Did Timer 1 produce the Interrupt?
		clock_count_ms = clock_count_ms + 10; //100 HZ clock = 10ms per clock tick
		//printf("in timer isr, clock_count_ms = %d\n",clock_count_ms);
   	    Timer1Control = 3;      	// if so clear interrupt and restart timer
   	}
}

/**********************************************************************************
** Timer Initialisation Routine
**********************************************************************************/
void Timer1_Init(void)
{
    Timer1Data = 0x03;		// program 100 hz time delay into timer 1.

/*
** timer driven off 25Mhz clock so program value so that it counts down in 0.01 secs
** the example 0x03 above is loaded into top 8 bits of a 24 bit timer so reads as
** 0x03FFFF a value of 0x03 would be 262,143/25,000,000, so is close to 1/100th sec
**
**
** Now write binary 00000011 to timer control register:
**	Bit0 = 1 (enable interrupt from that timer)
**	Bit 1 = 1 enable counting
*/

    Timer1Control = 3;
}



void main(void)
{

	char *BugMessage = "DE1-68k 15/11/2024 14:44";
    char *CopyrightMessage = "Copyright (C) PJ Davies 2016";
    char *nameAndStudentNumber = "Yair Linn";

	clock_count_ms = 0;
	Init_RS232() ;     // initialise the RS232 port

	InstallExceptionHandler(Timer_ISR, 25) ;		      // install handler for interrupts
    InstallExceptionHandler(Timer_ISR, 26) ;		      // install handler for interrupts
    InstallExceptionHandler(Timer_ISR, 27) ;		      // install handler for interrupts
    InstallExceptionHandler(Timer_ISR, 28) ;		      // install handler for interrupts
    InstallExceptionHandler(Timer_ISR, 29) ;		      // install handler for interrupts
    InstallExceptionHandler(Timer_ISR, 30) ;		      // install handler for interrupts
    InstallExceptionHandler(UnhandledIRQ7, 31) ;		      // install handler for interrupts
    InstallExceptionHandler(BusError,2) ;                          // install Bus error handler
    InstallExceptionHandler(AddressError,3) ;                      // install address error handler (doesn't work on soft core 68k implementation)
    InstallExceptionHandler(IllegalInstruction,4) ;                // install illegal instruction exception handler
    InstallExceptionHandler(Dividebyzero,5) ;                      // install /0 exception handler
    InstallExceptionHandler(Check,6) ;                             // install check instruction exception handler
    InstallExceptionHandler(Trapv,7) ;                             // install trapv instruction exception handler
    InstallExceptionHandler(PrivError,8) ;                         // install Priv Violation exception handler
    InstallExceptionHandler(UnitIRQ,15) ;                          // install uninitialised IRQ exception handler
    InstallExceptionHandler(Check,24) ;                            // install spurious IRQ exception handler
    Timer1_Init();


    FlushKeyboard() ;                        // dump unread characters from keyboard
    TraceException = 0 ;                     // clear trace exception port to remove any software generated single step/trace

    printf("\r\n%s", BugMessage) ;
    printf("\r\n%s", CopyrightMessage) ;
    printf("\r\n%s", nameAndStudentNumber);

    menu();

}