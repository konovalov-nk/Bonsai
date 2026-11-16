#ifndef _RENDERLOOP_H_
#define _RENDERLOOP_H_

#include "octree.h"
extern float TstartGlow;
extern float dTstartGlow;

void initGL(int argc, char** argv, const char *fullScreenMode, bool &stereo);
void initAppRenderer(int argc, char** argv, octree *tree, 
                     octree::IterationData &idata,
                     bool showFPS, bool stereo,
                     const std::string &paramsFile,
                     const std::string &colorParamsFile,
                     const std::string &presetDir,
                     const std::string &icDir,
                     const std::string &icLabel);

#endif // _RENDERLOOP_H_
