# Blink Example

This example demonstrates how to blink a LED using GPIO or RMT for the addressable LED, i.e. [WS2812](http://www.world-semi.com/Certifications/WS2812B.html).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.


#### Pin assignments

The table below shows the LED pin assignments.

LED pin                  | DShanMCU-Mio pin     |  Notes
-------------------------|----------------------|-------------
 DShanMCU_Mio            | GPIO48               |

### Configure the Project

Open the project configuration menu (`idf.py menuconfig`). 

In the `Example Configuration` menu:

* Select the LED type in the `Blink LED type` option.
    * Use `GPIO` for regular LED blink.
    * Use `RMT` for addressable LED blink.
        * Use `RMT Channel` to select the RMT peripheral channel.
* Set the GPIO number used for the signal in the `Blink GPIO number` option.
* Set the blinking period in the `Blink period in ms` option.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

As you run the example, you will see the LED blinking, according to the previously defined period. For the addressable LED, you can also change the LED color by setting the `pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);` (LED Strip, Pixel Number, Red, Green, Blue) with values from 0 to 255 in the `blink.c` file.

```
I (315) example: Example configured to blink addressable LED!
I (325) example: Turning the LED OFF!
I (1325) example: Turning the LED ON!
I (2325) example: Turning the LED OFF!
I (3325) example: Turning the LED ON!
I (4325) example: Turning the LED OFF!
I (5325) example: Turning the LED ON!
I (6325) example: Turning the LED OFF!
I (7325) example: Turning the LED ON!
I (8325) example: Turning the LED OFF!
```

Note: The color order could be different according to the LED model.

The pixel number indicates the pixel position in the LED strip. For a single LED, use 0.

