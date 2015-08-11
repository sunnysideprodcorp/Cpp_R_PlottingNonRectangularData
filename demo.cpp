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

#include "NumericList.h"
#include "ListOfNumericLists.h"
#include "Hist2D.h"

template <typename T>
void print2(std::ostream& os, const std::vector<T>& v) {
  std::copy(v.begin(), v.end(),
	    std::ostream_iterator<T>(os, ","));
  os << std::endl;
}


int getHour(int timestamp){
  time_t t = time_t(timestamp);
  struct tm *tm = gmtime(&t);
  char date[3];
  strftime(date, sizeof(date), "%H", tm);
  int result = std::atoi(date);
  result = (result + 5) % 24;
  return result;
}


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

int main()
{

  std::ifstream infile("data.tsv");
  
  std::string output;
  std::string line;
  bool first(true);
  std::vector<std::vector<int> > vec;


  std::map<std::string, std::unique_ptr<Hist2D<int>>> SubjectList;
  std::map<std::string, std::vector<std::vector<double>>> SubjectTraits;
  int column_counter = 0;
  while(std::getline(infile, line))
    {
      std::vector<int> subvec;
      std::string i, letter;
      std::istringstream iss(line);
      std::vector<int> stringvec1;
      std::vector<int> stringvec2;
      std::string keyString;

      while(iss >> i)
	{
	  if(column_counter == 2){
	    keyString = i;
	    if(!SubjectList.count(keyString)){
	       SubjectList[keyString] = make_unique<Hist2D<int>>(12, 12);
	       SubjectTraits[keyString] = std::vector<std::vector<double>>(0);			   
	    }
	  }
	  column_counter ++;
	  
	  if(  i.at(0) == '[' && first){
	    stringvec1.clear();
	    if(i.back() == ']') {	    
	      stringvec1.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      first = false;
	    }else{
	      stringvec1.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      iss >> i;	
	      while ( i.back() != ']'){
		stringvec1.push_back(std::atoi(i.c_str()));
		iss >> i;	
	      }
	      stringvec1.push_back(std::atoi(i.substr(0, i.length()-1).c_str()));
	      first = false;
	    }
	  }
	  else if(  i.at(0) == '[' && !first){
	    stringvec2.clear();
	    if(i.back() == ']') {	    
	      stringvec2.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      first = true;
	    }else{
	      stringvec2.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      iss >> i;	
	      while ( i.back() != ']'){
		stringvec2.push_back(std::atoi(i.c_str()));
		iss >> i;	
	      }
	      stringvec2.push_back(std::atoi(i.substr(0, i.length()-1).c_str()));
	      first = true;
	    }
	    std::transform(stringvec2.begin(), stringvec2.end(), stringvec2.begin(), getHour);
	    NumericList<int> newList(stringvec1, stringvec2);
	    std::vector<double> traitsList = newList.getAllData();
	    SubjectTraits[keyString].push_back(traitsList);
	    SubjectList[keyString]->addToHist(newList, Hist2D<int>::Alignment::ByX);
	    first = true;
	  }	  
	}
      column_counter = 0;
    }

  std::string filename;
  std::ofstream myfile;
  for(const auto &subject: SubjectList){
    filename =  subject.first + "_byX_"  +".txt";
    myfile.open (filename);
    subject.second->print(myfile);
    myfile.close();      
  }

  for(const auto &traits: SubjectTraits){
    filename = traits.first + "_traits_.csv";
    myfile.open (filename);      
      for(auto it = traits.second.begin(); it!=traits.second.end(); ++it){
	print2(myfile, *it);
      }
      myfile.close();
  }
}
 
