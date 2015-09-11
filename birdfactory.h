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

#ifndef BIRDFACTORY_H
#define BIRDFACTORY_H

#include "mastercontrol.h"
#include "bird.h"
#include "timeline.h"

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

enum class Gene{Scale, Red, Green, Blue, Size};

class BirdFactory : public Object
{
    OBJECT(BirdFactory);
public:
    BirdFactory(Context* context, MasterControl* masterControl);

    Color RandomColor();
    Vector<float>* GetSpecies(int id){ return species_[id]; }
    Vector3 SpotToTargetCenter(const IntVector2 spot, bool first);
    IntVector2 GetSpot(int id) { return spots_[id]; }
    int GetGenePairs() const { return genePairs_; }
    Vector3 GetFirstTarget(int id);
private:
    MasterControl* masterControl_;
    int genePairs_;
    Vector<SharedPtr<Bird> > birds_;
    HashMap<int, Vector<float>* > species_;
    HashMap<int, IntVector2> spots_;

    void CreateRandomSpecies(int id);
    void Speciate(int originalId, int newId);
    void CreateBird(int id, bool first = false);
    IntVector2 CreateSpot(IntVector2 original = IntVector2::ZERO);
};

#endif // BIRDFACTORY_H
