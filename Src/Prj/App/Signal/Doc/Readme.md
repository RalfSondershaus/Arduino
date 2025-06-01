# Signal

**Signal** is an Arduino-based application designed to control model railroad signals. The project provides support for input from buttons and DCC commands (experimental) using Arduino-compatible hardware. It includes modular components for input acquisition and output to LEDs, making it suitable for analogue and digital railroad configurations.

Each signal is controlled by an analog-to-digital (AD) value. The AD value ranges for the individual signal aspects are configurable. A resistor circuit allows push buttons to be used for controlling the signals.

Each signal is controlled by a DCC command. This is experimental. 

## Features

- Arduino Mega: Up to six signals
- Configuration of input buttons
- Configuration of DCC commands

## Getting Started

1. Clone the repository to your Arduino project directory.
2. Install avr_gcc and avrdude
3. Run the following command to build the project:

    ```sh
    Build/build.sh App/Signal mega avr_gcc arduino none rebuild
    ```
4. Connect the Arduino Mega.
5. Upload the code to your Arduino board.

    ```sh
    Build/build.sh App/Signal mega avr_gcc arduino none run
    ```

## Configuration

You can configure the application with a serial monitor.

## Folder Structure

- `Src/Prj/App/Signal/` - Main application source code
- `Doc/` - Documentation and usage guides

## License

This project is licensed under the MIT License.
