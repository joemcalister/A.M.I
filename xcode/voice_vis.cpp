//
//  voice_vis.cpp
//  ami_proto_2
//
//  Created by Joe on 16/02/2017.
//
//

#include "voice_vis.hpp"


voiceVisualization::voiceVisualization()
{
    
}

void voiceVisualization::draw(float _w, float _h)
{
    gl::pushModelMatrix();
    gl::color(0,0,0, 0.8);
    gl::translate(0, _h/2, 0);
    gl::begin(GL_TRIANGLE_FAN);
    
    float lastx = 0;
    float lasty = 0;
    for (int i=0; i<fft.getPicos().size(); i++)
    {
        gl::vertex(lastx, lasty);
        lasty = 0 - fft.getPicos()[i]*5;
        lastx = lastx+(_w/fft.getPicos().size());
    }
    gl::vertex(lastx, 0);
    
    lastx = 0;
    lasty = 0;
    for (int i=0; i<fft.getPicos().size(); i++)
    {
        gl::vertex(lastx, lasty);
        lasty = fft.getPicos()[i]*5;
        lastx = lastx+(_w/fft.getPicos().size());
    }
    gl::vertex(lastx, 0);
    
    gl::end();
    gl::popModelMatrix();
    gl::color(Color::white());
}
