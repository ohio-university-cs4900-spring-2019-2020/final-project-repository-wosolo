#include "GLViewFinal.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "WOGUILabel.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"

#include "ManagerSound.h"
#include "Maze.h"

using namespace Aftr;

GLViewFinal* GLViewFinal::New( const std::vector< std::string >& args )
{
   GLViewFinal* glv = new GLViewFinal( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewFinal::GLViewFinal( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewFinal::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewFinal::onCreate() is invoked after this module's LoadMap() is completed.
	ManagerSound::init();
}


void GLViewFinal::onCreate()
{
   //GLViewFinal::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewFinal::~GLViewFinal()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewFinal::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.

   // Simulate game
   if (gameOn) {
	   static_cast<CameraStandardEZNav*>(cam)->setCameraVelocityMultiplier(0.5f); // Force the camera to maintain a certain speed
	   // Reset height
	   Vector camCurPosition = cam->getPosition();
	   // Check if camera moved legally
	   std::pair<float, float> legalPos = Maze::isLegalMove(std::make_pair(camLastPosition.x, camLastPosition.y), std::make_pair(camCurPosition.x, camCurPosition.y));
	   camCurPosition = Vector(legalPos.first, legalPos.second, camCurPosition.z);
	   cam->setPosition(camLastPosition);
	   if (abs(camCurPosition.z - (Maze::getLength() / 2)) > 0.01) {
		   cam->setPosition(Vector(camCurPosition.x, camCurPosition.y, Maze::getLength() / 2));
		   camCurPosition = cam->getPosition();
	   }
	   // Did the player win?
	   std::pair<size_t, size_t> mazePos = Maze::convert(std::make_pair(camCurPosition.x, camCurPosition.y));
	   if (mazePos.first == Maze::rows - 1 && mazePos.second == Maze::columns - 1) {
		   gameOn = false;
		   win->isVisible = true;
		   again->isVisible = true;
		   return;
	   }

	   // Move the enemies
	   for (size_t i = 0; i < enemies.size(); i++) {
		   enemies[i].move(camCurPosition);
		   irrklang::ISound* this_sound = ManagerSound::getSound(std::to_string((int)i));
		   this_sound->setPosition(ManagerSound::convert(enemies[i].wo->getPosition())); // Update Sound Position
		   this_sound->setIsPaused(!enemies[i].chase); // Start playing sound if in chase mode
		   direction d = enemies[i].getDirection();
		   switch (d) {
			   case direction::LEFT:
				   enemies[i].wo->getModel()->setLookDirection(Vector(1, 0, 0));
				   break;
			   case direction::RIGHT:
				   enemies[i].wo->getModel()->setLookDirection(Vector(-1, 0, 0));
				   break;
			   case direction::UP:
				   enemies[i].wo->getModel()->setLookDirection(Vector(0, -1, 0));
				   break;
			   case direction::DOWN:
				   enemies[i].wo->getModel()->setLookDirection(Vector(0, 1, 0));
				   break;
			   default:
				   // Do nothing
				   break;
		   }
		   // Did the player lose?
		   if (camCurPosition.distanceFrom(enemies[i].wo->getPosition()) <= 1.5f) {
			   gameOn = false;
			   lose->isVisible = true;
			   again->isVisible = true;
			   //enemies[i].spawn(camCurPosition);
		   }
	   }
	   camLastPosition = camCurPosition;
   }
   else {
	   static_cast<CameraStandardEZNav*>(cam)->setCameraVelocityMultiplier(0); // Force the camera to maintain a certain speed
   }
   ManagerSound::setListenerPosition(cam->getPosition(), cam->getLookDirection());
}


void GLViewFinal::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewFinal::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewFinal::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewFinal::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewFinal::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_RETURN )
   {
	   // Start/Restart the game
	   if (!gameOn) {
			gameOn = true;
			cam->setPosition(0, 0, Maze::getLength() / 2);
			camLastPosition = Vector(0, 0, Maze::getLength() / 2);
			title->isVisible = false;
			title2->isVisible = false;
			win->isVisible = false;
			lose->isVisible = false;
			again->isVisible = false;
			createMaze();
	   }
   }
}


