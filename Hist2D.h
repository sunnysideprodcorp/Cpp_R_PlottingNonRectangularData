#ifndef HIST_2D
#define HIST_2D

#include "ListOfNumericLists.h"
#include "NumericList.h"

#include <ostream>
#include <boost/multi_array.hpp>


/*
Hist2D computes a Boost multi-array two dimensional histogram matrix
Hist2D is intended to be used to create two dimensional histograms of series
For this reason series alignment can be at the start of the serise, the end of the series
the max or min value of the series, or according to the corresponding x value of the series.
Hist2D can be computed on-the-fly with series added one at a time, or with a large number of series added together
*/

template <typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class Hist2D
{
public:
  //histogram can be aligned in a variety of ways
  enum class Alignment {
    Front, Back, AtMax, AtMin, ByX
      };

  //the defaults for Hist2D constructor are based on x varying from 0 to 23 hours and y varying from 0 to 25 (ranking, predetermined range from webscraping parameters)
  Hist2D(int xBins=26, int yBins=28, double xMin = -.1, double xMax = 24, double yMin = -.1, double yMax = 25.1) :
    m_xBins(xBins), m_yBins(yBins),  m_xMin(xMin), m_xMax(xMax), m_yMin(yMin), m_yMax(yMax), m_xVals(xBins+1), m_yVals(yBins+1),  m_matrixCount(boost::extents[yBins][xBins])
  {
    //all bins start counting from zero
    std::fill(m_matrixCount.origin(), m_matrixCount.origin() + m_matrixCount.size(), 0);

    //calculate binning increments for x and y to include all values between specified min and max, binned in equally sized and uniformly distributed ranges
    double xInc = (xMax - xMin)/xBins;
    double yInc = (yMax - yMin)/yBins;
    for(int i = 0; i<(xBins+1); i++){ m_xVals[i] = xMin + i*xInc; }
    for(int i = 0; i<(yBins+1); i++){ m_yVals[i] = yMin + i*yInc; }
  }
    
  //adds many series to histogram passed in through a container class, ListOfNumericLists
  void addToHist(const ListOfNumericLists<T>& listOfLists, Alignment alignment ){
    auto list = listOfLists.getList();
    for(auto it = list.begin(); it!=list.end(); ++it){
      addToHist(*it, alignment);
    }
  }

  //adds single series to histogram
  void addToHist(const NumericList<T>& numList, Alignment alignment) {
    const std::vector<T>& toAdd = numList.getYConst();
    int i, xInd, yInd;
    switch(alignment){
    case Alignment::Front :
      {
        //use this method to align all series at their beginning
        //this means the 'x' value will be ignored
	i = 0;
	for (auto it = toAdd.begin(); it != toAdd.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::Back :
      {
        //use this method to align all series at their end
        //this means the 'x' value will be ignored
	i = m_matrixCount.shape()[0] - 1;
	for (auto it = toAdd.rbegin(); it != toAdd.rend(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i--;
	}
	break;
      }
    case Alignment::AtMax :
      {
        //use this method to align all series at their 'max' (here it is the 'min' because rank starts low and goes high)
        //this means the 'x' value will be ignored
	i = m_matrixCount.shape()[0]/2;
	auto it = std::find(toAdd.begin(), toAdd.end(), numList.m_maxVal);
	auto minIndex = std::distance(toAdd.begin(), it);
	i -= minIndex;
	for (auto it = toAdd.begin(); it != toAdd.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::AtMin :
      {
        //use this method to align all series at their 'max' (here it is the 'min' because rank starts low and goes high)
        //this means the 'x' value will be ignored
	i = m_matrixCount.shape()[0]/2;
	auto it = std::find(toAdd.begin(), toAdd.end(), numList.m_minVal);
	auto minIndex = std::distance(toAdd.begin(), it);
	i -= minIndex;
	for (auto it = toAdd.begin(); it != toAdd.end(); ++it) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), i) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), *it) - m_yVals.begin() - 1;
	  m_matrixCount[yInd][xInd] +=1;
	  i++;
	}
	break;
      }
    case Alignment::ByX :
      {
        //use this method to align all series according to their 'x' value
        //'x' is not ignored in this alignment method
	const std::vector<T>& toAddX = numList.getXConst();
	for (unsigned int i = 0; i < toAddX.size(); i++) {
	  xInd = std::lower_bound(m_xVals.begin(), m_xVals.end(), toAddX[i]) - m_xVals.begin() - 1;
 	  yInd = std::lower_bound(m_yVals.begin(), m_yVals.end(), toAdd[i]) - m_yVals.begin() - 1;
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
  for(int i = 0; i < width; i++){
    for(int j = 0; j < (height -1); j++){
      os << " " << m_matrixCount[i][j] << ",  ";
    }
    //separate out last column of each line to avoid stray ','
    os << " " << m_matrixCount[i][height-1];
    os << std::endl;
  }
}

private:
  typedef boost::multi_array<T, 2> array_type;
  int m_xBins;
  int m_yBins;
  double m_xMin;
  double m_xMax;
  double m_yMin;
  double m_yMax;
  std::vector<double> m_xVals;
  std::vector<double> m_yVals;
  array_type m_matrixCount;

};

#endif // HIST_2D
