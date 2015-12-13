#include "PraalineLayersInitialiser.h"

#include "svgui/layer/LayerFactory.h"
#include "AnnotationGridLayer.h"
#include "ProsogramLayer.h"

void PraalineLayersInitialiser::initialise()
{
    LayerFactory &factory = LayerFactory::getInstance();
    factory.registerLayerType("AnnotationGrid", LayerFactory::LayerConfiguration(
                                  "annotationgrid", "Annotation Grid", "annotationgrid", false,
                                  QStringList() << "AnnotationGridModel",
                                  [](LayerFactory::LayerType) -> Layer * { return new AnnotationGridLayer(); },
                                  [](Model *) -> Model * { return 0; } ));
    factory.registerLayerType("Prosogram", LayerFactory::LayerConfiguration(
                                  "prosogram", "Prosogram", "prosogram", false,
                                  QStringList() << "ProsogramModel",
                                  [](LayerFactory::LayerType) -> Layer * { return new ProsogramLayer(); },
                                  [](Model *) -> Model * { return 0; } ));
}