void GLViewFinal::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewFinal::loadMap()
{

	// Initialize game variables
	gameOn = false;

   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 0, 0, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   // Build the title screen
   std::string comicSans = ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.TTF";
   title = WOGUILabel::New(nullptr);
   title->setText("MAZE ESCAPE");
   title->setColor(0, 255, 0, 255);
   title->setFontSize(30); //font size is correlated with world size
   title->setPosition(Vector(0.5f, 0.5f, 0));
   title->setFontOrientation(FONT_ORIENTATION::foCENTER);
   title->setFontPath(comicSans);
   worldLst->push_back(title);
   title2 = WOGUILabel::New(nullptr);
   title2->setText("Press Enter to start.");
   title2->setColor(0, 255, 0, 255);
   title2->setFontSize(20); //font size is correlated with world size
   title2->setPosition(Vector(0.5f, 0.4f, 0));
   title2->setFontOrientation(FONT_ORIENTATION::foCENTER);
   title2->setFontPath(comicSans);
   worldLst->push_back(title2);
   // Ready up some game end stuff
   lose = WOGUILabel::New(nullptr);
   lose->setText("YOU LOSE");
   lose->setColor(0, 255, 0, 255);
   lose->setFontSize(30); //font size is correlated with world size
   lose->setPosition(Vector(0.5f, 0.5f, 0));
   lose->setFontOrientation(FONT_ORIENTATION::foCENTER);
   lose->setFontPath(comicSans);
   lose->isVisible = false;
   worldLst->push_back(lose);
   win = WOGUILabel::New(nullptr);
   win->setText("YOU WIN!");
   win->setColor(0, 255, 0, 255);
   win->setFontSize(30); //font size is correlated with world size
   win->setPosition(Vector(0.5f, 0.5f, 0));
   win->setFontOrientation(FONT_ORIENTATION::foCENTER);
   win->setFontPath(comicSans);
   win->isVisible = false;
   worldLst->push_back(win);
   again = WOGUILabel::New(nullptr);
   again->setText("Press Enter to play again.");
   again->setColor(0, 255, 0, 255);
   again->setFontSize(20); //font size is correlated with world size
   again->setPosition(Vector(0.5f, 0.4f, 0));
   again->setFontOrientation(FONT_ORIENTATION::foCENTER);
   again->setFontPath(comicSans);
   again->isVisible = false;
   worldLst->push_back(again);

   firstTimeCreate();
}

// Handle creating the maze for the first time
void GLViewFinal::firstTimeCreate() {
	// Credit to MichaelTaylor3D for this model
	std::string virus(ManagerEnvironmentConfiguration::getLMM() + "/models/Virus.obj");
	int numEnemies = stoi(ManagerEnvironmentConfiguration::getVariableValue("numEnemies"));

	// Add some enemies
	for (int i = 0; i < numEnemies; i++) {
		// Add enemies
		MazeEnemy enemy;
		enemy.wo = WO::New(virus, Vector(0.01f, 0.01f, 0.01f), MESH_SHADING_TYPE::mstFLAT);
		worldLst->push_back(enemy.wo);
		enemies.push_back(enemy);
		enemy.wo->setLabel("enemy");
		// Add sound list. Need to be separate so multiple sounds can exist at once
		ManagerSound::addSound3D(std::to_string(i), ManagerEnvironmentConfiguration::getLMM() + "/sounds/pacman_chomp.wav", Vector(0, 0, 0));
		ManagerSound::getSound(std::to_string(i))->setIsLooped(true);
	}
	// I do not own this track. Song is from the game Professor Layton vs Phoenix Wright
	ManagerSound::play2D(ManagerEnvironmentConfiguration::getLMM() + "/sounds/Bewitching_Puzzles.mp3", true);
}

