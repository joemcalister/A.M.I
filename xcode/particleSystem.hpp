//
//  particleSystem.hpp
//  ami_proto_2
//
//  Created by Joe on 17/04/2017.
//
//  Based on the following tutorial https://www.packtpub.com/sites/default/files/9781849518703_Chapter_05.pdf

#ifndef particleSystem_hpp
#define particleSystem_hpp

#include <vector>
#include "Particle.hpp"

class ParticleSystem {
public:
    ~ParticleSystem();
    void update();
    void draw();
    void addParticle( Particle *particle );
    void destroyParticle( Particle *particle );
    
    std::vector<Particle*> particles;
};

#endif /* particleSystem_hpp */
