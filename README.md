# Solar powered BLE weather station

## Introduction

I wanted a weather sensor for the garden, and I wanted to use the [BTHome protocol](https://bthome.io/format/),
so as to make integration with [Home Assistant](https://www.home-assistant.io/) straightforward.

Whenever we create a solar powered project, it is essential to minimise energy consumption of the electronics. This project achieves this by sending only one
BLE advertisement at a time and entering deep sleep immediately upon the broadcast of that advertisment.

## Hardware used

I used a [Seeed Studio XIAO ESP32C3 module](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html),
an SHT20 temperature and humidity sensor, and I mounted the project on a [perma-proto board](https://www.aliexpress.com/item/4000301371545.html)

The solar power system I used came from [Jasper Sikken's excellent solar power harvester](https://www.tindie.com/products/jaspersikken/solar-harvesting-into-lithium-ion-capacitor/),
which keeps a 250F Lithium-Ion capacitor charged from solar energy, and provides 3.3v for the project.

## Software

I could not get the software to work with Visual Studio Code, so I wrote it using the [Arduino IDE](https://docs.arduino.cc/software/ide/) instead.

The code broadcasts a single BLE advertismenet about once every 10 seconds, and reads the sensors once every 6 broadcasts.

## Housing

I designed a housing for the project in [FreeCAD](https://www.freecad.org/) - the design is in the *Housing.FCStd* file. It is designed to be cable-tied to a
fence, and uses 2 threaded inserts to keep the circuit board in place, and one to keep the two parts of the housing attached.

## Performance

I believe the sensor will work in total darkness for at least 2 solid continuous weeks. So even in dullest darkest Ireland (where I live) I expect this sensor
to work continuously.
