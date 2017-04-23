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
#include "script_image.hpp"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTimeGl.h"

using namespace std;
using namespace cinder;

enum preset_states {DYNAMIC, INTRO1};

class script_object
{
public:
    string raw_text, image_src, sound_src, background_sound_src, movie_src;
    bool has_isolate, isolate_all, has_spotlight, is_video;
    int isolate_index, spotlight_index;
    float end_time;
    float local_start_time;
    cinder::gl::TextureRef image;
    vector<script_image> images;
    preset_states state;
};

#endif /* script_object_hpp */
