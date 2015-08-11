#ifndef LIST_NUMERIC_LISTS
#define LIST_NUMERIC_LISTS

#include "NumericList.h"

/*
This class holds a vector of NumericLists so that summary statistics can be computed across a set of series
rather than across an individual series. The max and min values of the x and y values of all series
taken together are tracked. Also the class computes and returns a concatenated single vector putting together
the entire series for cases when the user wants to perform additional custom summary statitics on the set as a whole
in a convenient container. 
Best to run Concatenate() only when list is complete.
 */

//note to self: consider adding a check on T like so:
//http://stackoverflow.com/questions/874298/c-templates-that-accept-only-certain-types 

template <typename T,
typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class ListOfNumericLists
{
 public:
 ListOfNumericLists()
   : m_wasConcatenated(false), m_statsCurrent(true)
    {}
 ListOfNumericLists(const std::vector<NumericList<T>>& list)
   : m_list(list), m_wasConcatenated(false), m_statsCurrent(false)
    {}

  //each time a NumericList is added to the ListOfNumericLists update aggregate measurements
  void AddToList(const NumericList<T>& list){
    m_list.push_back(list);
    m_maxLength = std::max(m_maxLength, list.length);
    m_minLength = std::min(m_minLength, list.length);
    m_maxVal = std::max(m_maxVal, list.m_maxVal);
    m_minVal = std::min(m_minVal, list.m_minVal);
    m_maxX = std::max(m_maxVal, list.m_maxX);
    m_minX = std::min(m_minVal, list.m_minX);    
    if(m_wasConcatenated){
      m_wasConcatenated = false;
      Concatenate();
    }
  }
  //concatenate values of all NumericLists
  void Concatenate(){
    if(!m_wasConcatenated){
      std::vector<T> toConcatY, toConcatX;
      std::vector<T> yHolder, xHolder;
      if(m_statsCurrent){
	for( auto it = m_list.begin(); it != m_list.end(); it++){
	  yHolder = (*it).getNumConst();
	  xHolder   = (*it).getXConst();
	  toConcatY.insert(toConcatY.end(), yHolder.begin(), yHolder.end());
	  toConcatX.insert(toConcatX.end(), xHolder.begin(), xHolder.end());
	}
      } else {
	for( auto it = m_list.begin(); it != m_list.end(); it++){
	  yHolder = (*it).getNumConst();
	  xHolder   = (*it).getXConst();
	  toConcatY.insert(toConcatY.end(), yHolder.begin(), yHolder.end());
	  toConcatX.insert(toConcatX.end(), xHolder.begin(), xHolder.end());
	  m_maxLength = std::max(m_maxLength, yHolder.length);
	  m_minLength = std::min(m_minLength, yHolder.length);
	}
      }
      m_concatenatedList = NumericList<T>(toConcatY, toConcatX);
    } 
  }

  const NumericList<T> getConcatenated() const{
    if(!m_wasConcatenated){ Concatenate(); }
    return m_concatenatedList;
  }
  const std::vector<double> getSummaryVals() {
    if(!m_statsCurrent) {
      //concatenate gets max and min length
      //also since concatenate creates a new NumericList, all other
      //computations are in that list for the concatenated series
      Concatenate();
      m_maxVal = m_concatenatedList.m_maxVal;
      m_minVal = m_concatenatedList.m_minVal;
      m_maxX   = m_concatenatedList.m_maxX;
      m_minX   = m_concatenatedList.m_minX;
    }
    return std::vector<double> {m_maxLength, m_minLength, m_maxVal, m_minVal, m_maxX, m_minX};
  }
  
  void print(std::ostream& os) const {
    for (auto it = m_list.begin(); it != m_list.end(); it++){
      (*it).print(os);
    }
  }
  
 private:
  std::vector<NumericList<T>> m_list;
  NumericList<T> m_concatenatedList;
  bool m_wasConcatenated; 
  bool m_statsCurrent;
  int  m_maxLength;
  int  m_minLength;
  T    m_maxVal;
  T    m_minVal;
  T    m_maxX;
  T    m_minX; 
};

#endif // LIST_NUMERIC_LISTS
