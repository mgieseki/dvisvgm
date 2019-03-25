add_library(optimizer
	AttributeExtractor.hpp      AttributeExtractor.cpp
	DependencyGraph.hpp
	GroupCollapser.hpp          GroupCollapser.cpp
	OptimizerModule.hpp
	SVGOptimizer.hpp            SVGOptimizer.cpp
	RedundantElementRemover.hpp RedundantElementRemover.cpp
	TransformSimplifier.hpp     TransformSimplifier.cpp
  	WSNodeRemover.hpp           WSNodeRemover.cpp
)
