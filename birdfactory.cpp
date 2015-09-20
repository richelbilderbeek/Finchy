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

#include "birdfactory.h"

BirdFactory::BirdFactory(Context *context, MasterControl *masterControl) : Object(context),
  masterControl_{masterControl},
  genePairs_{23}
{
    Model* birdModel = masterControl_->cache_->GetResource<Model>("Resources/Models/Finchy.mdl");
    genePairs_ = (int)birdModel->GetNumMorphs() + (int)Gene::Size;
}

void BirdFactory::Evolve(TimeLine* timeLine)
{
    for (unsigned e = 0; e < timeLine->events_.Size(); e++)
    {
        Finchy::Event* event = timeLine->events_[e];
        switch (event->type_){
                case Finchy::EventType::Immigration: {
                    CreateRandomSpecies(event->species_[0]);
                } break;
                case Finchy::EventType::Anagenesis: {
                    Speciate(event->species_[0], event->species_[1]);
                } break;
                case Finchy::EventType::Cladogenesis: {
                    Speciate(event->species_[0], event->species_[1]);
                    Speciate(event->species_[0], event->species_[2]);
                } break;
                default:break;
                }
    }
}

void BirdFactory::CreateRandomSpecies(int id)
{
    Vector<float>* newSpecies = new Vector<float>();
    Color color = RandomColor();
//    Vector3 targetCenter = Vector3(Random(-33.0f, 17.0f), Random(7.0f, 12.0f), Random(-35.0f, 40.0f));
    for (int g = 0; g <= genePairs_; g++){
        switch (g){
        case (int)Gene::Scale: newSpecies->Push(Random(1.5f, 2.0f)); break;
        case (int)Gene::Red:   newSpecies->Push(color.r_); break;
        case (int)Gene::Green: newSpecies->Push(color.g_); break;
        case (int)Gene::Blue:  newSpecies->Push(color.b_); break;
        default:
            float randomGene = 0.5f+Finchy::RandomSign()*0.5f;
            newSpecies->Push(randomGene);
            break;
        }
    }
    species_[id] = newSpecies;
    spots_[id] = CreateSpot();
    alive_[id] = false;
}

Vector3 BirdFactory::SpotToTargetCenter(const IntVector2 spot, bool first)
{
    if (!first){
        float x = (spot.x_ * 6.6f) - 33.0f;
        float y = Random(8.5f, 23.0f);
        float z = (spot.y_ * 7.5f) - 35.0f;
        return Vector3(x, y, z);
    } else {
        float x = (spot.x_ * 2.0f) - 11.0f + spot.y_%2;
        float y = Random(7.0f, 10.0f);
        float z = (spot.y_ * 1.5f) - 50.0f;
        return Vector3(x, y, z);
    }
}

void BirdFactory::Speciate(int originalId, int newId)
{
    Vector<float>* baseSpecies = species_[originalId];
    Vector<float>* newSpecies = new Vector<float>();

    //Shrink
    float size = Clamp(baseSpecies->At((int)Gene::Scale) + Random(-0.23f, 0.0f),
                       0.5f, 2.0f);
    //Change color
    Color color = Color(
            Clamp(baseSpecies->At((int)Gene::Red) + Finchy::RandomSign()*Random(0.125f, 0.25f), 0.0f, 1.0f),
            Clamp(baseSpecies->At((int)Gene::Green) + Finchy::RandomSign()*Random(0.125f, 0.25f), 0.0f, 1.0f),
            Clamp(baseSpecies->At((int)Gene::Blue) + Finchy::RandomSign()*Random(0.125f, 0.25f), 0.0f, 1.0f));
    //Scramble genes
    for (int g = 0; g <= genePairs_; g++){
        switch (g){
        case (int)Gene::Scale: newSpecies->Push(size);
            break;
        case (int)Gene::Red: newSpecies->Push(color.r_);
            break;
        case (int)Gene::Green: newSpecies->Push(color.g_);
            break;
        case (int)Gene::Blue: newSpecies->Push(color.b_);
            break;
        default:
            float originalGene = baseSpecies->At(g);
            float change = Finchy::RandomSign()*Random(0.5f, 0.75f);
            if (originalGene + change > 1.0f || originalGene + change < 0.0f) change *= Random(-0.1f, -0.23f);
            newSpecies->Push(Clamp(originalGene + change, 0.0f, 1.0f));
            break;
        }
    }
    species_[newId] = newSpecies;
    spots_[newId] = CreateSpot(spots_[originalId]);
    alive_[newId] = false;
}

IntVector2 BirdFactory::CreateSpot(IntVector2 original)
{
    if (original != IntVector2::ZERO){
        int tries = 0;
        IntVector2 jittered(original);
        while (spots_.Values().Contains(jittered)){
            if (tries > 23) return CreateSpot();
            IntVector2 jitter = IntVector2(Random(2, 3) * Finchy::RandomSign(),
                                           Random(2, 3) * Finchy::RandomSign());
            jitter.x_ = Clamp(jitter.x_, 1, 10);
            jitter.y_ = Clamp(jitter.y_, 1, 10);
            jittered = original + jitter;

            tries++;
        }
        return jittered;
    }
    else {
        IntVector2 newSpot(Random(1, 10), Random(1, 10));
        while (spots_.Values().Contains(newSpot)){
            newSpot = IntVector2(Random(1, 10), Random(1, 10));
        }
        return newSpot;
    }
}

//void BirdFactory::Immigration()
//{

//}

void BirdFactory::Anagenesis(int oldId, int newId, bool undo)
{
    for (unsigned b = 0; b < birds_.Size(); b++){
        if (birds_[b]->speciesId_ == oldId)
            birds_[b]->SetSpecies(GetSpecies(newId), SpotToTargetCenter(spots_[newId], birds_[b]->first_), newId);
    }
}

void BirdFactory::Extinction(int id, bool undo)
{
    for (unsigned b = 0; b < birds_.Size(); b++){
        if (birds_[b]->speciesId_ == id)
            birds_[b]->Die(undo);
    }
}

void BirdFactory::CreateBird(const int id, bool first)
{
    Bird* bird = new Bird(context_, masterControl_, first);
    bird->SetSpecies(species_[id], SpotToTargetCenter(spots_[id], first), id);
    bird->Set(Quaternion(45.0f+Random(90.0f)+Random(2)*180.0f+(!first*90.0f), Vector3::UP) * ((first*80.0f+10.0f)*Vector3::FORWARD+first*10.0f*Vector3::UP));
    birds_.Push(SharedPtr<Bird>(bird));
    birdNumbers_[id] += !first;
}

Color BirdFactory::RandomColor()
{
    Color randomColor;
    float hue = Random(6)/6.0f;
    float saturation = Random(3)*0.333f+0.333f;//pow(Random(), 3.0f);
    float value = Random(4)*0.25f+0.25f;//Random();
    randomColor.FromHSV(hue, saturation, value);
    return randomColor;
}
