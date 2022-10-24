| Supported Targets | [DShanMCU-Mio](https://forums.100ask.net/t/topic/1033) | 
| ----------------- | ----- |

# EPD 100ASK Example

[100ASK](https://www.100ask.net) EPD example.

## How to Use Example

### Hardware Required

* An ESP development board, with SPI EPD

Connection :

Depends on boards. Refer to `epd_100ask_example_main.c`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)


## Example Output

On [DShanMCU-Mio](https://forums.100ask.net/t/topic/1033) there will be:

```
I (1200) EPD_100ASK_DRIVERS: Initialized successfully!                          
I (1200) EPD_100ASK_PAINT: Initialized successfully!                            
I (1200) EPD_100ASK_EXAMPLE_MAIN: Base testing...                               
I (13730) EPD_100ASK_EXAMPLE_MAIN: Base test successfully!                      
I (18730) EPD_100ASK_EXAMPLE_MAIN: Paint testing...                             
I (27330) EPD_100ASK_EXAMPLE_MAIN: Paint test successfully!
```

At the meantime, observe the changes of the display.

