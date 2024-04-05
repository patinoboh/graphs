#ifndef GRAPH_DB_HPP
#define GRAPH_DB_HPP

#include <concepts>
#include <ranges>
#include <tuple>


// - - - - - E D G E _ D E C L A R A T I O N - - - - -


class edge {
public:
    auto id() const;

    auto get_properties() const;

    template<size_t I>
    decltype(auto) get_property() const;

    template<typename ...PropsType>
    void set_properties(PropsType &&...props);

    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    auto src() const;

    auto dst() const;
};


// - - - - - V E R T E X _ D E C L A R A T I O N - - - - -


class vertex {
public:
    auto id() const;

    auto get_properties() const;

    template<size_t I>
    decltype(auto) get_property() const;

    template<typename ...PropsType>
    void set_properties(PropsType &&...props);

    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    using neighbor_it_t =

    std::ranges::subrange<neighbor_it_t> edges() const;
};


// - - - - - G R A P H _ D E C L A R A T I O N - - - - -

// struct graph_schema {
//     using vertex_user_id_t =  // user identifier for vertices
//     using vertex_property_t = // types of vertex properties

//     using edge_user_id_t = // user identifier for edges
//     using edge_property_t = // types of edge properties
// };


template< typename T, template< typename> typename F>
struct my_type_transform;

template< typename ... L, template< typename P> class F>
struct my_type_transform< std::tuple< L ...>, F>
{
    using type = std::tuple< F< L> ...>;
};

template< typename T, template< typename> typename F>
using my_type_transform_t = typename my_type_transform< T, F>;


template<class GraphSchema>
class graph_db {
    using vertices_columnar_t = type_transform_t<typename GraphSchema::vertex_property_t, std::vector>
    using edges_columnar_t = type_transform_t<typename GraphSchema::edge_property_t, std::vector>
public:
    using vertex_t = 

    using edge_t =

    using vertex_it_t =

    using edge_it_t =

    using neighbor_it_t =

    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid);
    
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid);

    template<typename ...Props>
    vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid, Props &&...props);
    
    template<typename ...Props>
    vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t &vuid, Props &&...props);

    std::ranges::subrange<vertex_it_t> get_vertexes() const;

    edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2);
    edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2);

    template<typename ...Props>
    edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2, Props &&...props);
    
    template<typename ...Props>
    edge_t add_edge(const typename GraphSchema::edge_user_id_t &euid, const vertex_t &v1, const vertex_t &v2, Props &&...props);

    std::ranges::subrange<edge_it_t> get_edges() const;
};



#endif 
