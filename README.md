# **Model Previewer**
> Name will change

# Building
## Windows & Visual Studio


### Step 1: Create Solution File
```cmd
cd path/to/download/with/premake5.lua
premake5 vs(your visual studio version)
```
for example
```cmd
premake5 vs2022
```
### Step 2: Build
Open the solution file (.sln) and then press <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd> to build.
### Alternativly:
Open the solution file (.sln), right click it in the solution explorer, then press build
> I will add a video soon for you incompetent bunch

### Step 3: Run the executable
Now that the project was built, you just need to run it:
```cmd
cd path/to/download
cd bin/Release
Survive.exe
```
