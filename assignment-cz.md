# Grafy

## Zadání

Grafová databáze se skládá z vrcholů a hran, které mohou být mezi jednotlivými vrcholy.     Každý z elementů (vrchol/hrana) má zadaný uživatelský identifikátor daného typu (typ se načte ze schéma). Dále může mít k dispozici     další libovolné vlastnosti libovolného počtu a zadaných typů (počet a typy se načtou ze schéma). Tyto vlastnosti musí být uloženy     po sloupcích.

Naprogramujte API pro práci se zadanou grafovou databází, která splňuje zadané požadavky:

## API

- Podrobnější popis API najdete v přiloženém souboru `graph_db.hpp`

### Schéma

Schéma obsahuje typ uživatelského identifikátoru a typy vlastností pro vrcholy a hrany. Typy vlasností jsou zadané jako `std::tuple`. Budou typu `std::string`, případně splňující trait `std::is_arithmetic`

```
struct graph_schema {
        using vertex_user_id_t =  // typ uživatelského identifikátoru pro vrcholy
        using vertex_property_t = // typy všech vlastností vrcholů
    
        using edge_user_id_t = // typ uživatelského identifikátoru pro hrany
        using edge_property_t = // typy všech vlastností hran
    };
    
```

- Žádné vlastnosti je také korektní vstup, který se zadává jako `std::tuple<>`

### Databáze

Databáze přijímá schéma jako šablonový argument (trait) ze kterého si načte typy. Musí implementovat tyto  typy a funkce:

```
// GraphSchema je zadané schéma databáze
    template<class GraphSchema>
    class graph_db {
    public:
        using vertex_t = ... // typ který reprezentuje vrchol
        using edge_t = // typ který reprezentuje hranu
    
        using vertex_it_t = // typ který reprezentuje iterátor pro vrcholy
        using edge_it_t = // typ který reprezentuje iterátor pro hrany
        using neighbor_it_t = // typ který reprezentuje iterátor pro hrany ze zadaného vrcholu
    
        // Přidá nový vrchol se zadaným identifikátorem a defaultními vlastnostmi.
        vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid) {}
    
        // Přidá nový vrchol se zadaným identifikátorem a zadanými vlastnostmi.
        template<typename ...Props>
        vertex_t add_vertex(typename GraphSchema::vertex_user_id_t &&vuid, Props &&...props) {}
    
        // Vrátí begin() a end() iterátor pro vrcholy.
        std::ranges::range<vertex_it_t> get_vertexes() const {}
    
        // Přidá novou hranu mezi zadanými vrcholy se zadaným identifikátorem a defaultními vlastnostmi. 
        edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2) {}
    
        // Přidá novou hranu mezi zadanými vrcholy se zadaným identifikátorem a zadanými vlastnostmi.
        template<typename ...Props>
        edge_t add_edge(typename GraphSchema::edge_user_id_t &&euid, const vertex_t &v1, const vertex_t &v2, Props &&...props) {}
    
        // Vrátí begin() a end() iterátor pro hrany.
        std::ranges::range<edge_it_t> get_edges() const {}
    };
    
```

### Funkce třídy reprezentující vrcholy (`graph_db::vertex_t`)

Třída by měla sloužit jako proxy pro vrcholy a měla by obsahovat tuto funkci a typ:

```
// Vrací identifikátor vrcholu
    const GraphSchema::vertex_user_id_t &id() const;
    
    // Vrací všechny vlastnosti daného vrcholu
    GraphSchema::vertex_property_t get_properties() const;
    
    // Vrací danou vlastnost na I-tém místě
    template<size_t I>
    decltype(auto) get_property() const;
    
    // Nastaví hodnotu všech vlastností
    template<typename ...PropsType>
    void set_properties(PropsType &&...props);
    
    // Nastaví hodnotu zadané vlasnosti na I-tém místě
    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    // Typ iterátoru který iteruje přes hrany (stejný jako `graph_db::neighbor_it_t`)
    using neighbor_it_t = 
    
    // Funkce vrací begin() a end() iterátory pro hrany jdoucí z daného vrcholu. 
    std::ranges::range<neighbor_it_t> edges()
    
```

- Třída může vypadat libovolně, jen musíte správně nastavit její typ uvnitř grafové databáze
- Vlastnosti jsou číslované od 0
- Funkce nastavující všechny vlastnosti musí obsahovat všechny hodnoty. Hodnoty jsou předávány **přímo** (ne pomocí `std::tuple`)

### Funkce třídy reprezentující hrany (`graph_db::edge_t`)

Třída by měla sloužit jako proxy pro hrany a měla by obsahovat tyto funkce:

