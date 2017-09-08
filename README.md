# bus-iot
An implement of STM32469 about Bus-Station-Center IOT system.

This is a project for [ARM Design Contest](http://www.armdesigncontest.com/event/).

## Project Introduce
You can read the Chinese document in [`/document`](https://github.com/tigercosmos/bus-iot/blob/master/station), and the video is also there.

## Implement
This project has three parts, which implements an IOT system.

### Bus
The device on bus, which is in [`/bus`](https://github.com/tigercosmos/bus-iot/blob/master/bus) folder, and it is developed by [`mbed`](https://developer.mbed.org/).

### Station
The device in station, which is in [`/station`](https://github.com/tigercosmos/bus-iot/blob/master/station) folder, and it is developed by [`keil`](https://www.keil.com/). Opening [`Project.uvprojx`](https://github.com/tigercosmos/bus-iot/blob/master/station/project/MDK-ARM/Project.uvprojx) with `keil` would be fine.

### Center
The console center with frontend and backend, which is in [`/center`](https://github.com/tigercosmos/bus-iot/blob/master/center) folder.

You can see the frontend demo [here](https://tigercosmos.github.io/bus-monitor/#).

## Authers
- @tigercosmos 劉安齊
- @kartdsam 黃懋翔
- @selina4715 謝云綺

## License
MIT
