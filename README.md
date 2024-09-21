<h1 align="center">
  ReaderClock
</h1>

<p align="center">
    <a href="https://www.gnu.org/licenses/agpl-3.0">
    <img src="https://img.shields.io/badge/License-AGPL%20v3-blue.svg" />
  </a>
</p>

Simply time - but different!

<img height="400px" src="https://github.com/dorianim/reader-clock/assets/30153207/9211ac49-1091-4b3d-998f-f3657e29a3f0" />

A clock, which shows the current time in form of a quote from a book.

### Setup
- When the ReaderClock is powered in for the first time, it creates a Wi-Fi network
- You need to connect to this network and will get a configuration page
- On that page, press the button "Update Date and Time!"
- Now your ReaderClock has the same date, time and timezone as your phone
- You can always reconfigure this setting in the first five minutes after the ReaderClock is plugged in

### Updating
- To update, you need to disconnect and reconnect the ReaderClock from power to activate the configuration mode
- Then you need to connect to its Wi-Fi hotspot like above
- On the configuration page, scroll down to "Firmware Update" and upload the new firmware

### Disclaimer

Huge thanks to these people for publishing their quotes:

- Jaap Meijers [over here](https://www.instructables.com/Literary-Clock-Made-From-E-reader/)
- [literaryclock.com](http://literaryclock.com)

### Used Libs

- [GxEPD2](https://github.com/ZinggJM/GxEPD2)
- [RTClib](https://github.com/adafruit/RTClib)
- [Pico.css](https://picocss.com/)
- [moment.js](https://momentjs.com/)
- [moment-timezone.js](https://momentjs.com/timezone/)
- [posix-timezone](https://github.com/moment/moment-timezone/issues/314#issuecomment-198226333)

### Used hardware

- ESP32: [Amazon](https://www.amazon.de/-/en/dp/B08BZGC22Q), [AZDelivery](https://www.az-delivery.de/en/products/esp32-dev-kit-c-v4-unverlotet)
- RTC: [Amazon](https://www.amazon.de/-/en/dp/B07RGTFWS3), [AZDelivery](https://www.az-delivery.de/en/products/ds3231-real-time-clock)
- E-Ink display: [Amazon](https://www.amazon.de/gp/product/B074NR1SW2), [Waveshare](https://www.waveshare.com/product/displays/e-paper/4.2inch-e-paper-module.htm)
- Micro usb port: [Amazon](https://www.amazon.de/-/en/WayinTop-Pieces-Adaptor-Breakout-Converter/dp/B07W13X3TD/ref=sr_1_3)

### Pins

| Device  | Pin on Device | Pin on ESP |
| ------- | ------------- | ---------- |
| RTC     | SDA           | 21         |
|         | SCL           | 22         |
| DISPLAY | DIN           | 23         |
|         | CLK           | 18         | 
|         | CS            | 5          |
|         | DC            | 15         |
|         | RST           | 2          |
|         | BUSY          | 4          |
