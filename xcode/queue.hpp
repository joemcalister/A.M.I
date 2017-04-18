//
//  queue.hpp
//  ami_proto_2
//
//  Created by Joe on 12/02/2017.
//
//

#ifndef queue_hpp
#define queue_hpp

#include <stdio.h>
#include "script.hpp"

class queue
{
public:
    queue();
    void endShow();
    void loadInNextUser();
    void loadInTestUser();
    bool ready;
    bool isActive;
    bool loading;
    script currentScript;
    
private:
    int calmTimer;
    
};

#endif /* queue_hpp */
