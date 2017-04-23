//
//  ami_proto_2App.cpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/audio/Voice.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTimeGl.h"

#include "Warp.h"
#include "queue.hpp"
#include "voice_vis.hpp"
#include "particleSystem.hpp"


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
    int randInRange(int min, int max);
    
    
private:
	fs::path mSettings;
	gl::TextureRef stensil, stensil_large, rawbacking, shadowOverlay, spotlight, calibrate, imageOverlay;
    vector <gl::TextureRef> personal_snippets;
	WarpList mWarps;
	Area mSrcArea;
    audio::VoiceRef mainSound, backgroundSound, backingSound, startingSound, blipSound;
    queue q;
    voiceVisualization voiceVis;
    bool switchedOn, switchedTest;
    int randomIndex, randomTimer, randomIndexImage;
    
    ci::Area getPositionForImage(ci::Area image, ci::Area brain);
    
    // the particle system for stars, just one to reduce cpu load
    ParticleSystem mParticleSystem;
    

    // video
    void loadMovieFile( const fs::path &path );
    
    gl::TextureRef			mFrameTexture;
    qtime::MovieGlRef		mMovie;
    vector <script_object>  movie_script_references;

};

void ami_proto_2App::prepare(Settings *settings)
{
	settings->setWindowSize(1440, 900);
}

// load our movie
void ami_proto_2App::loadMovieFile( const fs::path &moviePath )
{
    try {
        // load up the movie, set it to loop, and begin playing
        mMovie = qtime::MovieGl::create( moviePath );
        mMovie->setLoop();
        mMovie->setVolume(0.0);
        //mMovie->play();
    }
    catch( ci::Exception &exc ) {
        console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
        mMovie.reset();
    }
    
    mFrameTexture.reset();
}

