# FlowTM4C

[We have conan packages!](https://bintray.com/cynarakrewe/CynaraConan)

Demo application using Flow on the Texas Instruments EK-TM4C129EXL launchpad. The application includes GPIO, UART, PWM and USB device (CDC).

### Getting started

1. Install dependencies:
`$ sudo apt-get install build-essential gcc-arm-none-eabi`
2. Download or clone the project.
3. [Get conan.](http://docs.conan.io/en/latest/installation.html)
4. Open a terminal and go to the root folder of the project, it contains `conanfile.py`.
5. Install the conan packages: `$ conan install`. This can take a while the first time.
6. Open TI Code Composer Studio [(get it from Texas Instruments)](http://processors.wiki.ti.com/index.php/Download_CCS).
7. Right click in the project explorer window and do `Import > CCS projects`. In the dialog, point the search-directory to the root folder and select the discovered `FlowTM4C` project.
8. Build and do something fancy!
