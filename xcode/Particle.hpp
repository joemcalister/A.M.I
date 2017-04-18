//
//  Particle.hpp
//  ami_proto_2
//
//  Created by Joe on 17/04/2017.
//
//  Based on the following tutorial https://www.packtpub.com/sites/default/files/9781849518703_Chapter_05.pdf

#ifndef Particle_hpp
#define Particle_hpp

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
class Particle{
public:
    Particle(const cinder::vec2& position, float radius,
             float mass, float drag);
    void update();
    void draw();
    cinder::vec2 position, prevPosition;
    cinder::vec2 forces;
    float radius;
    float mass;
    float drag;
    float alpha;
};

#endif /* Particle_hpp */
