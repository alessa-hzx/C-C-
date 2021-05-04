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
unsigned find_min(std::vector<bool> visited, std::vector<double> distance);

// define inf
const double INF = std::numeric_limits<double>::infinity();
const unsigned MAX = std::numeric_limits<unsigned>::max();

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

// Path class that has information about visited nodes, distances from each node to start node
// and link between two nodes for the shortest path
class Path{

public:
    unsigned start;
    unsigned end;
    std::vector<bool> visited;
    std::vector<double> distance;
    std::vector<unsigned> previous;
    Path(size_t n, unsigned init, unsigned goal);
    bool all_visited();
    friend std::ostream & operator << (std::ostream & os, const Path & rhs);
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
  Path find_path(unsigned start, unsigned end);
  friend std::ostream & operator << (std::ostream & os, const Graph & rhs);
};


////////////////////// #include "step2.hpp" ///////////////////////

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

// Path constructor
Path::Path(size_t n, unsigned init, unsigned goal): start(init), end(goal), visited(), distance(), previous(){
    for (size_t i = 0; i < n; i++){
        visited.push_back(false);
        distance.push_back(INF);
        previous.push_back(MAX);
    }
    distance[init] = 0; // initialized the distance of the start node as 0
    previous[init] = init;
}

// This function checks whether all the nodes have been vistied
bool Path::all_visited(){
    if (std::find(visited.begin(), visited.end(), false) == visited.end()){
        return true;
    }
    return false;
}

// Overload the << operator for path to print the shortest path from start to end node
std::ostream & operator << (std::ostream & os, const Path & rhs){
    if (rhs.distance[rhs.end] == INF){
        std::cerr << "No path exists.\n";
        exit(EXIT_FAILURE);
    }
    // find the whole path from the end
    std::vector<unsigned> r_path;
    unsigned curr = rhs.end;
    while (curr != rhs.start){
        r_path.push_back(curr);
        curr = rhs.previous[curr];
    }
    r_path.push_back(rhs.start);
    // print the path
    std::vector<unsigned>::reverse_iterator rit = r_path.rbegin();
    while (rit != r_path.rend()){
        os << (* rit) << " ";
        ++rit;
    }
    os << ": " << rhs.distance[rhs.end] << std::endl;
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

// Find the shortest path from start node to end node in the graph ++++++++++++++++++++++++++++
Path Graph::find_path(unsigned start, unsigned end){
    size_t num = get_num_nodes();
    Path p(num, start, end);
    while (! p.all_visited()){
        unsigned curr = find_min(p.visited, p.distance);
        if (p.visited[curr] == true){
            break;
        }
        std::vector<std::pair<unsigned, double> > check_li = edges[curr];
        std::sort(check_li.begin(), check_li.end());
        std::vector<std::pair<unsigned, double> >::iterator adj = check_li.begin();
        while (adj != check_li.end()){ // for each node adj to current node
            unsigned id = (* adj).first;
            if (p.visited[id] == true){ // skip the visited adj node
                ++adj;
                continue;
            }
            double w = (* adj).second;
            // if distance via current node less than recorded distance, update the record
            if ((w + p.distance[curr] < p.distance[id])){ 
                p.distance[id] = w + p.distance[curr]; 
                p.previous[id] = curr;
            }
            if ((curr < p.previous[id])&&(w + p.distance[curr] == p.distance[id])){
                    p.previous[id] = curr;
            } 
            ++adj;
        }
        p.visited[curr] = true; // mark current node visited
    }
    return p;
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

// This function finds the smallest distance for unvistied node(index)
// If several indices have the same smallest value, return the smallest id
unsigned find_min(std::vector<bool> visited, std::vector<double> distance){
    size_t n = visited.size();
    double min = INF;
    unsigned id = 0;
    for (size_t i = 0; i < n; i++){
        if (visited[i] == false){
            if (distance[i] < min){
                id = i;
                min = distance[i];
            }
        }
    }
    return id;
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

// This function checks whether a string can be converted to numbers(decimal allowed)       
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
    std::cerr << "Node ID should be integers.\n";
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

  if ((!isInt(n1)) || (!isInt(n2))) {
    std::cerr << "Edge shoule between two node IDs.\n";
    exit(EXIT_FAILURE);
  }
  if ((stoi(n1) > n - 1) || (stoi(n2) > n - 1)) {
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


///////////////////////////////// main //////////////////////////////////////////////

int main(int argc, char ** argv){
    if (argc != 4){
        std::cerr << "Wrong number of arguments.\n";
        return EXIT_FAILURE;
    }
    // open the map file
    std::ifstream f_map;
    f_map.open(argv[1]);
    if (f_map.fail()) {
        std::cerr << "Failed to open the grid map file.\n";
        return EXIT_FAILURE;
    }
    // read the start node and goal node
    std::string start = argv[2];
    std::string end = argv[3];
    if ((! isInt(start))|| (! isInt(end))){
        std::cerr << "Wrong node format.\n";
        return EXIT_FAILURE;
    }
    unsigned init = stoi(start);
    unsigned goal = stoi(end);                                                     
    Graph g = readMap(f_map);
    if ( (init > g.get_num_nodes() - 1) || (goal > g.get_num_nodes() - 1) ){
        std::cerr << "Node ID out of range.\n";
        return EXIT_FAILURE;
    }
    // find path 
    Path p = g.find_path(init, goal);
    // print shortest path
    std::cout << p;

    return EXIT_SUCCESS;
}
