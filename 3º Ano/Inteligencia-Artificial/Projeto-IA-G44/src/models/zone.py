from datetime import datetime
from datetime import timedelta

class Zone:
    """
    Represents a distribution zone with specific characteristics.

    Attributes:
        position: Tuple (x,y) representing zone location
        priority: Integer 1-5, where 5 is most critical
        population: Number of people in the zone
        time_window: Hours available to reach the zone
        creation_time: When the zone was created
    """

    def __init__(self, position, priority=1, population=100, time_window=24):
        self.position = position
        self.priority = priority
        self.population = population
        self.creation_time = datetime.now()
        self.time_window = timedelta(hours=time_window)

    def time_remaining(self):
        """Calculates remaining hours before zone becomes critical"""
        elapsed = datetime.now() - self.creation_time
        remaining = self.time_window - elapsed
        return max(remaining.total_seconds() / 3600, 0)  # Convert to hours, minimum 0

    def is_critical(self):
        """Returns True if less than 6 hours remaining"""
        return self.time_remaining() < 6

