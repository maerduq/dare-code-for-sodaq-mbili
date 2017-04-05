# DaRe Coding for Sodaq Mbili
This is an example implementation of [DaRe Coding for LoRaWAN](https://github.com/maerduq/dare-coding)

## Requirements
* This example has been tested with a Sodaq Mbili Rev. 4 in combination with an Embit LoRabee module. A different LoRa module could require some changes in the example code.
* The *SoftwareSerial* library is required 

## Setup
* You need at least the Arduino IDE version 1.6.7 (because it needs support for including sources from subdirectories)
* To get the Arduino IDE working for a Sodaq board follow [this manual](http://support.sodaq.com/sodaq-one/getting-started/)
* Enter your LoRaWAN ABP keys in the `CMD_DEV_ADDR`, `CMD_APPSKEY`, and `CMD_NWKSKEY` variables in `dare-coding-for-sodaq-mbili.ino`
