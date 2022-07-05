![hddled logo](./logo.png)

# HDDLED for Linux

The name speaks for itself - the program provides software emulation of a hardware HDD LED for Linux graphical desktops.

## Copyright and license

Copyright © 2022 Sławek Wernikowski (slawern)  
This code is released under the [MIT](./LICENSE) license.

## Motivation

The reason why I decided to start the project was a newly bought laptop, which - unfortunately - did not have an HDD LED. The lack of HDD LED flickering was a bit frustrating and finally, I decided to bring it back to life myself. 

The main idea was inspired by mdoege's [hd-led_pygtk](https://github.com/mdoege/hd-led_pygtk).

## Code

The code is entirely written in C99 with the use of GTK+ 3.0 and GDK 2.0. When launched, the program inserts a status icon into the notification area. Each block device recognized in the system is shown as a separate icon so you have the chance to monitor separately the activity of not only each built-in hard drive but also of all currently connected external devices. The program can also be run as a daemon while the colors of the software LEDs are configurable from the command line. If you want, you can also design your own LEDs' shapes and colors.

The default hddled icon presents two arrows: the upper one reflects disk reads and the lower one - disk writes. 

## Files

The project itself consists of the following files:

````
 main.c     - project's code
 icon.h     - LED icon's pattern in XPM format
 Makefile   - simple (definitely too simple) makefile for building the hddled binary
````

## Installing

You have to ensure that development files of both GTK+ 3.0 and GDK 2.0 are available in your environment. If yes, the following sequence should produce the hddled binary. 

````
git clone https://github.com/slawern/hddled.git
cd hddled
make
````

## Usage

When launched without any option, hddled will start immediately in non-daemon mode, showing one status icon for each detected block device (excluding loop devices). A list of possible options is presented when -h option is used.

````
$ ./hddled -h

Usage: hddled [options...]

Possible options are:
 -b RRGGBB       - LEDs' background color (defaults to 000000)
 -d              - daemonize hddled process
 -h              - print this help message and quit
 -i RRGGBB       - inactive LED color (defaults to 3C3C3C)
 -l              - print license notice and quit
 -r RRGGBB       - disk read LED color (defaults to 00FF00)
 -u milliseconds - interval between subsequent disk activity checks (defaults to 100)
 -q              - quit the previously started daemon
 -w RRGGBB       - disk write LED color (defaults to 0000FF)
````

## What's next?

The project is still in a very preliminary stage - I only tested it on my computer, so I'm not able to ensure its correct operation in all conditions. Your feedback will be much appreciated!