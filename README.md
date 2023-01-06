# gr-soapy: SoapySDR GNU Radio Out-Of-Tree Module
gr-soapy is a GNURadio wrapper for the SoapySDR library.

## Install

### Requirements
* GNU Radio ( > 3.7.7 )
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
4. `cd build`
5. `cmake ..`
6. `make`
7. `sudo make install`

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

### Coding style
For the C++ code, `gr-soapy` uses a slightly modified version of the 
**Stroustrup** style, which is a nicer adaptation of the well known K&R style.
In addition, we decided to decrease the indentation from 4 to 2 spaces.
This choice was made mainly to avoid braking statements with long namespaces.
We also found ourselves, that with smaller indentation we use more descriptive
variable names, avoiding frustrating abbreviations without phoenixes etc. 

At the root directory of the project there is the `astyle` options 
file `.astylerc` containing the proper configuration.
Developers can import this configuration to their favorite editor. 
In addition the `hooks/pre-commit` file contains a Git hook, 
that can be used to perform before every commit, code style formatting
with `astyle` and the `.astylerc` parameters.
To enable this hook developers should copy the hook at their `.git/hooks` 
directory. 
Failing to comply with the coding style described by the `.astylerc` 
will result to failure of the automated tests running on our CI services. 
So make sure that you either import on your editor the coding style rules 
or use the `pre-commit` Git hook.

## License

&copy; 2018 [Libre Space Foundation](http://librespacefoundation.org).

Licensed under the [GPLv3](LICENSE).
