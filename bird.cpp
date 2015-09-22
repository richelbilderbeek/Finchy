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

#include "bird.h"
#include "birdfactory.h"

int Bird::populationSize_ = 0;

Bird::Bird(Context *context, MasterControl *masterControl, bool first) : Object(context),
    first_{first},
    masterControl_{masterControl},
    birdFactory_{masterControl->birdFactory_},
    velocity_{Vector3::ZERO},
    maxVelocity_{20.0f},
    target_{targetCenter_},
    species_{0},
    previousSpecies_{0},
    speciesId_{0},
    morphTime_{2.0f},
    sinceSpeciesSet_{0.0f},
    sinceStateChange_{0.0f},
    stateDuration_{10.0f},
    touchDown_{false},
    dead_{false}
{
    ++populationSize_;

    rootNode_ = masterControl_->world_.scene_->CreateChild("Bird");
    rootNode_->Rotate(Quaternion(Random(360.0f), Vector3::UP));

    birdModel_ = rootNode_->CreateComponent<AnimatedModel>();
    birdModel_->SetModel(masterControl_->cache_->GetTempResource<Model>("Resources/Models/Finchy.mdl"));
    birdMaterial_ = masterControl_->cache_->GetTempResource<Material>("Resources/Materials/Finchy.xml");
    birdModel_->SetMaterial(birdMaterial_);
    birdModel_->SetCastShadows(true);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();

    state_ = BirdState::Flying;
    target_ = AirTarget();
    animCtrl_->Play("Resources/Models/Fly.ani", 0, true, 1.0f);
    stateDuration_ = first_ ? 5.0f : Random(5.0, 28.0f);

//    if (Random() > 0.23f){
//        animCtrl_->Play("Resources/Models/TailTwitch.ani", 1, true, 0.0f);
//        animCtrl_->SetStartBone("Resources/Models/TailTwitch.ani", "TailIK");
//    }
}

Bird::~Bird()
{
    --populationSize_;
}

void Bird::Set(Vector3 position) {
    rootNode_->SetPosition(position);
    rootNode_->SetEnabledRecursive(true);
    SubscribeToEvent(E_UPDATE, HANDLER(Bird, HandleUpdate));
}
void Bird::Disable() {
    rootNode_->SetEnabledRecursive(false);
    UnsubscribeFromAllEvents();
}

void Bird::SetSpecies(Vector<float>* species, Vector3 targetCenter, int id)
{
    previousSpecies_ = species_ ? species_ : species;
    species_ = species;
    speciesId_ = id;
    sinceSpeciesSet_ = 0.0f;

    targetCenter_ = targetCenter;
    if (state_ == BirdState::Flying) target_ = AirTarget();
}
void Bird::Die(bool undo)
{
    if (!first_) Disable();
    dead_ != undo;
    animCtrl_->StopAll(0.1f);
    animCtrl_->Play("Resources/Models/Die.ani", 1, false, 0.23f);
}

void Bird::Morph()
{
    float scaleMultiplier = first_? 1.0f : 0.666f;
    Color color = Color::BLACK;
    float lerpT = Clamp(sinceSpeciesSet_/morphTime_, 0.0f, 1.0f);
    for (int g = 0; g < ((int)birdModel_->GetNumMorphs() + (int)Gene::Size); g++){
        int morph = g - (int)Gene::Size;
        switch (g){
        case (int)Gene::Scale: rootNode_->SetScale(scaleMultiplier * Lerp(previousSpecies_->At(g), species_->At(g), lerpT));
            break;
        case (int)Gene::Red: color.r_ = Lerp(previousSpecies_->At(g),species_->At(g), lerpT);
            break;
        case (int)Gene::Green: color.g_ = Lerp(previousSpecies_->At(g), species_->At(g), lerpT);
            break;
        case (int)Gene::Blue: color.b_ = Lerp(previousSpecies_->At(g), species_->At(g), lerpT);
            break;
        default : birdModel_->SetMorphWeight(morph, Lerp(previousSpecies_->At(g), species_->At(g), lerpT));
            break;
        }
    }
    birdMaterial_->SetShaderParameter("MatDiffColor", color);
    birdMaterial_->SetShaderParameter("MatSpecColor", 0.5f*(color+Color::GRAY));
}

void Bird::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    if (dead_ && GetPosition().y_ < -5.0f) Disable();

    float timeStep = eventData[SceneUpdate::P_TIMESTEP].GetFloat();
    if (sinceSpeciesSet_ < morphTime_){
        Morph();
    }
    sinceSpeciesSet_ += timeStep;

    if (sinceStateChange_ > stateDuration_){
        switch (state_) {
        case BirdState::Flying: {
            SetState(BirdState::Landing);
        } break;
        case BirdState::Standing: {
            SetState(BirdState::Flying);
        } break;
        default:
            break;
        }
    }
    if(!(first_ && state_ == BirdState::Standing)) sinceStateChange_ += timeStep;

    switch (state_) {
    case BirdState::Flying: {
        Fly(timeStep);
    } break;
    case BirdState::Landing: {
        Land(timeStep);
    } break;
    case BirdState::Standing: {
        Stand(timeStep);
    } break;
    default:
        break;
    }

    //Move bird
    rootNode_->Translate(velocity_*timeStep, TS_WORLD);

    //Update rotation in accordance with the birds movement.
    if (velocity_.Length() > 0.01f){
        Quaternion rotation = rootNode_->GetWorldRotation();
        Quaternion aimRotation = rotation;
        aimRotation.FromLookRotation(velocity_);
        rootNode_->SetRotation(rotation.Slerp(aimRotation, 2.0f * timeStep * velocity_.Length()));
    }
}

