# Firefly Simulation with ATMEGA328p


## Compiling and flashing with WSL2 with Windows
### Install WSL2 in Windows
```
wsl --install
```
### Install AVR packages in WSL2
```
sudo apt-get install cmake gcc-avr avr-libc avrdude gdb git 
```
### Install USB-support for WSL2:
https://docs.microsoft.com/en-us/windows/wsl/connect-usb

Inside the WSL2 VM:
```
sudo apt install linux-tools-virtual hwdata
sudo update-alternatives --install /usr/local/bin/usbip usbip `ls /usr/lib/linux-tools/*/usbip | tail -n1` 20
```

Add file `/etc/udev/rules.d/99-avr-programmer.rules` with content
```
KERNEL=="ttyACM*", GROUP="dialout", MODE="0660", ACTION=="add"
```

### List and mount USB device (Admin cmd.exe):
```
usbipd wsl list
usbipd wsl attach --busid 3-3
```
