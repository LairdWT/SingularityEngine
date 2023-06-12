****************************************************************************************************
This engine works using a folder "libs" that contains the folders "glm", "glfw", and "vulkan_skd"
****************************************************************************************************

## How to use

### Windows only

1. Clone the repository
2. Create a "libs" folder in the root directory
3. Download the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)
4. Download the [GLFW](https://www.glfw.org/download.html)
5. Download the [GLM](https://glm.g-truc.net/0.9.9/index.html)
6. Install each of these into the "libs" folder, renaming them to match the folder names above - case sensitive
7. Open the project in Visual Studio
8. Follow the Vulkan SDK instructions for setting up the Include directories and dependencies (https://vulkan-tutorial.com/Development_environment)
9. run the CompileShaders.bat file
10. Make sure to use the new SDK path for the Include and Dependencies directories in the "libs" folder
11. Run in Release mode