// #include <LPC23xx.h> // LPC2378 microcontroller register definitions


// FIODIR2 = 0xFF;
// 		FIOPIN2 = 0x9;
// 		FIODIR1 = 0x1;
// 		//FIO0SET = 0XFFFEFFFF;
// 		FIOMASK1 = 0XFFFFFFFE;
// 		FIOPIN1 = 0x1;
// 		
// 		
// // Function to transfer one byte of data over SPI and receive a byte back
// int SPI0_Transfer(int data) {
//     S0SPDR = data;                  // Write data to SPI Data Register
//     while (!(S0SPSR & (1 << 7)));   // Wait for transfer to complete (SPIF flag)
//     return S0SPDR;                  // Return received data
// }

// // Function to select the PN532 by pulling SS (chip select) low
// void PN532_Select(void) {
//     FIOCLR0 = (1 << PN532_SSEL_PIN); // Pull SSEL low to select PN532
// }

// // Function to deselect the PN532 by pulling SS (chip select) high
// void PN532_Deselect(void) {
//     FIOSET0 = (1 << PN532_SSEL_PIN); // Pull SSEL high to deselect PN532
// }

// // Function to write a command to the PN532
// void PN532_WriteCommand(int *cmd, int cmd_len) {
//     PN532_Select();

//     SPI0_Transfer(0x01);         // Preamble
//     SPI0_Transfer(0x00);         // Start Code
//     SPI0_Transfer(0xFF);         // Start Code
//     SPI0_Transfer(cmd_len + 1);  // Length byte
//     SPI0_Transfer(~(cmd_len + 1) + 1); // Checksum for length

//     int checksum = 0;
//     SPI0_Transfer(0xD4);         // Host to PN532 Command Code
//     checksum += 0xD4;

//     for (int i = 0; i < cmd_len; i++) {
//         SPI0_Transfer(cmd[i]);  // Send each byte of the command
//         checksum += cmd[i];
//     }

//     SPI0_Transfer(~checksum + 1); // Checksum byte
//     SPI0_Transfer(0x00);          // Postamble

//     PN532_Deselect();
// }

// // Function to read a response from the PN532
// int PN532_ReadResponse(int *response, int response_len) {
//     PN532_Select();

//     // Wait for Preamble byte (0x01), retry until found or timeout
//     if (SPI0_Transfer(0x00) != 0x01) {  
//         PN532_Deselect();
//         return 0; // Error or no response
//     }
//     SPI0_Transfer(0x00); // Start Code
//     SPI0_Transfer(0xFF); // Start Code

//     int len = SPI0_Transfer(0x00);     // Length byte
//     SPI0_Transfer(0xFF - len + 1);         // Checksum for length

//     int checksum = 0;
//     for (int i = 0; i < len - 1; i++) {
//         response[i] = SPI0_Transfer(0x00); // Read each byte of the response
//         checksum += response[i];
//     }

//     int response_checksum = SPI0_Transfer(0x00);
//     PN532_Deselect();

//     if (((checksum + response_checksum) & 0xFF) != 0) {
//         return 0; // Checksum error
//     }
//     return len - 1; // Return length of valid data
// }

// // Function to send the "Get Firmware Version" command to the PN532
// void PN532_GetFirmwareVersion(void) {
//     int cmd[] = {0x02}; // Command code for Get Firmware Version
//     PN532_WriteCommand(cmd, sizeof(cmd));

//     int response[12];   // Buffer to hold the response
//     if (PN532_ReadResponse(response, sizeof(response))) {
//         // Process firmware version from response
//         // Example: response[0] = IC Version, response[1] = Version, etc.
//     } else {
//         // Handle error: no response or incorrect response
//     }
// }

// //  function to initialize SPI and test PN532 communication
// int main(void) {
//     //FIO0DIR |= (1 << PN532_SSEL_PIN);  // Set PN532 SSEL pin as output
//     //SPI0_Init();                       // Initialize SPI0

//     while (1) {
//         PN532_GetFirmwareVersion();    // Send Get Firmware Version command
//         // Add delay if needed, process response
//     }
// }

#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "LCD.h"                        /* Graphic LCD function prototypes    */
#include <stdlib.h>

#define PN532_CS_PIN     (1 << 10)  // Example: P0.10 used for CS
#define PN532_MOSI_PIN   (1 << 18)  // Example: P0.18 for MOSI
#define PN532_MISO_PIN   (1 << 17)  // Example: P0.17 for MISO
#define PN532_SCK_PIN    (1 << 15)  // Example: P0.15 for SCK

