#ifndef HIST_2D
#define HIST_2D

#include "VectorOfNumericVectors.h"
#include "NumericVector.h"

#include <ostream>
#include <boost/multi_array.hpp>



// Hist2D uses a Boost multi-array to store a two dimensional histogram 
// Alignment of the histogram can be done by index of a vector or by the x-value accompanying each y-value
// Hist2D can be computed on-the-fly with series added one at a time, or with a large number of series added together

template <typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  class Hist2D
  {
  public:
  // Hist2D can align histograms in a variety of ways
  enum class Alignment {
    // First four alignment types align all series according to indices of values rather than x dimension of series
    // These provide ways to compare values at a particular part of a series ("beginning, end") rather than
    // values relative to an actual x value such as time or displacement
    Front, // Each series of y values is binned with x as index of y axis and all NumericVectors start at 1
    Back,  // Each series of y values is binned with x as index of y axis and all NumericVectors end at last available x value in histogram 
    AtMax, // Each series of y values is aligned within histogram such that the max value is at the center x value of the histogram
    AtMin, // Each series of y values is aligned within histogram such that the min value is at the center x value of the histogram
    // Fifth alignment type aligns along 'true' x value, so can show how value relates to other dimension, such as value vs. time or value vs. displacement
    ByX    // Each series is aligned according to 'true' x value rather than index of y axis

  };

  Hist2D(int xBins=26, int yBins=28, double xMin = -.1, double xMax = 24, double yMin = -.1, double yMax = 25.1) :
  m_xBins(xBins), m_yBins(yBins),  m_xMin(xMin), m_xMax(xMax), m_yMin(yMin), m_yMax(yMax), m_xVals(xBins+1), m_yVals(yBins+1),  m_matrixCount(boost::extents[yBins][xBins])
  {
    // Initializes all bins to 0
    std::fill(m_matrixCount.origin(), m_matrixCount.origin() + m_matrixCount.size(), 0);

    // Calculates binning increments for x and y
    double xInc = (xMax - xMin)/xBins;
    double yInc = (yMax - yMin)/yBins;
    for ( int i = 0; i<(xBins+1); i++ ) { m_xVals[i] = xMin + i*xInc; }
    for ( int i = 0; i<(yBins+1); i++ ) { m_yVals[i] = yMin + i*yInc; }
  }
    
  // Adds all series to histogram as passed in through container VectorOfNumericVectors
  void addToHist(const VectorOfNumericVectors<T>& listOfVectors, Alignment alignment ){
    auto list = listOfVectors.getVector();
    for ( auto it = list.begin(); it!=list.end(); ++it ) {
      addToHist(*it, alignment);
    }
  }

  // Adds single series to histogram
  // See enum definition above for description of Alignment types
  void addToHist(const NumericVector<T>& numVector, Alignment alignment) {
    const std::vector<T>& toAddY = numVector.getYConst();
    int i, xInd, yInd;
    switch(alignment){
    case Alignment::Front :
      {
	i = 0;
	for (auto it = toAddY.begin(); it != toAddY.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::Back :
      {
	i = m_matrixCount.shape()[0] - 1;
	for (auto it = toAddY.rbegin(); it != toAddY.rend(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i--;
	}
	break;
      }
    case Alignment::AtMax :
      {
	i = m_matrixCount.shape()[0]/2;
	auto it = std::find(toAddY.begin(), toAddY.end(), numVector.m_maxY);
	auto minIndex = std::distance(toAddY.begin(), it);
	i -= minIndex;
	for (auto it = toAddY.begin(); it != toAddY.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::AtMin :
      {
	i = m_matrixCount.shape()[0]/2;
	auto it = std::find(toAddY.begin(), toAddY.end(), numVector.m_minY);
	auto minIndex = std::distance(toAddY.begin(), it);
	i -= minIndex;
	for (auto it = toAddY.begin(); it != toAddY.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::ByX :
      {
	const std::vector<T>& toAddX = numVector.getXConst();
	for (unsigned int i = 0; i < toAddX.size(); i++) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), toAddX[i]) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), toAddY[i]) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	}
	break;
      }
    default:
      std::cout << "Improper alignment parameter in Hist2D addToHist method" << std::endl;
    }  
  }
  
  void print(std::ostream& os) const {
    int width = m_matrixCount.shape()[0];
    int height = m_matrixCount.shape()[1];
    os << "xbins, ybins, xmin, xmax, ymin, ymax" << std::endl;
    os << m_xBins << ", " << m_yBins << ", " << m_xMin << ", " << m_xMax  << ", " << m_yMin << ", " << m_yMax << std::endl;
    for ( int i = 0; i < width; i++ ) {
      for ( int j = 0; j < (height -1); j++ ) {
	os << " " << m_matrixCount[i][j] << ",  ";
      }
      // Separates out last column of each line to avoid stray ',' at end of each row
      os << " " << m_matrixCount[i][height-1];
      os << std::endl;
    }
  }

  private:
  int m_xBins;
  int m_yBins;
  T m_xMin;
  T m_xMax;
  T m_yMin;
  T m_yMax;
  std::vector<double> m_xVals;
  std::vector<double> m_yVals;
  typedef boost::multi_array<T, 2> array_type;
  array_type m_matrixCount;


  };

#endif // HIST_2D
