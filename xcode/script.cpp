//
//  script.cpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#include "script.hpp"

script::script(bool test)
{
    // we set ready flag to false
    ready = false;
    begun = false;
    current_index = 0;
    
    // init a script
    if (!test)
    {
        json = network.fetch(); // test() to get the test script
    }else {
        json = network.test();
    }
    
    // parse this script
    if (int(json["success"]) == 0)
    {
        cout << "ERROR: error with script json parse: " << json["error_message"] << endl;
        
        // set the success flag -- this is how we determine if we should move on
        success = false;
        
    }else {
        // set success flag
        success = true;
        
        // script is fine        
        for(auto dict:json["script"])
        {
            // create script_object
            script_object s;
            s.raw_text = dict["text"];
            s.end_time = float(dict["duration"]);
            s.image = nil;
            s.image_src = "";
            s.background_sound_src = "";
            s.is_video = false;
            s.movie_src = "";
            
            // look for isolation
            string isolate = dict["isolate"];
            if (isolate == "none")
            {
                // it should eseentially appear blank
                s.isolate_all = true;
            }else if (isolate != "")
            {
                s.has_isolate = true;
                s.isolate_all = false;
                istringstream(isolate) >> s.isolate_index;
            }else {
                s.has_isolate = false;
                s.isolate_all = false;
            }
            
            // download the image locally if no video takeover
            if (dict["is_video"] == "true")
            {
                // video takeover
                cout << "video take over!" << endl;
                s.is_video = true;
                s.movie_src = dict["video_path"];
                
            }else {
                int cIndex = 0;
                for(auto brain:dict["objects"])
                {
                    // check if includes resource
                    if (brain["resource"] != "")
                    {
                        cout << "adding an image " << brain["resource"] << endl;
                        
                        // this is unfortunatly essential, as ocasionally a link may be delivered that wont read properly
                        try {
                            string url = brain["resource"];
                            std::ostringstream oss;
                            oss << BUFFER_FILES_PATH << random() << "-image.jpg";
                            std::string var = oss.str();
                            
                            cinder::BufferRef myData = loadStreamBuffer(cinder::loadUrlStream(url));
                            myData->write(cinder::writeFile(oss.str()));
                            
#warning maybe a memory leak here ^
                            cinder::gl::TextureRef img = cinder::gl::Texture::create(loadImage(cinder::loadFile(oss.str())), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                            
                            
                            // create script image object
                            script_image si;
                            si.image = img;
                            si.show_delay = brain["delay"];
                            
                            // push back the image object
                            s.images.push_back(si);
                        } catch (const std::overflow_error& e) {
                            // this executes if f() throws std::overflow_error (same type rule)
                            cinder::gl::TextureRef img = cinder::gl::Texture::create(loadImage(cinder::loadFile("/Users/joe/Desktop/ami_buffer_files/protected/sad-mac.gif")), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                            
                            // create script image object
                            script_image si;
                            si.image = img;
                            si.show_delay = brain["delay"];
                        } catch (const std::runtime_error& e) {
                            // this executes if f() throws std::underflow_error (base class rule)
                            cinder::gl::TextureRef img = cinder::gl::Texture::create(loadImage(cinder::loadFile("/Users/joe/Desktop/ami_buffer_files/protected/sad-mac.gif")), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                            
                            // create script image object
                            script_image si;
                            si.image = img;
                            si.show_delay = brain["delay"];
                        } catch (const std::exception& e) {
                            // this executes if f() throws std::logic_error (base class rule)
                            cinder::gl::TextureRef img = cinder::gl::Texture::create(loadImage(cinder::loadFile("/Users/joe/Desktop/ami_buffer_files/protected/sad-mac.gif")), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                            
                            // create script image object
                            script_image si;
                            si.image = img;
                            si.show_delay = brain["delay"];
                        } catch (...) {
                            // this executes if f() throws std::string or int or any other unrelated type
                            cinder::gl::TextureRef img = cinder::gl::Texture::create(loadImage(cinder::loadFile("/Users/joe/Desktop/ami_buffer_files/protected/sad-mac.gif")), cinder::gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                            
                            // create script image object
                            script_image si;
                            si.image = img;
                            si.show_delay = brain["delay"];
                        }
                        
                    }else {
                        script_image si;
                        si.image = nil;
                        si.show_delay = 0;
                        
                        // push back the image object
                        s.images.push_back(si);
                    }
                    
                    // check for spotlight
                    if (brain["spotlight"] == "true")
                    {
                        // there is a spotlight
                        s.has_spotlight = true;
                        s.spotlight_index = cIndex;
                    }
                    
                    // itterate cindex
                    cIndex++;
                }
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
            
            // look for presets
            if (dict["preset"] != "dynamic")
            {
                // there is a preset
                if (dict["preset"] == "INTRO1")
                {
                    // AMI Intro
                    cout << "Loading a preset" << endl;
                    s.state = INTRO1;
                    
                    
                }
            }else {
                s.state = DYNAMIC;
            }
            
            // append to lines
            lines.push_back(s);
        }
    }
    
    // declare it ready
    ready = true;

}

script::script()
{
    
}


script::script(string url)
{
    
}
