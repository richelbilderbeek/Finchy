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

#include "mastercontrol.h"
#include "birdfactory.h"
#include "finchycam.h"
#include "inputmaster.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_{false},
    timeLine_{new TimeLine()},
    time_{timeLine_->GetBeginEnd().first},
    speed_{0.023f}
{

}


void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "Finchy";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"TestVoxelWidget.log";
//    engineParameters_["FullScreen"] = false;
    engineParameters_["Headless"] = false;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{
    new InputMaster(context_, this);
    cache_ = GetSubsystem<ResourceCache>();
    graphics_ = GetSubsystem<Graphics>();
    renderer_ = GetSubsystem<Renderer>();

    defaultStyle_ = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
}

void MasterControl::CreateConsoleAndDebugHud()
{
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    cache_ = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    world_.cursor.uiCursor_ = new Cursor(context_);
    world_.cursor.uiCursor_->SetVisible(false);
    ui->SetCursor(world_.cursor.uiCursor_);
    world_.cursor.uiCursor_->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);

    //Create graphical timeline
    Node* timelineNode = world_.scene_->CreateChild("TimeLine");
    timelineNode->SetPosition(Finchy::Scale(TimeToMarkerPosition(0.0f), Vector3(0.0f, 1.0f, 1.0f)));
    timelineNode->SetScale(Vector3(42.0f, 1.0f, 1.0f));
    StaticModel* timelineModel = timelineNode->CreateComponent<StaticModel>();
    timelineModel->SetModel(cache_->GetResource<Model>("Resources/Models/Line.mdl"));
    timelineModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Line.xml"));
    for (unsigned e = 0; e < timeLine_->events_.Size(); e++)
    {
        Node* markerNode = world_.scene_->CreateChild("Marker");
        markerNode->SetPosition(Vector3::RIGHT*0.1f+TimeToMarkerPosition(timeLine_->events_[e]->time_));
        markerNode->SetScale(Vector3(1.0f, 1.0f, 0.5f));
        StaticModel* markerModel = markerNode->CreateComponent<StaticModel>();
        switch (timeLine_->events_[e]->type_){
        case Finchy::EventType::Immigration: {
            markerModel->SetModel(cache_->GetResource<Model>("Resources/Models/ImmigrationMarker.mdl"));
            markerModel->SetMaterial(cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(0)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[0]));
            markerNode->Translate(Vector3::FORWARD*0.4f);
        } break;
        case Finchy::EventType::Anagenesis: {
            markerModel->SetModel(cache_->GetResource<Model>("Resources/Models/AnagenesisMarker.mdl"));
            markerModel->SetMaterial(0,cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(0)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[1]));
            markerModel->SetMaterial(1,cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(1)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[0]));
            markerNode->Translate(Vector3::FORWARD*0.3f);
        } break;
        case Finchy::EventType::Cladogenesis: {
            markerModel->SetModel(cache_->GetResource<Model>("Resources/Models/CladogenesisMarker.mdl"));
            markerModel->SetMaterial(0,cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(0)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[2]));
            markerModel->SetMaterial(1,cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(1)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[0]));
            markerModel->SetMaterial(2,cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(2)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[1]));
            markerNode->Translate(Vector3::FORWARD*0.2f);
        } break;
        case Finchy::EventType::Extinction: {
            markerModel->SetModel(cache_->GetResource<Model>("Resources/Models/ExtinctionMarker.mdl"));
            markerModel->SetMaterial(cache_->GetTempResource<Material>("Resources/Materials/Marker.xml"));
            markerModel->GetMaterial(0)->SetShaderParameter("MatDiffColor", birdFactory_->GetSpeciesColor(timeLine_->events_[e]->species_[0]));
            markerNode->Translate(Vector3::FORWARD*0.1f);
        } break;
        default:break;
        }
    }
}

Vector3 MasterControl::TimeToMarkerPosition(float time)
{
    return Vector3(9.0f-4.5f*time, 0.0f, -53.0f);
}