void ami_proto_2App::setup()
{
	updateWindowTitle();
    
    // fill particles for stars
    int numParticle = 100;
    for( int i=0; i<numParticle; i++ ){
        float x = ci::randFloat( 0.0f, 500 );
        float y = ci::randFloat( 0.0f, 400 );
        float radius = ci::randFloat( 1.0f, 2.0f );
        float mass = radius;
        float drag = 0.95f;
        Particle *particle = new Particle
        ( cinder::vec2( x, y ), radius, mass, drag );
        mParticleSystem.addParticle( particle );
    }
    
    // allow uncapped framerate
	disableFrameRate();
    gl::enableVerticalSync(false);
    switchedTest = false;
    switchedOn = false;
    
	// initialize warps
	mSettings = getAssetPath("") / "warps.xml";
	if(fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
        for (int i=0; i<7; i++)
        {
            mWarps.push_back(WarpBilinear::create());
        }
	}

	// load test image
	try {
        
        stensil = gl::Texture::create(loadImage(loadAsset("brain-stensil-small.png")),
                                      gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
		stensil_large = gl::Texture::create(loadImage(loadAsset("brain-stensil-large.png")),
									  gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        rawbacking = gl::Texture::create(loadImage(loadAsset("space-backing.png")),
                                      gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        shadowOverlay = gl::Texture::create(loadImage(loadAsset("texture-overlay.png")),
                                            gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        spotlight = gl::Texture::create(loadImage(loadAsset("spotlight-soft.png")),
                                            gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        calibrate = gl::Texture::create(loadImage(loadAsset("uvs-calibration.jpeg")),
                                            gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
        imageOverlay = gl::Texture::create(loadImage(loadAsset("space-image-overlay.png")),
                                        gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR));
        
		mSrcArea = stensil->getBounds();
        
        // load in the personal snippets
        for (int i=1; i<18; i++)
        {
            std::ostringstream filename;
            filename << "sneak-" << i << ".png";
            personal_snippets.push_back(gl::Texture::create(loadImage(loadAsset(filename.str())),
                                                            gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter( GL_LINEAR_MIPMAP_LINEAR)));
        }

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
    // update particles
    mParticleSystem.update();
    
	// update fft
    if (mainSound != nil && mainSound->isPlaying())
    {
        voiceVis.fft.update();
    }
    
    // update queue / current experience
    if (!q.isActive && !q.loading && !q.ready && switchedOn)
    {
        // there's no user running, load the next one -- if there is one
        if (!switchedTest)
        {
            q.loadInNextUser();
        }else {
            q.loadInTestUser();
        }
        
    }else if (!q.isActive && q.ready)
    {
        // a user has been pre-loaded in -- start the show
        cout << "A user script is preloaded for playing.." << endl;
        
        // setup defaults
        q.currentScript.start_time = ci::app::getElapsedSeconds();
        q.currentScript.begun = true;
        q.currentScript.current_line = q.currentScript.lines[0];
        q.currentScript.current_line.local_start_time = ci::app::getElapsedSeconds();
        
        // load in the video clips
        for (script_object &line:q.currentScript.lines)
        {
            // this is a line
            if (line.is_video)
            {
                // this line has a video create a reference
                movie_script_references.push_back(line);
            }
        }
        
        // if there are any videos let's cue up the first
        if (movie_script_references.size() > 0)
        {
            cout << "pre-loading the first video" << endl;
            loadMovieFile(movie_script_references[0].movie_src);
            movie_script_references.erase(movie_script_references.begin());
        }
        
        // check the current sound scenario
        if (q.currentScript.current_line.sound_src.length() != 0)
        {
            audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.sound_src));
            mainSound = audio::Voice::create(sourceFile);
            mainSound->start();
            
            // patch into fft
            voiceVis.fft.init(mainSound->getInputNode());
        }
        
        // check background sound
        if (q.currentScript.current_line.background_sound_src.length() != 0)
        {
            audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.background_sound_src));
            backgroundSound = audio::Voice::create(sourceFile);
            backgroundSound->start();
        }
        
        // we need to start the backing sound
        audio::SourceFileRef sourceFile = audio::load(loadFile("/Users/joe/Desktop/ami_buffer_files/protected/Intro-beta-4_mixdown.wav"));
        backingSound = audio::Voice::create(sourceFile);
        backingSound->start();
        
        // note the start
        cout << "We are starting the show!" << endl;
        cout << "First Line: " << q.currentScript.current_line.raw_text << endl;
        
        // start the main play
        q.isActive = true;
        
    }else if (q.isActive)
    {
        // itterate time
        q.currentScript.current_time = ci::app::getElapsedSeconds() - q.currentScript.start_time;
        
        // check if video should be updated
        if (q.currentScript.current_line.is_video && mMovie)
        {
            // update movie texture
            cout << "Updating movie texture" << endl;
            
            // also check its actually playing
            if (!mMovie->isPlaying())
            {
                mMovie->play();
            }
            
            // update texture
            mFrameTexture = mMovie->getTexture();
            
        }else {
            // check for overflowing videos
            if (mMovie && mMovie->isPlaying())
            {
                mMovie->stop();
                mMovie = nil;
                mFrameTexture.reset();
                
                // potentially pre-load the next one
                if (movie_script_references.size() > 0)
                {
                    cout << "pre-loading another video" << endl;
                    loadMovieFile(movie_script_references[0].movie_src);
                    movie_script_references.erase(movie_script_references.begin());
                }
            }

        }
        
        // determine if should proceed to next timeline chunk
        if (q.currentScript.current_line.end_time < q.currentScript.current_time)
        {
            // we need to move on
            if (q.currentScript.current_index >= (q.currentScript.lines.size()-1))
            {
                // we've hit the end
                cout << "\nThe end of the show!" << endl;
                
                // stop the backing sound
                backingSound->stop();
                
                // send end show to queue
                q.endShow();
                
                
            }else {
                cout << endl;
                q.currentScript.current_index++;
                q.currentScript.current_line = q.currentScript.lines[q.currentScript.current_index];
                q.currentScript.current_line.local_start_time = ci::app::getElapsedSeconds();
                
                // pre load in the new video
                /*
                if (q.currentScript.current_line.is_video)
                {
                    cout << "Pre loading a video called "<< q.currentScript.current_line.movie_src << endl;
                    loadMovieFile(q.currentScript.current_line.movie_src);
                }
                 */
                
                // load in the potential sound
                if (q.currentScript.current_line.sound_src.length() != 0)
                {
                    audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.sound_src));
                    mainSound = audio::Voice::create(sourceFile);
                    mainSound->start();
                    
                    // patch into fft
                    voiceVis.fft.init(mainSound->getInputNode());
                }
                
                // check for background sound
                if (q.currentScript.current_line.background_sound_src.length() != 0)
                {
                    audio::SourceFileRef sourceFile = audio::load(loadFile(q.currentScript.current_line.background_sound_src));
                    backgroundSound = audio::Voice::create(sourceFile);
                    backgroundSound->start();
                }else {
                    // check if background sound is still playing from before
                    if (backgroundSound != nil && backgroundSound->isPlaying())
                    {
                        backgroundSound->stop();
                    }
                }
                
                cout << "New Line: " << q.currentScript.current_line.raw_text << endl;
            }
        }
    }
}

