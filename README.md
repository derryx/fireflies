# Firefly Simulation with ATMEGA328p


## Compiling and flashing with WSL2 with Windows
1. Install WSL2 in Windows
```
wsl --install
```
2. Install AVR packages in WSL2
```
sudo apt-get install cmake gcc-avr avr-libc avrdude gdb git 
```
3. Install USB-support for WSL2: https://docs.microsoft.com/en-us/windows/wsl/connect-usb
4. List and mount USB device (Admin cmd.exe):
```
usbipd wsl list
usbipd wsl attach --busid 3-3
```