#include "voreen/core/vis/processors/render/orthogonalslicerenderer.h"

#include "voreen/core/vis/properties/boolproperty.h"
#include "voreen/core/vis/properties/numericproperty.h"

namespace voreen {

OrthogonalSliceRenderer::OrthogonalSliceRenderer()
    : SliceRendererBase(),
    volumeDimensions_(1),
    slicePositions_(-1, -1, -1),
    useXYSliceProp_("useXYSliceProp", "render xy-slice:", true),
    useXZSliceProp_("useZXSliceProp", "render zx-slice:", false),
    useZYSliceProp_("useZYSliceProp", "render zy-slice:", false),
    sliceNoXYProp_("sliceNoXYProp", "xy-slice number:", 1, 1, 100),
    sliceNoXZProp_("sliceNoZXProp", "zx-slice number:", 1, 1, 100),
    sliceNoZYProp_("sliceNoZYProp", "zy-slice number:", 1, 1, 100),
    camProp_("camera", "Camera", new tgt::Camera(tgt::vec3(0.0f, 0.0f, 3.5f), tgt::vec3(0.0f, 0.0f, 0.0f), tgt::vec3(0.0f, 1.0f, 0.0f))),
    cameraHandler_(0),
    cpInport_(Port::INPORT, "coprocessor.slicepositionsInput", false),
    cpOutport_(Port::OUTPORT, "coprocessor.slicepositions")
{
    CallMemberAction<OrthogonalSliceRenderer> sliceChangeAction(this,
        &OrthogonalSliceRenderer::onSlicePositionChange);
    useXYSliceProp_.onChange(sliceChangeAction);
    useXZSliceProp_.onChange(sliceChangeAction);
    useZYSliceProp_.onChange(sliceChangeAction);
    sliceNoXYProp_.onChange(sliceChangeAction);
    sliceNoXZProp_.onChange(sliceChangeAction);
    sliceNoZYProp_.onChange(sliceChangeAction);

    addProperty(useXYSliceProp_);
    addProperty(sliceNoXYProp_);
    addProperty(useXZSliceProp_);
    addProperty(sliceNoXZProp_);
    addProperty(useZYSliceProp_);
    addProperty(sliceNoZYProp_);
    addProperty(camProp_);
    cameraHandler_ = new CameraInteractionHandler(&camProp_);
    addInteractionHandler(cameraHandler_);

    addPort(cpInport_);
    addPort(cpOutport_);

    onSlicePositionChange();
}

OrthogonalSliceRenderer::~OrthogonalSliceRenderer() {
    delete cameraHandler_;
}

Processor* OrthogonalSliceRenderer::create() const {
    return new OrthogonalSliceRenderer();
}

const std::string OrthogonalSliceRenderer::getProcessorInfo() const {
    return std::string("Renders slices orthogonal to each other and \
aligned to the x-, y- or/and z-axis.");
}

bool OrthogonalSliceRenderer::isReady() const {
    const std::vector<Port*>& inports = getInports();
    for (size_t i = 0; i < inports.size(); ++i)
        if(!inports[i]->isReady())
            return false;

    const std::vector<Port*>& outports = getOutports();
    for (size_t i = 0; i < outports.size(); ++i)
        if(!outports[i]->isReady())
            return false;

    // The co-processor is optional, so the processor is ready, even if the
    // processor's co-processor inport is not connected()!

    return true;
}

void OrthogonalSliceRenderer::process() {

    if (!inport_.isReady())
        return;

    if (inport_.hasChanged()) {
        volumeDimensions_ = inport_.getData()->getVolume()->getDimensions();
        updateNumSlices();  // validate the currently set values and adjust them if necessary
        transferFunc_.setVolumeHandle(inport_.getData());
    }

    outport_.activateTarget("OrthogonalSliceRenderer::process()");

    setupShader();
    if (ready() == false)
        return;

    const VolumeTexture* const tex = inport_.getData()->getVolumeGL()->getTexture();
    if (tex == 0) {
        LERROR("process(): VolumeTexture in VolumGL is NULL!");
        return;
    }

    OrthogonalSliceRenderer* other = cpInport_.getConnectedProcessor();
    if (other != 0) {
        tgt::ivec3 positions = tgt::clamp(other->getSlicePositions(), tgt::ivec3(-1),
            (volumeDimensions_ - tgt::ivec3(1)));
        setupSliceProperties(positions);
    } else {
        onSlicePositionChange();
        setSlicePropertiesVisible(true);
    }

    glActiveTexture(tm_.getGLTexUnit(transFuncTexUnit_));
    transferFunc_.get()->bind();
    glActiveTexture(tm_.getGLTexUnit(volTexUnit_));
    tex->bind();

    // important: save current camera state before using the processor's camera or
    // successive processors will use those settings!
    //
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    camProp_.get()->look();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (useXYSliceProp_.get() == true) {
        float sliceNo = static_cast<float>(sliceNoXYProp_.get() - 1);
        renderSlice(SLICE_XY, sliceNo);
    }

    if (useXZSliceProp_.get() == true) {
        float sliceNo = static_cast<float>(sliceNoXZProp_.get() - 1);
        renderSlice(SLICE_XZ, sliceNo);
    }

    if (useZYSliceProp_.get() == true) {
        float sliceNo = static_cast<float>(sliceNoZYProp_.get() - 1);
        renderSlice(SLICE_ZY, sliceNo);
    }

    deactivateShader();
    glActiveTexture(GL_TEXTURE0);
    outport_.deactivateTarget();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

const tgt::ivec3& OrthogonalSliceRenderer::getSlicePositions() const {
    return slicePositions_;
}

// protected methods
//

void OrthogonalSliceRenderer::onSlicePositionChange() {
    slicePositions_.x = sliceNoZYProp_.get();
    slicePositions_.y = sliceNoXZProp_.get();
    slicePositions_.z = sliceNoXYProp_.get();

    if (useXYSliceProp_.get() == false)
        slicePositions_.z *= -1;
    if (useXZSliceProp_.get() == false)
        slicePositions_.y *= -1;
    if (useZYSliceProp_.get() == false)
        slicePositions_.x *= -1;
}

void OrthogonalSliceRenderer::renderSlice(const SliceAlignment& sliceAlign, const float sliceNo)
{
    tgt::vec3 dim = static_cast<tgt::vec3>(volumeDimensions_ - tgt::ivec3(1));
    tgt::vec3 llb = (dim / -tgt::max(dim));
    tgt::vec3 textureSize = (llb * -2.0f);

    tgt::ivec3 permutation(0, 1, 2);
    switch (sliceAlign) {
        default:
        case SLICE_XY:
            break;
        case SLICE_XZ:
            permutation = tgt::ivec3(0, 2, 1);
            break;
        case SLICE_ZY:
            permutation = tgt::ivec3(2, 1, 0);
            break;
    }
    float s = sliceNo / (volumeDimensions_[permutation.z] - 1);

    tgt::vec3 ll = permuteComponents(tgt::vec3(0.0f, 0.0f, s), permutation);
    tgt::vec3 lr = permuteComponents(tgt::vec3(1.0f, 0.0f, s), permutation);
    tgt::vec3 ur = permuteComponents(tgt::vec3(1.0f, 1.0f, s), permutation);
    tgt::vec3 ul = permuteComponents(tgt::vec3(0.0f, 1.0f, s), permutation);

    glBegin(GL_QUADS);
        glTexCoord3fv(ll.elem);
        glVertex3fv( ((ll * textureSize) + llb).elem );

        glTexCoord3fv(lr.elem);
        glVertex3fv( ((lr * textureSize) + llb).elem );

        glTexCoord3fv(ur.elem);
        glVertex3fv( ((ur * textureSize) + llb).elem );

        glTexCoord3fv(ul.elem);
        glVertex3fv( ((ul * textureSize) + llb).elem );
    glEnd();
}

void OrthogonalSliceRenderer::setSlicePropertiesVisible(const bool visible) {
    sliceNoXYProp_.setVisible(visible);
    useXYSliceProp_.setVisible(visible);
    sliceNoXZProp_.setVisible(visible);
    useXZSliceProp_.setVisible(visible);
    sliceNoZYProp_.setVisible(visible);
    useZYSliceProp_.setVisible(visible);
}

void OrthogonalSliceRenderer::setupSliceProperties(const tgt::ivec3& positions) {
    if (slicePositions_ == positions)
        return;

    slicePositions_ = positions;
    bool useForeignSlices = false;
    if (slicePositions_.x > 0) {
        sliceNoZYProp_.set(slicePositions_.x);
        useZYSliceProp_.set(true);
        useForeignSlices = true;
    } else
        useZYSliceProp_.set(false);

    if (slicePositions_.y > 0) {
        sliceNoXZProp_.set(slicePositions_.y);
        useXZSliceProp_.set(true);
        useForeignSlices = true;
    } else
        useXZSliceProp_.set(false);

    if (slicePositions_.z > 0) {
        sliceNoXYProp_.set(slicePositions_.z);
        useXYSliceProp_.set(true);
        useForeignSlices = true;
    } else
        useXYSliceProp_.set(false);

    if (useForeignSlices == true)
        setSlicePropertiesVisible(false);
    else
        setSlicePropertiesVisible(true);

    updateNumSlices();
}

void OrthogonalSliceRenderer::updateNumSlices() {
    // set number of slice for xy-plane (along z-axis)
    //
    int numSlicesXY = volumeDimensions_.z;
    sliceNoXYProp_.setMaxValue(numSlicesXY);
    if (sliceNoXYProp_.get() > numSlicesXY)
        sliceNoXYProp_.set(numSlicesXY / 2);
    sliceNoXYProp_.updateWidgets();

    // set number of slices for zx-plane (along y-axis)
    //
    int numSlicesZX = volumeDimensions_.y;
    sliceNoXZProp_.setMaxValue(numSlicesZX);
    if (sliceNoXZProp_.get() > numSlicesZX)
        sliceNoXZProp_.set(numSlicesZX / 2);
    sliceNoXZProp_.updateWidgets();

    // set number of slices for sagittal plane (along x-axis)
    //
    int numSlicesZY = volumeDimensions_.x;
    sliceNoZYProp_.setMaxValue(numSlicesZY);
    if (sliceNoZYProp_.get() > numSlicesZY)
        sliceNoZYProp_.set(numSlicesZY / 2);
    sliceNoZYProp_.updateWidgets();
}

inline tgt::vec3 OrthogonalSliceRenderer::permuteComponents(const tgt::vec3& input,
                                                            const tgt::ivec3& permutation)
{
    return tgt::vec3(input[permutation.x], input[permutation.y], input[permutation.z]);
}

}   // namespace
