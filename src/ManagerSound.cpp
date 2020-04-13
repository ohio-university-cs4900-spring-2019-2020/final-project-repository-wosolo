#include <iostream>
#include "ManagerSound.h"

using namespace Aftr;

// This line is required to use IrrKlang as a singleton
irrklang::ISoundEngine* ManagerSound::engine;
std::map<std::string, irrklang::ISound*> ManagerSound::sounds;

irrklang::vec3df ManagerSound::convert(const Vector& vec){
	// y is negated as part of the conversion from Aftr coordinates to irrKlang coordinates
	return irrklang::vec3df(vec.x, -1 * vec.y, vec.z);
}

void ManagerSound::init() {
	engine = irrklang::createIrrKlangDevice(); // Create the device
	// If it didn't start, display an error message
	if (!engine) {
		std::cerr << "Unable to create IrrKlang engine" << std::endl;
	}
}

void ManagerSound::shutdown() {
	// Drop the engine, opposite of init
	for (std::map<std::string, irrklang::ISound*>::iterator it = sounds.begin(); it != sounds.end(); ++it){
		it->second->drop();
	}
	engine->drop();
}

void ManagerSound::addSound2D(const std::string& name, const std::string& file){
	sounds.insert(std::pair<std::string, irrklang::ISound*>(name, engine->play2D(file.c_str(), false, true)));
}

void ManagerSound::addSound3D(const std::string& name, const std::string& file, const Vector& pos){
	sounds.insert(std::pair<std::string, irrklang::ISound*>(name, engine->play3D(file.c_str(), convert(pos), false, true)));
}

irrklang::ISound*& ManagerSound::getSound(const std::string& name){
	return sounds.at(name);
}

irrklang::ISound* ManagerSound::play2D(const std::string& file, const bool& loop,
									   const bool& startPaused, const bool& track){
	// Wrap the engine's play2D function and convert to a c-string for it 
	return engine->play2D(file.c_str(), loop, startPaused, track);
} 

irrklang::ISound* ManagerSound::play3D(const std::string& file, const Vector& pos, const bool& loop,
									   const bool& startPaused, const bool& track){
	// Wrap the engine's play3D function and convert to a c-string and irrKlang vector for it 
	return engine->play3D(file.c_str(), convert(pos), loop, startPaused, track);
} 

void ManagerSound::setListenerPosition(const Vector& pos, const Vector& lookdir,
									   const Vector& vel, const Vector& updir)
{
	// Wrap the engine's setListenerPosition function and conver to irrKlang vectors for it 
	engine->setListenerPosition(convert(pos), convert(lookdir), convert(vel), convert(updir));
}