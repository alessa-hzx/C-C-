#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>


bool is_in(unsigned n, std::vector<std::pair<unsigned, double> > & adj);

// point class with calculate distance method and overloaded << operator        
class Point{
private:
    double x;
    double y;
public:
  Point(double x_in, double y_in);
  double distance_from(const Point & rhs) const;
  friend std::ostream & operator << (std::ostream & os, const Point & rhs);
};

// Graph class with a vector of nodes (ordered by ID) and a map of adjancent po\
ints for each node                                                              
class Graph{
private:
  std::vector<Point> nodes;
  std::map<unsigned, std::vector<std::pair<unsigned, double> > > edges;
public:
  Graph();
  void addNode(const Point & n);
  void addEdge(const std::pair<unsigned, unsigned> & e);
  size_t get_num_nodes();
  friend std::ostream & operator << (std::ostream & os, const Graph & rhs);
};

////////////////////// #include "step1.hpp" ///////////////////////

// point constructor                                                            
Point::Point(double x_in, double y_in) : x(x_in), y(y_in) {
}
// calculate distance between points                                            
double Point::distance_from(const Point & rhs) const {
  double dis;
  dis = sqrt(pow(x - rhs.x, 2) + pow(y - rhs.y, 2));
  return dis;
}
// overload the << operator for point                                           
std::ostream & operator<<(std::ostream & os, const Point & rhs) {
  os << "(" << rhs.x << "," << rhs.y << ")";
  return os;
}

// graph constructor                                                            
Graph::Graph() : nodes(), edges() {
}
// add node to the graph                                                        
void Graph::addNode(const Point & n) {
  nodes.push_back(n);
}
// add edge to the graph                                                        
void Graph::addEdge(const std::pair<unsigned, unsigned> & e) {
  unsigned n1 = e.first;
  unsigned n2 = e.second;
  double w = nodes[n1].distance_from(nodes[n2]);
  if (!is_in(n2, edges[n1])) {
    edges[n1].push_back(std::make_pair(n2, w));
    edges[n2].push_back(std::make_pair(n1, w));
  }
}
// get the number of nodes in the graph                                         
size_t Graph::get_num_nodes() {
  return nodes.size();
}
// overload the << operator for the graph                                       
std::ostream & operator<<(std::ostream & os, const Graph & rhs) {
  std::vector<Point>::const_iterator it = (rhs.nodes).begin();
  while (it != (rhs.nodes).end()) {  // print the nodes information             
    os << (*it) << " ";
    ++it;
  }
  os << std::endl;
  std::map<unsigned, std::vector<std::pair<unsigned, double> > >::const_iterator i = rhs.edges.begin();
  // print the edges information for each node                                  
  while (i != rhs.edges.end()) {
    unsigned id = (*i).first;
    os << id << ":";
    std::vector<std::pair<unsigned, double> > adjs = rhs.edges.at(id);
    // sort the adjacencies by node ID                                          
    std::sort(adjs.begin(), adjs.end());
    std::vector<std::pair<unsigned, double> >::const_iterator adj = adjs.begin(\
);
    while (adj != adjs.end()) {
      os << " " << (*adj).first << "," << (*adj).second;
      ++adj;
    }
    os << std::endl;
    ++i;
  }
  return os;
}

// This function checks whether a given node ID is in the given adjacent vector
bool is_in(unsigned n, std::vector<std::pair<unsigned, double> > & adj){
  std::vector<std::pair<unsigned, double> >::iterator it = adj.begin();
  while (it != adj.end()){
    if (n == (* it).first){
      return true;
    }
    ++it;
  }
  return false;
}

// This function checks whether a string can be converted to int numbers            
bool isInt(const std::string & s) {
  size_t n = s.length();
  for (size_t i = 0; i < n; i++) {
    if (!isdigit(s[i])) {
      return false;
    }
  }
  return true;
}

// This function checks whether a string can be converted to numbers            
bool isNumber(const std::string & s) {
  size_t n = s.length();
  for (size_t i = 0; i < n; i++) {
    if (!isdigit(s[i]) && (s[i] != '.')) {
      return false;
    }
  }
  return true;
}

// Driver function to sort the vector elements by first element(ID) of pairs    
bool sortbyid(const std::pair<unsigned, Point> & a,
              const std::pair<unsigned, Point> & b) {
  return (a.first < b.first);
}

