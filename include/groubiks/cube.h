
#ifndef GROUBIKS_CUBE_H
#define GROUBIKS_CUBE_H

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
typedef int vertex_t;
typedef int edge_t;

typedef struct {
    vertex_t m_vertices[8];
    edge_t m_edges[12];
} cube_t;

const cube_t g_solved_cube = {
    .m_vertices = { 0, 1, 2, 3, 4, 5, 6, 7 },
    .m_edges    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }
};

#endif