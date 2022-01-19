# Functional Equivalent of a Maps Application

Implementing functional equivalent of a maps application (i.e. Google Maps, Yandex Maps etc.). In order to implement such application, `Graph` and `Hash Table` data structures will be additionally implemented. Details of these data structures will be explained in the More_information.pdf .


Keywords: C++, Data Structures, Graphs, Dijkstra Shortest Path Algorithm, Hash Table, Quadratic Probing

##  Graph Implementation

The `Graph` data data structure follows the adjacency list style of implementation with slight differences. `Graph` Data Structure has non-negative weights for its edges. Additionally, edges on this data structure is bi-directional meaning that graph is not a directional graph. Additional functionality consists of masking an edge in the graph and
finding multiple “shortest” paths using masking functions. Data layout of `Graph` class and its helper structures can bee seen on More_information.pdf . Instead of linked list, graph holds its data (in this case edges and vertices) in dynamic arrays. For dynamic array implementation, class utilizes std::vector<T> class from the Standard Template Library (STL). `Graph` class and its helper structures are declared in Graph.h header file and their implementations are defined in Graph.cpp file.
  
## Hash Table
  
In addition to the `Graph` data structure maps application requires a hash table for storing found paths. `KeyedHashTable` class stores string and integer vector pairs. The string will be used to determine the location of the data using hash function. Data layout of the `KeyedHashTable` can be seen on More_information.pdf . Hash table will resolve its collisions using quadratic probing. `KeyedHashTable` class and its helper structures are declared in HashTable.h header file and their implementations are defined in HashTable.cpp file.

  ##  METU Maps
  
 `METUMaps` class combines the `Graph` data structure and `KeyedHashTable` data structure. Data layout of the class can be seen on More_information.pdf . Continuing with the same pattern; `METUMaps` class is declared on METUMaps.h header file and its definitions are in METUMaps.cpp file.
