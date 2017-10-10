# TextureGen

This is a post processing texturizer built to add texture to a reconstructed scene. 
The current version was built by Marcelo Gennari (marcelo.gennaridonascimento@wadham.ox.ac.uk).

# 1. Requirements
  - OpenGL (v 3.0.0)
  - Freeglut (v 2.8)
  
# 2. Build

Building procedures is straight forward.
```
  $ mkdir build
  $ cd build
  $ cmake <pathToTextureGen>/
  $ make
```

# 3. Running

The Program takes as an input the path to a directory with the following information:

    .
    ├── Poses                   # A directory with a List of txt files with the pose of the camera at each frame (SE3)
    ├── Frames                  # A directory with a List of ppm files named with 4 digits (e.g. 0001.ppm)
    ├── calib.txt               # A camera calib.txt file in the same way required by InfiniTAM (see https://github.com/victorprad/InfiniTAM)
    └── mesh.stl                # The Point cloud for the full reconstructed scene
    
 To run using this you just need to provide the path to the directory containing the above information:
 ```
  $ ./TextureGen /path/to/folder
 ```
 
# 4. Examples

Running the program without any arguments will use the Teddy example, which is located in the TextureGen/Teddy directory.

```
  $ ./TextureGen
 ```
