/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */
#ifndef OSC_CONDITION_CHOICE_REF_OBJECT_H
#define OSC_CONDITION_CHOICE_REF_OBJECT_H
#include <oscExport.h>
#include <oscObjectBase.h>
#include <oscObjectVariable.h>
#include <oscVariables.h>
#include <oscConditionChoiceObject.h>

namespace OpenScenario {


class OPENSCENARIOEXPORT referenceType: public oscEnumType
{
public:
    static referenceType *instance(); 
private:
    referenceType();
    static referenceType *inst;
};

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscConditionChoiceRefObject: public oscConditionChoiceObject
{
public:
	
    enum reference
    {
        relative,
		absolute,
    };
    oscConditionChoiceRefObject()
    {
		OSC_ADD_MEMBER(reference);
		reference.enumType = referenceType::instance();
    };
	oscEnum reference;
};

typedef oscObjectVariable<oscConditionChoiceRefObject *> oscConditionChoiceRefObjectMember;

}

#endif //OSC_CONDITION_CHOICE_REF_OBJECT_H