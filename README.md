# Brief Project Description and Intent:

This project is for my own self-study of CMake while referencing the pikuma course ["2D Game Engine with C++ and Lua"](https://pikuma.com/courses/cpp-2d-game-engine-development). To build, you will need the assets folder provided by Pikuma in the root directory. Platform specific instructions provided below.

I'm seeking to learn how to use CMake for build automation and testing. So, I do intend to make all major platforms supported and integrate testing.

# Windows Instructions:

## CLion:

If you intend to use CLion, be sure you have Windows' MSVC compiler install. This can be accomplished simply by having Visual Studio Community 2022 install. Then, just be sure Visual Studio is your default toolchain under File->Settings->Build, Execution, Deployment->Toolchains. Move it to the top of the list to make it your default compiler. This is necessary since we'll be using vcpkg to fetch the required libraries and vcpkg uses MSVC which includes runtime checks that are not supported by MinGW.

## Visual Studio:

If you intend to use Visual Studio, you will need to acquire Git. You can download Git from https://git-scm.com/downloads/win. Default installation settings should suffice for our needs. Then, of course, you'll need Visual Studio. Visual Studio Community 2022 is recommended. Once installed, be sure to open the Visual Studio Installer, select "modify", and check the option to add "Desktop development with C++" so that you have CMake support.

If the CMake configuration doesn't start automatically, right-click CMakeLists.txt and choose "Configure 2DGameEngine".

The assets folder is not included. Be sure to add your assets folder into the root directory of your project and CMake will copy to your build folder.


# Apple Instructions:

I don't have an Apple computer. Sorry. If I find one at a garage sale, I'll pick it up just for this purpose.

# Linux Instructions:

TODO: I want to provide a CMake to automate builds and tests on three flavors of linux; Debian, Arch, and Redhat.