```
// Vrací identifikátor hrany
    const GraphSchema::edge_user_id_t &id() const;
    
    // Vrací všechny vlastnosti dané hrany
    GraphSchema::edge_property_t get_properties() const;
    
    // Vrací danou vlastnost na I-tém místě
    template<size_t I>
    decltype(auto) get_property() const;
    
    // Nastaví hodnotu všech vlastností
    template<typename ...PropsType>
    void set_properties(PropsType &&...props);
    
    // Nastaví hodnotu zadané vlasnosti na I-tém místě
    template<size_t I, typename PropType>
    void set_property(const PropType &prop);

    // Vrátí zdrojový vrchol hrany.
    graph_db::vertex_t src()
    
    // Vrátí cílový vrchol hrany.
    graph_db::vertex_t dst()
    
```

- Návratový typ funkcí je vrchol, tedy `graph_db::vertex_t`
- Třída může vypadat libovolně, jen musíte správně nastavit její typ uvnitř grafové databáze
- Vlastnosti jsou číslované od 0
- Funkce nastavující všechny vlastnosti musí obsahovat všechny hodnoty. Hodnoty jsou předávány **přímo** (ne pomocí `std::tuple`)

### Iterátory (`graph_db::vertex_it_t`, `graph_db::edge_it_t`, `graph_db::neighbor_it_t`)

- Všechny iterátory jsou typu alespoň `output iterator`
- Třídy mohou vypadat libovolně, jen musíte správně nastavit jejich typ uvnitř grafové databáze a v případě `neighbor_it_t` i uvnitř třídy pro vrcholy.
- Iterátory vypisují elementy v pořádí v jakém byly vytvořeny.
- Pokud mám iterátor na nějaký element a vložím element stejného typu do databáze, tak chování je nedefinované
    - `vertex_t::edges()` + `add_edges()`
    - `get_vertexes()` + `add_vertex()`
    - `get_edges()` + `add_edge()`
- Pokud mám iterátor na nějaký element a vložím do databáze element jiného typu, iterátor by měl být správný.
    - `get_vertexes()` + `add_edge()`
    - `get_edges()` + `add_vertex()`

## Příklad

### Vstup

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

### Výstup

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

## Přílohy

- [prezentace](https://recodex.mff.cuni.cz/api/v1/uploaded-files/a35fefda-16fd-4c93-ac16-05982c327a0c/download)
    - Zde jsou k dispozice soubory, které jsou používané v Recodexu. Obsahuje i vzorový příklad pro otestování.
    Pokud Vám vzorový příklad funguje zde, tak by měl fungovat i v Recodexu :)
- [graph_db.hpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/dde6842e-6884-4ebe-8b5a-1c2623b7d2ea/download):
    - API k grafové databázi, k dispozici jsou funkce a typy a popis jednotlivých funkcí
    - Doplňte tento soubor, případně přidejte další soubory, neměňte
- [main.cpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/655ef30a-ec73-46fc-9836-93f2705cc35e/download)
    - main spouštějící testy, pro spuštění testů spusťte s parametrem `0`
    - **nenahrávat**
- [tests.hpp](https://recodex.mff.cuni.cz/api/v1/uploaded-files/57393366-3f3c-4517-a19e-c3ce0ca22413/download):
    - testovací třída, která obsahuje příklad
    - **nenahrávat**

## Poznámky

- Odevzdávejte soubor `graph_db.hpp`, případně další Vaše vytvořené soubory (se správnými `#include`)
- Pozor na const-correctness, některé funkce musí být `const`
    - Některé funkce např. `get_vertex() const` vrací objekt který dovoluje modifikovat databázi e.g. `vertex_t`.
    Ideálně by tomu tak samozřejmě nemělo být, ale vedlo by to k delšímu kódu. Ideálně byste chtěli mít provděpodobně druhý typ `const_vertex_t`, který bude použit pro immutable operace.
- Pozor na specializaci `std::vertor<bool>` a vracení referencí.
- Recodex používá g++ 9.2 with C++17 support

### Příklad ukládání po sloupcích

```c
// Ukládání po řádcích
    struct Properties { string p0; int p1; double p2; };
    vector<Properties> properties;
    properties[0].p0 = ... // přístup k první vlasnosti prvního elementu
    
    // Ukládání po sloupcích
    struct Properties { 
      vector<string> p0; 
      vector<int> p1; 
      vector<double> p2;
    };
    Properties properties;
    properties.p0[0] = ... // přístup k první vlasnosti prvního elementu
    
```

## Odkazy

- [návrhový vzor proxy](https://en.wikipedia.org/wiki/Proxy_pattern)
- [output iterátory](https://en.cppreference.com/w/cpp/named_req/OutputIterator)
- [ukládání po sloupcích](https://en.wikipedia.org/wiki/AoS_and_SoA)

## Aktualizace

- [10.4.] Poznámky (překladač, const-correctness), Přílohy (tests.hpp)