// Create the general parts of the maze. Alone, serves as a reset function.
void GLViewFinal::createMaze() {
	// Remove existing stuff
	for (int i = (int)worldLst->size() - 1; i >= 0; i--) {
		if ((worldLst->at(i)->getLabel() == "floor") || (worldLst->at(i)->getLabel() == "wall")) {
			WO* wo = worldLst->at(i);
			worldLst->eraseViaWOptr(wo);
			delete wo;
		}
	}
	// Initialize the maze
	size_t rows = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("mazeRows"));
	size_t columns = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("mazeColumns"));
	float chance = stof(ManagerEnvironmentConfiguration::getVariableValue("removalChance"));
	Maze::init(rows, columns);
	Maze::generateMaze();
	Maze::deleteWalls(chance);

	std::string floor(ManagerEnvironmentConfiguration::getLMM() + "/models/Floor.wrl");
	std::string start(ManagerEnvironmentConfiguration::getLMM() + "/models/Start.wrl");
	std::string end(ManagerEnvironmentConfiguration::getLMM() + "/models/End.wrl");
	std::string wall(ManagerEnvironmentConfiguration::getLMM() + "/models/Wall.wrl");

	WO* wo;
	float length = WO::New(floor, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT)->getModel()->getBoundingBox().getlxlylz().x;
	Maze::setLength(length);
	// Create the floor
	for (size_t i = 0; i < Maze::rows; i++) {
		for (size_t j = 0; j < Maze::columns; j++) {
			if (i == 0 && j == 0) wo = WO::New(start, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
			else if (i == Maze::rows - 1 && j == Maze::columns - 1) wo = WO::New(end, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
			else wo = WO::New(floor, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
			wo->setPosition(Vector(length * i, length * j, 0));
			wo->setLabel("floor");
			worldLst->push_back(wo);
		}
	}
	// Add the horizontal walls
	for (size_t j = 0; j < Maze::columns; j++) {
		for (size_t i = 0; i < Maze::rows - 1; i++) {
			if (Maze::h_walls[i][j]) {
				wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
				wo->setPosition(Vector(length * ((float)i + 0.5f), length * j, length / 2));
				wo->rotateAboutGlobalY(PI / 2);
				worldLst->push_back(wo);
				wo->setLabel("wall");
			}
		}
		// Add border horizontal walls
		wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		wo->setPosition(Vector(length * (-0.5f), length * j, length / 2));
		wo->rotateAboutGlobalY(PI / 2);
		worldLst->push_back(wo);
		wo->setLabel("wall");
		wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		wo->setPosition(Vector(length * ((float)(Maze::rows - 1) + 0.5f), length * j, length / 2));
		wo->rotateAboutGlobalY(PI / 2);
		worldLst->push_back(wo);
		wo->setLabel("wall");
	}
	// Add the vertical walls
	for (size_t i = 0; i < Maze::rows; i++) {
		for (size_t j = 0; j < Maze::columns - 1; j++) {
			if (Maze::v_walls[i][j]) {
				wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
				wo->setPosition(Vector(length * i, length * ((float)j + 0.5f), length / 2));
				wo->rotateAboutGlobalX(PI / 2);
				worldLst->push_back(wo);
				wo->setLabel("wall");
			}
		}
		// Add border vertical walls
		wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		wo->setPosition(Vector(length * i, length * (-0.5f), length / 2));
		wo->rotateAboutGlobalX(PI / 2);
		worldLst->push_back(wo);
		wo->setLabel("wall");
		wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		wo->setPosition(Vector(length * i, length * ((float)(Maze::columns - 1) + 0.5f), length / 2));
		wo->rotateAboutGlobalX(PI / 2);
		worldLst->push_back(wo);
		wo->setLabel("wall");
	}

	// Enemy variables from config
	size_t chase = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("chaseDistance"));
	float speed = stof(ManagerEnvironmentConfiguration::getVariableValue("enemySpeed"));
	size_t spawn = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("spawnDistance"));

	MazeEnemy::setChaseDistance(chase);
	MazeEnemy::setHeight(length / 2);
	MazeEnemy::setMoveSpeed(speed);
	MazeEnemy::setSpawnDistance(spawn);

	// Add some enemies
	for (size_t i = 0; i < enemies.size(); i++) {
		enemies[i].spawn(Vector(0, 0, 0));
	}
}
