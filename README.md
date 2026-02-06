# AoLibrary

A C++ library that is pretty much a collection of third-party libraries and my own implementations and some, and then centralized for ease of usage.

***

## About the Name

**Ao (青)** means *blue* in Japanese.

There is no deep technical meaning behind the name. I just really like the color blue.

***

## Installation

AoLibrary is a simple library and needs only a simple process of dropping AoLibrary folder into your project folder and that's it.

#### Basic Setup

1.) Copy and drop `AoLibrary` folder to your project folder, excluding the Visual Studio files. **NOTE:** Not the repository folder of the same name

2.) Add these folders to your project’s Include Directories:

 * `[Your project path]/AoLibrary`  

 * `[Your project path]/AoLibrary/third-party`

That’s all. All done!

### Visual Studio Setup

1.) Copy and paste `AoLibrary` folder

2.) Add the `AoLibrary` visual studio project files to your solution

3.) Add these folders to your project’s Include Directories:

 * `[Your project path]/AoLibrary`  

 * `[Your project path]/AoLibrary/third-party`
	
4.) The static library it builds is created on `[Your project path]/AoLibrary/lib/[build configuration]`

You can now use the library in your code:

```cpp
#include <aol/aol.h>
#include <aol/serialization/serialiation.h>
#include <aol/data_components/profiles.h>
```

***

## Third-Party Dependencies

AoLibrary relies on several external libraries. These are **not included** in the repository and must be obtained separately by the user.

For more details, see:

- [THIRD_PARTY_NOTICE.md](THIRD_PARTY_NOTICE.md)
- [BUILDING.md](BUILDING.md)

***

## License

This project is licensed under the MIT License.  
Third-party libraries used by the library remain under their respective licenses.