#include <Wire.h>

#define OLED_ADDRESS 0x3C // I2C address of the OLED display
#define SCREEN_WIDTH 64   // Width of the OLED screen in pixels
#define SCREEN_HEIGHT 32  // Height of the OLED screen in pixels

byte screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8]; // Off-screen buffer for double buffering

// Function to send a command to the OLED display
void sendCommand(byte command) {
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x00); // Control byte indicating command
  Wire.write(command);
  Wire.endTransmission();
}

// Function to send data to the OLED display
void sendData(byte data) {
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(0x40); // Control byte indicating data
  Wire.write(data);
  Wire.endTransmission();
}

void setup() {
  Wire.begin();

  // Initialize the OLED display
  sendCommand(0xAE); // Display off

  sendCommand(0x20); // Set memory addressing mode
  sendCommand(0x10); // Horizontal addressing mode

  sendCommand(0xB0); // Set page start address
  sendCommand(0);    // Page 0

  sendCommand(0xC8); // Set COM output scan direction

  sendCommand(0x00); // Set lower column start address
  sendCommand(0x10); // Set higher column start address

  sendCommand(0x40); // Set display start line

  sendCommand(0x81); // Set contrast control
  sendCommand(0xFF); // Maximum contrast

  sendCommand(0xA1); // Set segment remap
  sendCommand(0xA6); // Set normal display

  sendCommand(0xA8); // Set multiplex ratio
  sendCommand(0x1F); // 1/32 duty cycle

  sendCommand(0xA4); // Set entire display on/off
  sendCommand(0xD3); // Set display offset
  sendCommand(0x00);

  sendCommand(0xDA); // Set COM pins hardware configuration
  sendCommand(0x12);

  sendCommand(0xD5); // Set display clock divide ratio/oscillator frequency
  sendCommand(0x80);

  sendCommand(0x8D); // Enable charge pump regulator
  sendCommand(0x14);

  sendCommand(0xAF); // Display on
}

void clearScreen() {
  memset(screenBuffer, 0, sizeof(screenBuffer));
}

// Load buffer memory to the OLED itself
void updateScreen() {
  for (int page = 0; page < SCREEN_HEIGHT / 8; page++) {
    sendCommand(0xB0 + page); // Set page address
    sendCommand(0x00);        // Set lower column start address
    sendCommand(0x12);        // Set higher column start address

    for (int column = 0; column < SCREEN_WIDTH; column++) {
      sendData(screenBuffer[column + page * SCREEN_WIDTH]);
    }
  }
}

void drawPixel(int x, int y) {
  if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
    int bufferIndex = x + (y / 8) * SCREEN_WIDTH;
    screenBuffer[bufferIndex] |= (1 << (y % 8));
  }
}

void loop() {
  clearScreen(); // Clear the off-screen buffer
  
  drawPixel(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); // Pixel in the centre
  drawPixel(0,0); // Upper-left corner
  drawPixel(SCREEN_WIDTH-1, SCREEN_HEIGHT-1); // Right-bottom corner
  
  updateScreen(); // Copy the off-screen buffer to the screen (load to the screen)
}
