# HowTo - Espressif Toolchain installieren
## Folgende Befehle im Linux-Subsystem ausführen

* `sudo apt-get update`
* `sudo apt-get upgrade`
* `sudo apt-get install git autoconf build-essential gperf bison flex texinfo libtool libncurses5-dev wget gawk libc6-dev python-serial python-pip libexpat1-dev esptool libtool-bin unzip libgmp-dev libisl-dev`
* `sudo mkdir /opt/esp8266`
* `sudo chown Username:Username /opt/esp8266/`
* `cd /opt/esp8266`
* `wget https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz`
* `mv xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz xtensa-lx106-elf.tar.gz`
* `tar -xvzf xtensa-lx106-elf.tar.gz`
* `PATH=$PWD/xtensa-lx106-elf/bin/:$PATH` **<- Dieser Befehl muss bei jedem Start ausgeführt werden**
* `git clone https://github.com/espressif/ESP8266_NONOS_SDK.git`
* `rm xtensa-lx106-elf.tar.gz`
* `git clone https://github.com/espressif/esptool.git`
* `PATH=$PWD/esptool/:$PATH` **<- Dieser Befehl muss bei jedem Start ausgeführt werden**

## Folgende Befehle unter Windows ausführen

* In einen beliebigen Ordner unter `C:` wechseln</br>
(z.B. `cd C:/Users/%USERNAME%/Documents/`)
* `git clone https://github.com/Bresenham/Netzwerke2Projekt.git`

## Folgende Befehle im Linux-Subsystem ausführen

* `cd /mnt/c/%der Pfad zum Netzwerke2Projekt Repository%/ESP8266/`</br>
(z.B. `cd /mnt/c/Users/%USERNAME%/Documents/Netzwerke2Projekt/ESP8266/`)
* `make COMPILE=gcc BOOT=2 APP=0 SPI_SPEED=40 SPI_MODE=DIO SPI_SIZE_MAP=4`

# HowTo - Firmware auf den ESP8266 flashen
1. Zuerst die Espressif Toolchain installieren, sonst wird das nicht klappen.
1. Kabel *USB-A* auf *Micro-USB-B*. Das *Micro-USB-B*-Ende an das ESP8266-Board anschließen, *USB-A* an den Laptop / PC.
1. Im Geräte-Manager `Anschlüsse (COM & LPT)` nachschauen, welcher virtuelle COM-Port geöffnet wurde, beispielhaft nehmen wir an, `COM4` wurde geöffnet.
## Folgende Befehle im Linux-Subsystem ausführen
* `sudo chmod 666 /dev/ttyS4` - hier `ttyS4` durch die Nummer des virtuellen COM-Ports ersetzen</br>
**Dieser Befehl muss bei jedem Start ausgeführt werden**
* `make clean`
* `make COMPILE=gcc BOOT=2 APP=0 SPI_SPEED=40 SPI_MODE=DIO SPI_SIZE_MAP=4`
* `make flash PORT=/dev/ttyS4` - hier `ttyS4` durch die Nummer des virtuellen COM-Ports ersetzen</br>
Die LED des ESP8266 sollte nun für ein paar Sekunden wild blinken.</br>
Nach der Ausgabe `Leaving...\nHard resetting via RTS pin...` ist das Programm auf den ESP8266 aufgespielt.
