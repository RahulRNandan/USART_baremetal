# STM32F407 USART Example Project

## Project Overview

This project demonstrates how to configure and use the **USART2** peripheral on the **STM32F407** microcontroller to transmit and receive data. The microcontroller echoes any received data back to the sender using a simple loop.

## Features
- USART2 configured for serial communication at a baud rate of **9600 bps**.
- Pins **PA2** and **PA3** are configured for **USART2 TX** and **RX** respectively.
- Data received over USART is immediately transmitted back (echo).
  
## Microcontroller and Development Environment
- **Microcontroller**: STM32F407
- **IDE**: Any ARM Cortex IDE (e.g., Keil, STM32CubeIDE, or any GCC-based environment)
- **Compiler**: ARM GCC or equivalent
- **Language**: Embedded C
- **Communication Interface**: USART2, configured for bidirectional serial communication
- **GPIO Ports Used**: GPIOA

## Hardware Setup
- **PA2 (Pin 14)** is used for USART2 TX (Transmit).
- **PA3 (Pin 15)** is used for USART2 RX (Receive).
  
Ensure that PA2 and PA3 are connected to a USB-to-serial converter if you're using a PC for communication.

## Code Explanation
### 1. `volatile` Keyword

The `volatile` keyword is used in the register definitions to prevent the compiler from optimizing out repeated accesses to hardware registers, which can change unexpectedly due to external hardware events. This is crucial for registers like **USART_SR** (USART Status Register), **USART_DR** (Data Register), and others, as they are often updated by the hardware without CPU intervention.

By marking these registers as `volatile`, the compiler ensures that the program always re-reads the most current values directly from memory rather than using cached values. This guarantees accurate real-time interaction with the hardware.

For example:
- **USART_SR** contains flags such as:
  - **TXE** (Transmit Data Register Empty): Indicates when the transmit buffer is ready for new data.
  - **RXNE** (Receive Data Register Not Empty): Signals that new data has been received and is ready to be read.
  
- **USART_DR** holds the data to be transmitted or received, requiring volatile access to ensure correct data handling.

Other UART-related registers, such as **USART_BRR** (Baud Rate Register) and **USART_CR1, CR2, CR3** (Control Registers), also need to be declared as `volatile`. This ensures that any configuration changes or baud rate settings are accurately applied in real time.

```c
#define USART_SR   (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x00)))   // Status Register
#define USART_DR   (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x04)))   // Data Register
#define USART_BRR  (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x08)))   // Baud Rate Register
#define USART_CR1  (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x0C)))   // Control Register 1
#define USART_CR2  (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x10)))   // Control Register 2
#define USART_CR3  (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x14)))   // Control Register 3
```
Here’s an updated version of the section that provides more clarity and detail on the pin configuration for PA2 and PA3:

### 2. Pin Configuration (PA2 and PA3)

- **PA2** and **PA3** are designated as alternate-function pins configured for USART2 communication.
- **PA2** is configured as **TX** (Transmit), allowing the microcontroller to send data.
- **PA3** is configured as **RX** (Receive), enabling the microcontroller to receive data.
- Both pins are mapped to **AF7** (Alternate Function 7), which corresponds to USART2.

The configuration is done in two steps, modifying the **GPIOA_MODER** and **GPIOA_AFRL** registers.

#### Step 1: Set PA2 and PA3 to Alternate Function Mode
```c
GPIOA_MODER |= (2 << 4) | (2 << 6);  // Set PA2 and PA3 to alternate function mode
```
- **`GPIOA_MODER`**: This register controls the mode of each pin in GPIOA.
- **`(2 << 4)`**: The value `2` (binary `10`) shifted left by `4` positions sets bits 4-5 to `10`, configuring PA2 for alternate function mode.
- **`(2 << 6)`**: Similarly, the value `2` shifted left by `6` sets bits 6-7 to `10`, configuring PA3 for alternate function mode.
  
**Bitwise Operation:**
- Before this operation, bits 4-5 and 6-7 were `00` (input mode).
- After the operation, they become `10`, indicating alternate function mode for both PA2 and PA3.
2 and PA3 are correctly set up for USART2 communication, allowing the microcontroller to transmit and receive data effectively. The use of bitwise operations helps manipulate specific bits in the GPIO registers to achieve the desired pin configurations.
#### Step 2: Set AF7 (USART2) for PA2 and PA3
```c
GPIOA_AFRL |= (7 << 8) | (7 << 12);  // Set AF7 (USART2) for PA2 and PA3
```
- **`GPIOA_AFRL`**: This register selects the alternate function for pins PA0 to PA7.
- **`(7 << 8)`**: The value `7` (binary `0111`) shifted left by `8` positions sets bits 8-11 for PA2 to `0111`, mapping it to AF7 (USART2).
- **`(7 << 12)`**: Shifting `7` left by `12` sets bits 12-15 for PA3 to `0111`, also mapping it to AF7.

