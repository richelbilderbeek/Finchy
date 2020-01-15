/* Finchy
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef INPUTMASTER_H
#define INPUTMASTER_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

enum class JoystickButton {SELECT, LEFTSTICK, RIGHTSTICK, START, DPAD_UP, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT, L2, R2, L1, R1, TRIANGLE, CIRCLE, CROSS, SQUARE};

class InputMaster : public Object
{
    URHO3D_OBJECT(InputMaster, Object);
public:
    InputMaster(Context* context, MasterControl* masterControl);
    WeakPtr<Node> firstHit_;
private:
    MasterControl* masterControl_;
    Input* input_;

    HashSet<int> pressedKeys_;
    HashSet<int> pressedMouseButtons_;
    HashSet<int> pressedJoystickButtons_;

    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleMouseButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData);
};

#endif // INPUTMASTER_H
