/// \ingroup base
/// \class ttk::MorseSmaleComplex
/// \author Guillaume Favelier <guillaume.favelier@lip6.fr>
/// \author Julien Tierny <julien.tierny@lip6.fr>
/// \date February 2017.
///
/// \brief TTK processing package for the computation of Morse-Smale complexes.
///
/// \b Related \b publication \n
/// "Parallel Computation of 3D Morse-Smale Complexes" \n
/// Nithin Shivashankar, Vijay Natarajan \n
/// Proc. of EuroVis 2012. \n
/// Computer Graphics Forum, 2012.
///
/// \sa ttk::Triangulation
/// \sa vtkMorseSmaleComplex.cpp %for a usage example.

#ifndef _MORSESMALECOMPLEX_H
#define _MORSESMALECOMPLEX_H

// base code includes
#include<AbstractMorseSmaleComplex.h>
#include<MorseSmaleComplex2D.h>
#include<MorseSmaleComplex3D.h>

/*
 * Morse-Smale complex developer quick guide:
 *
 * What is the architecture?
 * -------------------------
 * The DiscreteGradient class contains the basic structures to define and build
 * a discrete gradient. It also has several functions that decrease the number
 * of unpaired cells as parallel post-processing steps. Even more work is done
 * on the gradient with a sequential simplification step. Finally, it is able
 * to build critical points and vpaths.
 * Its files are:
 *   DiscreteGradient.cpp
 *   DiscreteGradient.h
 *
 * The AbstractMorseSmaleComplex class contains whatever is common
 * between the MorseSmaleComplex2D class and the MorseSmaleComplex3D class
 * e.g. parameters, configuration functions, input and output data pointers.
 * In particular, it contains a DiscreteGradient attribute and a Triangulation
 * attribute.
 * Its files are:
 *   AbstractMorseSmaleComplex.cpp
 *   AbstractMorseSmaleComplex.h
 *
 * The MorseSmaleComplex2D class is derived from the AbstractMorseSmaleComplex
 * class. It is specialized in building the Morse-Smale complex on 2D
 * triangulations. This class uses the DiscreteGradient attribute to build a
 * valid discrete gradient before building the MSC outputs itself i.e.
 * critical points, 1-separatrices, segmentation.
 * Its files are:
 *   MorseSmaleComplex2D.cpp
 *   MorseSmaleComplex2D.h
 *
 * The MorseSmaleComplex3D class is derived from the AbstractMorseSmaleComplex
 * class. It does the same job as the MorseSmaleComplex2D class but on 3D
 * triangulations. Note that this class has a function
 * computePersistencePairs() to get the saddle-saddle pairs of the data.
 * It adds the saddle-connectors to the 1-separatrices and adds another output
 * for 2-separatrices.
 * Its files are:
 *   MorseSmaleComplex3D.cpp
 *   MorseSmaleComplex3D.h
 *
 * The MorseSmaleComplex class is derived from the AbstractMorseSmaleComplex
 * class. It is a convenience class that detects the dimensionality of the data
 * and uses the correct implementation of the Morse-Smale complex computation
 * (between MorseSmaleComplex2D and MorseSmaleComplex3D).
 * Its files are:
 *   MorseSmaleComplex.cpp
 *   MorseSmaleComplex.h
 *
 * How to build the gradient?
 * --------------------------
 * Everything that concerns the gradient is in the DiscreteGradient class.
 * In order to build a valid discrete gradient you need to first set the data
 * pointers to the input scalar field, offset field and triangulation. You need
 * to set also the data pointers to the output critical points, 1-separatrices,
 * 2-separatrices and segmentation. Additionnal parameters can be configured
 * like an iteration threshold, options to have PL-Compliant extrema and
 * saddles, an option to enable collecting of persistence pairs or
 * post-processing of the saddle-connectors. Note that they all have default
 * values that correspond to a standard scenario. Like any other TTK
 * module, the level of debug and the number of threads can be adjusted to suit
 * your needs.
 * Once all the parameters and data pointers are set, the function
 * buildGradient() builds the discrete gradient. As a substantial number of
 * unpaired cells is expected, it is strongly recommended to use after this
 * function the function buildGradient2() and after that the buildGradient3()
 * function if the input dataset is in the 3D domain.
 * Finally, you can apply reverseGradient() to auto-detect the PL critical
 * points and impose that the gradient is PL-Compliant (except on the
 * boundary).
 * Examples of such usage of the DiscreteGradient class can be found in
 * the execute() function of the MorseSmaleComplex2D class and
 * MorseSmaleComplex3D class as these classes need to compute a discrete
 * gradient to build their own outputs.
 *
 * Where is the simplification algorithm?
 * --------------------------------------
 * The main steps of the gradient simplification algorithm are stored in the
 * reverseGradient() function in the DiscreteGradient class. More informations
 * can be found in each simplify-like function as the process is slightly
 * different depending on the index of the critical points involved:
 *   simplifySaddleMaximumConnections() for reversal of (saddle,...,maximum)
 *   vpaths.
 *   simplifySaddleSaddleConnections1() for reversal of
 *   (2-saddle,...,1-saddle) vpaths.
 *   simplifySaddleSaddleConnections2() for reversal of
 *   (1-saddle,...,2-saddle) vpaths.
 *
 * How to add a scalar field on any output geometry?
 * -------------------------------------------------
 * First, go to AbstractMorseSmaleComplex.h and add the void* pointer to a STL
 * container (e.g. vector) as a class attribute. In the code, the attributes
 * of the same output are grouped together and are prefixed by its name.
 * So for example, the outputSeparatrices1_points_smoothingMask_ variable
 * represents the smoothingMask scalar field that is associated to the points
 * of the 1-separatrices of the Morse-Smale complex. As you just added a new
 * attribute, you need to update the setter corresponding to the output to
 * add this new element:
 * The setters available are:
 *   setOutputCriticalPoints()
 *   setOutputSeparatrices1()
 *   setOutputSeparatrices2()
 *   setOutputMorseComplexes()
 * As you added a new attribute in the class, you need to give it a default
 * value in the constructor in AbstractMorseSmaleComplex.cpp (typically nullptr
 * for a pointer).
 * Now, you need to overload the same setter than previously but in
 * MorseSmaleComplex.h this time in order to propagate the new field
 * information to the concrete implementations. As the MorseSmaleComplex2D and
 * MorseSmaleComplex3D classes are both derived from AbstractMorseSmaleComplex
 * they already have the updated version of the function.
 * Now that you have access to the data pointer inside the actual
 * implementation you can cast it from void* to dataType* and modify it to
 * your convenience.
 *
 * What part of the code is parallel?
 * ----------------------------------
 * From a global point a view, the part of the code that builds the gradient
 * as well as the two post-processing steps are accelerated by OpenMP (if
 * available).
 * The gradient simplification step is mostly sequential except the
 * initialization of internal structures in the initialize-like functions which
 * is done is parallel with OpenMP.
 * Then, each output of the Morse-Smale complex is computed in parallel with
 * OpenMP except the 2-separatrices of a 3D dataset that require heavy
 * synchronisation.
 * Finally, the generation of the geometry for the visualization is done
 * sequentially.
 * Complete list:
 * In the DiscreteGradient class:
 *   buildGradient()
 *   buildGradient2()
 *   buildGradient3()
 *   initializeSaddleMaximumConnections()
 *   initializeSaddleSaddleConnections1()
 *   initializeSaddleSaddleConnections2()
 * In the MorseSmaleComplex2D class:
 *   getSeparatrices1()
 *   setAscendingSegmentation()
 *   setDescendingSegmentation()
 * In the MorseSmaleComplex3D class:
 *   getSeparatrices1()
 *   getAscendingSeparatrices2()
 *   getDescendingSeparatrices2()
 *   setAscendingSegmentation()
 *   setDescendingSegmentation()
 */

