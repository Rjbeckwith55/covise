#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/EventVisitor>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

#include "coVRGizmo.h"
//FIXME: headers

    
GizmoDrawable::GizmoDrawable() : _gizmo(0), _mode(NO_GIZMO)
{
    osg::Matrix matrix;
    for ( int i=0; i<16; ++i )
        _editMatrix[i] = *(matrix.ptr() + i);
    _screenSize[0] = 200;
    _screenSize[1] = 100;
    
    setEventCallback( new GizmoEventCallback );
    setSupportsDisplayList( false );
    getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
}
    
GizmoDrawable::GizmoDrawable( const GizmoDrawable& copy, osg::CopyOp op)
:   osg::Drawable(copy, op),
    _transform(copy._transform), _gizmo(copy._gizmo), _mode(copy._mode)
{
}
    
void GizmoDrawable::setGizmoMode( Mode m, IGizmo::LOCATION loc)
{
    _mode = m;
    if ( _gizmo ) delete _gizmo;
    switch ( m )
    {
    case MOVE_GIZMO: _gizmo = CreateMoveGizmo(); break;
    case ROTATE_GIZMO: _gizmo = CreateRotateGizmo(); break;
    case SCALE_GIZMO: _gizmo = CreateScaleGizmo(); break;
    default: _gizmo = NULL; return;
    }
    
    if ( _gizmo )
    {
        _gizmo->SetEditMatrix( _editMatrix );
        _gizmo->SetScreenDimension( _screenSize[0], _screenSize[1] );
        _gizmo->SetLocation( loc );
        //_gizmo->SetDisplayScale( 0.5f );
    }
    std::cout<<"setGizmoMode"<<std::endl;

}
       
void GizmoDrawable::setTransform( osg::MatrixTransform* node )
{
    _transform = node;
    if ( !node ) return;
    
    const osg::Matrix& matrix = node->getMatrix();
    for ( int i=0; i<16; ++i )
        _editMatrix[i] = *(matrix.ptr() + i);
}
      
void GizmoDrawable::setScreenSize( int w, int h )
{
    _screenSize[0] = w;
    _screenSize[1] = h;
    if ( _gizmo )
        _gizmo->SetScreenDimension( w, h );
}
    
void GizmoDrawable:: applyTransform()
{
    if ( _gizmo && _transform.valid() )
        _transform->setMatrix( osg::Matrix(_editMatrix) );
}
    
void GizmoDrawable:: drawImplementation( osg::RenderInfo& renderInfo ) const
{
    osg::State* state = renderInfo.getState();
    state->disableAllVertexArrays();
    state->disableTexCoordPointer( 0 );
    
    glPushMatrix();
    glPushAttrib( GL_ALL_ATTRIB_BITS );
	if ( _gizmo )
	{
	    _gizmo->SetCameraMatrix( osg::Matrixf(state->getModelViewMatrix()).ptr(),
	                             osg::Matrixf(state->getProjectionMatrix()).ptr() );
	    _gizmo->Draw();
    }
    glPopAttrib();
    glPopMatrix();
    std::cout<<"drawImplementation"<<std::endl;
}
    
// int main( int argc, char** argv )
// {
//     osg::ref_ptr<osg::MatrixTransform> scene = new osg::MatrixTransform;
//     scene->addChild( osgDB::readNodeFile("/home/AD.EKUPD.COM/matthias.epple/trailer.3ds") );
    
//     osg::ref_ptr<GizmoDrawable> gizmo = new GizmoDrawable;
//     gizmo->setTransform( scene.get() );
//     gizmo->setGizmoMode( GizmoDrawable::MOVE_GIZMO );
    
//     osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//     geode->addDrawable( gizmo.get() );
//     geode->setCullingActive( false );  // allow gizmo to always display
//     geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );  // always show at last
    
//     osg::ref_ptr<osg::MatrixTransform> root = new osg::MatrixTransform;
//     root->addChild( scene.get() );
//     root->addChild( geode.get() );
    
//     osgViewer::Viewer viewer;
//     viewer.setSceneData( root.get() );
//     viewer.setCameraManipulator( new MyTrackballManipulator );
//     viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );
//     viewer.addEventHandler( new osgViewer::StatsHandler );
//     viewer.addEventHandler( new osgViewer::WindowSizeHandler );
// 	viewer.realize();
    
//     osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>( viewer.getCamera()->getGraphicsContext() );
//     if ( gw )
//     {
//         // Send window size for libGizmo to initialize
//         int x, y, w, h; gw->getWindowRectangle( x, y, w, h );
//         viewer.getEventQueue()->windowResize( x, y, w, h );
//     }
// 	return viewer.run();
// }
