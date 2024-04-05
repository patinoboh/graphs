# Graphs

## Description

The graph database contains vertices and edges which connects the vertices.     Every element has an identifier of certain type (the type from the schema) and arbitrary properties according to the schema.     All the properties need to be stored in the columnar store.

You need to create this API for the graph database:

## API

- More details about the API can be found in file `graph_db.hpp`

### Schema

The schema contains the type of the identifier and the types of the properties for vertices and edges.     The types of properties are set as a `std::tuple`. All the types are either `std::string` or fulfil `std::is_arithmetic` trait.

```cpp
struct graph_schema {
        using vertex_user_id_t =  // user identifier for vertices
        using vertex_property_t = // types of vertex properties
    
        using edge_user_id_t = // user identifier for edges
        using edge_property_t = // types of edge properties
    };
    
```

- No properties, i.e. `std::tuple<>`, are also valid input

### Database

Database takes the schema as a template parameter from which it can load the types. It must contain these types and functions:

```cpp
// GraphSchema is the schema
    template<class GraphSchema>
    class graph_db {
    public:
        using vertex_t = ... // the type representing a vertex
        using edge_t = // the type representing an edge
    
        using vertex_it_t = // the type representing a vertex iterator
        using edge_it_t = // the type representing an edge iterator
        using neighbor_it_t = // the type representing an edge iterator of a certain vertex
    
        // Add a vertex with given the given identifier and default properties
        vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid)
    
        // Add a vertex with given the given identifier and given properties
        template<typename ...Props>
        vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid, Props &&...props)
    
        // Returns vertex iterators to all vertices
        std::ranges::range<vertex_it_t> get_vertexes() const
     
        // Add an edge within 2 vertices with the given identifier and default properties
        edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2)
    
        // Add an edge within 2 vertices with the given identifier and given properties
        template<typename ...Props>
        edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2, Props &&...props)
    
        // Returns edge iterators to all edges
        std::ranges::range<edge_it_t> get_edges() const
    };
    
```

### Functions within vertex class (`graph_db::vertex_t`)

The class that behaves as the proxy a vertex and contains those types and functions:

```cpp
// Returns a user identifier
    const GraphSchema::vertex_user_id_t &id() const;
    
    // Returns all the properties
    GraphSchema::vertex_property_t get_properties() const;
    
    // Returns the I-th property
    template<size_t I>
    decltype(auto) get_property() const;
    
    // Set the values of all properties
    template<typename ...PropsType>
    void set_properties(PropsType &&...props);
    
    // Set the value of the I-th property
    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    // The iterator type that iterates over the outgoing edges (same as `graph_db::neighbor_it_t`)
    using neighbor_it_t = ...
    
    // Returns the iterator to the outgoing edges 
    std::ranges::range<neighbor_it_t> edges()
    
```

- The class can look arbitrary, just rename it with the correct typedef
- The properties are counted from 0
- The function setting all the properties must contain all the values. The values are passed **directly** (not wrapped inside `std::tuple`)

### Functions within edge class (`graph_db::edge_t`)

The class that behaves as the proxy an edge and contains those types and functions:

```cpp
// Returns a user identifier
    const GraphSchema::edge_user_id_t &id() const;
    
    // Returns all the properties
    GraphSchema::edge_property_t get_properties() const;
    
    // Returns the I-th property
    template<size_t I>
    decltype(auto) get_property() const;
    
    // Set the values of all properties
    template<typename ...PropsType>
    void set_properties(PropsType &&...props);
    
    // Set the value of the I-th property
    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    // Returns the source vertex
    graph_db::vertex_t src()
    
    // Returns the destination vertex
    graph_db::vertex_t dst()
    
```

- The class can look arbitrary, just rename it with the correct typedef
- The properties are counted from 0
- The function setting all the properties must contain all the values. The values are passed **directly** (not wrapped inside `std::tuple`)

### Iterators (`graph_db::vertex_it_t`, `graph_db::edge_it_t`, `graph_db::neighbor_it_t`)

- All the iterators are at least of `output iterator` type
- The class can look arbitrary, just rename it with the correct typedef
- Iterators return the values in the **same order** as the elements were added
- The behavior is undefined in case of interleaving of those functions:

    - `vertex_t::edges()` + `add_edges()`
    - `get_vertexes()` + `add_vertex()`
    - `get_edges()` + `add_edge()`
- The behavior is correct in case of interleaving of those functions:

    - `get_vertexes()` + `add_edge()`
    - `get_edges()` + `add_vertex()`

## Example

### Input

