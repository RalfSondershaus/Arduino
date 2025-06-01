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

The application can be configured via a serial interface using the AsciiCom protocol. With a serial terminal, you can send messages to the application and receive messages from it.

Note: The maximal length of a message is 64 characters.

### Supported Commands

#### SET_SIGNAL

| Command | Description                | Example Usage         |
|----------------|----------------------------|----------------------|
| `SET_SIGNAL id ASPECTS [aspect]+` | Set the aspects for a signal. `[aspect]+` can include up to `kNrSignalAspects` aspects. If fewer than `kNrSignalAspects` aspects are provided, only the specified aspects are updated; the remaining aspects will stay unchanged. A `1` means that the corresponding LED is on, `0` means off. The order of the bits corresponds to the order of the pins, see `SET_SIGNAL id TARGETS`. |`SET_SIGNAL 0 ASPECTS 11000 00100 00110 11001 11111`<br>sets aspects for signal 0. With respect to the example of `SET_SIGNAL id TARGETS` below, `11000`, the left-mode `1` corresponds to pin 13, the right-most `0` to pin 9.|
| `SET_SIGNAL id BLINKS [blink]+` | Set blink for a signal. `[blink]+` can include up to `kNrSignalAspects` blinks, so a blink per aspect. If fewer than `kNrSignalAspects` blinks are provided, only the specified blinks are updated; the remaining blinks will stay unchanged. A `1` means that the corresponding LED starts to blink, `0` means LED on without blink. | `SET_SIGNAL 0 BLINKS 00000 00000 00000 00000 00000`<br>signal 0 doesn't blink | 
| `SET_SIGNAL id TARGETS [(ONBOARD, ONB, EXTERN, EXT) pin]+` | Sets up to `NrSignalTargets` target ports for a signal. EXTERN (EXT) are currently not supported. `pin` is the Arduino pin number. | `SET_SIGNAL 0 TARGETS ONB 13 ONB 12 ONB 11 ONB 10 ONB 9` Set 5 targets for signal 0: pins 13, 12, 11, 10, and 9. |
| `SET_SIGNAL id INPUT CLASSIFIED id` | Sets input type: type is classifier with ID `id`. | `SET_SIGNAL 0 INPUT CLASSIFIED 0`<br>Assigns classifier 0 to signal 0. |
| `SET_SIGNAL id COT t1 t2` |  Sets change-over times (`t1` and `t2` use [10 ms]). | `SET_SIGNAL 0 COT 10 20`<br> set change over time of signal 0 to 100 ms and blink change over time to 200 ms. |

#### GET_SIGNAL

| Command | Description                | Example Usage         |
|----------------|----------------------------|----------------------|
| `GET_SIGNAL id ASPECTS` | Returns all `kNrSignalAspects` aspects for a signal. | `GET_SIGNAL 0 ASPECTS` | 
| `GET_SIGNAL id BLINKS` | Returns all `kNrSignalAspects` blinks for a signal. | `GET_SIGNAL 0 BLINKS` | 
| `GET_SIGNAL id TARGETS` | Returns all `kNrSignalTargets` target ports for signal id. | `GET_SIGNAL 0 TARGETS` | 
| `GET_SIGNAL id INPUT` | Returns the classifier and classifier ID. | `GET_SIGNAL 0 INPUT` | 
| `GET_SIGNAL id COT` | Returns change over time [10 ms] and blink change over time [10 ms]. | `GET_SIGNAL 0 COT` | 

#### SET_CLASSIFIER

| Command | Description                | Example Usage         |
|----------------|----------------------------|----------------------|
| `SET_CLASSIFIER id PIN pin` | Sets the Arduino analog input pin (ADC). | `SET_CLASSIFIER 0 PIN 56`<br> Set analog input pin 56 for classifier 0. | 
| `SET_CLASSIFIER id LIMITS slot-id min max` | Sets the AD limits for the given slot of the classifier. A classifier supports `kNrClassifierClasses` classes. `min` and `max` are given in raw values. The AD values are `4*min` and `4*max`. | `SET_CLASSIFIER 0 LIMITS 1 85 102`<br>sets limits to `85 (raw) = 4*85 = 340 (AD)` and `102 (raw) = 102*4 = 408` | 
| `SET_CLASSIFIER id DEBOUNCE t1` | Sets debounce time [10 ms] for a classifier. | `SET_CLASSIFIER 0 DEBOUNCE 5`<br>set a debounce time of 50 ms for classifier 0. | 

#### GET_CLASSIFIER

| Command | Description                | Example Usage         |
|----------------|----------------------------|----------------------|
| `GET_CLASSIFIER id PIN` | Gets the Arduino analog input pin (ADC). | `GET_CLASSIFIER 0 PIN` | 
| `GET_CLASSIFIER id LIMITS` | Gets the AD limits for the given slot of the classifier. A classifier supports `kNrClassifierClasses` classes. `min` and `max` are given in raw values. The AD values are `4*min` and `4*max`. | `GET_CLASSIFIER 0 LIMITS` | 
| `GET_CLASSIFIER id DEBOUNCE` | Gets debounce time [10 ms] for a classifier. | `GET_CLASSIFIER 0 DEBOUNCE` | 

## Folder Structure

- `Src/Prj/App/Signal/` - Main application source code
- `Doc/` - Documentation and usage guides

## License

This project is licensed under the MIT License.
