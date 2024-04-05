#ifndef GRAPH_DB_HPP
#define GRAPH_DB_HPP

#include <concepts>
#include <ranges>
#include <tuple>



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



class graph_db {
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

    /**
     * @brief Returns begin() and end() iterators to all edges in the database.
     * @return A ranges::subrange(begin(), end()) of edge iterators.
     */
    std::ranges::subrange<edge_it_t> get_edges() const;
};



#endif 
