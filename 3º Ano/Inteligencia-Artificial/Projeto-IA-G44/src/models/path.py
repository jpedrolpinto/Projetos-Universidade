class VehiclePath:
    """
    Represents a segment of the route assigned to a specific vehicle.

    Attributes:
        vehicle: Vehicle assigned to this path
        positions: List of positions traversed
        distance: Length of path in distance units
        weather_affected: Boolean indicating if path is affected by weather
    """

    def __init__(self, vehicle, positions):
        self.vehicle = vehicle
        self.positions = positions  # List of all positions traversed
        self.distance = len(positions) * 10  # each cell = 10km
        self.weather_affected = False

    def __repr__(self):
        weather_status = " (Weather Affected)" if self.weather_affected else ""
        positions_str = " -> ".join(map(str, self.positions))
        return f"{self.vehicle.name}: {positions_str} (distance: {self.distance}){weather_status}"

class PathSegment:
    """
    Represents a continuous segment of the path with the same terrain type.
    Each cell represents 10km of distance.
    """

    def __init__(self, positions, terrain_type):
        self.positions = positions
        self.terrain_type = terrain_type
        self.distance = len(positions) * 10  # each cell = 10km

    def __repr__(self):
        return f"Segment({self.terrain_type}: {self.positions}, distance: {self.distance})"