void MasterControl::CreateScene()
{
    world_.scene_ = new Scene(context_);

    world_.scene_->CreateComponent<Octree>();
    world_.scene_->CreateComponent<DebugRenderer>();

    //PhysicsWorld* physicsWorld = world.scene->CreateComponent<PhysicsWorld>();

    skyNode_ = world_.scene_->CreateChild("Sky");
    skyNode_->SetScale(500.0f); // The scale actually does not matter
    Skybox* skybox = skyNode_->CreateComponent<Skybox>();
    skybox->SetModel(cache_->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache_->GetResource<Material>("Materials/Skybox.xml"));

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = world_.scene_->CreateChild("DirectionalLight");
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(1.5f);
    light->SetColor(Color(0.8f, 0.95f, 0.9f));
    light->SetCastShadows(true);
    light->SetShadowIntensity(0.7f);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create island
    Node* islandNode = world_.scene_->CreateChild("Island");
    //islandNode->SetPosition(Vector3::DOWN * 8.0f);
    //islandNode->SetScale(16.0f);
    //islandNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));

    StaticModel* islandModel = islandNode->CreateComponent<StaticModel>();
    islandModel->SetModel(cache_->GetResource<Model>("Resources/Models/Island.mdl"));
    islandModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Island.xml"));

    Node* waterNode = world_.scene_->CreateChild("Water");
    waterNode->SetScale(256.0f);
    StaticModel* waterModel = waterNode->CreateComponent<StaticModel>();
    waterModel->SetModel(cache_->GetResource<Model>("Models/Plane.mdl"));
    waterModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Water.xml"));

    birdFactory_ = new BirdFactory(context_, this);
    birdFactory_->Evolve(timeLine_);
    needle_ = world_.scene_->CreateChild("Needle");
    StaticModel* needleModel_ = needle_->CreateComponent<StaticModel>();
    needleModel_->SetModel(cache_->GetResource<Model>("Resources/Models/TimeMarker.mdl"));
    needleModel_->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Green.xml"));
    //Create camera
    world_.camera_ = new FinchyCam(context_, this);

    AddGrowth(64);
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace SceneUpdate;
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    float deltaTime = -timeStep*speed_;
    Vector<Finchy::Event*> events;
    if (timeLine_->GetEvents(events, time_, time_+deltaTime)){
        for (unsigned e = 0; e < events.Size(); e++){
            ProcessEvent(events[e], deltaTime > 0.0f);
        }
    }
    time_ += deltaTime;
    needle_->SetPosition(TimeToMarkerPosition(time_));

    skyNode_->Rotate(Quaternion(deltaTime*5.0f, Vector3::UP));

    //Wave bushes
    for (unsigned b = 0; b < bushes_.Size(); b++){
        bushes_[b]->SetMorphWeight(0, (0.32f+0.21f*sin( world_.scene_->GetElapsedTime()*0.011f) *(0.5f+0.5f*sin(b + world_.scene_->GetElapsedTime()*(1.42f+0.01f*b)))));
    }

    Vector<int> speciesIds = birdFactory_->alive_.Keys();
    for (unsigned s = 0; s < speciesIds.Size(); s++){
        int id = speciesIds[s];
        if (birdFactory_->alive_[id] == true && birdFactory_->birdNumbers_[id] < 10)
            birdFactory_->CreateBird(id);
    }
}

void MasterControl::Exit()
{
    engine_->Exit();
}

bool MasterControl::OctreeRaycast(Ray& ray, PODVector<RayQueryResult> &hitResults, float maxDistance)
{
    RayOctreeQuery query(hitResults, ray, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    world_.scene_->GetComponent<Octree>()->Raycast(query);
    if (hitResults.Size()) return true;
    else return false;
}

Vector3 MasterControl::GetGroundPosition(Vector3 position)
{
    //Pick a random spot on the island
    Vector3 rayOrigin = Finchy::Scale(position, Vector3::ONE-Vector3::UP)+Vector3::UP*32.0f;

    Ray downwardsRay = Ray(rayOrigin, Vector3::DOWN);
    PODVector<RayQueryResult> hitResults;
    if (OctreeRaycast(downwardsRay, hitResults, 64.0f)){
        for (unsigned r = 0; r < hitResults.Size(); r++){
            RayQueryResult result = hitResults[r];
            if (result.node_->GetNameHash()==StringHash("Island")){
                Vector3 rayHitPos = result.position_;
                if (rayHitPos.y_ < -0.15f)
                    rayHitPos.y_ = -0.15f;
                return rayHitPos;
            }
        }
    }
    return Finchy::Scale(rayOrigin, Vector3(1.0f, 0.0f, 1.0f));
}

void MasterControl::AddGrowth(int number)
{
    for (int b = 0; b < number; b++){
        Vector3 position = GetGroundPosition(Vector3(Random(-30.0f, 30.0f), 5.0f, Random(-15.0f, 20.0f)));
        Node* bushNode = world_.scene_->CreateChild("Bush");
        bushNode->SetPosition(position);
        float width = Random(0.75f, 2.0f);
        bushNode->SetScale(Vector3(width, Random(0.5f, 1.5f), width));
        bushNode->Rotate(Quaternion(Random(360.0f), Vector3::UP),TS_WORLD);
        AnimatedModel* bushModel = bushNode->CreateComponent<AnimatedModel>();
        bushes_.Push(bushModel);
        bushModel->SetModel(cache_->GetResource<Model>("Resources/Models/Bush.mdl"));
        bushModel->SetMaterial(cache_->GetResource<Material>("Resources/Materials/Bush.xml"));
        bushModel->SetCastShadows(true);
    }
}

void MasterControl::ProcessEvent(Finchy::Event* event, bool undo)
{
    if (!undo){
        switch (event->type_){
        case Finchy::EventType::Immigration: {
            birdFactory_->CreateBird(event->species_[0], true);
        } break;
        case Finchy::EventType::Anagenesis: {
            birdFactory_->Anagenesis(event->species_[0], event->species_[1], true);
        } break;
        case Finchy::EventType::Cladogenesis: {
            birdFactory_->Anagenesis(event->species_[0], event->species_[1], true);
            birdFactory_->CreateBird(event->species_[2], true);
        } break;
        case Finchy::EventType::Extinction: {
            birdFactory_->Extinction(event->species_[0]);
        } break;
        default:break;
        }
    } else {
        switch (event->type_){
        case Finchy::EventType::Immigration: {

        } break;
        case Finchy::EventType::Anagenesis: {

        } break;
        case Finchy::EventType::Cladogenesis: {

        } break;
        case Finchy::EventType::Extinction: {

        } break;
        default:break;
        }
    }
}
