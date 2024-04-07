#ifndef GRAPH_DB_HPP
#define GRAPH_DB_HPP

#include <iostream>
#include <concepts>
#include <ranges>
#include <tuple>
#include <vector>
#include <set>
#include <iterator>
#include <map>

#include "vertexIterator.hpp"
#include "edgeIterator.hpp"
#include "neighbourIterator.hpp"

#include "isValidGraphSchema.hpp"


using idx_type = size_t;

template<class GraphSchema>
class graph_db;

template<typename GraphSchema>
class VertexIterator;

template<typename GrapchSchema>
class EdgeIterator;

template<class GraphSchema>
class NeighbourIterator;

// - - - - - V E R T E X _ D E C L A R A T I O N - - - - -


template<class GraphSchema>
class vertex {
public:
    graph_db<GraphSchema> &db;
    idx_type idx = 0;
    using vertex_id_t = typename GraphSchema::vertex_user_id_t;
    using vertex_property_t = typename GraphSchema::vertex_property_t;
    using neighbor_it_t = NeighbourIterator<GraphSchema>;

    vertex(graph_db<GraphSchema>& db, idx_type idx) : db(db), idx(idx) {};
    vertex(graph_db<GraphSchema>& db) : db(db) {};

    auto id() const{return db.vertex_ids[idx];}

    void printIdx(){std::cout << idx << std::endl;}
    void printAll(){db.printVertices();}
    
    neighbor_it_t get_neighbours() const{
        std::vector<size_t> neighbours(db.edges[idx].begin(), db.edges[idx].end());
        return neighbor_it_t(db, neighbours, 0);
    }
   
    auto get_properties() const{
        return db.get_vertex_properties(idx, std::make_index_sequence<std::tuple_size<vertex_property_t>::value>{});
    }

    template<size_t... Is, typename... PropTypes>
    void set_properties(std::index_sequence<Is...>, PropTypes&&... props) const {
        ((set_property<Is>(std::forward<PropTypes>(props))), ...);
    }

    template<typename ...PropsType>
    void set_properties(PropsType &&...props) const {
        set_properties(std::index_sequence_for<PropsType...>{}, std::forward<PropsType>(props)...);
    }    

    // std::ranges::subrange<neighbor_it_t> edges() const{
    //     auto it = get_neighbours();
    //     return std::ranges::subrange<neighbor_it_t>(it.begin(), it.end());
    // }

    template<size_t I>
    decltype(auto) get_property() const{
        return std::get<I>(db.vertex_props)[idx];
    }
    
    template<size_t I, typename PropType>
    void set_property(const PropType &prop) const {
        std::get<I>(db.vertex_props)[idx] = prop;
    }
};


// - - - - - E D G E _ D E C L A R A T I O N - - - - -


template<class GraphSchema>
class edge {
private:
    idx_type idx;
    idx_type from_idx;
    idx_type to_idx;
    graph_db<GraphSchema> &db;
public:
    using edge_id_t = typename GraphSchema::edge_user_id_t;
    using edge_property_t = typename GraphSchema::edge_property_t;

    edge(graph_db<GraphSchema>& db, idx_type idx, idx_type from, idx_type to) : db(db), idx(idx), from_idx(from), to_idx(to) {};

    edge_id_t id() const {return db.edge_ids[idx];}

    void printAll(){db.printEdges();}

    template<size_t I>
    decltype(auto) get_property() const{
        return std::get<I>(db.edge_props)[idx];
    }
    
    auto get_properties() const{
        return db.get_edge_properties(idx, std::make_index_sequence<std::tuple_size<edge_property_t>::value>{});
    }

    template<size_t... Is, typename... PropTypes>
    void set_properties(std::index_sequence<Is...>, PropTypes&&... props) const {
        ((set_property<Is>(std::forward<PropTypes>(props))), ...);
    }

    template<typename ...PropsType>
    void set_properties(PropsType &&...props) const {
        set_properties(std::index_sequence_for<PropsType...>{}, std::forward<PropsType>(props)...);
    }


