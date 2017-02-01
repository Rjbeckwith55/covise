/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef INFO_PLUGIN_H
#define INFO_PLUGIN_H
/****************************************************************************\ 
 **                                                            (C)2013 HLRS  **
 **                                                                          **
 ** Description: display text from TEXT attributes                           **
 **                                                                          **
 **                                                                          **
 ** Author: Martin Aumueller <aumueller@hlrs.de>                             **
 **                                                                          **
\****************************************************************************/

#include <cover/coVRPlugin.h>
#include <string>
#include <map>

using namespace opencover;

namespace vrui
{
class coRowMenu;
class coLabelMenuItem;
class coSubMenuItem;
}

class InfoPlugin : public coVRPlugin
{
public:
    InfoPlugin();
    ~InfoPlugin();

    // this will be called if a COVISE object arrives
    void addObject(const RenderObject *container, osg::Group *root, const RenderObject *, const RenderObject *, const RenderObject *, const RenderObject *);

    // this will be called if a COVISE object has to be removed
    void removeObject(const char *objName, bool replace);

private:
    vrui::coSubMenuItem *m_menuItem;
    vrui::coRowMenu *m_menu;
    typedef std::map<std::string, vrui::coLabelMenuItem *> ItemMap;
    ItemMap m_itemMap;
};
#endif
