| Supported Targets | [DShanMCU-Mio](https://forums.100ask.net/c/esp/esp32s3/50) |
| ----------------- | ------------ |

# Pin assignments

The table below shows the TFT LCD display pin assignments.

TFT LCD display Pin      | SPI pin | DShanMCU-Mio pin     |  Notes
-------------------------|---------|----------------------|-------------
 PWM                     |         | GPIO7                |
 RESET                   |         | GPIO6                |
 MOSI                    | MOSI    | GPIO11               |
 SCK                     | SCK     | GPIO12               |
 RS                      |         | GPIO5                |
 CS                      | CS      | GPIO10               |
 MISO                    | MISO    | -/GPIO13             |


The table below shows the MicroSD Card Adapter pin assignments.

MicroSD Card Adapter pin | SPI pin | DShanMCU-Mio pin     |  Notes
-------------------------|---------|----------------------|-------------
 CS                      | CS      | GPIO15/GPIO8         |
 SCK                     | SCK     | GPIO16/GPIO12        |
 MOSI                    | MOSI    | GPIO17/GPIO11        |
 MISO                    | MISO    | GPIO18/GPIO13        |


The table below shows the FC Joypad Adapter pin assignments.

FC Joypad Adapter pin    | DShanMCU-Mio pin     |  Notes
-------------------------|----------------------|-------------
 ID                      | GPIO40               |
 D+                      | GPIO41               |
 D-                      | GPIO42               |
