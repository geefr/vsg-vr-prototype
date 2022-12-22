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
#include <vsgvr/xr/Instance.h>
#include <vsgvr/xr/Traits.h>
#include <vsgvr/xr/EventHandler.h>
#include <vsgvr/xr/GraphicsBindingVulkan.h>
#include <vsgvr/xr/Session.h>

#include <vsg/app/CommandGraph.h>
#include <vsg/app/RecordAndSubmitTask.h>
#include <vsg/app/CompileManager.h>
#include <vsg/app/UpdateOperations.h>
#include <vsg/app/RenderGraph.h>
#include <vsg/ui/FrameStamp.h>

#include <string>
#include <vector>

namespace vsgvr {
    /**
     * A viewer which renders data to the OpenXR runtime
     * 
     * This class is similar to vsg::Viewer, however:
     * * The event handling loop is somewhat different, requiring the application to behave correctly, based on the result of Viewer::pollEvents
     * * This viewer has no presentation surface or vsg::Window association - Rendered data is passed to OpenXR only
     *   * Any elements in vsg which require a vsg::Window cannot work with this viewer (TODO)
     * * While some functions are similar, this class does not directly inherit from vsg::Viewer (TODO)
     */
    class VSGVR_DECLSPEC Viewer : public vsg::Inherit<vsg::Object, Viewer>
    {
        public:
            /**
             * Constructor
             * 
             * The OpenXR runtime is bound to vulkan through the graphicsBinding structure. Ensure that:
             * * The Vulkan version is compatible - GraphicsBindingVulkan::getVulkanRequirements
             * * That the required instance and device extensions are present - GraphicsBindingVulkan::getVulkanRequirements
             * * A specific PhysicalDevice is used - GraphicsBindingVulkan::getVulkanDeviceRequirements
             */
            Viewer(vsg::ref_ptr<Instance> xrInstance, vsg::ref_ptr<Traits> xrTraits, vsg::ref_ptr<GraphicsBindingVulkan> graphicsBinding);

            Viewer() = delete;
            ~Viewer();

            // TODO: Update this - Summary level of what to do, based on the event updates.
            //       Simple things don't need any input from app, so it might just reduce
            //       to a boolean good/exit status.

            /**
             * Whether polling for events succeeded, and what action the application
             * should perform next within the render loop 
             */
            enum class PollEventsResult {
                // The application should continue rendering
                Success,
                // OpenXR is idle, the application should reduce power usage
                // until further notice (but keep polling for events now and then)
                RuntimeIdle,
                // OpenXR is not running - The application should try again
                NotRunning,
                // OpenXR is running, but rendering is not required at the moment (headset not on head)
                // The application should continue the render loop, except for actually rendering content
                RunningDontRender,
                // OpenXR is running, and rendering is required
                RunningDoRender,
                // The user or OpenXR has requested that the application exits
                Exit,
            };

            /// Must be called regularly, within the render loop
            /// The application must handle the returned values accordingly,
            /// to avoid exceptions being thrown on subsequence calls
            auto pollEvents() -> PollEventsResult;

            /// Similar to vsg::Viewer, advance the framebuffers and prepare for rendering
            ///
            /// Internally this will acquire a frame / swapchain image from OpenXR. The application
            /// **must** call releaseFrame() once after rendering, even if this method returns false.
            ///
            /// @return Whether the application should render.
            bool advanceToNextFrame();

            /// Submit rendering tasks to Vulkan
            void recordAndSubmit();

            /// Release or end the current OpenXR frame
            ///
            /// This method **must** be called if advanceToNextFrame() was called
            void releaseFrame();

            // Manage the work to do each frame using RecordAndSubmitTasks.
            using RecordAndSubmitTasks = std::vector<vsg::ref_ptr<vsg::RecordAndSubmitTask>>;
            RecordAndSubmitTasks recordAndSubmitTasks;

            // TODO: These methods are required at the moment, and have some small differences from their vsg
            //       counterparts. Ideally these would not be duplicated, but this will likely require chnanges
            //       within vsg to correct. In the long run Viewer should only be concerned with the XR parts
            //       or may even be moved to be a 'Window' class.
            vsg::CommandGraphs createCommandGraphsForView(vsg::ref_ptr<vsg::Node> vsg_scene, std::vector<vsg::ref_ptr<vsg::Camera>>& cameras, bool assignHeadlight = true);
            void assignRecordAndSubmitTask(std::vector<vsg::ref_ptr<vsg::CommandGraph>> in_commandGraphs);
            void compile(vsg::ref_ptr<vsg::ResourceHints> hints = {});

            // OpenXR action sets, which will be managed by the viewer / session
            std::vector<vsg::ref_ptr<ActionSet>> actionSets;
            // Active actions sets, which will be synchronised each update
            // One or more may be active at a time, depending on user interaction mode
            std::vector<vsg::ref_ptr<ActionSet>> activeActionSets;

        private:
            vsg::ref_ptr<vsg::Camera> createCamera(const VkExtent2D& extent);

            void shutdownAll();
            void getViewConfiguration();

            void syncActions();
            // Attach action sets to the session and create action spaces
            // This method MUST be called ONCE - After action set bindings
            // have been suggested, and prior to scene rendering.
            // Viewer performs this automatically during the first
            // call to pollEvents
            void createActionSpacesAndAttachActionSets();
            void destroyActionSpaces();

            vsg::ref_ptr<Instance> _instance;
            vsg::ref_ptr<Traits> _xrTraits;
            vsg::ref_ptr<GraphicsBindingVulkan> _graphicsBinding;

            EventHandler _eventHandler;

            // Details of chosen _xrTraits->viewConfigurationType
            // Details of individual views - recommended size / sampling
            XrViewConfigurationProperties _viewConfigurationProperties;
            std::vector<XrViewConfigurationView> _viewConfigurationViews;

            // Session
            void createSession();
            void destroySession();
            vsg::ref_ptr<Session> _session;

            bool _firstUpdate = true;
            std::vector<XrActionSet> _attachedActionSets;

            // Per-frame
            XrFrameState _frameState;
            vsg::ref_ptr<vsg::FrameStamp> _frameStamp;

            std::vector<XrCompositionLayerBaseHeader*> _layers;
            XrCompositionLayerProjection _layerProjection;
            std::vector<XrCompositionLayerProjectionView> _layerProjectionViews;
    };
}

EVSG_type_name(vsgvr::Viewer);
