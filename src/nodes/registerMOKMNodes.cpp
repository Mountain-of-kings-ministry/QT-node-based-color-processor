#include "registerMOKMNodes.h"

#include "ocio/ConfigLoaderNode.h"
#include "ocio/ColorSpaceConvertNode.h"
#include "ocio/DisplayViewNode.h"
#include "ocio/FileTransformNode.h"
#include "ocio/LookTransformNode.h"
#include "ocio/RangeTransformNode.h"
#include "io/ImageLoaderNode.h"
#include "io/ImageSaverNode.h"
#include "output/PreviewNode.h"

void registerMOKMNodeTypes(NodeEditor::GraphModel *model)
{
    if (!model) return;

    model->registerCategory({"MOKM", "MOKM Color", QColor("#FACC15")});

    using NodeEditor::registerNodeType;
    registerNodeType<ConfigLoaderNode>(model, "MOKM");
    registerNodeType<ColorSpaceConvertNode>(model, "MOKM");
    registerNodeType<DisplayViewNode>(model, "MOKM");
    registerNodeType<FileTransformNode>(model, "MOKM");
    registerNodeType<LookTransformNode>(model, "MOKM");
    registerNodeType<RangeTransformNode>(model, "MOKM");
    registerNodeType<ImageLoaderNode>(model, "MOKM");
    registerNodeType<ImageSaverNode>(model, "MOKM");
    registerNodeType<PreviewNode>(model, "MOKM");
}
