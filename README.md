# VoronoiC
Simple Voronoi graph renderer made with [SDL2](https://www.libsdl.org/) and C

![](https://user-images.githubusercontent.com/31960595/189929905-039a72a8-69d5-4d3e-8595-907af8516c33.png)

# How to use
Run the .exe and place Voronoi points on screen to see a Voronoi graph appear

# How to compile
In order to compile from source [Meson build](https://mesonbuild.com/) and [Developer Command Prompt](https://docs.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022) are required

Open the Dev Command Prompt and open the source folder directory by using
```powershell
cd [Path_to_folder]
```

then setup a Meson Build directory
```powershell
meson setup build
```
and compile source
```powershell
meson compile -C build
```
