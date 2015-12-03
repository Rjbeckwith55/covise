/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */
#ifndef OPENSCENARIO_HEADER
#define OPENSCENARIO_HEADER

#ifndef OSC_START_CONDITION_H
#define OSC_START_CONGDITION_H
#include <oscExport.h>
#include <oscObjectBase.h>
#include <oscObjectVariable.h>
#include <oscVariables.h>
#include <oscNamedObject.h>
#include <oscCondition.h>

namespace OpenScenario {

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscStartCondition: public oscNamedObject
{
public:
    oscStartCondition()
    {
        OSC_ADD_MEMBER(delayTime);
		OSC_OBJECT_ADD_MEMBER(condition,"oscCondition");
    };
    oscDouble delayTime;
	oscConditionMember condition;
};

typedef oscObjectVariable<oscStartCondition *> oscStartConditionMember;

}

#endif //OSC_START_CONDITION_H
#endif