**Bitwise Operation:**
- Before this operation, bits 8-11 and 12-15 were set to `0000` (no alternate function).
- After the operation, they become `0111`, indicating AF7 for both PA2 and PA3.

The above configuration ensures that PA2 and PA3 are correctly set up for USART2 communication, allowing the microcontroller to transmit and receive data effectively. The use of bitwise operations helps manipulate specific bits in the GPIO registers to achieve the desired pin configurations.

Here’s an enhanced version of the section on bus architecture, providing more clarity and detail:

### 3. Bus Architecture: AHB1 and APB1

The STM32F407 microcontroller utilizes a hierarchical bus architecture to manage communication between the CPU and various peripherals. It has two primary buses:

- **AHB1 (Advanced High-Performance Bus)**: 
  - This bus is designed for high-speed data transfers and is used for connecting high-performance peripherals such as Flash memory, SRAM, and Direct Memory Access (DMA) controllers. 
  - The AHB1 bus offers high bandwidth and low latency, making it suitable for applications that require fast data processing and transfer.

- **APB1 (Advanced Peripheral Bus)**: 
  - This bus is used for slower peripheral devices that do not require the high-speed capabilities of the AHB1 bus. 
  - Common peripherals connected to the APB1 bus include USART (Universal Synchronous Asynchronous Receiver Transmitter), I2C (Inter-Integrated Circuit), and other low-speed interfaces.
  - The APB1 bus allows for lower power consumption, making it ideal for power-sensitive applications.

#### Enabling Peripherals
To use these peripherals, we must enable their respective clocks in the RCC (Reset and Clock Control) registers. This is done with the following code:

```c
RCC_AHB1ENR |= (1 << 0);  // Enable GPIOA clock
RCC_APB1ENR |= (1 << 17); // Enable USART2 clock
```

- **`RCC_AHB1ENR`**: This register controls the clock enabling for AHB1 peripherals.
  - `|= (1 << 0)`: This operation sets bit 0 of the AHB1ENR register to `1`, enabling the clock for **GPIOA**. 
  - This allows the GPIOA pins to function, making them available for input/output operations.

- **`RCC_APB1ENR`**: This register controls the clock enabling for APB1 peripherals.
  - `|= (1 << 17)`: This operation sets bit 17 of the APB1ENR register to `1`, enabling the clock for **USART2**.
  - Enabling the USART2 clock is essential for its operation, allowing it to transmit and receive data.

By enabling the clocks for GPIOA and USART2, we allow these peripherals to be operational. This is a crucial step in configuring the STM32F407 microcontroller for communication and data processing tasks, ensuring that both high-speed and low-speed peripherals can function correctly within their designated bus architecture.

Here’s an expanded version of section 4 that explains register offsets and bitwise operations more clearly:

### 4. Register Offsets and Bitwise Operations

In the STM32F407 microcontroller, registers such as `USART_SR` (Status Register) and `USART_DR` (Data Register) are accessed through memory-mapped I/O. This means that specific memory addresses correspond directly to hardware registers, allowing the CPU to interact with hardware peripherals through normal memory access instructions.

#### Register Offsets
Each register has a specific offset from a base address, allowing us to define registers in a way that the CPU can read from and write to them. For example:
- **`USART_SR`** is located at the offset `0x00` from the base address of USART2, which can be accessed as follows:
  ```c
  #define USART_SR (*((volatile uint32_t *)(USART2_BASE_ADDR + 0x00))) // Status Register
  ```

#### Bitwise Operations
Bitwise operations are fundamental in configuring and manipulating these registers, as many registers consist of multiple fields controlled by individual bits. The primary bitwise operations used in the code are:

- **Left Shift (`<<`)**: This operator shifts the bits of a number to the left by a specified number of positions. For example, `1 << 17` shifts the binary `00000000 00000000 00000000 00000001  (32 bits)` to the left 17 times, resulting in 
`00000000 00000000 10000000 00000000`. This operation sets the 17th bit to `1`, which is used to enable the clock for USART2 in the `RCC_APB1ENR` register.

- **Bitwise OR (`|`)**: This operator combines bits from two numbers. It sets a bit to `1` if either of the corresponding bits in the two numbers is `1`. For instance, the operation `RCC_APB1ENR |= (1 << 17)` effectively enables the clock for USART2 by setting the 17th bit to `1` without altering the other bits.

