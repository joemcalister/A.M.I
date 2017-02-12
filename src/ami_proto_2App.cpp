//
//  ami_proto_2App.cpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//


/*
 
 IDEA - as audio sounds choppy if generated seperately,
 generate them seperately, get time for them, generate script,
 then generate an entire audio file which is simply played, this would be so much more reliable!
 
 fft based visulisation across brains linked to her voice, something like this
 https://christianfloisand.wordpress.com/tag/cinder/
 
 */


#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/audio/Voice.h"

#include "Warp.h"
#include "portaudio.h"
#include "queue.hpp"


using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;

class ami_proto_2App : public App {
public:
	static void prepare(Settings *settings);

	void setup() override;
	void cleanup() override;
	void update() override;
	void draw() override;
	void resize() override;

	void mouseMove(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;

	void updateWindowTitle();
private:
	fs::path mSettings;
	gl::TextureRef stensil, rawbacking;
	WarpList mWarps;
	Area mSrcArea;
    audio::VoiceRef mainSound;
    queue q;
};

void ami_proto_2App::prepare(Settings *settings)
{
	settings->setWindowSize(1440, 900);
}

void ami_proto_2App::setup()
{
	updateWindowTitle();
	disableFrameRate();

	// initialize warps
	mSettings = getAssetPath("") / "warps.xml";
	if(fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarps.push_back(WarpBilinear::create());
		mWarps.push_back(WarpPerspective::create());
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}

	// load test image
	try {
		stensil = gl::Texture::create(loadImage(loadAsset("brain-stensil.png")),
									  gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        rawbacking = gl::Texture::create(loadImage(loadAsset("uvs-calibration.jpeg")),
                                      gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        

		mSrcArea = stensil->getBounds();

		// adjust the content size of the warps
		Warp::setSize(mWarps, stensil->getSize());
	}
	catch(const std::exception &e) {
		console() << e.what() << std::endl;
	}
}

void ami_proto_2App::cleanup()
{
	// save warp settings
	Warp::writeSettings(mWarps, writeFile(mSettings));
}

void ami_proto_2App::update()
{
	// there is nothing to update
    
    if (!q.isActive && !q.loading && !q.ready)
    {
        // there's no user running, load the next one -- if there is one
        cout << "Starting to load the next user" << endl;
        q.loadInNextUser();
        
    }else if (!q.isActive && q.ready)
    {
        // a user has been pre-loaded in -- start the show
        cout << "A user script is preloaded for playing.." << endl;
        
        // setup defaults
        q.currentScript.start_time = ci::app::getElapsedSeconds();
        q.currentScript.begun = true;
        q.currentScript.current_line = q.currentScript.lines[0];
        
        // check the current sound scenario
        if (q.currentScript.current_line.sound_src.length() != 0)
        {
            audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.sound_src));
            mainSound = audio::Voice::create(sourceFile);
            mainSound->start();
        }
        
        // note the start
        cout << "We are starting the show!" << endl;
        cout << "First Line: " << q.currentScript.current_line.raw_text << endl;
        
        // start the main play
        q.isActive = true;
        
    }else if (q.isActive)
    {
        // itterate time
        q.currentScript.current_time = ci::app::getElapsedSeconds() - q.currentScript.start_time;
        
        // determine if should proceed to next timeline chunk
        if (q.currentScript.current_line.end_time < q.currentScript.current_time)
        {
            // we need to move on
            if (q.currentScript.current_index >= (q.currentScript.lines.size()-1))
            {
                // we've hit the end
                cout << "\nThe end of the show!" << endl;
                q.endShow();
                
            }else {
                cout << endl;
                q.currentScript.current_index++;
                q.currentScript.current_line = q.currentScript.lines[q.currentScript.current_index];
                
                // load in the potential image
                if (q.currentScript.current_line.sound_src.length() != 0)
                {
                    audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.sound_src));
                    mainSound = audio::Voice::create(sourceFile);
                    mainSound->start();
                }
                
                cout << "New Line: " << q.currentScript.current_line.raw_text << endl;
            }
        }
    }
    
    
    /*
    if (main_script.ready)
    {
        // elapse the time
        if (main_script.begun == false)
        {
            main_script.start_time = ci::app::getElapsedSeconds();
            main_script.begun = true;
            main_script.current_line = &main_script.lines[0];
            
            // note the first line
            cout << "We are starting the show!" << endl;
            cout << "First Line: " << main_script.current_line->raw_text << endl;
            
            // check for the image
            if (main_script.current_line->image == nil && main_script.current_line->image_src.length() != 0)
            {
                // we need to load in an image
                cout << "Loading in an Image!" << endl;
                main_script.current_line->image = gl::Texture::create(loadImage(loadFile(main_script.current_line->image_src)), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
            }
            
            if (main_script.current_line->sound_src.length() != 0)
            {
                audio::SourceFileRef sourceFile = audio::load(loadFile(main_script.current_line->sound_src));
                mainSound = audio::Voice::create(sourceFile);
                mainSound->start();
                
                
            }
            
        }else {
            main_script.current_time = ci::app::getElapsedSeconds() - main_script.start_time;
            //cout << "current time =" << main_script.current_time << endl;
        }
        
        
        // determine if should proceed to next timeline chunk
        if (main_script.current_line->end_time < main_script.current_time)
        {
            // we need to move on
            if (main_script.current_index >= (main_script.lines.size()-1))
            {
                // we've hit the end
                cout << "\nThe end of the show!" << endl;
                main_script.ready = false;
            }else {
                cout << endl;
                main_script.current_index++;
                main_script.current_line = &main_script.lines[main_script.current_index];
                
                // load in the potential image
                if (main_script.current_line->image == nil && main_script.current_line->image_src.length() != 0)
                {
                    // we need to load in an image
                    cout << "Loading in an Image" << endl;
                    main_script.current_line->image = gl::Texture::create(loadImage(loadFile(main_script.current_line->image_src)), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
                }
                
                if (main_script.current_line->sound_src.length() != 0)
                {
                    audio::SourceFileRef sourceFile = audio::load(loadFile(main_script.current_line->sound_src));
                    mainSound = audio::Voice::create(sourceFile);
                    mainSound->start();
                }
                
                cout << "New Line: " << main_script.current_line->raw_text << endl;
            }
        }
    }
     */
}

