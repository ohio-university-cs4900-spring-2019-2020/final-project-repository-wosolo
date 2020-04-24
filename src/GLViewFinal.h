#pragma once

#include "WOGUILabel.h"
#include "GLView.h"

#include "ManagerSound.h"
#include "MazeEnemy.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewFinal
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewFinal : public GLView
{
public:
   static GLViewFinal* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewFinal();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

protected:
   GLViewFinal( const std::vector< std::string >& args );
   virtual void onCreate();   

   void createMaze();
   void firstTimeCreate();
   WOGUILabel* title;
   WOGUILabel* title2;
   WOGUILabel* lose;
   WOGUILabel* win;
   WOGUILabel* again;

   // Game variables
   bool gameOn;
   std::vector<MazeEnemy> enemies;
   std::vector<irrklang::ISound*> sounds;
   Vector camLastPosition;
};

/** \} */

} //namespace Aftr
