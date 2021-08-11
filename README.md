## ℹ️ This repository is archived 

It is now maintained at https://github.com/cariad-tech


---
<!---
  Copyright @ 2019 Audi AG. All rights reserved.
  
      This Source Code Form is subject to the terms of the Mozilla
      Public License, v. 2.0. If a copy of the MPL was not distributed
      with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
  
  If it is not possible or desirable to put the notice in a particular file, then
  You may include the notice in a location (such as a LICENSE file in a
  relevant directory) where a recipient would be likely to look for such a notice.
  
  You may add additional accurate notices of copyright ownership.
  -->
# fep_tooling_base Library {#mainpage}

# Description

FEP System description and Tooling 

_____________________
 
# Library Parts
 
This library contains following internal parts: 
 
## Meta Model

### Model Definition
Abstraction Model to describe a FEP System. 
See [meta_model.md](doc/input/meta_model.md)

### Model Parser


## fep_controller_base

 
## fep_launcher_base
________________________

# License Information
 
This library is provided under [MPL 2.0](doc/input/mpl.md)
Have also a look at doc/licence for all license information. 

See also [used licenses](doc/input/used_licenses.md)

________________________

# Change Log

For change history have a look at [Change Log](doc/changelog.md)
________________________
 
# Dependencies

## a_util library
 
The libraries above depend on the *a_util library* 

## How to build

Use conan and cortex if possible ! ;-)

### Build Environment
 
The libraries are build and tested only under following compilers and operating systems: 

#### Windows 10 64 Bit

* Visual Studio C++ 2015 Update 3.1 (Update 3 and KB3165756)
 
#### Linux Ubuntu 16.04 LTS

* On other distributions make at least sure that libc has version >= 2.23 and libstdc++ >= 6.0.21.
* gcc 5.4 
 
### How to build
 
If you can not reach the above online repositories the bundle of it is delivered within a separate download area or installation. 





 
