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
#include <cstring>

//#ifndef _STEP4_H_
//#define _STEP4_H_


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
  Point(double x_in, double y_in) : x(x_in), y(y_in){}
  double getx() const;
  double gety() const;
  double distance_from(const Point & rhs) const;
  friend std::ostream & operator << (std::ostream & os, const Point & rhs);
};

// Edge class
class Edge{
public:
    Point n1;
    Point n2;
    Edge(Point a, Point b): n1(a), n2(b){}
    bool isInterc(const Edge & rhs);
};

double calOrien(const Point & a, const Point & b, const Point & c);

// Path class that has information about visited nodes, 
// distances from each node to start node
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

// Graph class with a vector of nodes (ordered by ID) and a map of 
// adjancent points for each node
// as well as methods to set obstacles and find shortest path 
// using A* and Dijkstra                           
class Graph{
private:
  std::vector<Point> nodes;
  std::map<unsigned, std::vector<std::pair<unsigned, double> > > edges;
public:
  Graph() : nodes(), edges() {}
  void addNode(const Point & n);
  void addEdge(const std::pair<unsigned, unsigned> & e);
  size_t get_num_nodes();
  void set_obstacle(const std::vector<unsigned> obst);
  Path find_path(unsigned start, unsigned end, std::string algo);
  friend std::ostream & operator << (std::ostream & os, const Graph & rhs);
};



// #endif

////////////////////////////////////// #include "step4.hpp" ///////////////////////////

// ------------------------- methods for Point --------------------------
// get x coordinate of the point
double Point::getx() const {return x; }
// get y coordinate of the point
double Point::gety() const {return y;}                                                       

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

// ---------------------------methods for Edge -------------------------
// check if two edges intersect
bool Edge::isInterc(const Edge & rhs){
    std::vector<double> x_li{n1.getx(), n2.getx(), rhs.n1.getx(), rhs.n2.getx()};
    std::vector<double> y_li{n1.gety(), n2.gety(), rhs.n1.gety(), rhs.n2.gety()};
    double normx1 = fabs(x_li[0] - x_li[1]);
    double normx2  = fabs(x_li[2] - x_li[3]);
    double normy1 = fabs(y_li[0] - y_li[1]);
    double normy2 = fabs(y_li[2] - y_li[3]);
    std::sort(x_li.begin(), x_li.end());
    std::sort(y_li.begin(), y_li.end());
    double xmin = x_li[0];
    double xmax = x_li[3];
    double ymin = y_li[0];
    double ymax = y_li[3];
    // if their projections do not intersect
    if ((normx1 + normx2 < xmax - xmin) || (normy1 + normy2 < ymax - ymin)){
        return false;
    }
    // colinear or intersect
    double z1 = calOrien(n1, n2, rhs.n1);
    double z2 = calOrien(n1, n2, rhs.n2);
    double z3 = calOrien(rhs.n1, rhs.n2, n1);
    double z4 = calOrien(rhs.n1, rhs.n2, n2);
    if ((z1 * z2 <= 0) && (z3 * z4 <= 0)){
        return true;
    }
    return false;
}

// ----------------------------- methods for Path -----------------------------
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

// ------------------------------- methods for Graph ---------------------
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

// This function takes in an obstacle, then removes the edges blocked by the 
// obstacle in the graph
void Graph::set_obstacle(const std::vector<unsigned> obst){
    // construct an edge for each pair of nodes in obst
    std::vector<Edge> blocks;
    size_t n = obst.size();
    for (size_t i = 0; i < n; i++){
        unsigned id1 = obst[i];
        unsigned id2 = obst[(i +1) % n];
        Edge e(nodes[id1], nodes[id2]);
        blocks.push_back(e);
    }
    // go over all edges in the graph, check whether they intersect with blocks
    size_t num = get_num_nodes();
    for (size_t i1 = 0; i1 < num; i1++){ // each node in the graph
        std::vector<std::pair<unsigned, double> > adjs = edges[i1];
        for (size_t j = 0; j < adjs.size(); j++){ // each node adj to this node
            unsigned i2 = adjs[j].first;
            Edge curr(nodes[i1], nodes[i2]);
            for (size_t b = 0; b < blocks.size(); b++){ // each block
                if ((curr.isInterc(blocks[b])) && (adjs[j].second != INF) ){
                    edges[i1][j].second = INF;
                }
            }
        }
    }
}

