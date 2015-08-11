#ifndef NUMERIC_LISTS
#define NUMERIC_LISTS


#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

/*
This class holds a two-dimensional series, so can be used to hold time series, value vs. displacement series, etc. Class has been designed to be static, once contsructed not further modifications to the values. This facilitates lazy computation of relevant statistics. 
 */

template <typename T,
typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class NumericList
{
 public:
  T m_maxVal;
  T m_minVal;
  T m_maxX;
  T m_minX;
  int length;

  //constructor with two parameters takes a "y" value and "x" value to record a series and its "displacement" value
  //can be used to represent a time series or value vs. displacement, etc
 NumericList(const std::vector<T>& newY, const std::vector<T>& newX)
   : m_yAxis(newY), m_xAxis(newX)
  {
    m_maxVal = *max_element(m_yAxis.begin(), m_yAxis.end());
    m_minVal = *min_element(m_yAxis.begin(), m_yAxis.end());
    m_maxX   = *max_element(m_xAxis.begin(), m_xAxis.end());
    m_minX   = *min_element(m_xAxis.begin(), m_xAxis.end());
    length   = m_yAxis.size();
  }
  
  const std::vector <T>&  getYConst() const { return this->m_yAxis;}
  const std::vector <T>&  getXConst() const { return this->m_xAxis;}
  const std::vector <T>&  getNumUniqueConst() 
  {
    if(!uniqueComputed){
      computeUnique();
    }
    return m_uniqueNumbers;
  }
  int getUniqueCount() 
  {
    if(!uniqueComputed){
      computeUnique();
    }
    int diversity = (int) m_uniqueNumbers.size();
    return diversity; 
  }
  double getMeanUnique()
  {
    if(!uniqueComputed){
      computeUnique();
    }
    return  calcMean(false);
  }
  double getSDUnique() {
    if(!uniqueComputed){
      computeUnique();
    }
    return calcSD(false);
  }
  double getMean() const { return calcMean(true);}
  double getSD() const { return calcSD(true);}

  //count the number of times the direction of the series changes (increasing to decreasing or vice versa)
  //vased only on the value and its position in the value array, not based on the 'x' values
  int getInflectionCount(){
    if(!inflectionComputed){
      m_inflectionCount = 0;
      std::vector<T> m_inflection(m_yAxis.size() - 1);
      std::adjacent_difference(++m_yAxis.begin(), m_yAxis.end(), m_inflection.begin());
      for(unsigned int i = 1; i<m_inflection.size(); i++){
	if( (i > 0) && ((m_inflection[i] <0) != (m_inflection[i-1]<0)))
	  m_inflectionCount += 1;
      }      
      inflectionComputed = true;
    }
    return m_inflectionCount;
  }

  
  //return a summary vector indicating: 0. length of series, 1. min yAxis value, 2. max yAxis value, 3. min xAxis value, 4. max xAxis value
  //5. inflection count, 6. number of unique levels, 7. mean of series, 8. sd of series
  const std::vector<double> getAllData(){
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

  void print(std::ostream& os, const NumericList<T>& numList) const {
    std::cout << "Y values" << std::endl;
    auto yVec = numList.getYConst();
    printVec(os, yVec);
    std::cout << "X values" << std::endl;
    auto xVec = numList.getXConst();
    printVec(os, xVec);

  }
  
 private:
  std::vector<T> m_yAxis = { };
  std::vector<T> m_xAxis = { };
  std::vector<T> m_uniqueNumbers = { };
  std::vector<T> m_inflection = { };
  int m_inflectionCount;
  bool uniqueComputed = false;
  bool inflectionComputed = false;

  void printVec(std::ostream& os, const std::vector<T>& v) {
    for(int i = 0; i<(v.size()-1); i++){
      os << v[i] << ", ";
    }
    os << v[v.size()-1];
    os << std::endl;
  }

  //computes unique values within series
  //no precautionary steps are taken to deal with numeric error
  //so this method should only be used for int types
  void computeUnique(bool useDefault = true){   
    m_uniqueNumbers.assign(m_yAxis.begin(), m_yAxis.end());
    auto new_end = std::unique(m_uniqueNumbers.begin(), m_uniqueNumbers.end());
    m_uniqueNumbers.erase(new_end, m_uniqueNumbers.end());
  }

  //calculates the mean, of the series if useDefaul=true
  //if false, of the unique values in the series
  double calcMean(bool useDefault = true) const {
    //calculation for series
    if(useDefault){
      if (!m_yAxis.empty()) {
	double sum  = std::accumulate(m_yAxis.begin(), m_yAxis.end(), 0.0);
	return  sum / m_yAxis.size();
      } else {
	return -999.0;    }
    }
    //calculation for unique values in series
    else
      {
	if (!m_uniqueNumbers.empty()) {
	  double sum  = std::accumulate(m_uniqueNumbers.begin(), m_uniqueNumbers.end(), 0.0);
	  return  sum / m_uniqueNumbers.size();
	} else {
	  return -999.0;    }	  
      }
  }

  //calculates the standard deviation of the series  if useDefault=true
  //if false, of the unique values in the series
  double calcSD(bool useDefault = true) const {
    //calculation for all values in series
    if(useDefault){      
      if (!m_yAxis.empty()) {
	double mean = calcMean(useDefault);
	double square_sum = 0.0;
	for(unsigned int i = 0; i < m_yAxis.size(); i++){
	  square_sum += (m_yAxis[i] - mean) * (m_yAxis[i] - mean);
	  
	}
	square_sum /= m_yAxis.size();	
	return square_sum;
      } else {
	return -999.0;
      }
    }
    //calculation for unique values in series
    else
      {
	if (!m_uniqueNumbers.empty()) {
	  double square_sum = std::inner_product(m_uniqueNumbers.begin(), m_uniqueNumbers.end(), m_uniqueNumbers.begin(), 0.0);
	  double mean = calcMean(useDefault);
	  return std::sqrt(square_sum/m_uniqueNumbers.size() - mean*mean);
	} else {
	  return -999.0;
	}
      }
  }
};

#endif // NUMERIC_LISTS
