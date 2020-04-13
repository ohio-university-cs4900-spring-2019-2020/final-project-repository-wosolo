#pragma once

#include <string>
#include <map>
#include "Vector.h"
#include "irrKlang.h"

#pragma comment(lib, "irrKlang.lib")

namespace Aftr
{
	// This manager is meant to be a singleton container for the IrrKlang class
	// This will also act as a sort of converter between engine objects and IrrKlang objects
	class ManagerSound {
		protected: 
			static irrklang::ISoundEngine* engine;
			static std::map<std::string, irrklang::ISound*> sounds; // Store ISounds here for global use
			
		public:
			// Initialize the engine
			static void init();
			// Drops the engine
			static void shutdown();
			// New functions to add sounds for both 2D and 3D use
			// These functions will default to starting paused and unlooped.
			// Adding sounds in this way will not require dropping, this manager will handle it
			static void addSound2D(const std::string& name, const std::string& file);
			static void addSound3D(const std::string& name, const std::string& file, const Vector& pos);
			// New functions to get pointers to the sounds in the map
			// The name must match what was given in the above functions
			static irrklang::ISound*& getSound(const std::string& name);
			// Simple wrappers for the engine's play2D and play3D functions.
			// These functions will also convert std::strings to c-strings
			// Calling these functions with tracking on will require calling drop later
			static irrklang::ISound* play2D(const std::string& file, const bool& loop = false,
											const bool& startPaused = false, const bool& track = false);
			static irrklang::ISound* play3D(const std::string& file, const Vector& pos, const bool& loop = false, 
											const bool& startPaused = false, const bool& track = false);
			// Wrapper to set the listener position
			// The default value for the updir differs from irrKlang since Aftr's up 
			// direction is positive z rather than positive y
			static void setListenerPosition(const Vector& pos, const Vector& lookdir,
											const Vector& vel = Vector(0.0f, 0.0f, 0.0f), const Vector& updir = Vector(0.0f, 0.0f, 1.0f));
											
			// Manage conversion from Aftr vectors to irrKlang vectors
			// This was made into a public function to allow easy conversion when needing vectors from the getSound() function
			static irrklang::vec3df convert(const Vector& vec);
	};
}