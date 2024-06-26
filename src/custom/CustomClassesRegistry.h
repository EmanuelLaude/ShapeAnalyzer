#ifndef ShapeAnalyzer_RegisterCustomClasses_h
#define ShapeAnalyzer_RegisterCustomClasses_h

#include <memory>

#include <QWidget>

#include <vtkActor.h>

#include "Factory.h"

#include "tabs/CustomTab.h"
#include "tabs/ShapeInterpolationTab.h"
#include "tabs/MeshCheckTab.h"
#include "tabs/CorrespondenceColoringTab.h"
#include "tabs/FunctionTransferTab.h"
#include "tabs/IdentityMatchingTab.h"

#include "contextMenuItems/CustomContextMenuItem.h"
#include "contextMenuItems/ColorEigenfunctionContextMenuItem.h"
#include "contextMenuItems/ColorMetricContextMenuItem.h"
#include "contextMenuItems/ColorSignatureContextMenuItem.h"
#include "contextMenuItems/HeatDiffusionContextMenuItem.h"
#include "contextMenuItems/VoronoiCellsContextMenuItem.h"
#include "contextMenuItems/ExtractSegmentContextMenuItem.h"
#include "contextMenuItems/ShapeInfoContextMenuItem.h"
#include "contextMenuItems/SegmentIndicatorContextMenuItem.h"

#include "../util/HashMap.h"

#include "../domain/Shape.h"

#include "../domain/correspondences/PointCorrespondence.h"
#include "../domain/correspondences/FaceCorrespondence.h"

#include "../domain/metric/Metric.h"
#include "../domain/metric/EuclideanMetric.h"
#include "../domain/metric/GeodesicMetric.h"

#include "../domain/signatures/Signature.h"
#include "../domain/signatures/PetscWaveKernelSignature.h"

#include "../domain/segmentation/Segmentation.h"
#include "../domain/segmentation/VoronoiCellSegmentation.h"

using namespace custom;
using namespace custom::tabs;
using namespace custom::contextMenuItems;

typedef Factory<CustomTab, const HashMap<vtkActor*, shared_ptr<Shape>>&, const HashMap<shared_ptr<PointCorrespondence>, bool>&, const HashMap<shared_ptr<FaceCorrespondence>, bool>&, ShapeAnalyzerInterface*> CustomTabFactory;

typedef Factory<CustomContextMenuItem, shared_ptr<Shape>, ShapeAnalyzerInterface*> CustomContextMenuItemFactory;

/// \brief Namespace for all custom classes that have to be registered via a Factory
namespace custom {

///\brief Class for registration of concrete CustomTab and CustomContextMenuItem classes.
///\details The registered classes and their methods are automatically connected to the respective menu items and widgest in the QT GUI.
struct CustomClassesRegistry {
    /// \brief This function is responsible for the registration of all CustomTab classes in the CustomTabFactory.
    /// \details It is called in the constructor of ShapeAnalyzer. For each CustomTab an entry in the "View" main menu is created to display and hide the tab. Moreover the GUI has two tab containers that can contain multiple tabs at the same time: "Shapes" (upper right third of the GUI) and "Correspondences" (lower right third of the GUI). To which of the two containers the tab is added can be controlled by specifying a path in the label when the tab is registered: CustomTabFactory::getInstance()->Register<MyTab>("my_tab", "Shapes>>My tab") for the upper or CustomTabFactory::getInstance()->Register<MyTab>("my_tab", "Correspondences>>My tab") for the lower tab container. The second part of the label that comes after the ">>" is used as the actual label. The first sting argument in the Register function of the Factory is the unique key of the registered class.
    static void registerTabs() {
        //tabs
        CustomTabFactory::getInstance()->Register<IdentityMatchingTab>("identity_matching", "Correspondences>>Identity Matching");
        CustomTabFactory::getInstance()->Register<ShapeInterpolationTab>("shape_interpolation", "Shapes>>Shape Interpolation");
        CustomTabFactory::getInstance()->Register<MeshCheckTab>("mesh_check", "Shapes>>Mesh Checker");
        CustomTabFactory::getInstance()->Register<CorrespondenceColoringTab>("correspondence_coloring", "Correspondences>>Correspondence Coloring");
        CustomTabFactory::getInstance()->Register<FunctionTransferTab>("function_transfer", "Correspondences>>Function Transfer");
    }
    
    /// \brief This function is responsible for the registration of all CustomContextMenuItem classes in the CustomContextMenuItemFactory.
    /// \details It is called in the constructor of ShapeAnalyzer. For each CustomContextMenuItem an entry in the right click context menu of each shape is created. The label of the menu item can be specified via the second string argument in the register function. Also nested labels (paths) for can be specified. e.g. "Coloring>>Signature>>My signature" leads to the creation of a "My signature" menu item in the "Coloring>Signature" sub menu. The first sting argument in the Register function of the Factory is the unique key of the registered class.
    static void registerContextMenuItems() {
        //menu items
        CustomContextMenuItemFactory::getInstance()->Register<ShapeInfoContextMenuItem>("shape_info", "Shape Info");

        CustomContextMenuItemFactory::getInstance()->Register<ColorEigenfunctionContextMenuItem>("color_eigenfunction", "Coloring>>i-th Eigenfunction");

        CustomContextMenuItemFactory::getInstance()->Register<HeatDiffusionContextMenuItem<false>>("color_heatdiffusion_not_anim", "Coloring>>Heat diffusion");
        CustomContextMenuItemFactory::getInstance()->Register<HeatDiffusionContextMenuItem<true>>("color_heatdiffusion_anim", "Coloring>>Heat diffusion (Animated)");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorMetricContextMenuItem<GeodesicMetric>>("color_metric_geodesic", "Coloring>>Metric>>Geodesic");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorMetricContextMenuItem<EuclideanMetric>>("color_metric_euclidean", "Coloring>>Metric>>Euclidean");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorSignatureContextMenuItem<PetscWaveKernelSignature>>("color_signature_wavekernel", "Coloring>>Signature>>Wave Kernel");
        
        CustomContextMenuItemFactory::getInstance()->Register<VoronoiCellsContextMenuItem<GeodesicMetric>>("voronoicells_geodesic", "Segmentation>>Voronoi Cells>>Geodesic");
        
        CustomContextMenuItemFactory::getInstance()->Register<VoronoiCellsContextMenuItem<EuclideanMetric>>("voronoicells_euclidean", "Segmentation>>Voronoi Cells>>Euclidean");
        
        CustomContextMenuItemFactory::getInstance()->Register<SegmentIndicatorContextMenuItem>("segment_to_indicator", "Segmentation>>Segment to Indicator Function");
        
        CustomContextMenuItemFactory::getInstance()->Register<ExtractSegmentContextMenuItem>("extract_segment", "Segmentation>>Extract Segment (Create new Shape)");
    }
};
    
}

#endif
