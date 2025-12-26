# Signal

**Signal** is an Arduino-based application designed to control model railroad signals. 
The project provides support for input from buttons and DCC commands (experimental) using
Arduino-compatible hardware. It includes modular components for input acquisition and output to 
LEDs, making it suitable for analogue and digital railroad configurations.

Each signal can be controlled by an analog-to-digital (AD) value. The AD value ranges for the 
individual signal aspects are configurable. A resistor circuit allows push buttons to be used for 
controlling the signals.

Each signal can be controlled by a DCC command.

## Features

- Arduino Mega: Up to eight signals
- Configuration of two user-defined signals
- Three built-in signals:
  - Ausfahrsignal
  - Blocksignal
  - Einfahrsignal
- Configuration of input buttons, e.g., analog inputs and digital inputs
- Configuration of DCC commands

## How to flash binaries

1. Download the release assest `Signal.hex`
2. Install avrdude (https://github.com/avrdudes/avrdude)
3. Install required drivers
   - E.g., if your Arduino variant is using a CH340 chip, install the respective driver.
4. Run avrdude
   ```
   avrdude -C avrdude.conf -v -V -p m2560 -c wiring -P COMx -D -U flash:w:Signal.hex:i
   ```
   > Remark: We are using `avrdude.conf` that is installed with avrdude.

   > Remark: Option `-c` is the programmer-id. For Arduino Mega, the value needs to be `wiring`, 
     for Arduino Nano, it needs to be `arduino`.

   > Remark: Tested with `avrdude v8.1`

## How to build

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

The application can be configured via a serial interface, e.g., USB, using a Ascii protocol. 
With a serial terminal such as HTerm or the Visual Studio Code Terminal, you can send messages to 
the application and receive messages from it.

> Note: The maximal length of a message is 64 characters.

### Supported Commands

#### CVs

These commands are used to change the configuration.

| Command           | Description                                  | Example Usage  |
|-------------------|----------------------------------------------|----------------|
| `SET_CV id value` | Set the CV with id `cv_id` to `value value`. | `SET_CV 29 64` |
| `GET_CV id`       | Get the value of the CV with id `cv_id`.     | `GET_CV 29`    |
| `SET_SIGNAL idx id [ONB,EXT] output_pin step_size [ADC,DIG,DCC] input_pin` | Set CVs for signal `idx`<br>`42 + idx`: id<br>`50 + idx`: output type and pin<br>`58 + idx`: input type and pin<br>`74 + idx`: derived from step_size | `SET_SIGNAL 0 1 ONB 6 -1 ADC 54`    |
| `GET_SIGNAL idx`  | Get values for signal `idx` from CVSs<br>`42 + idx`: id<br>`50 + idx`: output type and pin<br>`58 + idx`: input type and pin<br>`74 + idx`: derived from step_size | `GET_SIGNAL 0` prints the parameters as provided by `SET_SIGNAL`  |
| `GET_PIN_CONFIG pin` | Print `output`or `input` for `pin`.       | `GET_PIN_CONFIG 10`  |
| `INIT`            | Initialize EEPROM with default values.       | `INIT`         |

#### Monitor

These commands are used to output internal data structures to the terminal.

| Command | Description                | Example Usage         |
|----------------|----------------------------|----------------------|
| `MON_LIST` | Print available RTE ports (`ifc-name`) via serial interface. | `MON_LIST`<br>Prints the available interfaces to the terminal. |
| `MON_START cycle-time ifc-name [id-first id-nr]` | Start to print current values of `ifc-name`. Currently, just one RTE port can be printed at one time. Cycle time is `cycle-time` [ms]. `id-first` and `id-nr` are optional and define the span of an array that is to be transmitted [`id-first`, `id-first + id-nr`]. | `MON_START 100 ifc_ad_values`<br>Reads and prints AD values of the classifiers every 100 ms. |
| `MON_STOP` | Stop to print the RTE port. | `MON_STOP`<br>Stops to print to the terminal. |

#### Misc

| Command           | Description                                  | Example Usage  |
|-------------------|----------------------------------------------|----------------|
| `SET_VERBOSE level` | Enable verbose debug messages up to `level` (0 - 3).       | `SET_VERBOSE 3`      |

#### List of CVs

Version 1.0

|CV|Bit pattern|R/W|Default value|Val. Range MEGA|Val. Range NANO|Supp. MEGA|Supp. NANO|Comment|
|--|-----------|---|-------------|---------------|---------------|----------|----------|-------|
|1|0b11111111|R+W|1|0 - 255|0 - 255|Y|Y|If CV 29 Bit 6 = 1: CV1 contains the eight least significant bits of the Output Address, i. e. Output Address modulo 256.<br><br>If CV 29 Bit 6 = 0: Decoder Address LSB, bits 0 - 5 of accessory decoder address"|
|2|0b11111111||0|||N|N|Auxiliary Activation, Auxiliary activation of outputs, Bits 1-8 = Auxiliary activation: = "0" output is not activated by an auxiliary input, "1" output can be activated by an auxiliary input|
|3|0b11111111||0|||N|N|Time On F1|
|4|0b11111111||0|||N|N|Time On F2|
|5|0b11111111||0|||N|N|Time On F3|
|6|0b11111111||0|||N|N|Time On F4|
|7|0x11110000|R|0|||Y|Y|Manufacturer Version ID: major ID, 0xFF = invalid data (e.g. never programmed)|
||0b00001111|R|1|||Y|Y|Manufacturer Version ID: minor ID|
|8|0b11111111|R|83|||Y|Y|Manufacturer ID: 'S'|
|9|0b00000111|R+W|0|0 - 7|0 - 7|Y|Y|If CV 29 Bit 6 = 1: contains the three most significant bits of the Output Address, i. e. Output Address divided by 256.<br><br>If CV 29 Bit 6 = 0: Decoder Address MSB, bits 6 - 8 of accessory decoder address|
|10 - 27|||0|||N|N|Reserved by NMRA for future use|
|28|||0|||N|N|Bi-Directional Communication Configuration (RailCom)|
|29|0b00000001|R|0|0|0|N|N|Configuration|
||0b00000010|R|0|0|0|N|N|Reserved for future use|
||0b00000100|R|0|0|0|N|N|Reserved for future use|
||0b00001000|R|0|0|0|N|N|Bi-Directional Communications (0 = disabled, 1 = enabled)|
||0b00010000|R|0|0|0|N|N|Reserved for future use|
||0b00100000|R|0|0|0|N|N|Decoder Type<br>0 = Basic Accessory<br>1 = Extended Accessory|
||0b01000000|R+W|1|1|1|Y|Y|Addressing Method<br>0 = Decoder Address Method<br>1 = Output Address Method<br>Defines how CV1 and CV9 are interpreted|
||0b10000000|R|1|1|1|N|N|Accessory Decoder<br>0 = Multifunction Decoder<br>1 = Accessory Decoder|
|30|||0|||N|N|Reserved by NMRA for future use|
|31|||0|||N|N|Indexed Area Pointers, Index High and Low Address|
|32|||0|||N|N|Indexed Area Pointers, Index High and Low Address|
|33|0x11110000|R|0|1|1|Y|Y|Manufacturer CV structure version ID: major ID. Change major ID if the change is not backwards compatible.|
||0b00001111|R|1|0|0|Y|Y|Manufacturer CV structure version ID: minor ID. Change minor ID if the change is backwards compatible.|
|39|0b00000001|R+W|1|N/A|N/A|N|N|DCC addressing mode, 0 = ROCO, 1 = RCN-213|
|40|0b11111111|R|8|0 - 8|0 - 4|Y|Y|Maximum number of signals|
|41|0b11111111|R|2|2|2|Y|Y|Number of built-in signal-IDs|
|42|0b11111111|R+W|0|0 - 8||Y|Y|Signal-ID of signal 1 (0 = signal not used, 1 = Ausfahrsignal, 2 = Blocksignal, >=128 user defined signal)|
|43|0b11111111|R+W|0|0 - 8||Y|Y|Signal-ID of signal 2|
|44|0b11111111|R+W|0|0 - 8||Y|Y|Signal-ID of signal 3|
|45|0b11111111|R+W|0|0 - 8||Y|Y|Signal-ID of signal 4|
|46|0b11111111|R+W|0|0 - 8||Y|N|Signal-ID of signal 5|
|47|0b11111111|R+W|0|0 - 8||Y|N|Signal-ID of signal 6|
|48|0b11111111|R+W|0|0 - 8||Y|N|Signal-ID of signal 7|
|49|0b11111111|R+W|0|0 - 8||Y|N|Signal-ID of signal 8|
|50|0b11000000|R+W|0|0-1|0-1|Y|Y|Signal 1: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||||Signal 1: First output pin number|
|51|0b11000000|R+W|0|0-1|0-1|Y|Y|Signal 2: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 2: First output pin number|
|52|0b11000000|R+W|0|0-1|0-1|Y|Y|Signal 3: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 3: First output pin number|
|53|0b11000000|R+W|0|0-1|0-1|Y|Y|Signal 4: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 4: First output pin number|
|54|0b11000000|R+W|0|0-1|0-1|Y|N|Signal 5: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 5: First output pin number|
|55|0b11000000|R+W|0|0-1|0-1|Y|N|Signal 6: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 6: First output pin number|
|56|0b11000000|R+W|0|0-1|0-1|Y|N|Signal 7: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 7: First output pin number|
|57|0b11000000|R+W|0|0-1|0-1|Y|N|Signal 8: First output is internal pin (0), external (1)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 8: First output pin number|
|58|0b11000000|R+W|0|0 - 2|0 - 2|Y|Y|Signal 1: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 1: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53). Not used for DCC because DCC address is calculated from Decoder Address + Signal ID|
|59|0b11000000|R+W|0|0 - 2|0 - 2|Y|Y|Signal 2: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 2: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|60|0b11000000|R+W|0|0 - 2|0 - 2|Y|Y|Signal 3: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 3: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|61|0b11000000|R+W|0|0 - 2|0 - 2|Y|Y|Signal 4: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|Y|Signal 4: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|62|0b11000000|R+W|0|0 - 2|0 - 2|Y|N|Signal 5: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 5: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|63|0b11000000|R+W|0|0 - 2|0 - 2|Y|N|Signal 6: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 6: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|64|0b11000000|R+W|0|0 - 2|0 - 2|Y|N|Signal 7: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 7: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|65|0b11000000|R+W|0|0 - 2|0 - 2|Y|N|Signal 8: Input is DCC (0) or ADC Pin (1) or Digital Input Pin (2)|
||0b00111111|R+W|0|0 - 53||Y|N|Signal 8: ADC Pin (0 - 15) or First Digital Input Pin (3 - 53)|
|66|0b11111100|R+W|0|0|0 - 1|Y|Y|Signal 1: Reserved for future use|
||0b00000011|R+W|0|0 - 1|0|Y|Y|Signal 1: If input is ADC Pin: which (user defined) classifier type|
|67|0b11000000|R+W|0|0|0 - 1|Y|Y|Signal 2: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 2: If input is ADC Pin: which classifier type|
|68|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 3: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 3: If input is ADC Pin: which classifier type|
|69|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 4: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 4: If input is ADC Pin: which classifier type|
|70|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 5: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 5: If input is ADC Pin: which classifier type|
|71|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 6: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 6: If input is ADC Pin: which classifier type|
|72|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 7: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 7: If input is ADC Pin: which classifier type|
|73|0b11000000|R+W|0|0 - 1|0 - 1|Y|Y|Signal 8: Reserved for future use|
||0b00111111|R+W|0|0|0|Y|Y|Signal 8: If input is ADC Pin: which classifier type|
|74|0b00000001|R+W|0|0 - 1|0 - 1|Y|Y|Signal 1: <br>0 = Output pin order is increasing.<br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|Y|Signal 1: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|75|0b00000001|R+W|0|0 - 1|0 - 1|Y|Y|Signal 2: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|Y|Signal 2: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|76|0b00000001|R+W|0|0 - 1|0 - 1|Y|Y|Signal 3: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|Y|Signal 3: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|77|0b00000001|R+W|0|0 - 1|0 - 1|Y|Y|Signal 4: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|Y|Signal 4: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|78|0b00000001|R+W|0|0 - 1|0 - 1|Y|N|Signal 5: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|N|Signal 5: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|79|0b00000001|R+W|0|0 - 1|0 - 1|Y|N|Signal 6: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|N|Signal 6: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|80|0b00000001|R+W|0|0 - 1|0 - 1|Y|N|Signal 7: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|N|Signal 7: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|81|0b00000001|R+W|0|0 - 1|0 - 1|Y|N|Signal 8: <br>0 = Output pin order is increasing. <br>1 = Output pin order is decreasing|
||0b00000010|R+W|0|0 - 1|0 - 1|Y|N|Signal 8: <br>0 = Output pin step size is 1. <br>1 = Output pin step size is 2.|
|82 - 111|||0|||||Reserved by NMRA for future use|
|112|0b11111111|R+W|5|0 - 255|0 - 255|Y|Y|Classifier type 1: Debounce time until a class is classified: 0 sec ... 2.55 sec  [10 ms]|
|113|0b11111111|R+W|45|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 0 (default: AD value 182)|
|114|0b11111111|R+W|148|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 1 (default: AD value 595)|
|115|0b11111111|R+W|97|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 2 (default: AD value 389)|
|116|0b11111111|R+W|34|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 3 (default: AD value 137)|
|117|0b11111111|R+W|255|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 4 (default: AD value 1023)|
|118|0b11111111|R+W|50|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 0 (default: AD value 202)|
|119|0b11111111|R+W|153|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 1 (default: AD value 615)|
|120|0b11111111|R+W|102|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 2 (default: AD value 409)|
|121|0b11111111|R+W|39|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 3 (default: AD value 157)|
|122|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 4 (default: AD value 0)|
|123|0b11111111|R+W|5|0 - 255|0 - 255|Y|Y|Classifier type 2: Debounce time until a class is classified: 0 sec ... 2.55 sec  [10 ms]|
|124|0b11111111|R+W|45|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 0|
|125|0b11111111|R+W|148|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 1|
|126|0b11111111|R+W|97|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 2|
|127|0b11111111|R+W|34|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 3|
|128|0b11111111|R+W|255|0 - 255|0 - 255|Y|Y|[4 LSB] Lower limit for class 4|
|129|0b11111111|R+W|50|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 0|
|130|0b11111111|R+W|153|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 1|
|131|0b11111111|R+W|102|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 2|
|132|0b11111111|R+W|39|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 3|
|133|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|[4 LSB] Upper limit for class 4|
|134|0b11110000|R|0|0|0|Y|Y|User-defined Signal-ID 128: Reserved|
||0b00001111|R+W|0|1 - 8|1 - 8|Y|Y|User-defined Signal-ID 128: Number of outputs (LEDs)|
|135|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 0 (one bit per output LED)|
|136|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 0 (one bit per output LED)|
|137|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 1|
|138|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 1|
|139|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 2|
|140|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 2|
|141|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 3|
|142|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 3|
|143|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 4|
|144|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 4|
|145|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 5|
|146|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 5|
|147|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 6|
|148|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 6|
|149|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Aspect 7|
|150|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Blink 7|
|151|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Change Over Time [10 ms]|
|152|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 128: Change Over Time Blink [10 ms]|
|153|0b11110000|R|0|0|0|Y|Y|User-defined Signal-ID 128: Reserved|
||0b00001111|R+W|0|1 - 8|1 - 8|Y|Y|User-defined Signal-ID 128: Number of outputs (LEDs)|
|154|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 0 (one bit per output LED)|
|155|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 0 (one bit per output LED)|
|156|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 1|
|157|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 1|
|158|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 2|
|159|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 2|
|160|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 3|
|161|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 3|
|162|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 4|
|163|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 4|
|164|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 5|
|165|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 5|
|166|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 6|
|167|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 6|
|168|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Aspect 7|
|169|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Blink 7|
|170|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Change Over Time [10 ms]|
|171|0b11111111|R+W|0|0 - 255|0 - 255|Y|Y|User-defined Signal-ID 129: Change Over Time Blink [10 ms]|

## Folder Structure

- `Src/Prj/App/Signal/` - Main application source code
- `Doc/` - Documentation and usage guides

## License

This project is licensed under the MIT License.