    template<size_t I, typename PropType>
    void set_property(const PropType &prop) const {
        std::get<I>(db.edge_props)[idx] = prop;
    }

    auto src() const{return vertex<GraphSchema>(db, from_idx);}

    auto dst() const{return vertex<GraphSchema>(db, to_idx);}
};


template< typename T, template< typename> typename F>
struct my_type_transform;

template< typename ... L, template< typename P> class F>
struct my_type_transform< std::tuple< L ...>, F>
{
    using type = std::tuple< F< L> ...>;
};

template< typename T, template< typename> typename F>
using my_type_transform_t = typename my_type_transform< T, F>::type;


// - - - - - P R I N T - - - - -


template<typename T>
void print_vector(const std::vector<T>& vector) {
    for (const auto& element : vector) {
        std::cout << element << '\n';
    }
}

template<typename Tuple, std::size_t... I>
void print_vectors_in_tuple(Tuple&& tuple, std::index_sequence<I...>) {
    ((std::cout << "Vector " << I << ":\n", print_vector(std::get<I>(tuple))), ...);
}

template<typename... Ts>
void print_vectors_in_tuple(std::tuple<Ts...>& tuple) {
    print_vectors_in_tuple(tuple, std::make_index_sequence<sizeof...(Ts)>());
}


// - - - - - G R A P H _ D E C L A R A T I O N - - - - -


template<class GraphSchema>
class graph_db {
    using vertices_columnar_t = my_type_transform_t<typename GraphSchema::vertex_property_t, std::vector>;    
    using edges_columnar_t = my_type_transform_t<typename GraphSchema::edge_property_t, std::vector>;
    
    using vertex_id_t = typename GraphSchema::vertex_user_id_t;
    using edge_id_t = typename GraphSchema::edge_user_id_t;

    using vertex_t = vertex<GraphSchema>;
    using edge_t = edge<GraphSchema>;

    using vertex_it_t = VertexIterator<GraphSchema>;
    using edge_it_t = EdgeIterator<GraphSchema>;
    using neighbor_it_t = NeighbourIterator<GraphSchema>;
private:
    
    size_t vertices_count = 0;
    size_t edges_count = 0;
    
    // vector_idx -> set of his edge_ids
    bool inRange(const vertex_t& v) const{return 0 <= v.idx < vertices_count;}

    
    // these two might be templated into just one
    template<typename... Props, std::size_t... Is>
    void stack_vertex(std::index_sequence<Is...>, Props&&... props) {
        (std::get<Is>(vertex_props).push_back(std::forward<Props>(props)), ...);
    }

    template<typename... Props, std::size_t... Is>
    void stack_edge(std::index_sequence<Is...>, Props&&... props) {
        (std::get<Is>(edge_props).push_back(std::forward<Props>(props)), ...);
    }

public:
    std::map<idx_type, std::set<idx_type>> edges;
    std::vector<vertex_id_t> vertex_ids;
    std::vector<edge_id_t> edge_ids;
    edges_columnar_t edge_props;
    vertices_columnar_t vertex_props;
    
    std::vector<idx_type> edge_sources;
    std::vector<idx_type> edge_destinations;
    
    graph_db() = default;
    
    template<std::size_t... Is>
    auto get_vertex_properties(idx_type index, std::index_sequence<Is...>) const {
        return std::make_tuple(std::get<Is>(vertex_props)[index]...);
    }

    template<std::size_t... Is>
    auto get_edge_properties(idx_type index, std::index_sequence<Is...>) const {
        return std::make_tuple(std::get<Is>(edge_props)[index]...);
    }

    template<typename... Props>
    void add_vertex_properties(Props&&... props) {
        stack_vertex(std::index_sequence_for<Props...>{}, std::forward<Props>(props)...);
    }

    template<typename... Props>
    void add_edge_properties(Props&&... props) {
        stack_edge(std::index_sequence_for<Props...>{}, std::forward<Props>(props)...);
    }
      
    void printVertices(){print_vectors_in_tuple(vertex_props);}
    void printEdges(){print_vectors_in_tuple(edge_props);}

