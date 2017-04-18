//
//  voice_vis.hpp
//  ami_proto_2
//
//  Created by Joe on 16/02/2017.
//
//

#ifndef voice_vis_hpp
#define voice_vis_hpp

#include <stdio.h>
#include "FFTOctaveAnalysis.h"
#include "Warp.h"

using namespace cinder;
using namespace ph::warping;

class voiceVisualization
{
public:
    FFTOctaveAnalysis fft;
    voiceVisualization();
    void draw(float _w, float _h);
};

#endif /* voice_vis_hpp */
