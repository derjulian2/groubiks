
#ifndef GROUBIKS_CUBE_HPP
#define GROUBIKS_CUBE_HPP

namespace groubiks {
    /*
    * the following colors are opposed in a regular cube:
    * blue   <> green
    * red    <> orange
    * yellow <> white
    */
    typedef enum {
        RED,
        BLUE,
        GREEN,
        WHITE,
        ORANGE,
        YELLOW
    } color_t;

    /*
     * a cube consists of 6 center-pieces, 12 edges and 8 vertices.
     * to fully encode a cube, we only save 20 numbers that determine
     * the exact state of the cube by enumerating all edges and vertices.
     *
     * the state of the cube is determined as follows:
     *
     * the default, solved cube is therefore encoded via two sorted arrays:
     * vertices: [ 0, 1, 2, ..., 7 ]
     * edges:    [ 0, 1, 2, ..., 11 ] 
     */
    class cube {
    public:
        using vertex_type = int;
        using edge_type = int;

        vertex_type vertices[8];
        edge_type edges[12];

        static constexpr cube get_solved();
    };

}

#endif