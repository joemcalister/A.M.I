//
//  script.hpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#ifndef script_hpp
#define script_hpp

#include <stdio.h>
#include <stdlib.h>
#include "networking.hpp"
#include "script_object.hpp"
#include "Resources.h"

using namespace std;
using json = nlohmann::json;

class script
{
public:
    script(bool test);
    script();
    script(string url);
    json json;
    vector<script_object> lines;
    bool ready, begun, success;
    float start_time, current_time;
    int current_index;
    script_object current_line;
private:
    networking network;
};

#endif /* script_hpp */
