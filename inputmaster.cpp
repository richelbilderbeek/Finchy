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

#include "inputmaster.h"
#include "finchycam.h"

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context),
    masterControl_{masterControl},
    input_{GetSubsystem<Input>()}
{
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(InputMaster, HandleMouseButtonDown));
    SubscribeToEvent(E_MOUSEBUTTONUP, HANDLER(InputMaster, HandleMouseButtonUp));
    SubscribeToEvent(E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, HANDLER(InputMaster, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, HANDLER(InputMaster, HandleJoystickButtonUp));
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();
    pressedKeys_.Insert(key);

    Graphics* graphics = GetSubsystem<Graphics>();
    switch (key){
        //Exit when ESC is pressed
    case KEY_ESC:{
        masterControl_->Exit();
    } break;
        //Take screenshot
    case KEY_S:{
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    } break;
    case KEY_F:{
        graphics->ToggleFullscreen();
    } break;
    case KEY_KP_PLUS: {
        masterControl_->SetSpeed(masterControl_->GetSpeed()+0.1f);
    } break;
    case KEY_KP_MINUS: {
        masterControl_->SetSpeed(masterControl_->GetSpeed()-0.1f);
    } break;
    case KEY_KP_0: case KEY_0: {
        masterControl_->SetSpeed(0.0f);
    } break;
    case KEY_KP_1: case KEY_1: {
        masterControl_->SetSpeed(0.1f);
    } break;
    case KEY_KP_2: case KEY_2: {
        masterControl_->SetSpeed(0.2f);
    } break;
    case KEY_KP_3: case KEY_3: {
        masterControl_->SetSpeed(0.3f);
    } break;
    case KEY_KP_4: case KEY_4: {
        masterControl_->SetSpeed(0.4f);
    } break;
    case KEY_KP_5: case KEY_5: {
        masterControl_->SetSpeed(0.5f);
    } break;
    case KEY_KP_6: case KEY_6: {
        masterControl_->SetSpeed(0.6f);
    } break;
    case KEY_KP_7: case KEY_7: {
        masterControl_->SetSpeed(0.7f);
    } break;
    case KEY_KP_8: case KEY_8: {
        masterControl_->SetSpeed(0.8f);
    } break;
    case KEY_KP_9: case KEY_9: {
        masterControl_->SetSpeed(0.9f);
    } break;
    case KEY_I: {
        masterControl_->SetSpeed(-masterControl_->GetSpeed());
    } break;
    case KEY_KP_MULTIPLY: {
        masterControl_->SetSpeed(masterControl_->GetSpeed()*2.0f);
    } break;
    case KEY_KP_DIVIDE: {
        masterControl_->SetSpeed(masterControl_->GetSpeed()*0.5f);
    } break;
    default: break;
    }
}
void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyUp;
    int key = eventData[P_KEY].GetInt();
    if (pressedKeys_.Contains(key)) pressedKeys_.Erase(key);

    switch (key){
    default: break;
    }
}

void InputMaster::HandleMouseButtonDown(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonDown;
    int button = eventData[P_BUTTON].GetInt();
    pressedMouseButtons_.Insert(button);
}


void InputMaster::HandleMouseButtonUp(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonUp;
    int button = eventData[P_BUTTON].GetInt();
    if (pressedMouseButtons_.Contains(button)) pressedMouseButtons_.Erase(button);
}

void InputMaster::HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData)
{
    using namespace JoystickButtonDown;
    int joystickId = eventData[P_JOYSTICKID].GetInt();
    int button = eventData[P_BUTTON].GetInt();
    pressedJoystickButtons_.Insert(button);
}

void InputMaster::HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData)
{
    using namespace JoystickButtonUp;
    int joystickId = eventData[P_JOYSTICKID].GetInt();
    int button = eventData[P_BUTTON].GetInt();
    if (pressedJoystickButtons_.Contains(button)) pressedJoystickButtons_.Erase(button);
}
