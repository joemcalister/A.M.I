//
//  queue.cpp
//  ami_proto_2
//
//  Created by Joe on 12/02/2017.
//
//

#include "queue.hpp"

// init
queue::queue()
{
    endShow();
}

void queue::endShow()
{
    ready = false;
    isActive = false;
    loading = false;
}

void queue::loadInNextUser()
{
    // set loading flag
    loading = true;
    
    // load in the next user -- this should search in future
    currentScript = script();
    
    // set loading flag to false
    ready = true;
    loading = false;
}

