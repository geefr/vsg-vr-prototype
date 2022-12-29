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
#include <vsg/core/ref_ptr.h>

#include <vsgvr/xr/Common.h>

namespace vsgvr {
  /// Composition layer definitions used by vsgvr::Viewer
  /// 
  /// These classes only expose the user-facing components such as the pose and scale of a quad layer. Assignment
  /// of spaces and swapchain images are handled by the Viewer automatically. All composition layers are defined in the Session's reference space.
  /// 
  /// Note: As layer definitions are significantly different, any additional layer classes require a corresponding update to vsgvr::Viewer itself.
  class VSGVR_DECLSPEC CompositionLayer : public vsg::Inherit<vsg::Object, CompositionLayer>
  {
  public:
    virtual ~CompositionLayer();

  protected:
    CompositionLayer();
  };

  /// XrCompositionLayerProjection - Planar projected images rendered for each eye, displaying the required view within the scene
  class VSGVR_DECLSPEC CompositionLayerProjection final : public vsg::Inherit<vsgvr::CompositionLayer, CompositionLayerProjection>
  {
    public:
      CompositionLayerProjection();
      CompositionLayerProjection(XrCompositionLayerFlags inFlags);
      virtual ~CompositionLayerProjection();
      XrCompositionLayerProjection getCompositionLayer() const;

      XrCompositionLayerFlags flags;

      /// @internal
      XrCompositionLayerProjection _compositionLayer;
      std::vector<XrCompositionLayerProjectionView> _layerProjectionViews;
  };

  /// XrCompositionLayerQuad - Rendered elements on a 2D quad, positioned in world space.
  /// In vsgvr::Viewer the contents of a scenegraph will be rendered to the quad - This may 
  /// be the main 'world' itself, or additional graphs for GUI elements, images, etc.
  class VSGVR_DECLSPEC CompositionLayerQuad final : public vsg::Inherit<vsgvr::CompositionLayer, CompositionLayerQuad>
  {
    public:
      CompositionLayerQuad();
      CompositionLayerQuad(XrPosef inPose, XrExtent2Df inSize, XrCompositionLayerFlags inFlags, XrEyeVisibility inEyeVisibility);
      virtual ~CompositionLayerQuad();
      XrCompositionLayerQuad getCompositionLayer() const;

      XrPosef pose = {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f}
      };
      XrExtent2Df size = {1.0, 1.0};
      XrCompositionLayerFlags flags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
      XrEyeVisibility eyeVisibility = XrEyeVisibility::XR_EYE_VISIBILITY_BOTH;

      /// @internal
      XrCompositionLayerQuad _compositionLayer;
  };
}

EVSG_type_name(vsgvr::CompositionLayer);
EVSG_type_name(vsgvr::CompositionLayerProjection);
EVSG_type_name(vsgvr::CompositionLayerQuad);
