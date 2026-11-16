Bonsai -- A GPU gravitational [BH]-tree code
============================================

Copyright [2010-2017] 
  Jeroen Bédorf <jeroen@bedorf.net>
  Evghenii Gaburov <egaburov@dds.nl>

License
-------

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this code except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

## BonsaiSPH

The version of Bonsai that has support for SPH can be found in the `BonsaiSPH` branch of this repository.

Modern build (CUDA 13 / RTX 50xx)
---------------------------------

Tested on an RTX 5090 (sm_120) with CUDA 13.4. The runtime was updated
for C++17/CCCL, current tree-walk buffer sizes, host stack usage, and
GLSL 1.20 shader rules.

Build inside the repository:

```bash
make              # visual demo -> build/bonsai2
make run-visual   # build and start the demo
make headless     # -> build-headless/bonsai2_slowdust
make run-headless
```

The Makefile detects the local compute capability, prefers Ninja and
gcc-15, and passes the matching `-gencode` flag (sm_120 on a 5090).
`make run-visual` also enables NVIDIA PRIME offload for CUDA-GL interop
when the display is connected to another GPU.

Manual CMake for sm_120:

```bash
cmake -S runtime -B build -G Ninja \
  -DUSE_MPI=OFF -DUSE_CUB=0 -DUSE_DUST=1 -DUSE_OPENGL=1 \
  -DCMAKE_CXX_COMPILER=g++-15 -DCMAKE_C_COMPILER=gcc-15 \
  -DCUDA_NVCC_FLAGS="-gencode;arch=compute_120,code=sm_120"
cmake --build build -j8
```

Render presets live in `presets/`. Launch with
`make run-visual PARAMS=presets/screen-1794842753.txt`, or use `F9`
inside the app to switch presets and `D` to save the current settings.

Bonsai demo keys
----------------

* [esc]	quit
* [h] / [~]	sliders
* [space]	toggle simulation
* [r]	toggle rendering
* [p]	move through particle render modes (volumetric, points, additive point sprites)
* [b]	toggle octree boxes
* [l]	toggle display light scatter buffer
* [c]	fit camera
* [[] decrement minimum octree display level
* []] increment minimum octree display level
* [-] decrement maximum octree display level
* [=] increment maximum octree display level
* [g]	toggle glow / post processing
* [f]	toggle fly mode (use wasd to steer, right mouse to go faster)
* [n]	detonate supernova
* [1] toggle direct (N^2) gravitation
* [F9]	preset browser (load / save / new)
* [D]	save current settings into the selected preset
* [t] / [T]	halve / double simulation dt
* [j]	cycle slider pages (render / color + dt)
* [?] / [F12]	toggle help
* [/]	hide / show the whole interface


Bonsai Program arguments
------------------------
Standard:

* -h    Display the help and shows default argument values
* -i    Input snapshot filename

* --dev     Device ID to run simulation on
* -t    Simulation time-step
* -T    Simulation end-time
* -e    Softening-value (will be squared)
* -o    Opening angle (theta)
* -r    Rebuild tree every # steps

* --snapname Snapshot base name (N-body time is appended in 000000 format) 
* --snapiter Snapshot iteration (N-body time)
* --valueadd Value to add to the snapshot name
* --log         Enable printfs
* --logfile Filename to store kernel timing information 
* --rmdist   Particle removal distance (uncommented in the code)

Demo specific:

* --reducebodies Cut down bodies dataset by # factor
* --reducedust   Cut down dust dataset by # factor
* --direct      Enable N^2 direct gravitation 
* --renderdev  Device ID to run the visualization on
* --fullscreen Set fullscreen mode string, format: [ width "x" height ][ ":"                        bitsPerPixel ][ "@" videoRate ]
                
* --displayfps Enable on-screen FPS display
* --Tglow      Enable glowing particles @ # Myr
* --dTglow     Reach full brightness in @ # Myr
* --params     Render parameter preset file [params.txt]
* --colorparams Color/camera parameter file [colorparams.txt]
* --presetdir  Directory scanned by the in-app preset browser



Compile tips and tricks
----------------------
Using CMake under Linux:

For Demo purposes:
cmake -DUSE_CUB=0 -DUSE_DUST=1 -DUSE_OPENGL=1

For production simulations 
cmake -DUSE_CUB=0 -DUSE_DUST=0

Using MPI under linux:
cmake -DCMAKE_CXX_COMPILER=mpicxx

Compilation for Fermi architecture:
cmake -DCOMPILE_SM30=0

Compilation for Tesla architecture:
Sorry not supported anymore, time to upgrade your hardware!

Compilation with device debugging:
cmake -DCUDA_DEVICE_DEBUGGING=1

Build debug configuration:
cmake -DCMAKE_BUILD_TYPE=Debug

(Or use ccmake CMakeCache.txt, to alter the properties)