// SPI command definitions
#define PN532_SPI_STAT_READ      0x02  // SPI status read command
#define PN532_SPI_DATA_WRITE     0x01  // SPI data write command
#define PN532_SPI_DATA_READ      0x03  // SPI data read command
char val[10];
// Function prototypes
void SPI_Init(void);
void PN532_WriteCommand(int *cmd, int len);
void PN532_ReadData(int *buffer, int len);
int SPI_Transfer(int data);
extern int i;
void delay_ms(int ms);
void detect_rfid_card(void);
int checksum ;
int mainrww(void) {
  lcd_print("HHHE");
			delay_ms(100000);  
	SPI_Init();  // Initialize SPI

    while (1) {
			lcd_print("HHHE");
			delay_ms(100000);
        detect_rfid_card();  // Continuously check for RFID cards
        delay_ms(500);       // Delay between detections
    }
}

// Function to detect RFID card
void detect_rfid_card(void) {
    // InListPassiveTarget command: Poll for one ISO14443A card
    int command[] = {0x4A, 0x01, 0x00};  // InListPassiveTarget, MaxTargets = 1, Baudrate = 106 kbps
    int response[64];                    // Buffer for response data

    // Send command to PN532
    PN532_WriteCommand(command, sizeof(command));

    // Wait for response
    delay_ms(50);  // Allow PN532 to process

    // Read response from PN532
    PN532_ReadData(response, sizeof(response));

    // Parse and print results
    if (response[6] == 0x00 && response[7] == 0x01) {  // Success, 1 card detected
        int uid_length = response[12];             // Length of UID
        int *uid = &response[13];                 // Pointer to UID bytes
        // Print UID (for debugging, replace with actual handling logic)
        for ( i = 0; i < uid_length; i++) {
            // Replace with actual code to display UID, e.g., UART print
					lcd_clear();
					sprintf(val, "%d",*uid);
					lcd_print ( val);
        }
    }
}

void SPI_Init(void) {
    // Configure SPI pins
    PINSEL0 &= ~(0x3 << 30); // Clear bits for P0.15 (SCK)
    PINSEL0 |= (0x3 << 30);  // Set SCK0
    PINSEL1 &= ~((0x3 << 2) | (0x3 << 4)); // Clear bits for P0.17 (MISO) and P0.18 (MOSI)
    PINSEL1 |= ((0x3 << 2) | (0x3 << 4));  // Set MISO0 and MOSI0

    // Configure CS pin as GPIO
    FIO0DIR |= PN532_CS_PIN;
    FIO0SET = PN532_CS_PIN; // Set CS high initially

    // Configure SPI
    S0SPCCR = 8;           // SPI clock = PCLK / 8
    S0SPCR = (1 << 5) |    // Enable SPI
             (1 << 3) |    // Master mode
             (1 << 2);     // CPOL = 0, CPHA = 0
}

void PN532_WriteCommand(int *cmd, int len) {
    FIO0CLR = PN532_CS_PIN; // Pull CS low

    // Send preamble and start code
    SPI_Transfer(0x00);
    SPI_Transfer(0x00);
    SPI_Transfer(0xFF);

    // Send length and length checksum
    SPI_Transfer(len + 1);         // Length (Data + TFI)
    SPI_Transfer(~(len + 1) + 1);  // Length checksum

    // Send TFI and data
    SPI_Transfer(0xD4);  // TFI (Host to PN532)
     checksum = 0xD4;
    for ( i = 0; i < len; i++) {
        SPI_Transfer(cmd[i]);
        checksum += cmd[i];
    }

    // Send checksum and postamble
    SPI_Transfer(~checksum + 1);
    SPI_Transfer(0x00);

    FIO0SET = PN532_CS_PIN; // Pull CS high
    delay_ms(2);            // Delay to let PN532 process the command
}

void PN532_ReadData(int *buffer, int len) {
    FIO0CLR = PN532_CS_PIN; // Pull CS low

    // Send SPI read status command
    SPI_Transfer(PN532_SPI_STAT_READ);

    // Wait for ready status
    while (SPI_Transfer(0x00) != 0x01);

    // Send data read command
    SPI_Transfer(PN532_SPI_DATA_READ);

    // Read data
    for ( i = 0; i < len; i++) {
        buffer[i] = SPI_Transfer(0x00);
    }

    FIO0SET = PN532_CS_PIN; // Pull CS high
}

int SPI_Transfer(int data) {
    S0SPDR = data;                 // Load data into SPI data register
    while (!(S0SPSR & (1 << 7)));  // Wait for transfer to complete
    return S0SPDR;                 // Return received data
}

void delay_ms(int ms) {
    for ( i = 0; i < ms * 1000; i++) {
        __asm("NOP");
    }
}