namespace ttk{

  class MorseSmaleComplex : public AbstractMorseSmaleComplex{

    public:

      MorseSmaleComplex();
      ~MorseSmaleComplex();

      int setIterationThreshold(const int iterationThreshold){
        return abstractMorseSmaleComplex_->setIterationThreshold(iterationThreshold);
      }

      int setReverseSaddleMaximumConnection(const bool state){
        return abstractMorseSmaleComplex_->setReverveSaddleMaximumConnection(state);
      }

      int setReverseSaddleSaddleConnection(const bool state){
        return abstractMorseSmaleComplex_->setReverveSaddleSaddleConnection(state);
      }

      int setComputeAscendingSeparatrices1(const bool state){
        return abstractMorseSmaleComplex_->setComputeAscendingSeparatrices1(state);
      }

      int setComputeDescendingSeparatrices1(const bool state){
        return abstractMorseSmaleComplex_->setComputeDescendingSeparatrices1(state);
      }

      int setComputeSaddleConnectors(const bool state){
        return abstractMorseSmaleComplex_->setComputeSaddleConnectors(state);
      }

      int setComputeAscendingSeparatrices2(const bool state){
        return abstractMorseSmaleComplex_->setComputeAscendingSeparatrices2(state);
      }

      int setComputeDescendingSeparatrices2(const bool state){
        return abstractMorseSmaleComplex_->setComputeDescendingSeparatrices2(state);
      }