- **Bitwise AND (`&`)**: This operator can be used to clear specific bits by masking. For example, to clear bits in a register, you might use a bitwise AND operation with a mask that has `0`s in the positions you want to clear.

#### Example in Context
- **Enabling USART2 Clock**:
  ```c
  RCC_APB1ENR |= (1 << 17); // Enable USART2 clock
  ```
  Here, we are enabling the clock for USART2 by setting bit 17 of the `RCC_APB1ENR` register to `1`.

- **Configuring GPIOA Mode**:
  ```c
  GPIOA_MODER |= (2 << 4) | (2 << 6); // Set PA2 and PA3 to alternate function mode
  ```
  In this example, `2 << 4` and `2 << 6` are used to set the mode for **PA2** and **PA3** to alternate function mode, which corresponds to their respective bit positions in the `GPIOA_MODER` register.

Bitwise operations are essential for configuring the microcontroller's registers effectively, allowing us to control hardware functionality at a granular level. By manipulating individual bits, we can enable clocks, configure pin modes, and manage data flow efficiently.

Here's the updated section for your README.md that includes the details about baud rate configuration:

### 5. Baud Rate Configuration
The baud rate is an essential parameter for serial communication, determining how fast data is transmitted over a UART interface. In this implementation, the baud rate is set to **9600 bps** by configuring the **USART_BRR** (Baud Rate Register). 

The specific value `0x683` is calculated using the following formula:
### USART_DIV Formula:

The formula for calculating `USART_DIV` is:
```
USART_DIV = f_CK / (16 × baud rate)
```
Where:

- `f_CK` is the clock frequency in Hz (in this case, 16 MHz).
- Baud rate is the communication speed, for example, 9600 bps.

Given:
```
USART_DIV = 16,000,000 / (16 × 9600) ≈ 104.1667
```
The integer part of `USART_DIV` is used to determine the Mantissa (104), and the fractional part (0.1667) is used to calculate the Fraction.

To set this up in the `USART_BRR` register:

- The Mantissa is placed in the upper 12 bits.
- The Fraction is calculated as follows:
```  
Fraction = round(0.1667 × 16) ≈ 2
```

So, the complete configuration for the **USART_BRR** value is:
```
USART_BRR = (Mantissa << 4) | Fraction = (104 << 4) | 2 = 0x683
```
### Explanation of the Shifting Operation
- **Mantissa << 4** shifts the **Mantissa** value of **104** to the left by **4 bits**, placing it in the higher part of the **USART_BRR** register, which corresponds to the 12 most significant bits.
- The **Fraction** value of **2** is then added to the lower 4 bits of the register using a bitwise OR operation (`|`).

Thus, the value `0x683` is set in the **USART_BRR** register to establish a baud rate of **9600 bps** under a **16 MHz** clock frequency. This configuration ensures reliable communication between devices using the specified baud rate.

### 6. Transmitting and Receiving Data
- **Transmission** is done by writing data to the **USART Data Register (USART_DR)**.
- **Reception** is done by reading data from the **USART Data Register (USART_DR)**.
  
The flags **USART_SR_TXE** (Transmit Data Register Empty) and **USART_SR_RXNE** (Receive Data Register Not Empty) ensure that the transmit or receive buffers are ready for the next operation.

```c
void usart2_write(char ch) {
    while (!(USART_SR & USART_SR_TXE));  // Wait until TX is empty
    USART_DR = (ch & 0xFF);              // Send data
}

char usart2_read() {
    while (!(USART_SR & USART_SR_RXNE)); // Wait until RX is not empty
    return (char)(USART_DR & 0xFF);      // Read received data
}
```

### 7. Full Initialization and Echo Loop
The `main` function initializes USART2 and enters an infinite loop where received data is echoed back to the sender.

```c
int main() {
    usart2_init();  // Initialize USART2
    
    while (1) {
        char received = usart2_read();  // Read a character
        usart2_write(received);         // Echo back the received character
    }
}
```

## How to Build and Run
1. Clone or download this project to your local development environment.
2. Open the project in your preferred IDE or compile it using `arm-none-eabi-gcc` (or a compatible toolchain).
3. Flash the binary to your STM32F407 using an ST-Link or any other programmer.
4. Use a terminal emulator (e.g., PuTTY or Minicom) to communicate with the USART2 peripheral via USB-to-serial.

## References
- **STM32F407 Reference Manual (RM0090)** – Provides detailed information about registers and peripheral configurations.
- **STM32F407 Datasheet** – Provides information on pin mappings and electrical characteristics.

## License
This project is open-source and free to use.
