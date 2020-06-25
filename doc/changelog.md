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
# FEP Tooling Base Changelog #

All notable changes to project [FEP Tooling Base] will be documented here.
For upcoming features and known issues see the bottom of this file.

The format is based on [Keep a Changelog][] and this project adheres to [Semantic Versioning][].

## [1.3.0] - 2020-02-14 ##
### Changes ###
- FEPSDK-1234 Launcher Improve error output on remote starting
- FEPSDK-1152 Porting to SCALEXIO QNX
- FEPSDK-2022 Cleanup CMake and conan as preparation for the open source distribution
- FEPSDK-1314 Prepare FEP SDK code to be distributed as OSS

## [1.2.1] - 2019-10-04 ##
### Changes ###
- FEPSDK-1188 Enable fep\_launcher to execute binaries from PATH environment variable
- FEPSDK-1264 Doc update for Remote Starter Aliases and its usage

### Fixes ###
- FEPSDK-1367 fep\_launcher can not use relative paths after update

## [1.2.0] - 2019-08-23 ##
### Changes ###
- FEPSDK-1137 - Update documentation for meta tooling
- FEPSDK-1113 - Add Changelog and version infos to tooling base
- FEPSDK-1097 - Change for Meta Tooling - Common Todo

### Fixes ###
- [FEPSDK-1107] - Paths added to the meta model content must be always relative to the file
- [FEPSDK-1091] - Shutdown fails sporadically
- [FEPSDK-1082] - The FEP Launcher should return 1 when launching failed
- [FEPSDK-1081] - The FEP Launcher does not shutdown started Particpants in case of error by launching

## [1.1.0] - 2019-02-21 ##
### Changes ###
- [FEPSDK-1109] - Extend the element interface by a type
- [FEPSDK-1098] - Add Source of tooling base to the installation package and add debug information (pdbs) to the Debug package
- [FEPSDK-1072] - Create Meta Model Writers
- [FEPSDK-1063] - Create Element Description Writer

## [1.0.0] - 2018-12-03 ##
### Changes ###
- [FEPSDK-992] - Implement FEP Launcher for FEP SDK 2
- [FEPSDK-993] - Implement FEP Controller for FEP SDK 2
- [FEPSDK-1035] - Add support for Remote Starter to Launcher

## [Upcoming][] ##
Shows the upcoming features.

## [Known issues][] ##
Shows the currently known issues.
