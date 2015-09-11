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

#ifndef TIMELINE_H
#define TIMELINE_H

#include <fstream>

#include <Urho3D/Urho3D.h>
#include "helper.h"

using namespace Urho3D;

namespace Finchy{

enum EventType {Immigration, Anagenesis, Cladogenesis, Extinction};

struct Event{
    int id_;
    float time_;
    EventType type_;
    Vector<int> species_;
};
}

class TimeLine
{
public:
    TimeLine();

    int GetNumSpecies() { int num = speciesIds_.Size(); return num; }
    bool GetEvents(PODVector<Finchy::Event*> &events, float from, float til);
private:
    Vector<int> speciesIds_;
    Vector<Finchy::Event*> events_;
    int ReadSpecies(std::string species);
};

#endif // TIMELINE_H
