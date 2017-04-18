//
//  script_image.hpp
//  ami_proto_2
//
//  Created by Joe on 09/03/2017.
//
//

#ifndef script_image_hpp
#define script_image_hpp

#include <stdio.h>

class script_image
{
    public:
        cinder::gl::TextureRef image;
        float show_delay;
};

#endif /* script_image_hpp */
