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

#include "timeline.h"

TimeLine::TimeLine()
{
    std::ifstream f("Resources/Data/Example2.txt");

    while (!f.eof())
    {
        std::string id_str;
        std::string time_str;
        std::string type_str;
        std::string species1_str;
        std::string species2_str;
        std::string species3_str;
        f >> id_str;
        if (id_str.empty()) break;
        f >> time_str >> type_str >> species1_str >> species2_str >> species3_str;

        Finchy::Event* d = new Finchy::Event();

        d->id_ = std::stoi(id_str.substr(1,id_str.size() - 3));

        char eventType_char = type_str[1];
        switch (eventType_char){
        case 'I': d->type_ = Finchy::EventType::Immigration;  break;
        case 'A': d->type_ = Finchy::EventType::Anagenesis;   break;
        case 'C': d->type_ = Finchy::EventType::Cladogenesis; break;
        case 'E': d->type_ = Finchy::EventType::Extinction;   break;
        default: break;
        }

        d->time_ = std::stod(time_str.substr(1,time_str.size() - 2));

        int species1 = ReadSpecies(species1_str);
        int species2 = ReadSpecies(species2_str);
        int species3 = ReadSpecies(species3_str);
        if (species1) {
            d->species_.Push(species1);
            if (!speciesIds_.Contains(species1)) speciesIds_.Push(species1);
        }
        if (species2) {
            d->species_.Push(species2);
            if (!speciesIds_.Contains(species2)) speciesIds_.Push(species2);
        }
        if (species3) {
            d->species_.Push(species3);
            if (!speciesIds_.Contains(species3)) speciesIds_.Push(species3);
        }
        events_.Push(d);
    }    
}

int TimeLine::ReadSpecies(std::string species)
{
    if (species.size() > 2)
        return std::stoi(species.substr(1, species.size() - 2));
    else return 0;
}

bool TimeLine::GetEvents(Vector<Finchy::Event *> &events, float from, float til)
{
    for (unsigned e = 0; e < events_.Size(); e++){
        float eventTime = events_[e]->time_;
        if (eventTime < from && eventTime > til){
            events.Push(events_[e]);
        }
    }
    return !events.Empty();
}

std::pair<float, float> TimeLine::GetBeginEnd()
{
    return {ceil(events_[0]->time_), floor(events_.Back()->time_)};
}
