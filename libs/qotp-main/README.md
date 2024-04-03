# qotp - Qt-Based OTP Library

`qotp` is a Qt-based library providing HMAC-Based One-Time Password (HOTP) generation functionality as per RFC 4226 and Time-Based One-Time Password (TOTP) generation as per RFC 6238.

It's designed to be easy to use in any Qt application, offering both standard HOTP and convenient wrappers for HOTP generation using Base32 and Base64 encoded secrets.

## Features

| Feature | Description |
|---|---|
| 🚀 HOTP Generation | Implements the HMAC-Based One-Time Password algorithm as specified in [RFC 4226](https://datatracker.ietf.org/doc/html/rfc4226). |
| 🚀 TOTP Generation | Implements the Time-Based One-Time Password algorithm as specified in [RFC 6238](https://datatracker.ietf.org/doc/html/rfc6238). |
| ❗ Convenience Wrappers | Provides functions for generating HOTP using Base32 or Base64 encoded secrets, making integration easier. |
| 🤌 Qt Integration | Seamlessly integrates with Qt applications, leveraging Qt data types and functionalities for a native feel. |

## Getting Started

### Prerequisites
- Qt 6.x
- CMake 3.20 or higher (for building the project)

### Installation

1. Clone the Repository
```
git clone https://github.com/svnscha/qotp.git
cd qotp
```

2. Build with CMake
```
mkdir build && cd build
cmake ..
make
make install
```

## Usage
Using `qotp` is straightforward and intuitive. For detailed function descriptions and parameters, please refer to the documentation in `libqotp/qotp.h`.

Below is an example demonstrating how to generate an HOTP value, taken from unit tests:

```cpp
void test_match_rfc()
{
    const auto key = QByteArrayView("12345678901234567890");
    QCOMPARE(libqotp::hotp(key, 0), QLatin1String("755224"));
    ...
}
```

This snippet shows how to generate an HOTP code using a predefined secret key and a counter value. It's a simple illustration of the library's core functionality in action.


## Running Tests
To run the tests, use the following steps:

1. Configure the project with testing enabled:
```
cmake -DWITH_TESTING=ON ..
```

2. Build and run the tests:
```
make
ctest
```

## Contributing
Contributions to `qotp` are welcome! Feel free to open issues or submit pull requests.

## Development

### Setting up Git Hooks

This project uses Git hooks to enforce certain rules and perform checks before committing code.

To enable these hooks, run the following command:

```bash
git config core.hooksPath .githooks
```


## License
This project is licensed under the MIT License.
