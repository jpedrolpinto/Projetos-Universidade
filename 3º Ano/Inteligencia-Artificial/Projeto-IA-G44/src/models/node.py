class Node:
    """
    Represents a position in the maze graph.
    Includes equality and hash methods for graph operations.
    """

    def __init__(self, position: tuple):
        self.position = position

    def __eq__(self, other):
        if other is None:
            return False
        return self.position == other.position

    def __hash__(self):
        return hash(self.position)

    def __repr__(self):
        return f"Node{self.position}"

class InfoCaminho:
    """
    Stores information about a found path and nodes traversed during search.
    path: List of nodes in final path
    path_traversed: List of all nodes visited during search
    """

    def __init__(self, path, path_traversed):
        self.path = path
        self.path_traversed = path_traversed