// This function reads the nodes input and stores in a pair (id, point)         
std::pair<unsigned, Point> getNode(std::string & line) {
  if (line.find(" ") == std::string::npos) {
    std::cerr << "Wrong node format.\n";
    exit(EXIT_FAILURE);
  }
  // get node id                                                                
  size_t delim = line.find(" ");
  std::string id = line.substr(0, delim);
  if (!isInt(id)) {
    std::cerr << "Node ID should be numbers.\n";
    exit(EXIT_FAILURE);
  }
  unsigned i = std::stoi(id);
  // get node position                                                          
  std::string coor = line.substr(delim + 1);
  if (coor.find(" ") == std::string::npos) {
    std::cerr << "Wrong node format/section.\n";
    exit(EXIT_FAILURE);
  }
  std::string x = coor.substr(0, coor.find(" "));
  std::string y = coor.substr(coor.find(" ") + 1);
  if ((!isNumber(x)) || (!isNumber(y))) {
    std::cerr << "Coordinate must be two numbers.\n";
    exit(EXIT_FAILURE);
  }
  Point p(std::stof(x), std::stof(y));
  std::pair<unsigned, Point> node = std::make_pair(i, p);
  return node;
}

// This function reads the edges input and stores in a pair (node1, node2)      
std::pair<unsigned, unsigned> getEdge(std::string & line, size_t n) {
  if (line.find(" ") == std::string::npos) {
    std::cerr << "Wrong edge format.\n";
    exit(EXIT_FAILURE);
  }

  std::string n1 = line.substr(0, line.find(" "));
  std::string n2 = line.substr(line.find(" ") + 1);
  if ((n1 == "")||(n2 == "")){
      std::cerr << "Each edge line should have two numbers.\n";
      exit(EXIT_FAILURE);
  }
  if ((!isInt(n1)) || (!isInt(n2))) {
    std::cerr << "Edge shoule between two node IDs.\n";
    exit(EXIT_FAILURE);
  }
  if ((stoi(n1) > (int)n - 1) || (stoi(n2) > (int)n - 1)) {
    std::cerr << "Wrong node IDs indicating the edge.\n";
    exit(EXIT_FAILURE);
  }
  std::pair<unsigned, unsigned> edge = std::make_pair(stoi(n1), stoi(n2));
  return edge;
}

// This function reads the map from input file, and generates a graph containin\
g nodes and edges                                                               
Graph readMap(std::ifstream & map) {
  std::string line;
  int section = 0;
  std::vector<std::pair<unsigned, Point> > n_vec;
  Graph g = Graph();
  while (!map.eof()) {
    std::getline(map, line);
    if (line == "") {
      continue;
    }
    // identify the section                                                     
    if ((line.find("$nodes") != std::string::npos)) {
      section++;
      continue;
    }
    // if finish reading the nodes, sort nodes by ID and add to graph           
    if (line.find("$edges") != std::string::npos) {
      size_t num = n_vec.size();
      if (num > 0) {
        std::sort(n_vec.begin(), n_vec.end(), sortbyid);
        for (size_t i = 0; i < num; i++) {
          g.addNode(n_vec[i].second);
        }
      }
      section++;
      continue;
    }
    // read nodes                                                               
    if (section == 1) {
      std::pair<unsigned, Point> n = getNode(line);
      n_vec.push_back(n);
    }
    // read edges                                                               
    if (section == 2) {
      if (g.get_num_nodes() > 0) {
        std::pair<unsigned, unsigned> e = getEdge(line, g.get_num_nodes());
        g.addEdge(e);
      }
    }
  }
  if (section < 2) {
    std::cerr << "Should have two sections.\n";
    exit(EXIT_FAILURE);
  }
  return g;
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cerr << "Wrong number of arguments.\n";
    return EXIT_FAILURE;
  }
  // open file                                                                  
  std::ifstream f_map;
  f_map.open(argv[1]);
  if (f_map.fail()) {
    std::cerr << "Failed to open the grid map file.\n";
    return EXIT_FAILURE;
  }
  // read file into graph                                                       
  Graph g = readMap(f_map);
  // // print out rhe graph                                                     
  std::cout << g;

  return EXIT_SUCCESS;
}




