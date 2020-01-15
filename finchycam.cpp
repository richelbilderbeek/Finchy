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

#include "finchycam.h"

FinchyCam::FinchyCam(Context *context, MasterControl *masterControl):
    Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(FinchyCam, HandleSceneUpdate));

    float viewRange= 123.0f;
    rootNode_ = masterControl_->world_.scene_->CreateChild("Camera");
    camera_ = rootNode_->CreateComponent<Camera>();
    camera_->SetFov(60.0f);
    camera_->SetFarClip(viewRange);
    rootNode_->SetPosition(Vector3(0.0f, 0.0f, -23.0f));
    rootNode_->SetRotation(Quaternion(12.0f, 0.0f, 0.0f));
    rootNode_->Translate(Vector3::BACK * 40.0f);

    Zone* zone = rootNode_->CreateComponent<Zone>();
    zone->SetAmbientColor(Color(0.666f, 0.75f, 0.75f));
    zone->SetFogEnd(viewRange);
    zone->SetFogColor(Color::WHITE);
    zone->SetFogStart(42.0f);

    SetupViewport();
}

void FinchyCam::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, masterControl_->world_.scene_, camera_));
    viewport_ = viewport;

//    //Add anti-asliasing and bloom
//    effectRenderPath_ = viewport_->GetRenderPath()->Clone();
//    effectRenderPath_->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
//    effectRenderPath_->SetEnabled("FXAA3", true);
//    effectRenderPath_->Append(masterControl_->cache_->GetResource<XMLFile>("PostProcess/Bloom.xml"));
//    effectRenderPath_->SetShaderParameter("BloomThreshold", 0.6f);
//    effectRenderPath_->SetShaderParameter("BloomMix", Vector2(0.75f, 0.5f));
//    effectRenderPath_->SetEnabled("Bloom", true);

    Renderer* renderer = GetSubsystem<Renderer>();
//    viewport_->SetRenderPath(effectRenderPath_);
    renderer->SetViewport(0, viewport_);
}

void FinchyCam::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
}
