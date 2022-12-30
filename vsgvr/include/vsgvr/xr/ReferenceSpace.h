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
#include <vsgvr/xr/Common.h>

namespace vsgvr {
  class Session;

  /// A XrSpace handle representing a pose and orientation within one of the base reference space types
  ///
  /// Reference spaces are used primarily by the Session and CompositionLayers in order to position content.
  class VSGVR_DECLSPEC ReferenceSpace : public vsg::Inherit<vsg::Object, ReferenceSpace>
  {
  public:
    ReferenceSpace() = delete;
    ReferenceSpace(XrSession session, XrReferenceSpaceType referenceSpaceType);
    /// Note: XrPosef is in the OpenXR coordinate system - X-right, Y-up, Z-back
    ReferenceSpace(XrSession session, XrReferenceSpaceType referenceSpaceType, XrPosef poseInReferenceSpace);
    virtual ~ReferenceSpace();

    XrSpace getSpace() const { return _space; }
    XrSpaceLocation locate(XrSpace baseSpace, XrTime time);

  private:
    void createSpace(XrSession session, XrReferenceSpaceType referenceSpaceType, XrPosef poseInReferenceSpace);
    void destroySpace();

    XrSpace _space = XR_NULL_HANDLE;
  };
}

EVSG_type_name(vsgvr::ReferenceSpace);