// Find the shortest path from start node to end node in the graph
// if algo == "a", use the A* algorithm; if not specified or "d", use Dijkstra
Path Graph::find_path(unsigned start, unsigned end, std::string algo = "d"){
  std::cout << edges[8][1].first <<", " << edges[8][1].second << std::endl;
    size_t num = get_num_nodes();
    Path p(num, start, end);
    unsigned fmin = start;
    unsigned curr;
    bool stop = false;
    while ((! p.all_visited()) && !stop){
        if (algo == "d"){
          curr = find_min(p.visited, p.distance);
        } else if (algo == "a"){
          curr = fmin;
          std::cout << "Next node: " << fmin << std::endl;
        }
        if (p.visited[curr] == true){
            break;
        }
        p.visited[curr] = true; // mark current node visited
        std::vector<std::pair<unsigned, double> > check_li = edges[curr];
        std::sort(check_li.begin(), check_li.end());
        std::vector<std::pair<unsigned, double> >::iterator adj = check_li.begin();
        // set f and next current node as the first adj node
        fmin = (* adj).first;
        double f = (* adj).second + nodes[fmin].distance_from(nodes[end]);
        // for each node adj to current node
        while (adj != check_li.end()){ 
            unsigned id = (* adj).first;
            if (p.visited[id] == true){ // skip the visited adj node
                ++adj;
                continue;
            }
            std::cout << "check adj: " << id;
            double w = (* adj).second;
            std::cout << "weight" << w <<std::endl;
            // if distance via current node less than recorded distance, update the record
            if ((w + p.distance[curr] < p.distance[id]) && (w!= INF) ){ 
                p.distance[id] = w + p.distance[curr]; 
                p.previous[id] = curr;
            }
            if ((curr < p.previous[id])&&(w + p.distance[curr] == p.distance[id])&& (w!= INF)){
                    p.previous[id] = curr;
            } 
            if (algo == "a"){
              // A* stops when access the goal node
              if (id == end){
                stop = true;
                break;
              }
              double h = nodes[id].distance_from(nodes[end]);
              std::cout << "weight" << w <<std::endl;
              std::cout << "f: " << h + w << std::endl;
              if (h + w < f){
                f = h + w;
                fmin = id;
              }
              if ((h + w == f) && (id < fmin)){
                fmin = id;
              }
              
            }
            ++adj;
          }
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


// -------------------------------- helper functions ------------------------

// This function calculates orienation for three points
double calOrien(const Point & a,const Point & b,const Point & c){
    double z = (b.gety() - a.gety())*(c.getx() - b.getx()) - (b.getx() - a.getx())*(c.gety() - b.gety());
    return z;
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
  if ((stoi(n1) > (int)n - 1) || (stoi(n2) > (int)n - 1)) {
    std::cerr << "Wrong node IDs indicating the edge.\n";
    exit(EXIT_FAILURE);
  }
  std::pair<unsigned, unsigned> edge = std::make_pair(stoi(n1), stoi(n2));
  return edge;
}

// This function reads the map from input file, and generates a graph 
// containing nodes and edges                                                               
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

// This function reads obstacles information from input
// and stores in a vector of vector
std::vector<std::vector<unsigned> > read_obs(std::ifstream & obs){
    std::string line;
    std::vector<std::vector<unsigned> > ans;
    int section = 0;
    while (! obs.eof()){
        std::getline(obs, line);
        if (line == ""){
            continue;
        }
        if (line.find("$obstacles") != std::string::npos){
            section++;
            continue;
        }
        if (section == 1){
            std::vector<unsigned> b;
            if (line.find(" ") == std::string::npos){
                std::cerr << "An obstacle must has two or more nodes.\n";
                exit(EXIT_FAILURE);
            }
            while (line.find(" ")!= std::string::npos){
                std::string id = line.substr(0, line.find(" "));
                if (!isInt(id)){
                    std::cerr << "Node ID must be integers.\n";
                    exit(EXIT_FAILURE);
                }
                b.push_back(std::stoi(id));
                line = line.substr(line.find(" ") + 1);
            }
            if (!isInt(line)){
                std::cerr << "Node ID must be integers.\n";
                exit(EXIT_FAILURE);
            }
            b.push_back(std::stoi(line));
            ans.push_back(b);
        }
    }
    if (section != 1){
        std::cerr << "Wrong format for obstacle file.\n";
        exit(EXIT_FAILURE);
    }
    return ans;
}

// This function takes in a vector of obstacle, then sets obstacles in
// the graph
void setObs(std::vector<std::vector<unsigned> > obs, Graph & g){
    for (size_t i = 0; i < obs.size(); i++){
        g.set_obstacle(obs[i]);
    }
}


//------------------------------------ main -------------------------------

int main(int argc, char ** argv){
  if (argc < 5){
    std::cerr << "Wrong number of arguments.\n";
    return EXIT_FAILURE;
  }
  if (argc > 6){
    std::cerr << "Wrong number of arguments.\n";
    return EXIT_FAILURE;
  }
  int c = 1;
  std::string algo = "d";
  if (argc == 6){
    if (std::strcmp(argv[1], "-a") == 0){
      c = 2;
      algo = "a";
    } else{
      std::cerr << "No option as " << argv[1] << std::endl;
      return EXIT_FAILURE;
    }
  }
  // open grid map
  std::ifstream f_map;
  f_map.open(argv[c]);
  if (f_map.fail()) {
    std::cerr << "Failed to open the grid map file.\n";
    return EXIT_FAILURE;
  }
  // open obstacle file
  std::ifstream f_obs;
  f_obs.open(argv[c + 1]);
  if (f_obs.fail()){
    std::cerr << "Failed to open the obstacle file.\n";
    return EXIT_FAILURE;
  }
  // read the start node and goal node
  std::string start = argv[c + 2];
  std::string end = argv[c + 3];
  if ((! isInt(start))|| (! isInt(end))){
    std::cerr << "Wrong node format.\n";
    return EXIT_FAILURE;
  }
  unsigned init = stoi(start);
  unsigned goal = stoi(end);
  // create graph                                                     
  Graph g = readMap(f_map);
  if ( (init > g.get_num_nodes() - 1) || (goal > g.get_num_nodes() - 1) ){
    std::cerr << "Node ID out of range.\n";
    return EXIT_FAILURE;
  }
  std::cout << g;
  // read obstacles
  std::vector<std::vector<unsigned> > obs = read_obs(f_obs);
  std::cout << "obs:" << obs[0][0]<<std::endl;
  // set obstacles
  setObs(obs, g);
  std::cout << g;
  // find path: Dijkstra/A*
  Path p = g.find_path(init, goal, algo);
  // print path
  std::cout << p;
  return EXIT_SUCCESS;
}