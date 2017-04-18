//
//  particleSystem.cpp
//  ami_proto_2
//
//  Created by Joe on 17/04/2017.
//
//  Based on the following tutorial https://www.packtpub.com/sites/default/files/9781849518703_Chapter_05.pdf

#include "particleSystem.hpp"

ParticleSystem::~ParticleSystem(){
    for( std::vector<Particle*>::iterator it = particles.begin(); it
        != particles.end(); ++it ){
        delete *it;
    }
    particles.clear();
}

void ParticleSystem::update(){
    for( std::vector<Particle*>::iterator it = particles.begin(); it
        != particles.end(); ++it ){
        (*it)->update();
    }
}

void ParticleSystem::draw(){
    for( std::vector<Particle*>::iterator it = particles.begin(); it
        != particles.end(); ++it ){
        (*it)->draw();
    }
}

void ParticleSystem::addParticle( Particle *particle ){
    particles.push_back( particle );
}

void ParticleSystem::destroyParticle( Particle *particle ){
    std::vector<Particle*>::iterator it = std::find( particles.
                                                    begin(), particles.end(), particle );
    delete *it;
    particles.erase( it );
}
