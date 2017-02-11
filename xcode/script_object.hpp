//
//  script_object.hpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#ifndef script_object_hpp
#define script_object_hpp

#include <stdio.h>

using namespace std;

class script_object
{
public:
    string raw_text, image_src, sound_src;
    float end_time;
    cinder::gl::TextureRef	image;
};

#endif /* script_object_hpp */
