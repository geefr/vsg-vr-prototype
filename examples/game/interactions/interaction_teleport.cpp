
#include "interaction_teleport.h"
#include "game.h"

#include <vsg/all.h>

Interaction_teleport::Interaction_teleport(vsg::ref_ptr<vsgvr::OpenXRInstance> xrInstance,
  vsg::ref_ptr<vsgvr::OpenXRActionPoseBinding> leftHandPose,
  vsg::ref_ptr<vsg::Switch> teleportTarget,
  vsg::ref_ptr<vsg::Group> ground)
  : _leftHandPose(leftHandPose)
  , _teleportTarget(teleportTarget)
  , _ground(ground)
{
  _actionSet = vsgvr::OpenXRActionSet::create(xrInstance, "teleport", "Teleport");

  // Pose bindings - One for each hand
  _action = vsgvr::OpenXRAction::create(xrInstance, _actionSet, XrActionType::XR_ACTION_TYPE_BOOLEAN_INPUT, "teleport", "Teleport");

  _actionSet->actions = {
    _action,
  };

  /*_actionsToSuggest["/interaction_profiles/khr/simple_controller"] = {
    {_action, "/user/hand/left/input/select/click"},
  };*/
  _actionsToSuggest["/interaction_profiles/oculus/touch_controller"] = {
    {_action, "/user/hand/left/input/x/click"},
  };
}

Interaction_teleport::~Interaction_teleport() {}

void Interaction_teleport::frame(vsg::ref_ptr<vsg::Group> scene, Game& game)
{
  if (_action->getStateValid())
  {
    auto state = _action->getStateBool();
    if (state.isActive && state.currentState && !_teleportButtonDown)
    {
      _teleportButtonDown = true;
    }
  }
    
  if (_teleportButtonDown && _leftHandPose->getTransformValid())
  {
    // Raycast from controller aim to world, colliding with anything named "ground"
    // https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#semantic-path-standard-pose-identifiers
    vsg::dvec3 intersectStart = game.getPlayerOriginInWorld() + (_leftHandPose->getTransform() * vsg::dvec3{0.0, 0.0, 0.0});
    vsg::dvec3 intersectEnd = _leftHandPose->getTransform() * vsg::dvec3{0.0, 0.0, -100.0};

    auto intersector = vsg::LineSegmentIntersector::create(intersectStart, intersectEnd);
    // TODO: Intersect whole scene, or sub-scene matched on tags? For now we can't be anywhere but the ground plane
    _ground->accept(*intersector);
    if (!intersector->intersections.empty())
    {
      _teleportTargetValid = true;
      _teleportTarget->setAllChildren(true);

      _teleportPosition = intersector->intersections.front()->worldIntersection;

      for (auto& child : _teleportTarget->children)
      {
        if (auto m = child.node->cast<vsg::MatrixTransform>())
        {
          // TODO: Double check this rotate - Due to the wonky axis mapping / Need some helpers for vsgworld_to_vsgvrworld?
          // TODO: Or, put markers within the vsg world space, with just pose bindings and such being special -> Forcing everyone to have an
          //       extra 'world' node at the top of their scene graph?
          m->matrix = vsg::translate(_teleportPosition - game.getPlayerOriginInWorld()) * vsg::rotate(vsg::radians(90.0), {1.0, 0.0, 0.0});
        }
      }
    }
    else
    {
      _teleportTargetValid = false;
      _teleportTarget->setAllChildren(false);
    }
  }

  if (_action->getStateValid())
  {
    auto state = _action->getStateBool();
    if (state.isActive && state.currentState == false && _teleportButtonDown)
    {
      if (_teleportTargetValid)
      {
        game.setPlayerOriginInWorld(_teleportPosition);
      }
      _teleportButtonDown = false;
      _teleportTargetValid = false;
      _teleportTarget->setAllChildren(false);
    }
  }
}
