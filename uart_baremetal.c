#include <stdint.h> // For fixed-width integer types (e.g., uint32_t), ensuring portability.

// Base addresses for peripherals
#define USART2_Base_ADDR  0x40004400  // USART2 peripheral base address
#define GPIOA_BASE_ADDR   0x40020000  // GPIOA base address for configuring PA2 (TX) and PA3 (RX)
#define RCC_BASE_ADDR     0x40023800  // Reset and Clock Control (RCC) base address

// Memory-mapped I/O Register Definitions for USART2 Peripheral
#define USART_SR    (*((volatile uint32_t *)(USART2_Base_ADDR + 0x00))) // Status Register - Indicates USART status (e.g., TXE, RXNE)
#define USART_DR    (*((volatile uint32_t *)(USART2_Base_ADDR + 0x04))) // Data Register - Holds the data for transmission/reception
#define USART2_BRR  (*((volatile uint32_t *)(USART2_Base_ADDR + 0x08))) // Baud Rate Register - Configures baud rate
#define USART2_CR1  (*((volatile uint32_t *)(USART2_Base_ADDR + 0x0C))) // Control Register 1 - Enables USART, configures word length, parity
#define USART2_CR2  (*((volatile uint32_t *)(USART2_Base_ADDR + 0x10))) // Control Register 2 - Configures stop bits, clock polarity, etc.
#define USART2_CR3  (*((volatile uint32_t *)(USART2_Base_ADDR + 0x14))) // Control Register 3 - Configures flow control, interrupts, etc.

// RCC Register Offsets for Clock Control
#define RCC_AHB1ENR (*((volatile uint32_t *)(RCC_BASE_ADDR + 0x30))) // AHB1 Peripheral Clock Enable Register - Enables GPIOA clock
#define RCC_APB1ENR (*((volatile uint32_t *)(RCC_BASE_ADDR + 0x40))) // APB1 Peripheral Clock Enable Register - Enables USART2 clock

// GPIOA Register Definitions
#define GPIOA_MODER (*((volatile uint32_t *)(GPIOA_BASE_ADDR + 0x00))) // Mode Register - Configures GPIO pin modes (input, output, alternate function)
#define GPIOA_AFRL  (*((volatile uint32_t *)(GPIOA_BASE_ADDR + 0x20))) // Alternate Function Low Register - Configures alternate functions for pins PA0-PA7

// USART Status Register Flags
#define USART_SR_TXE (1 << 7)   // TXE flag - Transmit Data Register Empty
#define USART_SR_RXNE (1 << 5)  // RXNE flag - Read Data Register Not Empty

// Function to Initialize USART2 Peripheral
void usart2_init() {
    // Enable clocks for GPIOA and USART2
    RCC_AHB1ENR |= (1 << 0);   // Enable GPIOA clock
    RCC_APB1ENR |= (1 << 17);  // Enable USART2 clock

    // Configure GPIOA Pins PA2 (TX) and PA3 (RX) as alternate function (AF7 for USART2)
    GPIOA_MODER &= ~((3 << 4) | (3 << 6));   // Clear PA2 and PA3 mode bits
    GPIOA_MODER |= (2 << 4) | (2 << 6);      // Set PA2 and PA3 to alternate function mode
    GPIOA_AFRL &= ~((0xF << 8) | (0xF << 12)); // Clear alternate function for PA2 and PA3
    GPIOA_AFRL |= (7 << 8) | (7 << 12);      // Set AF7 (USART2) for PA2 and PA3

    // Configure USART2: Baud rate, TX, RX
    USART2_CR1 = 0;          // Clear CR1 to ensure USART is disabled before configuration
    USART2_BRR = 0x683;      // Set baud rate to 9600 (assuming 16 MHz clock)
    USART2_CR1 |= (1 << 3) | (1 << 2); // Enable Transmit (TX) and Receive (RX)
    USART2_CR1 |= (1 << 13); // Enable USART2
}

// Function to Transmit a Single Character via USART2
void usart2_write(char ch) {
    while (!(USART_SR & USART_SR_TXE));  // Wait until Transmit Data Register is empty
    USART_DR = (ch & 0xFF);              // Send the character
}

// Function to Receive a Single Character via USART2
char usart2_read() {
    while (!(USART_SR & USART_SR_RXNE)); // Wait until data is available
    return (char)(USART_DR & 0xFF);      // Read and return the received character
}

// Main Function: Initializes USART2, echoes received characters
int main() {
    usart2_init();  // Initialize USART2

    while (1) {
        char received = usart2_read(); // Read a character
        usart2_write(received);        // Echo the received character back
    }
}
