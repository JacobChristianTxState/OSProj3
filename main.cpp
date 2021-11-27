/* Created by Jacob Chrisitan and Levi Jacoby */

#include <iostream>
#include <sstream>
#include <random>
#include <deque>
#include <algorithm>
#include "ResultsLogger.h"

constexpr int MIN = 0;
constexpr int MAX = 49;
constexpr int RAND_NUMS_TO_GENERATE = 100;


std::deque<int> generatePageReferenceStringVector(int i){
  //Generates pseudo random numbers based on the
  //Mersenne Twister pseudo-random generator of 32-bit numbers with a state size of 19937 bits.
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<int> distr(MIN, MAX);
  std::deque<int> pageReferenceStringVector;
  
  //for logging results of the random page values produced
  std::stringstream ss;
  std::string fileName = "page-reference-vector-used-" + std::to_string(i) + ".txt";
  ResultsLogger pageReferenceRunVector(fileName);
  pageReferenceRunVector.OpenFile();
  pageReferenceRunVector.WriteToFile("The following vector was used for the currently saved run: ");
  
  for (int i = 0; i < RAND_NUMS_TO_GENERATE; i++) {
    int randomPageValue = distr(eng);
    pageReferenceStringVector.push_back(randomPageValue);
    ss << std::to_string(randomPageValue) << " ";
  }
  pageReferenceRunVector.WriteToFile(ss.str());
  pageReferenceRunVector.CloseFile();
  return pageReferenceStringVector;
}

bool pageFoundInCollection(const std::deque<int> frame, const int pageReference) {
  //Returns if element is found in deque collection
  return std::find(frame.begin(), frame.end(), pageReference) != frame.end();
}

unsigned int runFIFOPageReplacement(const size_t size, std::deque<int> pageReferenceString) {
  std::deque<int> frames;
  unsigned int numberOfFaults = 0;
  
  //Get the first pages to fully fill frame list
  while(frames.size() < size && !pageReferenceString.empty()) {
    if (!pageFoundInCollection(frames, pageReferenceString.front())) {
      frames.push_front(pageReferenceString.front());
      numberOfFaults++;
    }
    pageReferenceString.pop_front();
  }
  
  //While we have pages to schedule
  while(!pageReferenceString.empty()) {
    //If page is not found in the current frame list
    if (!pageFoundInCollection(frames, pageReferenceString.front())) {
      //Push the current element to the front, pop the oldest element in the back
      frames.push_front(pageReferenceString.front());
      frames.pop_back();
      numberOfFaults++;
    }
    pageReferenceString.pop_front();
  }
  return numberOfFaults;
}

unsigned int runLRUPageReplacement(const size_t size, std::deque<int> pageReferenceString) {
  //Using stack implementation vs counting implementation
  std::deque<int> frames;
  std::deque<int> lruStack;
  size_t numberOfFaults = 0;
  
  //Get the first pages to fully fill frame list
  while(frames.size() < size && !pageReferenceString.empty()) {
    int page = pageReferenceString.front();
    //if not found in collection
    if (!pageFoundInCollection(frames, page)) {
      //push frame to front of the stack/deque
      frames.push_back(page);
      lruStack.push_front(page);
      numberOfFaults++;
    } else {
      //Find the element within the stack, pop it out, and place it in the back again
      auto itStack = std::find(lruStack.begin(), lruStack.end(), page);
      int page = *itStack;
      lruStack.erase(itStack);
      lruStack.push_front(page);
    }
    pageReferenceString.pop_front();
  }
  
  //After filling initial page faults, iterate through the rest of the list and replace via LRU algorithm
  while (!pageReferenceString.empty()) {
    if (!pageFoundInCollection(frames, pageReferenceString.front())) {
      numberOfFaults++;
      
      int lru = lruStack.back();
      lruStack.pop_back();
      auto it = std::find(frames.begin(), frames.end(), lru);
      frames[it - frames.begin()] = pageReferenceString.front();
      
      if (!pageFoundInCollection(lruStack, pageReferenceString.front())) { 
        lruStack.push_front(pageReferenceString.front());
      }
    } else {
      auto it = std::find(lruStack.begin(), lruStack.end(), pageReferenceString.front());
      int page = *it;
      lruStack.erase(it);
      lruStack.push_front(page);
      }
    pageReferenceString.pop_front();
  }
  return numberOfFaults;
}

unsigned int runOPTPageReplacement(const size_t size, std::deque<int> pageReferenceString) {
  std::deque<int> frames;
  unsigned int numberOfFaults = 0;
  
  //Fill in all initial frames
  while(frames.size() < size && !pageReferenceString.empty()) {
    if (!pageFoundInCollection(frames, pageReferenceString.front())) {
      frames.push_front(pageReferenceString.front());
      numberOfFaults++;
    }
    pageReferenceString.pop_front();
  }
  
  //Iterate through the rest of the page references in list
  while (!pageReferenceString.empty()) {
    //Page is not found in current frames
    if (!pageFoundInCollection(frames, pageReferenceString.front())) {
      numberOfFaults++;
      
      long furthestPage = -1;
      auto furthestIndex = frames.begin();
      
      //Iterate through each frame
      for (auto it = frames.begin(); it != frames.end(); ++it) {
          //Find refererence frame value
          auto referenceToFrameValue = std::find(pageReferenceString.begin(), pageReferenceString.end(), *it);
          //Get index of frame value currently in list
          auto distanceToPage = std::distance(pageReferenceString.begin(), referenceToFrameValue);
        //If it's currently the furthest, mark it as such
        if (distanceToPage >furthestPage) {
          furthestPage = distanceToPage;
          furthestIndex = it;
        }
      }
      //Iteration math to get index of frame
      frames[furthestIndex - frames.begin()] = pageReferenceString.front();
    }
    //Move to the next page
    pageReferenceString.pop_front();
  }

  return numberOfFaults;
}

int main(int argc, const char * argv[]) {
  for (int i = 0; i < 3; i++) {
    std::stringstream ss; 
    std::string fileName = "project-3-results-" + std::to_string(i+5) + ".csv";
    ResultsLogger resultsLogger(fileName);
    resultsLogger.OpenFile();
    std::deque<int> pageReference = generatePageReferenceStringVector(i+1);
    resultsLogger.WriteToFile("Number of Frames,FIFO,LRU,OPT");

    for (size_t i = 1; i <= 30; i++) {
      unsigned int fifo = runFIFOPageReplacement(i, pageReference);
      unsigned int lru = runLRUPageReplacement(i, pageReference);
      unsigned int opt = runOPTPageReplacement(i, pageReference);
      ss << i << "," << fifo << "," << lru << "," << opt << "\n";

    // std::cout << "Number of faults for FIFO w/ " << i << " frames: " << runFIFOPageReplacement(i, pageReference) << "\n";
    // std::cout << "Number of faults for LRU  w/ " << i << " frames: " << runLRUPageReplacement(i, pageReference) << "\n";
    // std::cout << "Number of faults for OPT  w/ " << i << " frames: " << runOPTPageReplacement(i, pageReference) << "\n\n";
  }
  resultsLogger.WriteToFile(ss.str());
  resultsLogger.CloseFile();
  
  //NOTE: Confirmation of accurate results tested here: http://solver.assistedcoding.eu/page_replacement
  }
  return 0;
}

