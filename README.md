# Daikin AC Control Project

This repository contains the code and documentation for controlling Daikin air conditioners using PlatformIO.

## Features

- Control Daikin AC units via IR signals
- Support for multiple AC models
- Easy integration with home automation systems

## Requirements

- PlatformIO
- Arduino framework
- IRremote library

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/Daikin-AC.git
    ```
2. Open the project with PlatformIO:
    ```sh
    cd Daikin-AC
    platformio init --board <your_board>
    ```
3. Install the required libraries:
    ```sh
    platformio lib install "IRremote"
    ```

## Usage

1. Configure the AC model and settings in `src/main.cpp`.
2. Build and upload the code to your microcontroller:
    ```sh
    platformio run --target upload
    ```
3. Use the provided functions to send IR signals to your Daikin AC unit.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any questions or suggestions, please open an issue or contact the repository owner.