void ami_proto_2App::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());

	if(stensil) {
		// repeat for all brains
        int index = 0;
		for(auto &warp:mWarps) {
			
            
            // check for isolation
            if ((((q.currentScript.current_line.has_isolate && q.currentScript.current_line.isolate_index == index)||(!q.currentScript.current_line.has_isolate)) && !q.currentScript.current_line.isolate_all)&&(q.isActive))
            {
                // we should draw as it's either isolated for this brain or there is no isolation
                
                // begin warp
                warp->begin();
                
                // if this current line has an image do it :D -- easy!
                if (q.currentScript.current_line.images.size() > index && !q.currentScript.current_line.is_video)
                {
                    if (q.currentScript.current_line.images[index].image != nil && (ci::app::getElapsedSeconds() - q.currentScript.current_line.local_start_time) > q.currentScript.current_line.images[index].show_delay)
                    {
                        // should draw
                        gl::draw(q.currentScript.current_line.images[index].image, q.currentScript.current_line.images[index].image->getBounds(), getPositionForImage(q.currentScript.current_line.images[index].image->getBounds(), warp->getBounds()));
                        
                        // draw the overlay to blend the image in
                        gl::draw(imageOverlay, imageOverlay->getBounds(), warp->getBounds());
                        
                    }else {
                        // should be plain space
                        gl::draw(rawbacking, rawbacking->getBounds(), warp->getBounds());
                    }
                    
                }else if (q.currentScript.current_line.is_video) {
                    // draw our video
                    if( mFrameTexture) {
                        gl::draw( mFrameTexture, getPositionForImage(mFrameTexture->getBounds(), warp->getBounds()) );
                    }
                }else {
                    // shouldnt draw -- no texture for this brain index
                    gl::draw(rawbacking, rawbacking->getBounds(), warp->getBounds());
                }
                
                // draw particles
                mParticleSystem.draw();
                
                // draw shadow overlay
                gl::draw(shadowOverlay, shadowOverlay->getBounds(), warp->getBounds());
                
                // draw visulisation
                if (index == 4)
                {
                    voiceVis.draw(warp->getBounds().getWidth(), warp->getBounds().getHeight());
                    
                    // draw the large brain stensil
                    gl::draw(stensil_large, mSrcArea, warp->getBounds());
                }else {
                    // stretch image to fit the area with warp->get bounds
                    gl::draw(stensil, mSrcArea, warp->getBounds());
                }
                
                
                // check if spotlight is needed
                if (q.currentScript.current_line.has_spotlight && q.currentScript.current_line.spotlight_index == index)
                {
                    //gl::draw(spotlight, spotlight->getBounds(), warp->getBounds());
                }
                
                // end warp
                warp->end();
                
            }else if (!q.isActive) {
                // not active
                
                // the piece is indeed switched on
                if (switchedOn)
                {
                    // potentially load in blip sound
                    if (blipSound == nil)
                    {
                        audio::SourceFileRef sourceFile = audio::load(loadFile("/Users/joe/Desktop/ami_buffer_files/protected/intro_blob.wav"));
                        blipSound = audio::Voice::create(sourceFile);
                    }

                    // determine the index required
                    int past_seconds = ci::app::getElapsedSeconds();
                    if ((past_seconds % 2 == 0) && (randomTimer != past_seconds))
                    {
                        randomTimer = past_seconds;
                        int last = randomIndex;
                        while (last == randomIndex)
                        {
                            randomIndex = rand() % 7;
                        }
                        
                        // random index for the image
                        int last_img = randomIndexImage;
                        while (last_img == randomIndexImage)
                        {
                            
                            randomIndexImage = rand() % personal_snippets.size();
                        }
                        
                        // play the blip sound!
                        blipSound->start();
                    }
                    
                    if (index == randomIndex)
                    {
                        // begin warp
                        warp->begin();

                        // draw stensil
                        gl::draw(rawbacking, rawbacking->getBounds(), warp->getBounds());
                        
                        // should draw
                        gl::draw(personal_snippets[randomIndexImage], personal_snippets[randomIndexImage]->getBounds(), getPositionForImage(personal_snippets[randomIndexImage]->getBounds(), warp->getBounds()));
                        
                        /*
                        if( mFrameTexture ) {
                            Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( warp->getBounds(), true );
                            gl::draw( mFrameTexture, centeredRect );
                        }
                         */
                        
                        // draw the overlay to blend the image in
                        gl::draw(imageOverlay, imageOverlay->getBounds(), warp->getBounds());
                        
                        // draw the shadow overlay over
                        gl::draw(shadowOverlay, shadowOverlay->getBounds(), warp->getBounds());
                        
                        // draw the particles
                        mParticleSystem.draw();
                        
                        // correct stensil
                        if (index == 4)
                        {
                            voiceVis.draw(warp->getBounds().getWidth(), warp->getBounds().getHeight());
                            
                            // draw the large brain stensil
                            gl::draw(stensil_large, mSrcArea, warp->getBounds());
                        }else {
                            
                            // stretch image to fit the area with warp->get bounds
                            gl::draw(stensil, mSrcArea, warp->getBounds());
                        }

                        // end warp
                        warp->end();
                    }
                }else {
                    // not switched on
                    // begin warp
                    
                    if (!q.isActive)
                    {
                        warp->begin();
                        
                        // draw visulisation
                        gl::draw(calibrate, calibrate->getBounds(), warp->getBounds());
                        if (index == 4)
                        {
                            // draw the large brain stensil
                            gl::draw(stensil_large, mSrcArea, warp->getBounds());
                        }else {
                            // stretch image to fit the area with warp->get bounds
                            gl::draw(stensil, mSrcArea, warp->getBounds());
                        }
                        
                        // end warp
                        warp->end();
                    }
                    
                }
            }
  
            // itterate index
            index++;
        }
    }
}

