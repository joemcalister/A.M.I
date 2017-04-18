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
    // calm timer prevents constant calling of this function
    if (calmTimer % (60*5) == 0)
    {
        calmTimer = 0;
        cout << "Looking to load the next user..." << endl;
        
        // check if there is currently a user
        loading = true;
        
        // load in the next user -- this should search in future
        currentScript = script(false);
        
        if (currentScript.success)
        {
            // it was okay!
            ready = true;
            loading = false;
        }else {
            // we didnt find anything
            loading = false;
        }
        
        
    }
    calmTimer++;
}

void queue::loadInTestUser()
{
    // calm timer prevents constant calling of this function
    // load in the next user -- this should search in future
    currentScript = script(true);
    
    if (currentScript.success)
    {
        // it was okay!
        ready = true;
        loading = false;
    }else {
        // we didnt find anything
        loading = false;
    }

}
