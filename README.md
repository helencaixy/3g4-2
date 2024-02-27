# 3G4 Lab Vulkan Version

## Build Instructions

### Windows (Confirmed to work)

#### Method 1
- Install Visual Studio 2022, or newer
- Install Vulkan SDK from [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/)
- Install wxWidgets version >= 3.2 from [https://www.wxwidgets.org/downloads/](https://www.wxwidgets.org/downloads/). Install the Windows Binaries, following the given instructions there
- Clone this repository
- Open the repository as a folder in Visual Studio
- Select either "Windows x64 Debug" or "Windows x64 Release" in the dropdown top toolbar
- Build project

#### Method 2
- Install Visual Studio Code
- Install C/C++ and CMake Tools extensions
- Install Vulkan SDK from [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/)
- Install wxWidgets version >= 3.2 from [https://www.wxwidgets.org/downloads/](https://www.wxwidgets.org/downloads/). Install the Windows Binaries, following the given instructions there
- Clone this repository
- Open the repository as a folder in Visual Studio Code
- Select either "Windows x64 Debug" or "Windows x64 Release" in the dropdown bottom status
- Build project

### Ubuntu (Confirmed to work)
- Install the required build, and Vulkan packages:
```
apt install build-essential git libgtk-3-dev
```
- Install the Vulkan SDK, following the instructions at [https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html](https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html)
- Install wxWidgets version >= 3.2 following the instructions at [https://docs.codelite.org/wxWidgets/repo32/](https://docs.codelite.org/wxWidgets/repo32/)
- Clone this repository
- Install CMake, and optionally CMake GUI
```
apt install cmake cmake-gui
```
- Create the project files using CMake GUI
	- Using CMake GUI, set the source directory to the cloned repository, and the build directory to /src within
	- Configure, and generate the project files. Select your preferred generator; Makefile and Ninja are confirmed to work
	- Project files can be found in /src folder
- Alternatively, use CMake from a terminal
	- ```cd``` into the cloned repository
	- ```cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B src```
- Build project
	- Build from an IDE or,
	- Build from the terminal
		- ```cd``` into the cloned repository
		- ```cd src```
		- ```make```

### Other Linux Distros (Rocky Confirmed Working, Others Untested)
- Follow instructions for Ubuntu, substituting ```apt``` for your prefered package manager, eg. ```dnf```, and packages for their equivalents

### macOS (Confirmed to work on macOS 13 Ventura)

#### Method 1 - Homebrew (recommended)
- Install Vulkan SDK from [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/)
- Install Homebrew if it is not already installed:
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
- Install wxWidgets:
```
brew install wxwidgets
```
- Install CMake:
```
brew install cmake
```
- ```cd``` into the cloned repository
- ```cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B src```
- Build project
    - ```cd``` into the cloned repository
    - ```cd src```
    - ```make```
		
#### Method 2 - MacPorts
- Install Vulkan SDK from [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/)
- Install MacPorts if it is not already installed, following the instructions at [https://www.macports.org/install.php](https://www.macports.org/install.php)
- Install wxWidgets:
```
sudo port install wxWidgets-3.2
```
- Create an alias to 'wx-config  in '/usr/local/bin'
    - When in Finder, press Cmd+Shift+'.' to show hidden files and folders
    - Find 'wx-config', which is likely in '/opt/local/Library/Frameworks/wxWidgets.framework/Versions/wxWidgets/<version no.>/bin/'
    - Select the file and choose 'File > Make Alias'
    - Copy the alias file to '/usr/local/bin'
    - Rename the alias to 'wx-config'
- Install CMake:
```
sudo port install cmake
```
- ```cd``` into the cloned repository
- ```cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -B src```
- Build project
    - ```cd``` into the cloned repository
    - ```cd src```
    - ```make```
		
#### Xcode Development (optional)
- Follow macOS Method 1 or 2 until CMake is installed
- ```cd``` into the cloned repository
- ```cmake -G "Xcode" -B src```
- Open Xcode project in the /build folder
- Change the build configuration to 'Release'
- Build project


## Repository Credits
Repository originally forked from wxVulkanTutorial [https://github.com/jimorc/wxVulkanTutorial](https://github.com/jimorc/wxVulkanTutorial). wxVukanTutorial is a port of the Hello Triangle portion of the Vulkan Tutorial at [www.vulkan-tutorial.com](https://www.vulkan-tutorial.com).