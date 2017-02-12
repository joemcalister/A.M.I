//
//  script.cpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#include "script.hpp"

script::script()
{
    // we set ready flag to false
    ready = false;
    begun = false;
    current_index = 0;
    
    // init a test script
    json = network.test();
    
    // parse this script
    if (int(json["success"]) == 0)
    {
        cout << "ERROR: error with script json parse: " << json["error_message"] << endl;
        
    }else {
        // script is fine        
        for(auto dict:json["script"])
        {
            // create script_object
            script_object s;
            s.raw_text = dict["text"];
            s.end_time = float(dict["duration"]);
            s.image = nil;
            s.image_src = dict["resource"];
            
            // download the image locally
            if (dict["resource"] != "")
            {
                // save the image locally
                string url = dict["resource"];
                
                // create the random filepath
                std::ostringstream oss;
                oss << BUFFER_FILES_PATH << random() << "-image.jpg";
                std::string var = oss.str();
                
                cinder::BufferRef myData = loadStreamBuffer(cinder::loadUrlStream(url));
                myData->write(cinder::writeFile(oss.str()));
                
                #warning maybe a memory leak here ^
                s.image_src = oss.str();
                s.image = cinder::gl::Texture::create(loadImage(cinder::loadFile(s.image_src)), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
            }
            
            // fix the sound by downloading it locally
            if (dict["voice"] != "")
            {
                // save it locally
                string url = dict["voice"];
                
                // create the random filepath
                std::ostringstream oss;
                oss << BUFFER_FILES_PATH << random() << "-speech.wav";
                std::string var = oss.str();
                
                cinder::BufferRef myData = loadStreamBuffer(cinder::loadUrlStream(url));
                myData->write(cinder::writeFile(oss.str()));
                
                #warning maybe a memory leak here ^
                s.sound_src = oss.str();
            }else {
                s.sound_src = "";
            }
            
            // append to lines
            lines.push_back(s);
        }
    }
    
    // declare it ready
    ready = true;

}

script::script(string url)
{
    
}
