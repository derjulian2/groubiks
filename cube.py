
from enum import Enum
from typing import TypeAlias

Face : TypeAlias = str
Move : TypeAlias = str 
Position  : TypeAlias = str

# erzeuger: list[Move] = ["R", "L", "U", "D", "F", "B"]

def invert_dict(a : dict) -> dict:
    res : dict = { }
    for i in a.keys():
        res.update({ a[i] :  i })
    return res

r_dict : dict = { "U" : "B",
                  "B" : "D",
                  "D" : "F",
                  "F" : "U" }

l_dict : dict = invert_dict(r_dict)

u_dict : dict = { "F" : "L",
                  "L" : "B",
                  "B" : "R",
                  "R" : "F" }

d_dict : dict = invert_dict(u_dict)

f_dict : dict = { "U" : "R",
                  "R" : "D",
                  "D" : "L",
                  "L" : "U" }

b_dict : dict = invert_dict(f_dict)

func_dict : dict = { "R" : r_dict,
                     "r" : l_dict,
                     "L" : l_dict,
                     "l" : r_dict,
                     "U" : u_dict,
                     "u" : d_dict,
                     "D" : d_dict,
                     "d" : u_dict,
                     "F" : f_dict,
                     "f" : b_dict,
                     "B" : b_dict,
                     "b" : f_dict }

class Piece:
    pass

class Corner(Piece):
    def __init__(self, colors : list[Face]):
        assert(len(colors) == 3)
        self.colors = colors

    def turn(self, mv : Move):
        pass

class Edge(Piece):
    def __init__(self, colors : list[Face], orientation : Position):
        assert(len(colors) == 2)
        self.colors = colors
        self.orientation = orientation

    def turn(self, mv : Move):
        for i in mv:
            if self.orientation[0] == i.capitalize():
                self.orientation = self.orientation.replace(self.orientation[1], func_dict[i][self.orientation[1]])
            elif self.orientation[1] == i.capitalize():
                self.orientation = self.orientation.replace(self.orientation[0], func_dict[i][self.orientation[0]])
                

class Cube:
    def __init__(self, corners : list[Face] , edges : list[Face] ):
        assert(len(corners) == 8)
        assert(len(edges) == 12)
        # weiße seite der weiß-rot-grünen corner
        self.corner_anchor : str = "RFU"
        # weiße seite der weiß-roten edge
        self.edge_anchor : str = "RU"
        self.corners : list[Face] = corners
        self.edges : list[Face] = edges

move : Move = "FRUUURRRUUURURRRFFFRURRRUUURRRFRFFF"
move_2 : Move = "FRuruRUrfRUrurFRf"
e1 = Edge([ "white", "red" ], "UB")
print(f"before: {e1.orientation}")
e1.turn(move_2)
print(f"applied move: {move_2}")
print(f"after: {e1.orientation}")