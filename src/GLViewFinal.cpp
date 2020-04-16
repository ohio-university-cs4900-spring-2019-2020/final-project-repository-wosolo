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

//If we want to use way points, we need to include this.
#include "FinalWayPoints.h"

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

   // Move the enemies
   for (size_t i = 0; i < enemies.size(); i++) {
	   enemies[i].move(Vector(0, 0, 0));
	   if (Vector(0, 0, Maze::getLength() / 2).distanceFrom(enemies[i].wo->getPosition()) <= 1) {
		   enemies[i].spawn(Vector(0, 0, 0));
	   }
   }
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

   if( key.keysym.sym == SDLK_1 )
   {

   }
}


void GLViewFinal::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewFinal::loadMap()
{
	// Initialize the maze
	size_t rows = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("mazeRows"));
	size_t columns = (size_t)stoi(ManagerEnvironmentConfiguration::getVariableValue("mazeColumns"));
	float chance = stof(ManagerEnvironmentConfiguration::getVariableValue("removalChance"));
	Maze::init(rows, columns);
	Maze::generateMaze();
	Maze::deleteWalls(chance);

   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );

   std::string floor( ManagerEnvironmentConfiguration::getLMM() + "/models/Floor.wrl" );
   std::string wall(ManagerEnvironmentConfiguration::getLMM() + "/models/Wall.wrl");
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
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
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
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

   float length = WO::New(floor, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT)->getModel()->getBoundingBox().getlxlylz().x;
   Maze::setLength(length);
   // Create the floor
   for (size_t i = 0; i < Maze::rows; i++) {
	   for (size_t j = 0; j < Maze::columns; j++) {
		   wo = WO::New(floor, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		   wo->setPosition(Vector(length * i, length * j, 0));
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
		   }
	   }
	   // Add border horizontal walls
	   wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	   wo->setPosition(Vector(length* (-0.5f), length* j, length / 2));
	   wo->rotateAboutGlobalY(PI / 2);
	   worldLst->push_back(wo);
	   wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	   wo->setPosition(Vector(length * ((float)(Maze::rows - 1) + 0.5f), length * j, length / 2));
	   wo->rotateAboutGlobalY(PI / 2);
	   worldLst->push_back(wo);
   }
   // Add the vertical walls
   for (size_t i = 0; i < Maze::rows; i++) {
	   for (size_t j = 0; j < Maze::columns - 1; j++) {
		   if (Maze::v_walls[i][j]) {
			   wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
			   wo->setPosition(Vector(length * i, length * ((float)j + 0.5f), length / 2));
			   wo->rotateAboutGlobalX(PI / 2);
			   worldLst->push_back(wo);
		   }
	   }
	   // Add border vertical walls
	   wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	   wo->setPosition(Vector(length * i, length * (-0.5f), length / 2));
	   wo->rotateAboutGlobalX(PI / 2);
	   worldLst->push_back(wo);
	   wo = WO::New(wall, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	   wo->setPosition(Vector(length * i, length * ((float)(Maze::columns - 1) + 0.5f), length / 2));
	   wo->rotateAboutGlobalX(PI / 2);
	   worldLst->push_back(wo);
   }

   MazeEnemy::setChaseDistance(5);
   MazeEnemy::setHeight(length / 2);
   MazeEnemy::setMoveSpeed(0.15f);
   MazeEnemy::setSpawnDistance(2);
   // Add some enemies
   for (size_t i = 0; i < 3; i++) {
	   MazeEnemy enemy;
	   enemy.wo = WO::New(wall, Vector(0.3f, 0.3f, 0.3f), MESH_SHADING_TYPE::mstFLAT);
	   enemy.spawn(Vector(0, 0, 0));
	   worldLst->push_back(enemy.wo);
	   enemies.push_back(enemy);
   }

   createFinalWayPoints();
}


void GLViewFinal::createFinalWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
