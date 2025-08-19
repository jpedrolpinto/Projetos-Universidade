class VehicleCapability:
    """
    Defines what types of terrain a vehicle can traverse.
    capabilities: List of TerrainType values that the vehicle can handle
    """

    def __init__(self, capabilities: list):
        self.capabilities = capabilities

class Vehicle:
    """
    Represents a vehicle with specific capabilities and characteristics.

    Attributes:
        name: Vehicle identifier
        weight: Vehicle weight in kg
        capacity: Maximum load capacity
        speed: Base speed in km/h
        capabilities: TerrainTypes the vehicle can traverse
        fuel_capacity: Maximum fuel capacity
        current_fuel: Current fuel level
    """

    def __init__(self, name: str, weight: int, capacity: int, speed: int,
                 capabilities: VehicleCapability, fuel_capacity: int):
        self.name = name
        self.weight = weight
        self.capacity = capacity
        self.speed = speed
        self.capabilities = capabilities
        self.fuel_capacity = fuel_capacity
        self.current_fuel = fuel_capacity

    def can_complete_journey(self, distance):
        """Checks if vehicle has enough fuel for given distance"""
        fuel_needed = self.calculate_fuel_needed(distance)
        return self.current_fuel >= fuel_needed

    def calculate_fuel_needed(self, distance):
        """Calculates fuel needed for a given distance based on vehicle weight"""
        return (self.weight / 1000) * distance * 0.1

    def consume_fuel(self, distance):
        """Reduces current fuel based on distance traveled"""
        fuel_needed = self.calculate_fuel_needed(distance)
        self.current_fuel -= fuel_needed

    def refuel(self):
        """Refills vehicle to maximum fuel capacity"""
        self.current_fuel = self.fuel_capacity