void Bird::SetState(BirdState state)
{
    animCtrl_->StopAll(1.0f);
    sinceStateChange_ = 0.0f;
    stateDuration_ = first_? 10.0f : Random(5.0f, 10.0f);
    if (state == BirdState::Landing) {
        target_ = GroundTarget();
        animCtrl_->Play("Resources/Models/Soar.ani", 0, true, 1.0f);
    }
    if (state == BirdState::Standing) {
        if (!dead_) {
            if (first_) birdFactory_->alive_[speciesId_] = true;
            animCtrl_->Play("Resources/Models/LookAround.ani", 0, true, 1.0f);
        }
        else {
            animCtrl_->StopAll(0.1f);
            animCtrl_->Play("Resources/Models/Die.ani", 1, false, 0.23f);
        }
        rootNode_->LookAt(Finchy::Scale(GetPosition()+rootNode_->GetDirection()*23.0f, Vector3(1.0f, 0.0f, 1.0f)));
    }
    if (state == BirdState::Flying) {
        if (!first_) stateDuration_ += Random(5.0f, 10.0f);
        target_ = AirTarget();
        animCtrl_->Play("Resources/Models/Fly.ani", 0, true, 1.0f);
    }
    state_ = state;
}
BirdState Bird::GetState()
{
    return state_;
}

void Bird::Fly(float timeStep)
{
    Vector3 targetDelta = target_ - GetPosition();
    Vector3 beforeVelocity = velocity_;
    bool limit = velocity_.Angle(targetDelta) < 90.0f && velocity_.Length() > maxVelocity_;
    if (!limit) velocity_ += 6.0f * timeStep * targetDelta.Normalized()*Clamp(targetDelta.Length(), 2.0f, 3.0f);
    velocity_ += dead_*timeStep*Vector3::DOWN*23.0f;
    if (targetDelta.Angle(rootNode_->GetDirection()) > 90.0f && seenTarget_){
        target_ = AirTarget();
        seenTarget_ = false;
    }
    else if (targetDelta.Angle(rootNode_->GetDirection()) < 90.0f){
        seenTarget_ = true;
    }
    velocity_ *= 0.99f;
    Vector3 acceleration = velocity_ - beforeVelocity;
    animCtrl_->SetSpeed("Resources/Models/Fly.ani", 3.0f - species_->At((int)Gene::Scale)+0.1f*acceleration.y_);
    if (acceleration.Length() / (0.01f+timeStep) < 7.0f) animCtrl_->Play("Resources/Models/Soar.ani", 1, true, 0.23f);
    else animCtrl_->Stop("Resources/Models/Soar.ani", 0.1f);
}

void Bird::Land(float timeStep)
{
    Vector3 targetDelta = target_ - GetPosition();
    if (targetDelta.Length() < 0.5f){
        if (!touchDown_){
            touchDown_ = true;
            animCtrl_->StopAll(0.1f);
            animCtrl_->Play("Resources/Models/Land.ani", 0, false, 0.1f);
        }
        Quaternion rotation = rootNode_->GetWorldRotation();
        Vector3 eulerRotation = rotation.EulerAngles();
        Quaternion aimRotation = Quaternion(0.0f, eulerRotation.y_, 0.0f);
        if (first_) aimRotation = Quaternion(0.0f, rootNode_->GetDirection().x_ > 0.0f? 90.0f : -90.0f, 0.0f);
        rootNode_->SetRotation(rotation.Slerp(aimRotation, 5.0f*timeStep));

        if (targetDelta.Length() < 0.2f || targetDelta.y_ > 0.0f) {
            SetState(BirdState::Standing);
        }
    }
    velocity_ += 23.0f * timeStep * targetDelta.Normalized() * Clamp( targetDelta.Length()*0.5f, 2.0f, 4.0f ) - Finchy::Scale(velocity_, Vector3(0.023f, 0.23f, 0.023f));
    velocity_ *= Clamp( 0.2f + targetDelta.Length() * 0.666f, 0.2f, 0.95f );
    velocity_.x_ = Clamp(velocity_.x_, -targetDelta.Length(), targetDelta.Length());
    velocity_.y_ = Clamp(velocity_.y_, -targetDelta.Length(), targetDelta.Length());
}

void Bird::Stand(float timeStep)
{
    Vector3 targetDelta = target_ - GetPosition();
    velocity_ = targetDelta*0.5f;
    if (!dead_ && (stateDuration_ - sinceStateChange_) < 0.23f && touchDown_) {
        touchDown_ = false;
        animCtrl_->StopAll(0.1f);
        animCtrl_->Play("Resources/Models/LiftOff.ani", 0, false, 0.1f);
    }

}

Vector3 Bird::AirTarget()
{
    float randomAngle = Finchy::RandomSign() * Random(10.0f, 42.0f);
    Vector3 planarDirection = (rootNode_->GetDirection() * (Vector3::ONE-Vector3::UP)).Normalized();
    Vector3 newTarget = GetPosition() + (Random(5.0f, 10.0f) * (Quaternion(randomAngle, Vector3::UP) * planarDirection));
    float heightGain = Clamp((10.0f - target_.y_)/23.0f, 0.0f, 0.5f);
    newTarget.y_ = Clamp(target_.y_ + Random(-0.5f, 0.5f) + heightGain, 1.0f, 23.0f);
    newTarget = (newTarget + targetCenter_) * 0.5f;
    return newTarget;
}

Vector3 Bird::GroundTarget()
{
    //Pick a spot on the island
    Vector3 rayOrigin = 0.5f*(AirTarget()+targetCenter_);
    if (first_) rayOrigin = targetCenter_;
    return masterControl_->GetGroundPosition(rayOrigin);
}

