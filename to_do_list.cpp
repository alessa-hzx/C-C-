#include <list>
#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <fstream>
#include <vector>

typedef std::string task_t;

class TaskDep {
 private:
  const task_t task;
  std::list<task_t> pres;

 public:
                                                                 
  TaskDep(const task_t & t): task(t) {}
  const task_t & getTask() const{
      return task;
  }
  void addPredecessor(const task_t & t){
      pres.push_back(t);
  }
  std::list<task_t> getPredecessors() const{
      return pres;
  }
};

////////////////////////////////////////////////////////////////////

// this function reads tasks and their predecessors from the input file
// then add all tasks with predecessors (as their order in the input file) to a list
std::list<TaskDep> makelist(std::ifstream & f){
    std::list<TaskDep> list;
    std::string line;
    while (! f.eof()){
        std::getline(f, line);
        if (line == ""){
            continue;
        }
        // get task
        task_t t;
        std::list<task_t> pres;
        std::string delim = ":";
        if (line.find(delim) == std::string::npos){
            std::cerr << "Invalid input.\n";
            exit(EXIT_FAILURE);
        }
        if (line.find(delim) == 0){
            t  = "";
        } else{
            t = line.substr(0, line.find(delim));
        }
        TaskDep task(t);
        // get deps
        if (line.length() > t.length() + 1){ // if there are any value
        if (line[line.find(delim)+1] != ' '){
            std::cerr << "Invalid input.\n";
            exit(EXIT_FAILURE);
            }
        std::string v = line.substr(line.find(delim) + 2);
        std::size_t p = 0;
        while (v.find(", ") != std::string::npos){
            task.addPredecessor(v.substr(p, v.find(", ")));
            p = v.find(", ") + 2;
            v = v.substr(p);
            }
        task.addPredecessor(v);
        }
        // add to list
        list.push_back(task);
    }
    return list;
}

// This function create an answer list, ordered according to dependences
std::list<task_t> maketodo(std::list<TaskDep> deplist){
    std::list<task_t> anslist;
    
    while (! deplist.empty()){ // if still have task in the original list
        size_t n = deplist.size();
        std::list<TaskDep>::iterator it = deplist.begin();
        while (it != deplist.end()){ // for every task in the original list
            // if task has no predecessors
            if ( (*it).getPredecessors().empty() ){
            anslist.push_front((* it).getTask());
            it = deplist.erase(it);
            continue;
            }
            // check whether all predecessors are in the anslist
            std::list<task_t> pres = (* it).getPredecessors();
            if (! anslist.empty() ){
                std::list<task_t>::iterator it_ans = anslist.begin();
                std::list<task_t>::iterator loc;
                std::size_t count = 0; // count the number of predecessors matched
                while (it_ans != anslist.end()){
                    std::list<task_t>::iterator check = std::find(pres.begin(), pres.end(), (* it_ans));
                    if (check != pres.end()){
                        ++count;
                        ++it_ans;
                        loc = it_ans;
                    } else
                    {
                        ++it_ans;
                    }
                }
                if (count == pres.size()){
                    anslist.insert(loc, (* it).getTask());
                    it = deplist.erase(it);
                    continue;
                }         
            }
            ++it;
        }
        // if go over all the original list and did not delete anything
        if (n == deplist.size()){ 
            std::cerr << "Dependecies do not allow a to-do list to be created.\n";
            exit(EXIT_FAILURE);
        }
    }
    return anslist;
}


// This function prints out the to-do list
void print_todo(std::list<task_t> anslist){
    
    if (! anslist.empty()){
        std::list<task_t>::iterator it = anslist.begin();
        std::cout << * it;
        ++it;
        while(it != anslist.end()){
            std::cout << ", " << * it;
            ++it;
        }
        std::cout << std::endl;
    }
}


int main(int argc, char ** argv) {
    if (argc != 2){
        std::cerr << "Need one input file.\n";
        return EXIT_FAILURE;
    }
  // open file for reading 
  std::cout << "open file.\n";
    std::ifstream file;
    file.open(argv[1]);
    if (file.fail()){
        std::cerr << " Cannot open file.\n";
        return EXIT_FAILURE;
    }                                                     
  // make dependency list of tasks from input file 
    std::list<TaskDep> deplist = makelist(file);  
  // order list according to dependencies       
    std::list<task_t> ans = maketodo(deplist);                              
  // print todo list
    print_todo(ans);                                                           
  return EXIT_SUCCESS;
}

