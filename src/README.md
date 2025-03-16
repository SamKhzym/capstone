# Simulator Source Code

The folders and files for this project are as follows:

...

The standard I'm using to make tests
https://crccalc.com/?crc=32,12&method=CRC-8/SMBUS&datatype=hex&outtype=hex

It works on
# Simulator Source Code

Steps for running SIL program: 
1. Go to src folder
2. pip install -r requirements.txt
3. Go to the controller folder
4. mkdir build; cd build
5. cmake ../configs/sil
6. cmake --build . --config Release -j
7. Make sure controller_shared.dll is in the bin folder
8. Go to simulator folder
9. python sil.py

Steps for running HIL program:
1. Make sure you can run the SIL
2. Follow the steps here to install Zephyr: https://docs.zephyrproject.org/latest/develop/getting_started/index.html#clone-zephyr
   1. On Windows:
   2. python -m venv zephyrproject\.venv (with python >= 3.10)
   3. zephyrproject\.venv\Scripts\Activate.ps1
   4. pip install west
   5. west init zephyrproject
   6. cd zephyrproject
   7. west update
   8. west zephyr-export
   9. west packages pip --install
   10. cd zephyrproject/zephyr
   11. west sdk install
3. use: west build -p always -b stm32h735g_disco samples\net\sockets\echo
 - This will create a .elf file that can be flashed onto the board
4. Open powershell and used this command: netsh interface show interface
 - Write down which ethernet you found active (ipconfig /all can help to determine which is correct)
5. Next you have to set the ip address of your ethernet to match the project configurations (check the IPv6 address in prj.conf file). The board will have an address like fd12:3456:789a::1 and your ethernet must be fd12:3456:789a::2.
6. Use this command to set the IPV6 address for the computer: netsh interface ipv6 add address "Ethernet 4" fd12:3456:789a::2
 - You might have to change Ethernet 4 above 
 - Note that the demo example from above has a different IP address

7. Download the STM32 Cube Programmer
8. Plug in the STM board micro usb cable for power and ethernet for connection
9.  Open the programmer, go to erasing and programming, set the file path to the elf file created by west build  
10. Set reset mode to "Hardware Reset" and click green connect button then start programming.
11. Press the black button on the board to reset. 

12. Can now go into command terminal and ping: ping fd12:3456:789a::1 
 - Should see that it is responding back and this will happen every time even if unplugging the setup 

13. Can now open up telnet (I have installed this on my computer for these tests but can also send these TCP requests any other way desired) 

14. The bind port for this code is 4242 so I have used: o fd12:3456:789a::1 4242 to open the TCP server that is on the microcontroller 

15. Through telnet I have also used sen "230" and sen "130" and verified that it is indeed picking it up in wireshark. 

16. Now to build our safety board code use: west build -p always -b stm32h735g_disco -s [PATH_TO_CAPSTONE/src/controller/configs/hil]
 - This points zephyr to the cmakelists that configures the build
- Follow the same steps to flash the code onto the board
