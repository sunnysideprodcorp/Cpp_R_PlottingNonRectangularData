#ifndef NUMERIC_VECTOR
#define NUMERIC_VECTOR

#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

// NumericVector is designed to hold one-dimensional series. For example, it can hold time series, value vs. distance, paired data where the x-values are numerically related to one another,  etc. 
// NumericVector is best used to hold data that will be fully loaded before any analysis is done. This facilitates lazy computation of relevant statistics. 
// NumericVector can be templated to any arithmetic type.  
template <typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  class NumericVector
  {
  public:
  T m_maxY;
  T m_minY;
  T m_maxX;
  T m_minX;
  int length;

  //Default constructor
  NumericVector() {}
  
  // NumericVector requires two std::vectors of type T for allocation and initialization.
  // Some relevant attributes of the two vectors are immediately computed upon class initialization
  NumericVector(const std::vector<T>& newY, const std::vector<T>& newX)
  : m_yAxis(newY), m_xAxis(newX)
  {
    m_maxY = *max_element(m_yAxis.begin(), m_yAxis.end());
    m_minY = *min_element(m_yAxis.begin(), m_yAxis.end());
    m_maxX   = *max_element(m_xAxis.begin(), m_xAxis.end());
    m_minX   = *min_element(m_xAxis.begin(), m_xAxis.end());
    length   = m_yAxis.size();
  }

  // Retrieves constant references to the x or y vectors stored in NumericVector 
  const std::vector <T>&  getYConst() const { return this->m_yAxis;}
  const std::vector <T>&  getXConst() const { return this->m_xAxis;}

  // Summary statistics for data stored in m_yAxis
  double getMean() const { return calcMean(true);}
  double getSD() const { return calcSD(true);}

  const std::vector <T>&  getSortedValues() 
  {
    if ( !m_sortedComputed ) {
      computeSorted();
    }
    return m_sortedNumbers;
  }
  
  // Retrieves constant reference to a vector containing a single copy of each
  // unique values stored in m_yAxis. Probably has little utility when T is not int.
  // Has not been tested with non-int T class types
  const std::vector <T>&  getUniqueValues() 
  {
    if ( !m_uniqueComputed ) {
      computeUnique();
    }
    return m_uniqueNumbers;
  }

  // Returns the number of unique values stored in m_yAxis
  //  Probably has little utility when T is not int
  int getUniqueCount()
  {
    if ( !m_uniqueComputed ) {
      computeUnique();
    }
    return (int) m_uniqueNumbers.size();
  }
 
  double getMeanUnique()
  {
    if ( !m_uniqueComputed ) {
      computeUnique();
    }
    return  calcMean(false);
  }
 
  double getSDUnique() {
    if ( !m_uniqueComputed ) {
      computeUnique();
    }
    return calcSD(false);
  }
 
  // Counts the number of times the direction of the series changes
  // (increasing to decreasing or vice versa)
  // based only on the value and its position in the m_yAxis vector
  int getInflectionCount(){
    if ( !m_inflectionComputed ) {
      m_inflectionCount = 0;
      std::vector<T> m_inflection(m_yAxis.size() - 1);
      std::adjacent_difference(++m_yAxis.begin(), m_yAxis.end(), m_inflection.begin());
      for ( unsigned int i = 1; i<m_inflection.size(); i++ ) {
	if ( (i > 0) && ((m_inflection[i] <0) != (m_inflection[i-1]<0)))
	  m_inflectionCount += 1;
      }      
      m_inflectionComputed = true;
    }
    return m_inflectionCount;
  }

  
  //return a summary vector indicating:
  // 0. length of series, 1. min yAxis value, 2. max yAxis value, 3. min xAxis value,
  // 4. max xAxis value, 5. inflection count, 6. number of unique levels,
  // 7. mean of series, 8. sd of series
  const std::vector<double> getAllData(){   // Note that doubles are used regardless of templating type, and code does not safeguard against potential casting errors (e.g. going from long int -> double)
    std::vector<double> result(9);
    result[0]          = m_yAxis.size();   

    auto min_max_value = std::minmax_element(m_yAxis.begin(), m_yAxis.end());    
    result[1]          = *(min_max_value.first); 
    result[2]          = *(min_max_value.second);

    min_max_value      = std::minmax_element(m_xAxis.begin(), m_xAxis.end());
    result[3]          = *(min_max_value.first);
    result[4]          = *(min_max_value.second);

    result[5]          = getInflectionCount();
    result[6]          = getUniqueCount();

    result[7]          = calcMean();
    result[8]          = calcSD();

    return result;
  }

  //  void print(std::ostream& os, const NumericVector<T>& numList) const {
  void print(std::ostream& os) const {
    os << "Y values" << std::endl;
    printVec(os, m_yAxis);
    os << "X values" << std::endl;
    printVec(os, m_xAxis);
  }
  
  private:
  std::vector<T> m_yAxis = { };
  std::vector<T> m_xAxis = { };
  std::vector<T> m_uniqueNumbers = { };
  std::vector<T> m_sortedNumbers = { };
  std::vector<T> m_inflection = { };
  int m_inflectionCount;
  bool m_uniqueComputed = false;
  bool m_sortedComputed = false;
  bool m_inflectionComputed = false;

  void printVec(std::ostream& os, const std::vector<T>& v) const {
    for ( int i = 0; i<(v.size()-1); i++ ) {
      os << v[i] << ", ";
    }
    os << v[v.size()-1];
    os << std::endl;
  }


  // Computes ordered vector of all yAxis values
  void computeSorted() {   
      m_sortedNumbers.assign(m_yAxis.begin(), m_yAxis.end());
      std::sort(m_sortedNumbers.begin(), m_sortedNumbers.end());
  }

  
  // Computes unique values within series
  // No precautionary steps are taken to deal with numeric error
  // so this method should robably be restructed to int types
  // but such restriction is not forced in the code
  void computeUnique(bool useDefault = true){   
    if ( !m_sortedComputed ) {
      computeSorted();
    }
    m_uniqueNumbers.assign(m_sortedNumbers.begin(), m_sortedNumbers.end());
    auto new_end = std::unique(m_uniqueNumbers.begin(), m_uniqueNumbers.end());
    m_uniqueNumbers.erase(new_end, m_uniqueNumbers.end());
  }

  // Calculates the mean of m_yAxis if useDefault is true
  // If useDefault is false, calculates the mean of m_yAxis's 
  // unique values not weighted by frequency
  double calcMean(bool useDefault = true) const {
    if ( useDefault ) {                        //calculation for m_yAxis
      if ( !m_yAxis.empty() ) {
	double sum  = std::accumulate(m_yAxis.begin(), m_yAxis.end(), 0.0);
	return  sum / m_yAxis.size();
      } else return -999.0;
    } else                                  //calculation for unique values of m_yAxis
      {
	if ( !m_uniqueNumbers.empty() ) {
	  double sum  = std::accumulate(m_uniqueNumbers.begin(), m_uniqueNumbers.end(), 0.0);
	  return  sum / m_uniqueNumbers.size();
	} else return -999.0;	  
      }
  }

  // Calculates the standard deviation of m_yAxis if useDefault is true
  // If useDefault is false, calculates the mean of m_yAxis's 
  // unique values not weighted by frequency
  double calcSD(bool useDefault = true) const {
    if ( useDefault ) {      
      if ( !m_yAxis.empty() ) {  //calculation for m_yAxis
	double mean = calcMean(useDefault);
	double square_sum = 0.0;
	for ( unsigned int i = 0; i < m_yAxis.size(); i++ ) {
	  square_sum += (m_yAxis[i] - mean) * (m_yAxis[i] - mean);
	  
	}
	square_sum /= m_yAxis.size();	
	return square_sum;
      } else {
	return -999.0;
      }
    } else                                 //calculation for unique values of m_yAxis
      {
	if ( !m_uniqueNumbers.empty() ) {
	  double square_sum = std::inner_product(m_uniqueNumbers.begin(), m_uniqueNumbers.end(), m_uniqueNumbers.begin(), 0.0);
	  double mean = calcMean(useDefault);
	  return std::sqrt(square_sum/m_uniqueNumbers.size() - mean*mean);
	} else {
	  return -999.0;
	}
      }
  }
  };

#endif // NUMERIC_VECTOR
