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

#include <vsgvr/app/UserOrigin.h>
#include <vsg/maths/transform.h>

namespace vsgvr
{
    UserOrigin::UserOrigin() {}

    UserOrigin::UserOrigin(vsg::dmat4 _matrix)
      : Inherit(_matrix)
    {}

    UserOrigin::UserOrigin(vsg::dvec3 position, vsg::dquat orientation, vsg::dvec3 scale)
    {
      setOriginInScene(position, orientation, scale);
    }

    UserOrigin::UserOrigin(vsg::dvec3 playerGroundPosition, vsg::dvec3 position, vsg::dquat orientation, vsg::dvec3 scale)
    {
      setUserInScene(playerGroundPosition, position, orientation, scale);
    }

    void UserOrigin::setOriginInScene(vsg::dvec3 position, vsg::dquat orientation, vsg::dvec3 scale)
    {
      matrix = vsg::inverse(
        vsg::translate(position) * 
        vsg::rotate(orientation) * 
        vsg::scale(scale)
      );
    }

    void UserOrigin::setUserInScene(vsg::dvec3 playerGroundPosition, vsg::dvec3 newPosition, vsg::dquat orientation, vsg::dvec3 scale)
    {
      matrix = vsg::inverse(
        vsg::translate(newPosition) *
        vsg::rotate(orientation) *
        vsg::scale(scale) *
        vsg::translate(-playerGroundPosition)
      );
    }
}
