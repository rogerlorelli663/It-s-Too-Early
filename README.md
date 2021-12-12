# It-s-Too-Early

## Installation
* Upload Project1.ino to main Arduino unit.
* Upload TransmitterV2.ino to secondary Arduino unit that is connected to RPi and nRF24L01 chip.
* Place contents of directory var/ into the /var of the RPi webserver
    * Check Following: 
        * threshold_interface.html is writeable to group
        * tableclear.py is executable
        * project.py is executable