      int setComputeAscendingSegmentation(const bool state){
        return abstractMorseSmaleComplex_->setComputeAscendingSegmentation(state);
      }

      int setComputeDescendingSegmentation(const bool state){
        return abstractMorseSmaleComplex_->setComputeDescendingSegmentation(state);
      }

      int setComputeFinalSegmentation(const bool state){
        return abstractMorseSmaleComplex_->setComputeFinalSegmentation(state);
      }

      int setReturnSaddleConnectors(const bool state){
        return abstractMorseSmaleComplex_->setReturnSaddleConnectors(state);
      }

      int setSaddleConnectorsPersistenceThreshold(const double threshold){
        return abstractMorseSmaleComplex_->setSaddleConnectorsPersistenceThreshold(threshold);
      }

      int setupTriangulation(Triangulation* const data){
        inputTriangulation_=data;
        const int dimensionality=inputTriangulation_->getCellVertexNumber(0)-1;

        switch(dimensionality){
          case 2:
            abstractMorseSmaleComplex_=&morseSmaleComplex2D_;
            break;

          case 3:
            abstractMorseSmaleComplex_=&morseSmaleComplex3D_;
            break;
        }

        abstractMorseSmaleComplex_->setupTriangulation(inputTriangulation_);

        return 0;
      }

      inline int setDebugLevel(const int& debugLevel){
        morseSmaleComplex2D_.setDebugLevel(debugLevel);
        morseSmaleComplex3D_.setDebugLevel(debugLevel);
        debugLevel_=debugLevel;

        return 0;
      }

      inline int setThreadNumber(const int& threadNumber){
        morseSmaleComplex2D_.setThreadNumber(threadNumber);
        morseSmaleComplex3D_.setThreadNumber(threadNumber);
        threadNumber_=threadNumber;

        return 0;
      }

      inline int setWrapper(const Wrapper* const wrapper){
        morseSmaleComplex2D_.setWrapper(wrapper);
        morseSmaleComplex3D_.setWrapper(wrapper);

        return 0;
      }

      inline int setInputScalarField(void* const data){
        morseSmaleComplex2D_.setInputScalarField(data);
        morseSmaleComplex3D_.setInputScalarField(data);
        return 0;
      }

      inline int setInputOffsets(void* const data){
        morseSmaleComplex2D_.setInputOffsets(data);
        morseSmaleComplex3D_.setInputOffsets(data);

        return 0;
      }

      inline int setOutputCriticalPoints(int* const criticalPoints_numberOfPoints,
          vector<float>* const criticalPoints_points,
          vector<int>* const criticalPoints_points_cellDimensons,
          vector<int>* const criticalPoints_points_cellIds,
          void* const criticalPoints_points_cellScalars,
          vector<char>* const criticalPoints_points_isOnBoundary,
          vector<int>* const criticalPoints_points_PLVertexIdentifiers,
          vector<int>* const criticalPoints_points_manifoldSize){
        morseSmaleComplex2D_.setOutputCriticalPoints(criticalPoints_numberOfPoints,
            criticalPoints_points,
            criticalPoints_points_cellDimensons,
            criticalPoints_points_cellIds,
            criticalPoints_points_cellScalars,
            criticalPoints_points_isOnBoundary,
            criticalPoints_points_PLVertexIdentifiers,
            criticalPoints_points_manifoldSize);
        morseSmaleComplex3D_.setOutputCriticalPoints(criticalPoints_numberOfPoints,
            criticalPoints_points,
            criticalPoints_points_cellDimensons,
            criticalPoints_points_cellIds,
            criticalPoints_points_cellScalars,
            criticalPoints_points_isOnBoundary,
            criticalPoints_points_PLVertexIdentifiers,
            criticalPoints_points_manifoldSize);

        return 0;
      }

