//
//  Particle.cpp
//  ami_proto_2
//
//  Created by Joe on 17/04/2017.
//
//  Based on the following tutorial https://www.packtpub.com/sites/default/files/9781849518703_Chapter_05.pdf

#include "Particle.hpp"

Particle::Particle( const cinder::vec2& position, float radius, float
                   mass, float drag ){
    this->position = position;
    this->radius = radius;
    this->mass = mass;
    this->drag = drag;
    prevPosition = position;
    forces = cinder::vec2(0,0);
    
    // create alpha
    alpha = (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

void Particle::update(){
    cinder::vec2 temp = position;
    cinder::vec2 vel = ( position - prevPosition ) * drag;
    position += vel + forces / mass;
    prevPosition = temp;
    forces = cinder::vec2(0,0);
    
    // alpha update
    if (alpha > 0.0)
    {
        alpha -= 0.0005;
    }else {
        alpha = 1.0;
    }
}

void Particle::draw(){
    
    //ci::gl::color(255, 255, 255, alpha); THIS CAUSES AN ODD GLITCH
    
    if (alpha > 0.5)
    {
        ci::gl::drawSolidCircle( position, radius );
    }

}
