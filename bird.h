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

#ifndef BIRD_H
#define BIRD_H

#include "mastercontrol.h"

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

enum class BirdState { Standing, Hopping, LiftingOff, Flying, Landing };

class Bird : public Object
{
    OBJECT(Bird);
public:
    Bird(Context* context, MasterControl* masterControl, bool first = false);

    bool first_;
    void Set(Vector3 position);
    Vector3 GetPosition() { return rootNode_->GetWorldPosition(); }
    bool IsEnabled() { return rootNode_->IsEnabled(); }
    void SetSpecies(Vector<float> *species, Vector3 targetCenter);
    void Morph();
    BirdState GetState();
private:
    MasterControl* masterControl_;
    BirdFactory* birdFactory_;

    SharedPtr<Node> rootNode_;
    Vector3 velocity_;
    float maxVelocity_;
    Vector3 targetCenter_;
    Vector3 target_;
    BirdState state_;
    bool seenTarget_;
    bool touchDown_;

    AnimatedModel* birdModel_;
    SharedPtr<Material> birdMaterial_;
    AnimationController* animCtrl_;

    float sinceSpeciesSet_;
    float morphTime_;
    float sinceStateChange_;
    float stateDuration_;
    Vector<float>* species_;
    Vector<float>* previousSpecies_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void Disable();
    void Fly(float timeStep);
    void Land(float timeStep);
    Vector3 AirTarget();
    Vector3 GroundTarget();
    void SetState(BirdState state);
    void Stand(float timeStep);
};

#endif // BIRD_H
