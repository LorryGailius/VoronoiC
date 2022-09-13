# VoronoiC
Simple Voronoi graph renderer using SDL

# How to use
Run the .exe and place Voronoi points on screen to see a Voronoi graph apear

# How to compile
In order to compile from source [Meson build](https://mesonbuild.com/) and [Developer Command Prompt](https://docs.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022) is required

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