    // std::ranges::subrange<vertex_it_t> get_vertexes() {
    //     // vertex_it_t begin(*this, 0, vertices_count);
    //     // vertex_it_t end(*this, vertices_count, vertices_count);
    //     // return {begin, end};
    //     return std::ranges::subrange<edge_it_t>(
    //         VertexIterator(*this, 0, edges_count), 
    //         VertexIterator(*this, edges_count, edges_count)
    //     );        
    // }

    // std::ranges::subrange<edge_it_t> get_edges() const{
    //     return std::ranges::subrange<edge_it_t>(
    //         edge_it_t(*this, 0, edge_sources[0], edge_destinations[0], edges_count), 
    //         edge_it_t(*this, edges_count, 0, 0, edges_count)
    //     );
    // }
    
    vertex_it_t get_iterator(){
        return vertex_it_t(*this, 0, vertices_count);
    }

    edge_it_t get_edge_iterator(){
        return edge_it_t(*this, 0, edge_sources[0], edge_destinations[0], edges_count);
    }    


    template<typename Props>
    void fill_default_values(Props& props) {
        std::apply([this](auto&... args) {
            ([&args] {
                args.emplace_back();
            }(), ...);
        }, props);
    }
    
    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid){        
        vertex_ids.push_back(std::move(vuid));    
        fill_default_values(vertex_props);
        return vertex_t(*this, vertices_count++);
    }
    
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid){
        vertex_ids.push_back(vuid);
        fill_default_values(vertex_props);        
        return vertex_t(*this, vertices_count++);        
    }

    template<typename ...Props>
    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid, Props &&...props){        
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::vertex_property_t>::value, "Incorrect number of properties");

        add_vertex_properties(std::forward<Props>(props)...);
        vertex_ids.push_back(std::move(vuid));
        return vertex_t(*this, vertices_count++);
    }
    
    template<typename ...Props>
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid, Props &&...props){
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::vertex_property_t>::value, "Incorrect number of properties");

        add_vertex_properties(std::forward<Props>(props)...);
        vertex_ids.push_back(vuid);
        return vertex_t(*this, vertices_count++);
    }

    edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2){
        if(!inRange(v1) || !inRange(v2)){
            throw std::out_of_range("vertex out of range");
        }
        edges[v1.idx].insert(edges_count);
        edge_ids.push_back(std::move(euid));
        edge_sources.push_back(v1.idx);
        edge_destinations.push_back(v2.idx);
        fill_default_values(edge_props);

        return edge_t(*this, edges_count++, v1.idx, v2.idx);
    }
    
    edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2){
        if(!inRange(v1) || !inRange(v2)){
            throw std::out_of_range("vertex out of range");
        }
        edges[v1.idx].insert(v2.idx);
        edge_ids.push_back(euid);
        edge_sources.push_back(v1.idx);
        edge_destinations.push_back(v2.idx);

        fill_default_values(edge_props);

        return edge_t(*this, edges_count++, v1.idx, v2.idx);
    }

    template<typename ...Props>
    edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2, Props &&...props){
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::edge_property_t>::value,
                      "Incorrect number of properties");

        if(!inRange(v1) || !inRange(v2)){
            throw std::out_of_range("vertex out of range");
        }
        edges[v1.idx].insert(v2.idx);
        edge_ids.push_back(std::move(euid));
        edge_sources.push_back(v1.idx);
        edge_destinations.push_back(v2.idx);
        add_edge_properties(std::forward<Props>(props)...);

        return edge_t(*this, edges_count++, v1.idx, v2.idx);
    }
    
    template<typename ...Props>
    edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2, Props &&...props){
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::edge_property_t>::value,
                      "Incorrect number of properties");

        if(!inRange(v1) || !inRange(v2)){
            throw std::out_of_range("vertex out of range");
        }
        
        edges[v1.idx].insert(v2.idx);
        edge_ids.push_back(euid);
        edge_sources.push_back(v1.idx);
        edge_destinations.push_back(v2.idx);
        add_edge_properties(std::forward<Props>(props)...);

        return edge_t(*this, edges_count++, v1.idx, v2.idx);
    }
};


#endif 