```c
struct graph_schema {
        using vertex_user_id_t = std::string;
        using vertex_property_t = std::tuple<std::string, int, double, char>;
    
        using edge_user_id_t = float;
        using edge_property_t = std::tuple<std::string, bool>;
    };
    
    using gdb_t = graph_db<graph_schema>;
    
    void fill(gdb_t &gdb) {
        auto v1 = gdb.add_vertex("prvni");
        v1.set_properties("str1", 1, 1, false);
        auto v2 = gdb.add_vertex("druhy");
        v2.set_property<2>(2.2);
        auto v3 = gdb.add_vertex("treti", "str3", 3, 3.3, 3);
        gdb.add_vertex("ctvrty");
    
        auto e1 = gdb.add_edge(1.2, v1, v2);
        e1.set_properties("str1", false);
        gdb.add_edge(2.3, v2, v3, "str2", true);
        auto e3 = gdb.add_edge(1.3, v1, v3);
        e3.set_property<1>(true);
    
        // A simple check that storage is columnar & get_property doesn't return a copy.
        assert(&v1.template get_property<0>() + 1 == &v2.template get_property<0>());
    }
    
    void print(const gdb_t &gdb) {
        std::cout << "Graph:\n";
        auto [vertexes_begin, vertexes_end] = gdb.get_vertexes();
        std::for_each(vertexes_begin, vertexes_end, [](auto &&vertex) {
            std::cout << "(" << vertex.id() << ") with p1=='" << vertex.template get_property<0>()
                    << "', p2=='" << std::get<1>(vertex.get_properties()) << "'\n";
            auto [neigbor_edges_begin, neighbor_edges_end] = vertex.edges();
            std::for_each(neigbor_edges_begin, neighbor_edges_end, [&vertex](auto &&edge){
                assert(edge.src().id() == vertex.id());
                std::cout << "  (" << edge.src().id() << ")-[" << edge.id() << "]->("
                        << edge.dst().id() << ") with p1=='" << std::get<0>(edge.get_properties())
                        << "', p2=='" << edge.template get_property<1>() << "'\n";
            });
        });
    }
    
    void clear(gdb_t &gdb) {
        auto [vertexes_begin, vertexes_end] = gdb.get_vertexes();
        std::for_each(vertexes_begin, vertexes_end, [](auto &&vertex) {
            vertex.set_properties("", 0, 0, 0);
            vertex.template set_property<0>(" ");
            auto [neigbor_edges_begin, neighbor_edges_end] = vertex.edges();
            std::for_each(neigbor_edges_begin, neighbor_edges_end, [&vertex](auto &&edge){
                assert(edge.src().id() == vertex.id());
                edge.set_properties("", false);
                edge.template set_property<0>(" ");
            });
        });
    }
    
    int main() {
        gdb_t gdb;
        fill(gdb);
        print(gdb);
        clear(gdb);
        print(gdb);
    }
    
```

### Output

```
Graph:
    (prvni) with p1=='str1', p2=='1'
      (prvni)-[1.2]->(druhy) with p1=='str1', p2=='0'
      (prvni)-[1.3]->(treti) with p1=='', p2=='1'
    (druhy) with p1=='', p2=='0'
      (druhy)-[2.3]->(treti) with p1=='str2', p2=='1'
    (treti) with p1=='str3', p2=='3'
    (ctvrty) with p1=='', p2=='0'
    Graph:
    (prvni) with p1==' ', p2=='0'
      (prvni)-[1.2]->(druhy) with p1==' ', p2=='0'
      (prvni)-[1.3]->(treti) with p1==' ', p2=='0'
    (druhy) with p1==' ', p2=='0'
      (druhy)-[2.3]->(treti) with p1==' ', p2=='0'
    (treti) with p1==' ', p2=='0'
    (ctvrty) with p1==' ', p2=='0'
    
```

## Attachments

- All the files that are used for testing inside the ReCodex. If you are able to compile it (with g++) on your local machine
    you should not have problems in Recodex :)
- [presentation](https://recodex.mff.cuni.cz/api/v1/uploaded-files/a35fefda-16fd-4c93-ac16-05982c327a0c/download)
    - The presentation
- [graph_db.hpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/dde6842e-6884-4ebe-8b5a-1c2623b7d2ea/download):
    - graph database's API
    - Implement and submit this file.
    - **Don't change the name!**
- [main.cpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/655ef30a-ec73-46fc-9836-93f2705cc35e/download)
    - main starting the tests (start with argument `0`)
    - **don't submit**
- [tests.hpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/57393366-3f3c-4517-a19e-c3ce0ca22413/download):
    - test class with all the tests
    - **don't submit**

## Notes

- Submit file `graph_db.hpp` and all you other potential files (don't forget to `#include`)
- Try achieve const-correctness as much as possible (some functions need to be `const`)
    - Some functions, e.g. `get_vertex() const` returns an object which let you modify the database, e.g. `vertex_t`
    (This is not ideal, but we needed for the simplicity of the exercise. Ideally you should have a second immutable object.)
- Beware of `std::vector<bool>` specialization, it doesn't need to return references.
- Recodex uses g++ 9.2 with C++17 support

### Axample of columnar storage

```c
// A row storage
    struct Properties { string p0; int p1; double p2; };
    vector<Properties> properties;
    properties[0].p0 = ... // The access to the first property of the first element
    
    // A columnar storage
    struct Properties { 
      vector<string> p0; 
      vector<int> p1; 
      vector<double> p2;
    };
    Properties properties;
    properties.p0[0] = ... // The access to the first property of the first element
    
```

## Links

- [design pattern proxy](https://en.wikipedia.org/wiki/Proxy_pattern)
- [output iterators](https://en.cppreference.com/w/cpp/named_req/OutputIterator)
- [columnar storage](https://en.wikipedia.org/wiki/AoS_and_SoA)