ci::Area ami_proto_2App::getPositionForImage(ci::Area image, ci::Area brain)
{
    // check if portrait or square
    if ((image.getHeight() > image.getWidth())||(image.getHeight() == image.getWidth()))
    {
        float imageHeight = image.getHeight();
        float imageWidth = image.getWidth();
        float scale = brain.getWidth()/imageWidth;
        imageHeight = imageHeight * scale;
        
        // check if height is now larger than brain, if so center
        float y = 0;
        if (imageHeight > brain.getHeight())
        {
            // it's larger
            y = (brain.getHeight()-imageHeight)/2;
        }else {
            // place in the center
            y = (brain.getHeight()-imageHeight)/2;
        }
        
        return ci::Area(0,y,brain.getWidth(), imageHeight);
        
    }else {
        // it's landscape
        float imageHeight = image.getHeight();
        float imageWidth = image.getWidth();
        float scale = brain.getHeight()/imageHeight;
        imageWidth = imageWidth * scale;
        
        // check if width is now larger than brain, if so center
        float x = (brain.getWidth()-imageWidth)/2;
        
        return ci::Area(x,0,imageWidth, brain.getHeight());
    }
 
}

int ami_proto_2App::randInRange(int min, int max)
{
    srand (time(NULL));
    return rand() % max + min;
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
            case KeyEvent::KEY_1:
                // turn discovery on
                switchedOn = !switchedOn;
                break;
            case KeyEvent::KEY_t:
                // turn discovery on but with test script on
                switchedTest = true;
                switchedOn = !switchedOn;
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
