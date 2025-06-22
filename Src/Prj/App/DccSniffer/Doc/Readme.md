**DccSniffer** listens to DCC signals on a specified interrupt pin, decodes them into packets (using the dcc::Decoder), and transmits the packets over the serial interface in a compact binary format. It also provides periodic status output with statistics such as interrupt count, fetches, and buffer usage.

## BinaryProtocol Binary Protocol Description
  
Each decoded DCC packet is sent as a binary message with the following structure:
`<0x2E> <len> <data bytes> <0x00>`
  - `0x2E`: Start-of-packet marker (kCodeResponse)
  - `len`:  Number of data bytes in the packet (1 byte)
  - `data bytes`: The raw DCC packet bytes (len bytes)
  - `0x00`: End-of-packet marker
  
Example (for a 3-byte DCC packet `0xAA 0xBB 0xCC`):
> `2E 03 AA BB CC 00`
  
> **Note**: The ASCII output function (PrintAscii) is provided for debugging and prints each byte as two hex digits separated by `-`.
  
## Usage
  - Connect the DCC signal to the configured interrupt pin (default: pin 2).
  - Open a serial terminal at the configured baud rate (default: 115200).
  - Each received DCC packet will be output in the binary protocol format described above.
  - You can use the Windows application DccSniffer to visualize the packets in real-time.
  - You can connect with a Windows terminal program (e.g., PuTTY or HTerm) to see the 
    ASCII output (between the binary packets) for debugging purposes.

## Features

- Listen to DCC packets
- Stream DCC packets to serial interface
- Stream statictics to serial interface

## Getting Started

1. Clone the repository to your Arduino project directory.
2. Install avr-gcc and avrdude
3. Run the following command to build the project:

    ```sh
    Build/build.sh App/Signal mega avr_gcc arduino none rebuild
    ```
4. Connect the Arduino Mega.
5. Upload the code to your Arduino board.

    ```sh
    Build/build.sh App/Signal mega avr_gcc arduino none run
    ```

## Folder Structure

- `Src/Prj/App/DccSniffer/` - Main application source code
- `Src/Gen/Dcc/` - DCC decoder
- `Src/Gen/` - Generic functionality (such as embedded version of std::array etc)
- `Src/Prj/App/DccSniffer/Doc/` - Documentation and usage guides (this file=

## License

This project is licensed under the MIT License.
