![Image of A.M.I](https://joemcalister.com/img/final/articles/ami/ami-hero-2-lower.jpg)

# A.M.I
This is the frontend of an art piece titled A.M.I. More information on the piece can be found [here](https://joemcalister.com/ami). Predominantly this was used to projection map the brain sculptures alongside playing the script downloaded from the server for each user. Programmed in c++ using cinder.

## Basic structure
* "assets" contains all the textures required for initial setup e.g. silhouettes for brains.
* "blocks" 'cinder blocks', the mapping system and FFT for the voice visualisation.
* "particle.cpp" and "particleSystem.cpp" small particle system for the space theme.
* "json.hpp" JSON for Modern C++.
* "networking.cpp" what I use to get network information on the user via CURL.
* "queue.cpp" the queue system, looks for the next user when required to do so.
* "script.cpp" script class, this contained every user's script, the audio and video files for each line.
* "script_image.cpp" unused, prototype class.
* "voice_vis.cpp" voice visualisation, ran off a FFT linked to the voice audio channel.

