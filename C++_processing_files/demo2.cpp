#include <fstream>
#include <memory>
#include <map>
#include <list>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <boost/multi_array.hpp>

#include "NumericVector.h"
#include "VectorOfNumericVectors.h"
#include "Hist2D.h"

template <typename T>
void print_vec ( std::vector<T> vec ){
  for(auto it = vec.begin(); it  != vec.end(); ++it){
    std::cout << *it << std::endl;   
  }  
 }

int main()
{

  std::vector<int> value{1, 2, 3, 4, 3, 5, 1}; //value data ('y' coordinate)
  std::vector<int> time{1, 2, 3, 4, 5, 6, 7}; //time data ('x' coordinate)
  
  NumericVector<int> newVector(value, time);
  newVector.print(std::cout);
  
  auto unique = newVector.getUniqueValues();
  std::cout << "printing unique values in newVector" << std::endl;
  print_vec(unique);
  
  VectorOfNumericVectors<int> BigVec;
  for(int i = 0; i < 3; i++){
    BigVec.AddToVector(newVector);
  }
  std::cout << "printing BigVec" << std::endl;
  BigVec.print(std::cout);
  
  auto a = BigVec.getConcatenated();
  std::cout << "printing concatenated values" << std::endl;
  a.print(std::cout);

  std::cout << "printing sorted concatenated values" << std::endl;
  auto sorted_values = a.getSortedValues();
  print_vec(sorted_values);

  std::cout << "printing unique concatenated values" << std::endl;
  auto unique_concatenated_values = a.getUniqueValues();
  print_vec(unique_concatenated_values);
}
