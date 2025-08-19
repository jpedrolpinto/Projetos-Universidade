import random
from datetime import datetime
from .terrain import TerrainType
from .weather import WeatherCondition
from .zone import Zone

class Maze:
    """
    Represents the problem environment with terrain, zones, and weather.

    The maze is represented as a 2D grid where:
    - 'I' is the starting position
    - 'F1', 'F2', etc. are delivery zones
    - '#' are walls/obstacles
    - 'A' is water terrain
    - 'T' is rough terrain
    - ' ' is normal road
    """

    def __init__(self, layout):
        self.layout = layout
        self.start = None
        self.goals = []
        self.goal_names = {}
        self.zones = {}
        self.weather = WeatherCondition()
        self._find_start_and_goals()

    def _find_start_and_goals(self):
        """Identifies start position and goal zones in the maze"""
        for i in range(len(self.layout)):
            for j in range(len(self.layout[i])):
                cell = self.layout[i][j]
                if cell == 'I':
                    self.start = (i, j)
                elif cell.startswith('F'):
                    self.goals.append((i, j))
                    self.goal_names[(i, j)] = cell
                    # Create zone with random characteristics
                    self.zones[(i, j)] = Zone(
                        (i, j),
                        priority=random.randint(1, 5),
                        population=random.randint(50, 500),
                        time_window=random.randint(12, 48)
                    )

    def update_weather(self):
        """Randomly updates weather conditions and affected zones"""
        weather_types = [WeatherCondition.CLEAR, WeatherCondition.RAIN, WeatherCondition.STORM]
        new_weather = random.choice(weather_types)
        affected_zones = set(random.sample(self.goals, k=random.randint(0, len(self.goals))))
        self.weather.update_weather(new_weather, affected_zones)

    def is_passable(self, i, j, capabilities):
        """
        Checks if a position is passable by a vehicle with given capabilities.
        Takes into account terrain type, weather conditions, and maze boundaries.
        """
        # Check maze boundaries
        if i < 0 or i >= len(self.layout) or j < 0 or j >= len(self.layout[0]):
            return False

        cell = self.layout[i][j]

        # Check for storm-affected zones
        if self.weather.current_weather == WeatherCondition.STORM and (i, j) in self.weather.affected_zones:
            return False

        # Check different terrain types
        if cell == '#':  # Wall
            return False
        if cell == 'I' or cell.startswith('F'):  # Start or goal
            return True
        if cell == ' ':  # Normal road
            return True
        if cell == 'A':  # Water
            return TerrainType.WATER in capabilities.capabilities
        if cell == 'T':  # Rough terrain
            return TerrainType.ROUGH in capabilities.capabilities

        return False
