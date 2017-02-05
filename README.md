# FlowTM4C

[![badge](https://img.shields.io/badge/conan.io-FlowTM4C%2F1.0-green.svg?logo=data:image/png;base64%2CiVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAMAAAAolt3jAAAA1VBMVEUAAABhlctjlstkl8tlmMtlmMxlmcxmmcxnmsxpnMxpnM1qnc1sn85voM91oM11oc1xotB2oc56pNF6pNJ2ptJ8ptJ8ptN9ptN8p9N5qNJ9p9N9p9R8qtOBqdSAqtOAqtR%2BrNSCrNJ/rdWDrNWCsNWCsNaJs9eLs9iRvNuVvdyVv9yXwd2Zwt6axN6dxt%2Bfx%2BChyeGiyuGjyuCjyuGly%2BGlzOKmzOGozuKoz%2BKqz%2BOq0OOv1OWw1OWw1eWx1eWy1uay1%2Baz1%2Baz1%2Bez2Oe02Oe12ee22ujUGwH3AAAAAXRSTlMAQObYZgAAAAFiS0dEAIgFHUgAAAAJcEhZcwAACxMAAAsTAQCanBgAAAAHdElNRQfgBQkREyOxFIh/AAAAiklEQVQI12NgAAMbOwY4sLZ2NtQ1coVKWNvoc/Eq8XDr2wB5Ig62ekza9vaOqpK2TpoMzOxaFtwqZua2Bm4makIM7OzMAjoaCqYuxooSUqJALjs7o4yVpbowvzSUy87KqSwmxQfnsrPISyFzWeWAXCkpMaBVIC4bmCsOdgiUKwh3JojLgAQ4ZCE0AMm2D29tZwe6AAAAAElFTkSuQmCC)](http://www.conan.io/source/FlowTM4C/1.0/spiessensm/stable)

Demo application using Flow on the Texas Instruments EK-TM4C129EXL launchpad. The application includes GPIO, UART, PWM and USB device (CDC).

###Getting started

1. Install dependencies:
`$ sudo apt-get install build-essential gcc-arm-none-eabi`
2. Download or clone the project.
3. [Get conan.](http://docs.conan.io/en/latest/installation.html)
4. Open a terminal and go to the root folder of the project, it contains `conanfile.py`.
5. Install the conan packages: `$ conan install`. This can take a while the first time.
6. Open TI Code Composer Studio [(get it from Texas Instruments)](http://processors.wiki.ti.com/index.php/Download_CCS).
7. Right click in the project explorer window and do `Import > CCS projects`. In the dialog, point the search-directory to the root folder and select the discovered `FlowTM4C` project.
8. Build and do something fancy!