void ami_proto_2App::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());

	if(stensil) {
		// repeat for all brains
		for(auto &warp:mWarps) {
			
            // begin warp
            warp->begin();

            // display raw backing
            gl::draw(rawbacking, rawbacking->getBounds(), warp->getBounds());
            
            // if this current line has an image do it :D -- easy!
            if (q.currentScript.current_line.image != nil)
            {
                gl::draw(q.currentScript.current_line.image, q.currentScript.current_line.image->getBounds(), warp->getBounds());
            }
            
            // stretch image to fit the area with warp->get bounds
            gl::draw(stensil, mSrcArea, warp->getBounds());

            // end warp
            warp->end();
			
		}
	}
}

void ami_proto_2App::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);
}

void ami_proto_2App::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if(!Warp::handleMouseMove(mWarps, event)) {
		// let your application perform its mouseMove handling here
	}
}

void ami_proto_2App::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if(!Warp::handleMouseDown(mWarps, event)) {
		// let your application perform its mouseDown handling here
	}
}

void ami_proto_2App::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if(!Warp::handleMouseDrag(mWarps, event)) {
		// let your application perform its mouseDrag handling here
	}
}

void ami_proto_2App::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if(!Warp::handleMouseUp(mWarps, event)) {
		// let your application perform its mouseUp handling here
	}
}

void ami_proto_2App::keyDown(KeyEvent event)
{
	// pass this key event to the warp editor first
	if(!Warp::handleKeyDown(mWarps, event)) {
		// warp editor did not handle the key, so handle it here
		switch(event.getCode()) {
			case KeyEvent::KEY_ESCAPE:
				// quit the application
				quit();
				break;
			case KeyEvent::KEY_f:
				// toggle full screen
				setFullScreen(!isFullScreen());
				break;
			case KeyEvent::KEY_v:
				// toggle vertical sync
				gl::enableVerticalSync(!gl::isVerticalSyncEnabled());
				break;
			case KeyEvent::KEY_w:
				// toggle warp edit mode
				Warp::enableEditMode(!Warp::isEditModeEnabled());
				break;
			case KeyEvent::KEY_a:
				// toggle drawing a random region of the image
				if(mSrcArea.getWidth() != stensil->getWidth() || mSrcArea.getHeight() != stensil->getHeight())
					mSrcArea = stensil->getBounds();
				else {
					int x1 = Rand::randInt(0, stensil->getWidth() - 150);
					int y1 = Rand::randInt(0, stensil->getHeight() - 150);
					int x2 = Rand::randInt(x1 + 150, stensil->getWidth());
					int y2 = Rand::randInt(y1 + 150, stensil->getHeight());
					mSrcArea = Area(x1, y1, x2, y2);
				}
				break;
			case KeyEvent::KEY_SPACE:
				// toggle drawing mode
				updateWindowTitle();
				break;
		}
	}
}

void ami_proto_2App::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if(!Warp::handleKeyUp(mWarps, event)) {
		// let your application perform its keyUp handling here
	}
}

void ami_proto_2App::updateWindowTitle()
{
	getWindow()->setTitle("A.M.I Prototype v2");
}

CINDER_APP(ami_proto_2App, RendererGl(RendererGl::Options().msaa(8)), &ami_proto_2App::prepare)
