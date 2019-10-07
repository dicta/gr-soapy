# gr-soapy: SoapySDR GNU Radio Out-Of-Tree Module
gr-soapy is a GNURadio wrapper for the SoapySDR library.

## Install

### Requirements
* GNU Radio ( >= 3.8)
* CMake ( > 3.1)
* G++
* VOLK
* git
* SoapySDR

Additionally the user must install the corresponding plugin from
the Pothosware repository (https://github.com/pothosware) depending
on the device they want to use.

### Installation

1. `git clone https://gitlab.com/librespacefoundation/gr-soapy`
2. `cd gr-soapy`
3. `mkdir build`
4. `cmake ..`
5. `make`
6. `sudo make install`

If this is the first time you are building the gr-soapy module run
`sudo ldconfig`

#### Advanced
By default, the **gr-soapy** module will use the default installation prefix.
This highly depends on the Linux distribution. You can use the `CMAKE_INSTALL_PREFIX`
variable to alter the default installation path.
E.g:

`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`


Another common control option is the library suffix of the Linux distribution.
There are distributions like Fedora, openSUSE, e.t.c that the their 64-bit version
use the `lib64` folder to store the 64-bit versions of their dynamic libraries.
On the other hand, distributions like Ubuntu do the exact opposite. They use
`lib` directory for the libraries of the native architecture and place the 32-bit versions
on the `lib32` directory. In any case the correct library directory suffix
can be specified with the `LIB_SUFFIX` variable. For example:

`cmake -DLIB_SUFFIX=64 -DCMAKE_INSTALL_PREFIX=/usr -DINCLUDE_DEBUG_BLOCKS=OFF ..`

will install the libraries at the `/usr/lib64` directory.


## License

&copy; 2018 [Libre Space Foundation](http://librespacefoundation.org).

Licensed under the [GPLv3](LICENSE).
