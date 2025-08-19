class TerrainType:
    """
    Defines the different types of terrain in the maze.
    """
    ROAD = ' '  # Regular road that all vehicles can traverse
    WATER = 'A'  # Water terrain that only boats can traverse
    ROUGH = 'T'  # Rough terrain that only trucks can traverse

    @staticmethod
    def get_type(cell):
        if cell == 'A':
            return TerrainType.WATER
        elif cell == 'T':
            return TerrainType.ROUGH
        elif cell == '#':
            return None
        else:  # ' ' ou 'I' ou 'F1', 'F2', etc.
            return TerrainType.ROAD
