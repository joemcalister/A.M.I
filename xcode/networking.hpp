//
//  networking.hpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#ifndef networking_hpp
#define networking_hpp

#include <stdio.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class networking
{
public:
    json test();
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
  
};

#endif /* networking_hpp */
