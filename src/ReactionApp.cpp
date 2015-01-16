#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "cinder/MayaCamUI.h"



#include "RDSim.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ReactionApp : public AppNative {
  public:
    void prepareSettings( Settings* settings);
	void setup();
    void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
    void resize();
	void update();
	void draw();
    
    
    params::InterfaceGlRef	mParams;
    RDSim mRDSim;
    bool  mUpdateRDSim;
    
    MayaCamUI	mMayaCam;
};

void ReactionApp::prepareSettings(Settings *settings) {
    settings->setWindowSize( 1100, 800);
    settings->setFrameRate( 60.0f );

}

void ReactionApp::setup()
{
    mRDSim = RDSim();
    mUpdateRDSim = true;
    
    CameraPersp cam;
	cam.setEyePoint( Vec3f( 3.f, 3.5f, 3.f ) );
	cam.setCenterOfInterestPoint( Vec3f::zero() );
	cam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	mMayaCam.setCurrentCam( cam );
    
    
	mParams = params::InterfaceGl::create( "Parameters", Vec2i( 175, 100 ) );
	mParams->addParam( "Reaction u", &mRDSim.mParameterU, "min=.000005 max=.0003 step=.000001 keyIncr=u keyDecr=U" );
	mParams->addParam( "Reaction v", &mRDSim.mParameterV, "min=.000005 max=.0003 step=.000001 keyIncr=v keyDecr=V" );
	mParams->addParam( "Reaction k", &mRDSim.mParameterK, "min=0.03 max=0.07 step=0.001 keyIncr=k keyDecr=K" );
	mParams->addParam( "Reaction f", &mRDSim.mParameterF, "min=0.0 max=0.08 step=0.001 keyIncr=f keyDecr=F" );
    mParams->addParam( "Reaction dx", &mRDSim.dx, "min=0.0001; max=1; step=0.0001 keyIncr=d keyDecr=D" );
}

void ReactionApp::keyDown( KeyEvent event ) {
    if ( event.getChar() == 'r' )
        mRDSim.reset();
    if ( event.getChar() == 's' )
        mUpdateRDSim = !mUpdateRDSim;
}

void ReactionApp::mouseDown( MouseEvent event )
{
//    mRDSim.reset();
    mMayaCam.mouseDown( event.getPos() );
}

void ReactionApp::mouseDrag( MouseEvent event )
{
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void ReactionApp::update()
{
    if ( mUpdateRDSim )
        mRDSim.update();
}

void ReactionApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.1, 0.1, 0.1 ) );
    
    gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
    gl::enableDepthRead();
	gl::enableDepthWrite();
    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glEnable( GL_COLOR_MATERIAL );
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    
    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0};
    GLfloat light0_position[] = {0.2f, 0.5f, 0.2f, 1.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    
    GLfloat light1_diffuse[] = {1.0, 1.0, 1.0};
    GLfloat light1_position[] = {0.2f, 2.f, 0.2f, 1.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    
    gl::enable( GL_LIGHTING );
//    gl::drawSphere( Vec3f(0.2,0.5,0.2), 0.1f);
    mRDSim.draw();
    gl::disable( GL_LIGHTING );
//    
//    gl::color(1, 0, 0);
//    gl::drawVector( Vec3f::zero(), Vec3f(1,0,0) );
//    gl::color(0, 1, 0);
//    gl::drawVector( Vec3f::zero(), Vec3f(0,1,0) );
//    gl::color(0, 0, 1);
//    gl::drawVector( Vec3f::zero(), Vec3f(0,0,1) );
    gl::popMatrices();
    
//    mParams->draw();
}

void ReactionApp::resize() {
    CameraPersp cam =  mMayaCam.getCamera();
    cam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
    mMayaCam.setCurrentCam( cam );
}

CINDER_APP_NATIVE( ReactionApp, RendererGl )
