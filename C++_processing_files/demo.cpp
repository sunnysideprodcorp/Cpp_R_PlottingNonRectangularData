// to do: remove unnecessary includes
#include <map>
#include <iostream>
#include <fstream>
#include <string>



#include "NumericVector.h"
#include "VectorOfNumericVectors.h"
#include "Hist2D.h"

const std::string FILENAME_COMPONENT = "_front_";

template <typename T>
void print(std::ostream& os, const std::vector<T>& v) {
  std::copy(v.begin(), v.end(),
	    std::ostream_iterator<T>(os, ","));
  os << std::endl;
}

// Specific to the data file of Reddit rankings
// Returns the UTC hour corresponding to input timestamp input as time since epoch
int getHour(int timestamp){
  time_t t = time_t(timestamp);
  struct tm *tm = gmtime(&t);
  char date[3];
  strftime(date, sizeof(date), "%H", tm);
  int result = std::atoi(date);
  result = (result + 5) % 24;
  return result;
}

// make_unique defined manually because not available from compiler
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

int main()
{

  std::ifstream infile("../data/data.tsv");
  std::string line;
  bool first(true);
  std::vector<std::vector<int> > vec;

  std::map<std::string, std::unique_ptr<Hist2D<int>>> SubjectList;
  std::map<std::string, std::vector<std::vector<double>>> SubjectTraits;
  int column_counter = 0;

  while ( std::getline(infile, line) ) {
      std::vector<int> subvec;
      std::string i, letter;
      std::istringstream iss(line);
      std::vector<int> stringvec1;
      std::vector<int> stringvec2;
      std::string keyString;

      // Reads in info from data.tsv, where useful data is in columns
      // 2 (reddit thread name), 3 (timestamp),
      // 4 (ranks recorded for a given thread), 5 (time ranks recorded)
      while (iss >> i) {
	if ( column_counter == 2 ) {    // Column 2 contains name of subreddit to which a particular thread (row) belongs
	    keyString = i;
	    if ( !SubjectList.count(keyString) ) {
	      SubjectList[keyString] = make_unique<Hist2D<int>>(15, 25, -.1, 15.1);
	       SubjectTraits[keyString] = std::vector<std::vector<double>>(0);			   
	    }
	  }
	  column_counter ++;
	  // Two columns of interest start with "[" because they contain a list of values within the column
	  if ( i.at(0) == '[' && first) {
	    stringvec1.clear();
	    if ( i.back() == ']' ) {	    
	      stringvec1.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      first = false;
	    } else {
	      stringvec1.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      iss >> i;	
	      while ( i.back() != ']' ) {
		stringvec1.push_back(std::atoi(i.c_str()));
		iss >> i;	
	      }
	      stringvec1.push_back(std::atoi(i.substr(0, i.length()-1).c_str()));
	      first = false;
	    }
	  } else if ( i.at(0) == '[' && !first ) {
	    stringvec2.clear();
	    if ( i.back() == ']' ) {	    
	      stringvec2.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      first = true;
	    } else {
	      stringvec2.push_back(std::atoi(i.substr(1, i.length() - 1).c_str()));
	      iss >> i;	
	      while ( i.back() != ']' ) {
		stringvec2.push_back(std::atoi(i.c_str()));
		iss >> i;	
	      }
	      stringvec2.push_back(std::atoi(i.substr(0, i.length()-1).c_str()));
	      first = true;
	    }
	    std::transform(stringvec2.begin(), stringvec2.end(), stringvec2.begin(), getHour);

	    // This shows how you can instantiate a NumericVector
	    // Add its traits to a vector of vectors of doubles
	    // and add its value to the corresponding 2d histogram
	    NumericVector<int> newVector(stringvec1, stringvec2);
	    std::vector<double> traitsVector = newVector.getAllData();
	    SubjectTraits[keyString].push_back(traitsVector);
	    SubjectList[keyString]->addToHist(newVector, Hist2D<int>::Alignment::Front);
	    first = true;
	  }	  
	}
      column_counter = 0; // Reset column counter before moving to next row of file
    }

  std::string filename;
  std::ofstream myfile;

  // Saves each histogram to a text file named for corresponding subreddit 
  for ( const auto &subject: SubjectList ) {
    filename = "../data/" + subject.first + FILENAME_COMPONENT + ".txt";
    myfile.open (filename);
    subject.second->print(myfile);
    myfile.close();      
  }

  // For each Reddit thread in a given subreddit, saves the traits of that thread
  for ( const auto &traits: SubjectTraits ) {
    filename = "../data/" + traits.first + "_traits_.csv";
    myfile.open (filename);      
      for ( auto it = traits.second.begin(); it!=traits.second.end(); ++it ) {
	print(myfile, *it);
      }
      myfile.close();
  }
}
 
