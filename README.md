# 3D-Graphics-and-Visualizations


Overview

MyVillage is a real-time 3D graphics project built with OpenGL, GLFW, and GLEW. It features an interactive scene with camera movement, realistic lighting, textures, and model rendering.

Features 🚀

🏡 3D Scene Rendering – A fully designed village environment
🎥 Camera System – Smooth movement with first-person & third-person views
🌌 Skybox – Adds depth and realism to the scene
🔦 Lighting & Shadows – Dynamic light sources and shadow mapping
📦 OBJ Model Loading – Uses TinyObjLoader to import .obj models
🎨 Texture Mapping – Applied via stb_image.h
Technologies Used 🛠️

C++ & OpenGL – Core graphics engine
GLFW & GLEW – Window and OpenGL extensions
GLM – Math library for transformations
stb_image.h – Image loading for textures
TinyObjLoader – For loading .obj 3D models
Controls 🎮

WASD – Move the camera
Arrow Keys – Rotate objects
Mouse – Adjust camera view
Esc – Exit the application
Installation & Running ⚡

Clone the repository
git clone https://github.com/yourusername/MyVillage.git
cd MyVillage
Install dependencies
Ensure GLFW, GLEW, and a C++ compiler are installed
On Linux, install with:
sudo apt install libglfw3-dev libglew-dev
Build and run
mkdir build && cd build
cmake ..
make
./MyVillage
Screenshots 📸

![Uploading MyVillage.png…]()


Future Improvements ✨

✅ Particle effects (fire, smoke)
✅ Physics enhancements (collision detection)
✅ More detailed textures and normal mapping
