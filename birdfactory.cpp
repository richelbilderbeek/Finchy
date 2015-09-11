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

    for (int s = 0; s < 5/*masterControl_->timeLine_.GetNumSpecies()*/; s++){
        CreateRandomSpecies(s);
        for (int b = 0; b < 42; b++){
            CreateBird(s, b == 0);
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
}

Vector3 BirdFactory::SpotToTargetCenter(const IntVector2 spot, bool first)
{
    if (!first){
        float x = (spot.x_ * 6.6f) - 33.0f;
        float y = Random(8.5f, 23.0f);
        float z = (spot.y_ * 7.5f) - 35.0f;
        return Vector3(x, y, z);
    } else {
        float x = (spot.x_ * 2.0f) - 11.0f;
        float y = Random(7.0f, 10.0f);
        float z = (spot.y_ * 1.5f) - 50.0f;
        return Vector3(x, y, z);
    }
}

///Should include spot differentiation
Vector<float>* BirdFactory::Speciate(int id)
{
    Vector<float>* baseSpecies = species_[id];
    Vector<float>* newSpecies = new Vector<float>();

    //Shrink
    float size = Clamp(baseSpecies->At((int)Gene::Scale) + Random(-0.23f, 0.0f),
                       0.5f, 2.0f);
    //Change color
    Color color = Color(
            Clamp(baseSpecies->At((int)Gene::Red) + Random(-0.1f, 0.075f), 0.0f, 1.0f),
            Clamp(baseSpecies->At((int)Gene::Green) + Random(-0.1f, 0.075f), 0.0f, 1.0f),
            Clamp(baseSpecies->At((int)Gene::Blue) + Random(-0.1f, 0.075f), 0.0f, 1.0f));
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
            float change = Random(-0.1f, 0.1f);
            change += change < 0.0f ? -0.23f : 0.23f;
            if (originalGene + change > 1.0f || originalGene + change < 0.0f) change *= Random(-0.1f, -0.23f);
            newSpecies->Push(Clamp(originalGene + change, 0.0f, 1.0f));
            break;
        }
    }
    return newSpecies;
}

IntVector2 BirdFactory::CreateSpot(IntVector2 original)
{
    if (original != IntVector2::ZERO){
        IntVector2 jittered(original);
        while (spots_.Values().Contains(jittered)){
            IntVector2 jitter = IntVector2(Random(2, 3) * Finchy::RandomSign(),
                                           Random(2, 3) * Finchy::RandomSign());
            jitter.x_ = Clamp(jitter.x_, 1, 10);
            jitter.y_ = Clamp(jitter.y_, 1, 10);
            jittered = original + jitter;
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
//void BirdFactory::Anagenesis(int id)
//{

//}
//void BirdFactory::Cladogenesis(int id)
//{

//}
//void BirdFactory::Extinction(int id)
//{

//}

void BirdFactory::CreateBird(const int id, bool first)
{
    Bird* bird = new Bird(context_, masterControl_, first);
    bird->SetSpecies(species_[id], SpotToTargetCenter(spots_[id], first));
    bird->Set(Quaternion(Random(360.0f), Vector3::UP) * (100.0f*Vector3::FORWARD+10.0f*Vector3::UP));
    birds_.Push(SharedPtr<Bird>(bird));
}

Color BirdFactory::RandomColor()
{
    Color randomColor;
    float hue = Random(6)/6.0f;
    float saturation = pow(Random(), 3.0f);
    float value = Random();
    randomColor.FromHSV(hue, saturation, value);
    return randomColor;
}
