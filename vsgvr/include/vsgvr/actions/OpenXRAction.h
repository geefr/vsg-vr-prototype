/*
Copyright(c) 2022 Gareth Francis

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <vsg/core/Inherit.h>

#include <vsgvr/xr/OpenXRCommon.h>

namespace vsgvr {
    class OpenXRInstance;
    class OpenXRActionSet;
    class OpenXRSession;
    
    class VSGVR_DECLSPEC OpenXRAction : public vsg::Inherit<vsg::Object, OpenXRAction>
    {
        public:
            OpenXRAction(vsg::ref_ptr<OpenXRInstance> instance, OpenXRActionSet* actionSet, XrActionType actionType, std::string name, std::string localisedName, std::vector<std::string> subPaths = {} );
            virtual ~OpenXRAction();

            XrAction getAction() const { return _action; }
            XrActionType getActionType() const { return _actionType; }
            std::string getName() const { return _name; }
            std::string getLocalisedName() const { return _localisedName; }
            const std::vector<std::string>& getSubPaths() const { return _subPaths; }
             
            void syncInputState(vsg::ref_ptr<OpenXRInstance> instance, vsg::ref_ptr<OpenXRSession> session, std::string subPath = {});

            // TODO: Writing lots of wrappers on openxr api isn't great,
            // but this is worse. Rework to subclasses for each type,
            // perhaps allow custom actions by the app providing a
            // lambda that makes the appropriate OpenXR state update?
            XrActionStateBoolean getStateBool( std::string subPath = {} ) const;
            XrActionStateFloat getStateFloat( std::string subPath = {} ) const;
            XrActionStateVector2f getStateVec2f( std::string subPath = {} ) const;
            bool getStateValid( std::string subPath = {} ) const;

        private:
            void createAction(vsg::ref_ptr<OpenXRInstance> instance, OpenXRActionSet* actionSet);
            void destroyAction();

        protected:
            XrActionType _actionType;
            std::string _name;
            std::string _localisedName;
            std::vector<std::string> _subPaths;
            XrAction _action = XR_NULL_HANDLE;

            struct ActionState {
                XrActionStateBoolean _stateBool = {};
                XrActionStateFloat _stateFloat = {};
                XrActionStateVector2f _stateVec2f = {};
                bool _stateValid = false;
            };
            // <subPath, state>
            std::map<std::string, ActionState> _state;
    };
}

EVSG_type_name(vsgvr::OpenXRAction);
