#ifndef GRAPH_DB_HPP
#define GRAPH_DB_HPP

#include <iostream>
#include <concepts>
#include <ranges>
#include <tuple>
#include <vector>
#include <set>
#include <unordered_map>


// - - - - - E D G E _ D E C L A R A T I O N - - - - -


using idx_type = size_t;

template<class GraphSchema>
class graph_db;


template<class GraphSchema>
class edge {
private:
    idx_type idx;
    idx_type from_idx;
    idx_type to_idx;
    graph_db<GraphSchema> *db;
public:
    using edge_id_t = typename GraphSchema::edge_user_id_t;
    using edge_property_t = typename GraphSchema::edge_property_t;

    // edge_id_t id() const{return }

    // auto get_properties() const;

    // template<size_t I>
    // decltype(auto) get_property() const;

    // template<typename ...PropsType>
    // void set_properties(PropsType &&...props);

    // template<size_t I, typename PropType>
    // void set_property(const PropType &prop);

    // auto src() const;

    // auto dst() const;
};


// - - - - - V E R T E X _ D E C L A R A T I O N - - - - -


template<class GraphSchema>
class vertex {
private:
public:
    graph_db<GraphSchema> &db;
    idx_type idx = 0;
    using vertex_id_t = typename GraphSchema::vertex_user_id_t;
    using vertex_property_t = typename GraphSchema::vertex_property_t;

    vertex_id_t vuid;
    
    // vertex(const graph_db<GraphSchema>& db, idx_type idx) : db(db), idx(idx) {};
    // vertex(const graph_db<GraphSchema>& db, idx_type&& idx) : db(db), idx(std::move(idx)) {};

    vertex(graph_db<GraphSchema>& db, idx_type idx) : db(db), idx(idx) {};
    vertex(graph_db<GraphSchema>& db) : db(db) {};

    void printIdx(){
        std::cout << idx << std::endl;
    }
    
    void test(){
        // db.test();
        std::cout << "test z vertexu" << std::endl;
        db.test();
    }


    // vertex(graph_db<GraphSchema> *db, size_t idx) : db(db), idx(idx) {}
    // vertex_idx_t id() const{return vuid;}

    // auto get_properties() const;

    // template<size_t I>
    // decltype(auto) get_property() const;

    // template<typename ...PropsType>
    // void set_properties(PropsType &&...props);

    // template<size_t I, typename PropType>
    // void set_property(const PropType &prop);

    // using neighbor_it_t =

    // std::ranges::subrange<neighbor_it_t> edges() const;
};




template< typename T, template< typename> typename F> // TODO : co ked dam prec typename?
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




// // - - - - - S E T - - - - -
// DOES NOT WORK ANYMORE

// template<typename Tuple, typename T, std::size_t... I>
// void set_property(Tuple&& tuple, const T& value, size_t idx, std::index_sequence<I...>) {
//     ((I == idx ? std::get<I>(tuple).push_back(value) : void()), ...);
// }

// template<typename... Ts, typename T>
// void set_property(std::tuple<Ts...>& tuple, const T& value, size_t index) {
//     set_property(tuple, value, index, std::make_index_sequence<sizeof...(Ts)>());
// }



// auto properties = std::make_tuple(2, 2.2, true, "dva");
//     std::apply([&gdb](auto&&... args) {
//         gdb.add_vertex("v2", std::forward<decltype(args)>(args)...);
//     }, properties);
// print_vectors_in_tuple(gdb.vertices);


// - - - - - G R A P H _ D E C L A R A T I O N - - - - -




// using vertex_property_t = std::tuple<std::string, int, double, char>;

template<class GraphSchema>
class graph_db {
    using vertices_columnar_t = my_type_transform_t<typename GraphSchema::vertex_property_t, std::vector>;
    using edges_columnar_t = my_type_transform_t<typename GraphSchema::edge_property_t, std::vector>;
    using vertex_id_t = typename GraphSchema::vertex_user_id_t;
    using edge_id_t = typename GraphSchema::edge_user_id_t;
    using vertex_t = vertex<GraphSchema>;
    using edge_t = edge<GraphSchema>;
private:
    
    size_t vertices_count = 0;
    vertices_columnar_t vertex_props;
    edges_columnar_t edge_props;
    std::vector<vertex_id_t> vertex_ids;
    std::vector<edge_id_t> edge_ids;
    
    std::unordered_map<idx_type, std::set<idx_type>> edges;
    bool inRange(const vertex_t& v) const{return 0 <= v.idx < vertices_count;}

    template<typename... Props, std::size_t... Is>
    void add_to_columns_impl(std::index_sequence<Is...>, Props&&... props) {
        (std::get<Is>(vertex_props).push_back(std::forward<Props>(props)), ...);
    }

    template<typename... Props>
    void add_vertex_properties(Props&&... props) {
        add_to_columns_impl(std::index_sequence_for<Props...>{}, std::forward<Props>(props)...);
    }

public:
    graph_db() = default;

    void test(){
        std::cout << "test" << std::endl;
        print_vectors_in_tuple(vertex_props);
    }

    // using vertex_it_t =

    // using edge_it_t =

    // using neighbor_it_t =


    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid){        
        vertex_ids.push_back(std::move(vuid));
        return vertex_t(*this, vertices_count++);        
    }
    
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid){
        vertex_ids.push_back(vuid);
        return vertex_t(*this, vertices_count++);
    }


    template<typename ...Props>
    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid, Props &&...props){        
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::vertex_property_t>::value,
                      "Incorrect number of properties");

        add_vertex_properties(std::forward<Props>(props)...);
        vertex_ids.push_back(std::move(vuid));
        return vertex_t(*this, vertices_count++);
    }
    
    template<typename ...Props>
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid, Props &&...props){
        static_assert(sizeof...(Props) == std::tuple_size<typename GraphSchema::vertex_property_t>::value,
                      "Incorrect number of properties");

        add_vertex_properties(std::forward<Props>(props)...);
        vertex_ids.push_back(vuid);
        return vertex_t(*this, vertices_count++);
    }

    // std::ranges::subrange<vertex_it_t> get_vertexes() const;

    // edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2){
    //     if(!inRange(v1) || !inRange(v2)){
    //         throw std::out_of_range("vertex out of range");
    //     }
    //     edges[v1.idx].insert(v2.idx);

    //     return edge_t(); // TODO
    // }
    
    // edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2);

    // template<typename ...Props>
    // edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2, Props &&...props);
    
    // template<typename ...Props>
    // edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2, Props &&...props);

    // std::ranges::subrange<edge_it_t> get_edges() const;
};







#endif 
