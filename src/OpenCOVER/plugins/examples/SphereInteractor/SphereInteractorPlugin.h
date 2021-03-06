/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef _SPHEREINTERACTOR_PLUGIN_H
#define _SPHEREINTERACTOR_PLUGIN_H
/****************************************************************************\ 
 **                                                            (C)2010 HLRS  **
 **                                                                          **
 ** Description: This plugin shows how to work with coPickSensor             **
 **              Therefore a red Sphere was created. This sphere can be moved**
 **              by user interaction.                                        **
 **                                                                          **
 ** Author: A.Gottlieb		                                                  **
 **                                                                          **
 ** History:  								                                         **
 ** Jan-10  v1	    				       		                                   **
 **                                                                          **
 **                                                                          **
\****************************************************************************/
#include <cover/coVRPlugin.h>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/LightModel>

#include <OpenVRUI/coTrackerButtonInteraction.h>
#include <cover/coVRPluginSupport.h>
#include <cover/coVRSelectionManager.h>
#include <PluginUtil/coSensor.h>

using namespace std;
using namespace covise;
using namespace opencover;

class mySensor; //sensor class, based on coPickSensor

class SphereInteractorPlugin : public coVRPlugin
{
    friend class mySensor;
public:
    SphereInteractorPlugin();
    ~SphereInteractorPlugin();

    // this will be called in PreFrame
    void preFrame();

private:
    void setStateSet(osg::StateSet *stateSet);

    osg::Sphere *mySphere;
    osg::MatrixTransform *mymtf;
    osg::ref_ptr<osg::TessellationHints> hint;

    mySensor *aSensor;
    coTrackerButtonInteraction *myinteraction;

    bool interActing;
    coSensorList sensorList;
};
class mySensor : public coPickSensor
{
public:
    mySensor(osg::Node *node, std::string name, coTrackerButtonInteraction *_interactionA, osg::ShapeDrawable *cSphDr);
    ~mySensor();

    void activate();
    void disactivate();
    std::string getSensorName();
    bool isSensorActive();

private:
    std::string sensorName;
    bool isActive;
    coTrackerButtonInteraction *_interA;
    osg::ShapeDrawable *shapDr;
};
#endif
