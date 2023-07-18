****************************************************************************************************
This project requires using a folder "libs" that contains the folders "glm", "glfw", and "vulkan_skd"
****************************************************************************************************

This project began as a demonstration of the "Render a triangle" test using the Vulkan API, and has grown into a simple game engine project.
I will continue to grow and update the functionality as time allots.
No warranty is included or implied in this project.

## How to use

### Windows only

1. Clone the repository
2. Download the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)
3. Download the [GLFW](https://www.glfw.org/download.html)
4. Download the [GLM](https://glm.g-truc.net/0.9.9/index.html)
5. Install each of these into the "libs" folder, renaming them to match the folder names above - case sensitive
6. Open the project in Visual Studio
7. Follow the Vulkan SDK instructions for setting up the Include directories and dependencies (https://vulkan-tutorial.com/Development_environment)
8. run the CompileShaders.bat file
9. Make sure to use the new SDK path for the Include and Dependencies directories in the "libs" folder
10. Run in Release mode
