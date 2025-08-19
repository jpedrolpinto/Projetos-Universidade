class WeatherCondition:
    """
    Manages weather conditions that affect vehicle movement.
    - CLEAR: Normal conditions
    - RAIN: Reduces speed to 70%
    - STORM: Reduces speed to 40% and may block certain zones
    """
    CLEAR = "CLEAR"
    RAIN = "RAIN"
    STORM = "STORM"

    def __init__(self):
        self.current_weather = self.CLEAR
        self.affected_zones = set()  # Stores zones affected by weather

    def update_weather(self, new_weather, affected_zones):
        """Updates weather condition and affected zones"""
        self.current_weather = new_weather
        self.affected_zones = affected_zones

    def get_speed_multiplier(self, vehicle_type):
        """Returns speed multiplier based on current weather conditions"""
        multipliers = {
            self.CLEAR: 1.0,  # No speed reduction
            self.RAIN: 0.7,  # 30% speed reduction
            self.STORM: 0.4  # 60% speed reduction
        }
        return multipliers.get(self.current_weather, 1.0)

