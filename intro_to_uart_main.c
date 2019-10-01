/*
 * This is a simple program to introduce UART.
 * Pushing S2 on the BoosterPack toggles the LED1 on the Launchpad at any time.
 * The program gets characters sent from your Laptop and decides whether it is Number, Letter or Other. Then,
 * it sends back N, L or O to the laptop in response.
 * Pushing S1 changes the baudrate.
 */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef enum {baud9600, baud19200, baud38400, baud57600} UARTBaudRate_t;

// GPIO HAL functions
 void InitializeGPIO();
 void TurnOn_Launchpad_LED1();
 void TurnOff_Launchpad_LED1();
 void Toggle_Launchpad_LED1();
 bool S2Pushed();
 bool S1Pushed();

int main(void) {

    WDT_A_hold(WDT_A_BASE);
    InitializeGPIO();

    eUSCI_UART_Config uartConfig = {
         EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 3MHz
         19,                                           // UCBR   = 19
         8,                                            // UCBRF  = 8
         0x55,                                         // UCBRS  = 0x55
         EUSCI_A_UART_NO_PARITY,                       // No Parity
         EUSCI_A_UART_LSB_FIRST,                       // LSB First
         EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
         EUSCI_A_UART_MODE,                            // UART mode
         EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
    };

    // initialize and enable EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);

    // make sure Tx and Rx pins of EUSCI_A0 work for UART and not a regular GPIO pin
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    UARTBaudRate_t baudRate = baud9600;
    char rChar, tChar;

    while (1)
    {
        if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
                == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
            {
            rChar = UART_receiveData(EUSCI_A0_BASE);

            // Depending on if the received char is a Number, a Letter, or Otherwise, the transmit char is N, L or O
            if (('0'<=rChar) && (rChar <= '9'))
                tChar = 'N';
            else if ((('a'<=rChar) && (rChar <= 'z')) ||
                     (('A'<=rChar) && (rChar <= 'Z')))
                tChar = 'L';
            else
                tChar = 'O';


            if (UART_getInterruptStatus (EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                    == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
                UART_transmitData(EUSCI_A0_BASE, tChar);
            }

        // Pushing S1 changes the baudrate
        if (S1Pushed())
        {
            switch(baudRate)
            {
            case baud9600:
                uartConfig.clockPrescalar = 9;
                uartConfig.firstModReg = 12;
                uartConfig.secondModReg = 0x22;
                UART_initModule(EUSCI_A0_BASE, &uartConfig);
                UART_enableModule(EUSCI_A0_BASE);
                baudRate = baud19200;
                break;

            case baud19200:
                // to complete
                break;

            case baud38400:
                // to complete
                break;
            } // end of switch-case
        } // if S1 pushed

        if (S2Pushed())
            Toggle_Launchpad_LED1();


    } // end of while(1)

}



// GPIO HAL
void TurnOn_Launchpad_LED1()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
void TurnOff_Launchpad_LED1()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void Toggle_Launchpad_LED1()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void TurnOn_Launchpad_LED2()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
}
void TurnOff_Launchpad_LED2()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

#define PRESSED 0
#define RELEASED 1

char SwitchStatus_BoosterPack_S1()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1));
}

char SwitchStatus_BoosterPack_S2()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5));
}

// This is not debounced but it is OK for now
bool S1Pushed() {
    // This is a small state machine that we call the push-button machine
    char cur_status = SwitchStatus_BoosterPack_S1();
    char prev_status = RELEASED;
    bool isPushed = false;

    if ((cur_status == PRESSED) && (prev_status==RELEASED))
        isPushed = true;
    prev_status = cur_status;

    return isPushed;
}

// This is not debounced but it is OK for now
bool S2Pushed() {
    // This is a small state machine that we call the push-button machine
    char cur_status = SwitchStatus_BoosterPack_S2();
    char prev_status = RELEASED;
    bool isPushed = false;

    if ((cur_status == PRESSED) && (prev_status==RELEASED))
        isPushed = true;
    prev_status = cur_status;

    return isPushed;
}
void InitializeGPIO() {

    // S1, S2 on BoosterPack initialization
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN1);

    // LED1 and red LED2 on Launchpad initialization
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
}



