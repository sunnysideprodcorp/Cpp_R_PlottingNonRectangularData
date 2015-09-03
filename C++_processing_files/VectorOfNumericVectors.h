#ifndef VECTOR_NUMERIC_VECTORS
#define VECTOR_NUMERIC_VECTORS

#include "NumericVector.h"

/*
This class holds a vector of NumericVectors so that summary statistics can be computed across a set of series
rather than across an individual series. The max and min values of the x and y values of all series
taken together are tracked. Also the class computes and returns a concatenated single vector putting together
the entire series for cases when the user wants to perform additional custom summary statitics on the set as a whole
in a convenient container. 
Best to run Concatenate() only when list is complete.
 */

template <typename T,
typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class VectorOfNumericVectors
{
 public:
 VectorOfNumericVectors()
   : m_wasConcatenated(false), m_statsCurrent(false)
    {}
 VectorOfNumericVectors(const std::vector<NumericVector<T>>& vec)
   : m_vec(vec), m_wasConcatenated(false), m_statsCurrent(false)
    {}

  //each time a NumericVector is added to the VectorOfNumericVectors update aggregate measurements
  void AddToVector(const NumericVector<T>& list){
    m_vec.push_back(list);
    m_maxLength = std::max(m_maxLength, list.length);
    m_minLength = std::min(m_minLength, list.length);
    m_maxY = std::max(m_maxY, list.m_maxY);
    m_minY = std::min(m_minY, list.m_minY);
    m_maxX = std::max(m_maxY, list.m_maxX);
    m_minX = std::min(m_minY, list.m_minX);    

    if ( m_wasConcatenated ) {
      std::vector<T> newX, newY;
      
      newX.assign(m_concatenatedVector.getXConst().begin(), m_concatenatedVector.getXConst().end());
      newX.insert(newX.end(), list.getXConst().begin(), list.getXConst().end());

      newY.assign(m_concatenatedVector.getYConst().begin(), m_concatenatedVector.getYConst().end());
      newY.insert(newY.end(), list.getYConst().begin(), list.getYConst().end());
      
      m_concatenatedVector = NumericVector<T>(newY, newX);
      }
  }
 
  //concatenate values of all NumericVectors
  void Concatenate(){
    if ( !m_wasConcatenated ) {
      std::vector<T> toConcatY, toConcatX;
      std::vector<T> yHolder, xHolder;
      if ( m_statsCurrent ) {
	for ( auto it = m_vec.begin(); it != m_vec.end(); it++){
	  yHolder = (*it).getYConst();
	  xHolder   = (*it).getXConst();
	  toConcatY.insert(toConcatY.end(), yHolder.begin(), yHolder.end());
	  toConcatX.insert(toConcatX.end(), xHolder.begin(), xHolder.end());
	}
      } else {
	for ( auto it = m_vec.begin(); it != m_vec.end(); it++ ) {
	  yHolder = (*it).getYConst();
	  xHolder   = (*it).getXConst();
	  toConcatY.insert(toConcatY.end(), yHolder.begin(), yHolder.end());
	  toConcatX.insert(toConcatX.end(), xHolder.begin(), xHolder.end());
	  int new_length = yHolder.size();
	  m_maxLength = std::max(m_maxLength, new_length);
	  m_minLength = std::min(m_minLength, new_length);
	}
      }
      m_concatenatedVector = NumericVector<T>(toConcatY, toConcatX);
    } 
  }

  const NumericVector<T> getConcatenated() {
    if ( !m_wasConcatenated ) Concatenate(); 
    return m_concatenatedVector;
  }
  const std::vector<double> getSummaryVals() {
    if ( !m_statsCurrent ) {
      Concatenate();
      m_maxY = m_concatenatedVector.m_maxY;
      m_minY = m_concatenatedVector.m_minY;
      m_maxX   = m_concatenatedVector.m_maxX;
      m_minX   = m_concatenatedVector.m_minX;
    }
    return std::vector<double> {m_maxLength, m_minLength, m_maxY, m_minY, m_maxX, m_minX};
  }
  
  void print(std::ostream& os) const {
    for ( auto it = m_vec.begin(); it != m_vec.end(); it++ ) {
      (*it).print(os);
    }
  }
  
 private:
  std::vector<NumericVector<T>> m_vec;
  NumericVector<T> m_concatenatedVector;
  bool m_wasConcatenated; 
  bool m_statsCurrent;
  int  m_maxLength;
  int  m_minLength;
  T    m_maxY;
  T    m_minY;
  T    m_maxX;
  T    m_minX; 
};

#endif // VECTOR_NUMERIC_VECTORS
