#include <reg51.h>

// LCD control pins
sbit RS = P3^7;
sbit EN = P3^6;
sbit D4 = P3^2;
sbit D5 = P3^3;
sbit D6 = P3^4;
sbit D7 = P3^5;

// IR sensors for each slot
sbit IR_SENSOR_1 = P1^0;  // IR sensor for slot 1
sbit IR_SENSOR_2 = P1^1;  // IR sensor for slot 2
sbit IR_SENSOR_3 = P1^2;  // IR sensor for slot 3
sbit IR_SENSOR_4 = P1^3;  // IR sensor for slot 4
sbit IR_SENSOR_5 = P1^4;  // IR sensor for slot 5

// Servo motor control pin
sbit SERVO = P2^0;      // Servo motor connected to P2.0

unsigned char slots[5] = {0, 0, 0, 0, 0};  // Array to track slot status (0 = empty, 1 = full)

// Function to introduce a delay
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 114; j++);
}

// Send command to LCD
void LCD_Command(unsigned char command) {
    RS = 0;
    D4 = (command & 0x10) >> 4;
    D5 = (command & 0x20) >> 5;
    D6 = (command & 0x40) >> 6;
    D7 = (command & 0x80) >> 7;
    EN = 1;
    delay_ms(1);
    EN = 0;

    D4 = (command & 0x01);
    D5 = (command & 0x02) >> 1;
    D6 = (command & 0x04) >> 2;
    D7 = (command & 0x08) >> 3;
    EN = 1;
    delay_ms(1);
    EN = 0;
    delay_ms(2);
}

// Send data to LCD
void LCD_Data(unsigned char dat) {
    RS = 1;
    D4 = (dat & 0x10) >> 4;
    D5 = (dat & 0x20) >> 5;
    D6 = (dat & 0x40) >> 6;
    D7 = (dat & 0x80) >> 7;
    EN = 1;
    delay_ms(1);
    EN = 0;

    D4 = (dat & 0x01);
    D5 = (dat & 0x02) >> 1;
    D6 = (dat & 0x04) >> 2;
    D7 = (dat & 0x08) >> 3;
    EN = 1;
    delay_ms(1);
    EN = 0;
    delay_ms(2);
}

// Initialize LCD
void LCD_Init() {
    delay_ms(20);
    LCD_Command(0x02);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x01);
    delay_ms(2);
}

// Display a string on LCD
void LCD_DisplayString(char* str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

// Update slot status based on IR sensors
void UpdateSlotStatus() {
    slots[0] = (IR_SENSOR_1 == 0) ? 1 : 0;
    slots[1] = (IR_SENSOR_2 == 0) ? 1 : 0;
    slots[2] = (IR_SENSOR_3 == 0) ? 1 : 0;
    slots[3] = (IR_SENSOR_4 == 0) ? 1 : 0;
    slots[4] = (IR_SENSOR_5 == 0) ? 1 : 0;
}

// Display parking status
void DisplayParkingStatus() {
    unsigned char i;
    unsigned char allFull = 1;

    LCD_Command(0x01);  // Clear display
    delay_ms(2);

    // Display the first 3 slots on the first line
    LCD_Command(0x80);  // Move cursor to the first line
    for (i = 0; i < 3; i++) {
        LCD_Data('S');
        LCD_Data(i + '1');
        LCD_Data('=');
        LCD_Data(slots[i] ? 'F' : 'E');  // F = Full, E = Empty
        if (slots[i] == 0) {
            allFull = 0;
        }
        if (i < 2) {
            LCD_Data(' ');  // Add a space between slots
        }
    }

    // Display the next 2 slots on the second line
    LCD_Command(0xC0);  // Move cursor to the second line
    for (i = 3; i < 5; i++) {
        LCD_Data('S');
        LCD_Data(i + '1');
        LCD_Data('=');
        LCD_Data(slots[i] ? 'F' : 'E');  // F = Full, E = Empty
        if (slots[i] == 0) {
            allFull = 0;
        }
        if (i < 4) {
            LCD_Data(' ');  // Add a space between slots
        }
    }

    // If all slots are full, display "Parking Full"
    if (allFull) {
        LCD_Command(0x01);  // Clear display
        delay_ms(2);
        LCD_Command(0x80);  // Move cursor to the first line
        LCD_DisplayString("Parking Full");
    }
}

// Main function
void main() {
    P3 = 0xFF;  // Set Port 3 as output for LCD
    P1 = 0xFF;  // Set Port 1 as input for IR sensors
    P2 = 0x00;  // Set Port 2 as output for servo motor
    LCD_Init();

    // Display the welcome message
    LCD_Command(0x01);
    delay_ms(2);
    LCD_Command(0x80);
    LCD_DisplayString("Welcome to KLE");
    LCD_Command(0xC0);
    LCD_DisplayString("Parking System");
    delay_ms(2000);

    while (1) {
        UpdateSlotStatus();  // Continuously update slot status
        DisplayParkingStatus();  // Display the updated status
        delay_ms(500);  // Refresh every 500ms
    }
}
