# Hardware Abstraction Layer (HAL)

The Hardware Abstraction Layer (HAL) provides a consistent interface for the MDK-Predator application to interact with the underlying hardware, regardless of the specific hardware platform being used. This makes the application more portable and easier to maintain.

## Architecture

The HAL is divided into two main parts:

*   **`hal.h`**: This header file defines the interface for the HAL. It includes function declarations for all the hardware-specific operations that are currently spread throughout the codebase.
*   **`hal.c`**: This implementation file contains the hardware-specific code that implements the HAL interface.

## Usage

To use the HAL, simply include the `hal.h` header file and call the desired HAL functions. The HAL will automatically handle all the hardware-specific details.
