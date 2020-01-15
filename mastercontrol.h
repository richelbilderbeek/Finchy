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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Engine/DebugHud.h>

#include "helper.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class FinchyCam;
class BirdFactory;
class InputMaster;

typedef struct GameWorld
{
    SharedPtr<FinchyCam> camera_;
    SharedPtr<Scene> scene_;
    SharedPtr<Node> voidNode_;
    struct {
        SharedPtr<Node> sceneCursor_;
        SharedPtr<Cursor> uiCursor_;
        PODVector<RayQueryResult> hitResults_;
    } cursor;
} GameWorld;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

namespace {
StringHash const N_VOID = StringHash("Void");
StringHash const N_CURSOR = StringHash("Cursor");
}

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
    friend class InputMaster;
public:
    MasterControl(Context* context);
    GameWorld world_;
    SharedPtr<ResourceCache> cache_;
    SharedPtr<Graphics> graphics_;
    SharedPtr<BirdFactory> birdFactory_;
    Node* skyNode_;

    virtual void Setup();
    virtual void Start();
    virtual void Stop();

    void Exit();
    bool OctreeRaycast(Ray &ray, PODVector<RayQueryResult> &hitResults, float maxDistance = 23.0f);
    Vector3 GetGroundPosition(Vector3 position);
    float GetTime() { return time_; }
    TimeLine* timeLine_;
    void SetSpeed(float speed) { speed_ = Clamp(speed, -10.0f, 10.0f); }
    float GetSpeed() { return speed_; }
    Vector3 TimeToMarkerPosition(float time);
private:
    Vector<AnimatedModel*> bushes_;
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;
    float time_;
    Node* needle_;
    float speed_;

    void CreateConsoleAndDebugHud();
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void CreatePlatform(const Vector3 pos);
    void UpdateCursor(float timeStep);

    bool paused_;
    void AddGrowth(int number);
    void ProcessEvent(Finchy::Event *event, bool undo = false);
};

#endif // MASTERCONTROL_H