      inline int setOutputSeparatrices1(int* const separatrices1_numberOfPoints,
          vector<float>* const separatrices1_points,
          vector<char>* const separatrices1_points_smoothingMask,
          vector<int>* const separatrices1_points_cellDimensions,
          vector<int>* const separatrices1_points_cellIds,
          int* const separatrices1_numberOfCells,
          vector<int>* const separatrices1_cells,
          vector<int>* const separatrices1_cells_sourceIds,
          vector<int>* const separatrices1_cells_destinationIds,
          vector<int>* const separatrices1_cells_separatrixIds,
          vector<char>* const separatrices1_cells_separatrixTypes,
          void* const separatrices1_cells_separatrixFunctionMaxima,
          void* const separatrices1_cells_separatrixFunctionMinima,
          void* const separatrices1_cells_separatrixFunctionDiffs,
          vector<char>* const separatrices1_cells_isOnBoundary){
        morseSmaleComplex2D_.setOutputSeparatrices1(separatrices1_numberOfPoints,
            separatrices1_points,
            separatrices1_points_smoothingMask,
            separatrices1_points_cellDimensions,
            separatrices1_points_cellIds,
            separatrices1_numberOfCells,
            separatrices1_cells,
            separatrices1_cells_sourceIds,
            separatrices1_cells_destinationIds,
            separatrices1_cells_separatrixIds,
            separatrices1_cells_separatrixTypes,
            separatrices1_cells_separatrixFunctionMaxima,
            separatrices1_cells_separatrixFunctionMinima,
            separatrices1_cells_separatrixFunctionDiffs,
            separatrices1_cells_isOnBoundary);
        morseSmaleComplex3D_.setOutputSeparatrices1(separatrices1_numberOfPoints,
            separatrices1_points,
            separatrices1_points_smoothingMask,
            separatrices1_points_cellDimensions,
            separatrices1_points_cellIds,
            separatrices1_numberOfCells,
            separatrices1_cells,
            separatrices1_cells_sourceIds,
            separatrices1_cells_destinationIds,
            separatrices1_cells_separatrixIds,
            separatrices1_cells_separatrixTypes,
            separatrices1_cells_separatrixFunctionMaxima,
            separatrices1_cells_separatrixFunctionMinima,
            separatrices1_cells_separatrixFunctionDiffs,
            separatrices1_cells_isOnBoundary);

        return 0;
      }

      inline int setOutputSeparatrices2(int* const separatrices2_numberOfPoints,
          vector<float>* const separatrices2_points,
          int* const separatrices2_numberOfCells,
          vector<int>* const separatrices2_cells,
          vector<int>* const separatrices2_cells_sourceIds,
          vector<int>* const separatrices2_cells_separatrixIds,
          vector<char>* const separatrices2_cells_separatrixTypes,
          void* const separatrices2_cells_separatrixFunctionMaxima,
          void* const separatrices2_cells_separatrixFunctionMinima,
          void* const separatrices2_cells_separatrixFunctionDiffs,
          vector<char>* const separatrices2_cells_isOnBoundary){
        morseSmaleComplex3D_.setOutputSeparatrices2(separatrices2_numberOfPoints,
            separatrices2_points,
            separatrices2_numberOfCells,
            separatrices2_cells,
            separatrices2_cells_sourceIds,
            separatrices2_cells_separatrixIds,
            separatrices2_cells_separatrixTypes,
            separatrices2_cells_separatrixFunctionMaxima,
            separatrices2_cells_separatrixFunctionMinima,
            separatrices2_cells_separatrixFunctionDiffs,
            separatrices2_cells_isOnBoundary);

        return 0;
      }

      inline int setOutputMorseComplexes(void* const ascendingManifold,
          void* const descendingManifold,
          void* const morseSmaleManifold){
        morseSmaleComplex2D_.setOutputMorseComplexes(ascendingManifold,
            descendingManifold,
            morseSmaleManifold);
        morseSmaleComplex3D_.setOutputMorseComplexes(ascendingManifold,
            descendingManifold,
            morseSmaleManifold);

        return 0;
      }

      template<typename dataType>
        int execute(){
          const int dimensionality=inputTriangulation_->getCellVertexNumber(0)-1;

          switch(dimensionality){
            case 2:
              morseSmaleComplex2D_.execute<dataType>();
              break;

            case 3:
              morseSmaleComplex3D_.execute<dataType>();
              break;
          }

          return 0;
        }

    protected:

      AbstractMorseSmaleComplex* abstractMorseSmaleComplex_;
      MorseSmaleComplex2D morseSmaleComplex2D_;
      MorseSmaleComplex3D morseSmaleComplex3D_;

  };
}

#endif // MORSESMALECOMPLEX_H