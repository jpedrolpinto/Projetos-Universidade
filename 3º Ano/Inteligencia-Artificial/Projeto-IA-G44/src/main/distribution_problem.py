import networkx as nx
import matplotlib.pyplot as plt
from queue import Queue, LifoQueue, PriorityQueue
import math

from src.models.node import Node, InfoCaminho
from src.models.path import VehiclePath
from src.models.metrics import AlgorithmMetrics
from src.models.terrain import TerrainType
from src.models.vehicle import Vehicle, VehicleCapability
from src.models.maze import Maze
from src.models.metrics import measure_algorithm_performance

class DistributionProblem:
    """
    Main class that handles the distribution problem solution.
    Combines maze environment, vehicles, and path-finding algorithms.
    """

    def __init__(self, maze: Maze, vehicles: list):
        self.maze = maze
        self.vehicles = vehicles
        self.g = self.build_graph()
        self.metrics = AlgorithmMetrics()

    def build_graph(self):
        """
        Builds a NetworkX graph representation of the maze.
        Nodes are passable positions, edges connect adjacent passable positions.
        """
        graph = nx.Graph()

        # Iterate through maze positions
        for i in range(len(self.maze.layout)):
            for j in range(len(self.maze.layout[i])):
                # Check if any vehicle can pass through this position
                can_pass = False
                for vehicle in self.vehicles:
                    if self.maze.is_passable(i, j, vehicle.capabilities):
                        can_pass = True
                        break

                if can_pass:
                    current_node = Node((i, j))
                    graph.add_node(current_node)

                    # Check adjacent positions
                    for di, dj in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                        ni, nj = i + di, j + dj
                        adj_can_pass = False
                        for vehicle in self.vehicles:
                            if self.maze.is_passable(ni, nj, vehicle.capabilities):
                                adj_can_pass = True
                                break

                        if adj_can_pass:
                            adjacent_node = Node((ni, nj))
                            graph.add_edge(current_node, adjacent_node)

        return graph

    def visualize_graph(self):
        """
        Creates a visual representation of the maze graph using matplotlib.
        """
        plt.figure(figsize=(8, 6))
        pos = {node: (node.position[1], -node.position[0]) for node in self.g.nodes()}

        # Draw basic nodes and edges
        nx.draw_networkx_nodes(self.g, pos, node_color='lightblue', node_size=300)
        nx.draw_networkx_edges(self.g, pos)

        # Add position labels to nodes
        labels = {node: f"({node.position[0]},{node.position[1]})" for node in self.g.nodes()}
        nx.draw_networkx_labels(self.g, pos, labels, font_size=8)

        # Highlight start node in green
        start_node = Node(self.maze.start)
        if start_node in self.g:
            nx.draw_networkx_nodes(self.g, pos, nodelist=[start_node],
                                   node_color='green', node_size=300)

        # Highlight goal nodes in red
        goal_nodes = [Node(goal) for goal in self.maze.goals]
        goal_nodes = [node for node in goal_nodes if node in self.g]
        if goal_nodes:
            nx.draw_networkx_nodes(self.g, pos, nodelist=goal_nodes,
                                   node_color='red', node_size=300)

        plt.title("Maze Graph Visualization")
        plt.grid(True)
        plt.axis('equal')
        plt.show()

    def manhattan_distance(self, start: tuple, goal: tuple) -> float:
        """
        Calculates Manhattan distance between two points.
        Used as heuristic in A* and Greedy search.
        """
        return abs(start[0] - goal[0]) + abs(start[1] - goal[1])

    def euclidean_distance(self, start: tuple, goal: tuple) -> float:
        """
        Calculates Euclidean distance between two points.
        Alternative heuristic for pathfinding algorithms.
        """
        return math.sqrt((start[0] - goal[0]) ** 2 + (start[1] - goal[1]) ** 2)

    @measure_algorithm_performance
    def procura_BFS(self, start: tuple, end: Node = None):
        start_node = Node(start)
        if end is None:
            end_nodes = {Node(x) for x in self.maze.goals}
        else:
            end_nodes = {end}

        if start_node not in self.g or not all(end_node in self.g for end_node in end_nodes):
            return InfoCaminho([], [])

        visited = set()
        queue = Queue()
        queue.put(start_node)
        visited.add(start_node)

        parent = {start_node: None}
        path_traversed = []

        while not queue.empty():
            current = queue.get()
            path_traversed.append(current)

            if current in end_nodes:
                return self._reconstruct_path(current, parent, path_traversed)

            for neighbor in self.g.neighbors(current):
                if neighbor not in visited:
                    visited.add(neighbor)
                    parent[neighbor] = current
                    queue.put(neighbor)

        return InfoCaminho([], path_traversed)

    @measure_algorithm_performance
    def procura_DFS(self, start: tuple, end: Node = None):
        start_node = Node(start)
        if end is None:
            end_nodes = {Node(x) for x in self.maze.goals}
        else:
            end_nodes = {end}

        if start_node not in self.g or not all(end_node in self.g for end_node in end_nodes):
            return InfoCaminho([], [])

        visited = set()
        stack = LifoQueue()
        stack.put(start_node)

        parent = {start_node: None}
        path_traversed = []

        while not stack.empty():
            current = stack.get()
            if current not in visited:
                visited.add(current)
                path_traversed.append(current)

                if current in end_nodes:
                    return self._reconstruct_path(current, parent, path_traversed)

                for neighbor in self.g.neighbors(current):
                    if neighbor not in visited:
                        parent[neighbor] = current
                        stack.put(neighbor)

        return InfoCaminho([], path_traversed)

    @measure_algorithm_performance
    def procura_greedy(self, start: tuple, end: Node = None):
        start_node = Node(start)
        if end is None:
            end_nodes = {Node(x) for x in self.maze.goals}
        else:
            end_nodes = {end}

        if start_node not in self.g or not all(end_node in self.g for end_node in end_nodes):
            return InfoCaminho([], [])

        goal_pos = next(iter(end_nodes)).position

        from itertools import count
        counter = count()

        visited = set()
        queue = PriorityQueue()
        queue.put((self.manhattan_distance(start_node.position, goal_pos), next(counter), start_node))

        parent = {start_node: None}
        path_traversed = []

        while not queue.empty():
            _, _, current = queue.get()
            if current not in visited:
                visited.add(current)
                path_traversed.append(current)

                if current in end_nodes:
                    return self._reconstruct_path(current, parent, path_traversed)

                for neighbor in self.g.neighbors(current):
                    if neighbor not in visited:
                        parent[neighbor] = current
                        priority = self.manhattan_distance(neighbor.position, goal_pos)
                        queue.put((priority, next(counter), neighbor))

        return InfoCaminho([], path_traversed)

    @measure_algorithm_performance
    def procura_astar(self, start: tuple, end: Node = None):
        start_node = Node(start)
        if end is None:
            end_nodes = {Node(x) for x in self.maze.goals}
        else:
            end_nodes = {end}

        if start_node not in self.g or not all(end_node in self.g for end_node in end_nodes):
            return InfoCaminho([], [])

        goal_pos = next(iter(end_nodes)).position

        g_score = {start_node: 0}
        f_score = {start_node: self.manhattan_distance(start_node.position, goal_pos)}

        from itertools import count
        counter = count()

        open_set = PriorityQueue()
        open_set.put((f_score[start_node], next(counter), start_node))

        parent = {start_node: None}
        path_traversed = []
        closed_set = set()

        while not open_set.empty():
            current = open_set.get()[2]

            if current in closed_set:
                continue

            closed_set.add(current)
            path_traversed.append(current)

            if current in end_nodes:
                return self._reconstruct_path(current, parent, path_traversed)

            for neighbor in self.g.neighbors(current):
                if neighbor in closed_set:
                    continue

                tentative_g_score = g_score[current] + 1

                if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                    parent[neighbor] = current
                    g_score[neighbor] = tentative_g_score
                    f_score[neighbor] = g_score[neighbor] + self.manhattan_distance(neighbor.position, goal_pos)
                    open_set.put((f_score[neighbor], next(counter), neighbor))

        return InfoCaminho([], path_traversed)

    @measure_algorithm_performance
    def procura_dijkstra(self, start: tuple, end: Node = None):
        start_node = Node(start)
        if end is None:
            end_nodes = {Node(x) for x in self.maze.goals}
        else:
            end_nodes = {end}

        if start_node not in self.g or not all(end_node in self.g for end_node in end_nodes):
            return InfoCaminho([], [])

        # Inicializa as estruturas de dados
        distances = {start_node: 0}  # Custos do inÃ­cio atÃ© cada nÃ³
        parent = {start_node: None}  # Para reconstruir o caminho
        visited = set()  # NÃ³s jÃ¡ visitados
        path_traversed = []  # NÃ³s explorados na ordem

        # Usa PriorityQueue para sempre pegar o nÃ³ com menor custo
        from itertools import count
        counter = count()  # Para desempate quando custos sÃ£o iguais
        pq = PriorityQueue()
        pq.put((0, next(counter), start_node))

        while not pq.empty():
            # Pega o nÃ³ com menor custo
            current_cost, _, current_node = pq.get()

            # Se jÃ¡ visitamos este nÃ³ com um custo menor, pula
            if current_node in visited:
                continue

            visited.add(current_node)
            path_traversed.append(current_node)

            # Verifica se chegamos a um objetivo
            if current_node in end_nodes:
                return self._reconstruct_path(current_node, parent, path_traversed)

            # Explora os vizinhos
            for neighbor in self.g.neighbors(current_node):
                if neighbor in visited:
                    continue

                # Calcula novo custo (cada movimento tem custo 1)
                new_cost = current_cost + 1

                # Se encontramos um caminho melhor atÃ© este vizinho
                if neighbor not in distances or new_cost < distances[neighbor]:
                    distances[neighbor] = new_cost
                    parent[neighbor] = current_node
                    pq.put((new_cost, next(counter), neighbor))

        return InfoCaminho([], path_traversed)

    def analyze_path_with_capacity(self, path: list) -> list:
        """
        Analyzes a path and determines optimal vehicle assignments for each segment.
        """
        vehicle_assignments = []
        current_pos_index = 0

        while current_pos_index < len(path):
            # Get current position
            current_node = path[current_pos_index]
            i, j = current_node.position
            terrain = self.maze.layout[i][j]

            # Find suitable vehicle
            best_vehicle = None
            best_score = -1

            for vehicle in self.vehicles:
                if vehicle.current_fuel <= 0:
                    continue

                # Verificar estritamente se o veículo pode atravessar este terreno
                can_traverse = False
                if terrain == 'A':  # Agua
                    can_traverse = TerrainType.WATER in vehicle.capabilities.capabilities
                elif terrain == 'T':  # Terreno acidentado
                    can_traverse = TerrainType.ROUGH in vehicle.capabilities.capabilities
                else:  # Estrada normal
                    can_traverse = True
                    # Barco nao pode andar em terra
                    if TerrainType.WATER in vehicle.capabilities.capabilities and terrain != 'A':
                        can_traverse = False

                if can_traverse:
                    score = (
                            (vehicle.speed / vehicle.weight) * 1000 *
                            (vehicle.current_fuel / vehicle.fuel_capacity) *
                            self.maze.weather.get_speed_multiplier(vehicle.name)
                    )
                    if score > best_score: #testa os veiculos com os seus scores nesta posiçao
                        best_score = score
                        best_vehicle = vehicle

            if not best_vehicle:
                return []

            # Calculate segment for this vehicle
            segment_positions = []
            next_pos_index = current_pos_index
            distance = 0

            while next_pos_index < len(path):
                next_node = path[next_pos_index]
                ni, nj = next_node.position
                next_terrain = self.maze.layout[ni][nj]

                # VerificaÃ§Ã£o estrita de terreno para o veÃ­culo atual
                can_traverse = False
                if next_terrain == 'A':  # Agua
                    can_traverse = TerrainType.WATER in best_vehicle.capabilities.capabilities
                elif next_terrain == 'T':  # Terreno acidentado
                    can_traverse = TerrainType.ROUGH in best_vehicle.capabilities.capabilities
                else:  # Estrada normal
                    can_traverse = True
                    # Barco nao pode andar em terra
                    if TerrainType.WATER in best_vehicle.capabilities.capabilities and next_terrain != 'A':
                        can_traverse = False

                # Stop if can't traverse or reached capacity
                if not can_traverse or distance + 10 > best_vehicle.capacity:
                    break

                segment_positions.append((ni, nj))
                if next_pos_index > current_pos_index:
                    distance += 10
                next_pos_index += 1

            if segment_positions:
                # Create vehicle path
                path_assignment = VehiclePath(best_vehicle, segment_positions)

                # Quick check for weather effects
                weather_affected = False
                for pos in segment_positions:
                    if pos in self.maze.weather.affected_zones:
                        weather_affected = True
                        break
                path_assignment.weather_affected = weather_affected

                vehicle_assignments.append(path_assignment)
                best_vehicle.consume_fuel(distance)

                # Move to next uncovered position
                current_pos_index = next_pos_index
            else:
                current_pos_index += 1

        return vehicle_assignments

    def encontrar_melhor_rota_completa(self, algoritmo=''):
        """
        Finds the optimal route to visit all zones considering multiple factors.
        """
        # Initialize metrics for this route search
        self.metrics.start_new_route_search(algoritmo)

        algoritmos = {
            'BFS': self.procura_BFS,
            'DFS': self.procura_DFS,
            'GREEDY': self.procura_greedy,
            'ASTAR': self.procura_astar,
            'DIJKSTRA': self.procura_dijkstra
        }

        if algoritmo not in algoritmos:
            print(f"Algoritmo {algoritmo} não implementado!")
            return None

        procura_func = algoritmos[algoritmo]

        # Update weather conditions
        self.maze.update_weather()
        print(f"\nCondições Meteorológicas Atuais: {self.maze.weather.current_weather}")
        print(f"Zonas Afetadas: {self.maze.weather.affected_zones}")

        # Calculate priority scores for each zone
        zonas_nao_visitadas = []
        for goal in self.maze.goals:
            zone = self.maze.zones[goal]
            priority_score = (
                    zone.priority * 0.4 +
                    (1 / max(1, zone.time_remaining())) * 0.4 +
                    (zone.population / 500) * 0.2
            )
            zonas_nao_visitadas.append({
                'posicao': goal,
                'zone': zone,
                'score': priority_score,
                'nome': self.maze.goal_names[goal]
            })

        # Sort zones by priority score
        zonas_nao_visitadas.sort(key=lambda x: x['score'], reverse=True)

        # Initialize variables for complete route
        rota_completa = []
        posicao_atual = self.maze.start
        custo_total = 0
        distribuicao_total_veiculos = []

        print("\nPlanejando rota completa:")
        print("-" * 50)

        # Process each zone in priority order
        for zona in zonas_nao_visitadas:
            print(f"\nAnalisando rota para {zona['nome']}:")
            print(f"Prioridade: {zona['zone'].priority}/5")
            print(f"População: {zona['zone'].population}")
            print(f"Tempo Restante: {zona['zone'].time_remaining():.1f} horas")

            # Find path to next zone
            result = procura_func(posicao_atual, Node(zona['posicao']))

            if not result.path:
                print(f"Não foi possível encontrar caminho para {zona['nome']}!")
                continue

            # Analyze vehicle distribution for this segment
            vehicle_assignments = self.analyze_path_with_capacity(result.path)

            if not vehicle_assignments:
                print("Não há veículos disponíveis com combustível suficiente!")
                continue

            # Calculate segment cost
            custo_segmento = 0
            for vp in vehicle_assignments:
                base_cost = vp.distance
                weather_multiplier = 1.5 if vp.weather_affected else 1.0
                vehicle_cost = base_cost * weather_multiplier
                custo_segmento += vehicle_cost

            # Add penalty for vehicle changes
            custo_segmento += (len(vehicle_assignments) - 1) * 20

            # Update route information
            rota_completa.extend(result.path[:-1])
            distribuicao_total_veiculos.extend(vehicle_assignments)
            custo_total += custo_segmento
            posicao_atual = zona['posicao']

            print(f"Distância do segmento: {len(result.path) - 1} movimentos")
            print(f"Custo do segmento: {custo_segmento}")
            print("\nDistribuição de veículos para este segmento:")
            for vp in vehicle_assignments:
                print(f"- {vp}")

        # Add final position
        if rota_completa:
            rota_completa.append(Node(posicao_atual))

            # Print complete route summary
            print("\n" + "=" * 30)
            print("Rota Completa")
            print("=" * 30)
            for vp in distribuicao_total_veiculos:
                print(f"- {vp}")
            print("\nCusto Total da Rota:", custo_total)
            print("=" * 30)

            # Print metrics comparison
            self.metrics.print_comparison()

        return {
            'rota': rota_completa,
            'custo_total': custo_total,
            'distribuicao_veiculos': distribuicao_total_veiculos
        }

    @staticmethod
    def _reconstruct_path(current: Node, parent: dict, path_traversed: list) -> InfoCaminho:
        """
        Reconstructs the path from start to goal using parent dictionary.
        """
        path = []
        while current is not None:
            path.append(current)
            current = parent[current]
        path.reverse()
        return InfoCaminho(path, path_traversed)

def create_specific_scenario():
    """
    Creates a predefined scenario for testing the distribution problem.

    Returns:
        DistributionProblem instance with:
        - Maze with mixed terrain types
        - Three delivery zones
        - Three vehicle types (car, truck, boat)
    """
    # Define maze layout
    labyrinth = [
        ['#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'],
        ['I', ' ', '#', 'T', 'T', '#', ' ', 'T', 'T', ' ', ' ', ' ', ' ', 'F1'],
        [' ', 'T', 'T', 'T', '#', 'T', ' ', ' ', 'T', 'T', ' ', ' ', ' ', ' '],
        ['#', 'T', 'T', ' ', ' ', 'T', 'T', ' ', '#', 'A', 'A', 'A', 'A', ' '],
        ['#', ' ', 'T', 'A', 'A', ' ', ' ', ' ', '#', 'A', 'A', 'A', ' ', ' '],
        ['#', 'A', 'A', 'A', 'A', 'A', ' ', ' ', ' ', '#', 'A', ' ', ' ', ' '],
        ['#', 'A', 'A', '#', ' ', ' ', '#', ' ', ' ', ' ', '#', 'A', ' ', ' '],
        ['#', 'A', 'A', '#', ' ', ' ', '#', ' ', '#', ' ', 'A', 'A', 'A', ' '],
        ['#', 'A', '#', '#', ' ', 'T', 'T', ' ', ' ', 'A', 'A', ' ', '#', '#'],
        ['#', ' ', 'A', 'A', 'A', ' ', 'T', ' ', 'T', 'T', 'A', ' ', '#', '#'],
        ['#', ' ', 'A', 'A', 'A', ' ', 'T', ' ', 'T', 'T', 'A', 'T', '#', '#'],
        ['#', ' ', 'A', 'A', 'A', ' ', '#', ' ', ' ', 'A', 'A', 'T', '#', '#'],
        ['#', ' ', 'F3', '#', ' ', ' ', '#', ' ', 'A', ' ', ' ', 'T', 'T', 'F2'],
        ['#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#']
    ]
    maze = Maze(labyrinth)

    # Define vehicle capabilities
    car_capabilities = VehicleCapability([TerrainType.ROAD])
    truck_capabilities = VehicleCapability([TerrainType.ROAD, TerrainType.ROUGH])
    boat_capabilities = VehicleCapability([TerrainType.WATER])

    # Create vehicles
    vehicles = [
        Vehicle("car", 1500, 500, 80, car_capabilities, fuel_capacity=1000),
        Vehicle("truck", 2000, 800, 60, truck_capabilities, fuel_capacity=1500),
        Vehicle("boat", 1200, 400, 50, boat_capabilities, fuel_capacity=800)
    ]

    return DistributionProblem(maze, vehicles)

def menu(problema: DistributionProblem):
    """
    Interactive menu for the distribution problem.

    Provides options to:
    1. Find complete route using different algorithms
    2. Visualize the search graph
    3. Update weather conditions
    4. Check vehicle status
    5. Check zone status

    Args:
        problema: DistributionProblem instance to work with
    """
    while True:
        print("\nEscolha uma opcao:")
        print("1 - Encontrar rota completa com BFS")
        print("2 - Encontrar rota completa com DFS")
        print("3 - Encontrar rota completa com Greedy")
        print("4 - Encontrar rota completa com A*")
        print("5 - Encontrar rota completa com Dijkstra")
        print("6 - Visualizar grafo de procura")
        print("7 - Atualizar condicoes meteorologicas")
        print("8 - Ver status dos veiculos")
        print("9 - Ver status das zonas")
        print("10 - Sair")

        try:
            opcao = int(input("Digite o numero da opcao: "))

            if opcao in range(1, 6):
                algoritmos = {
                    1: "BFS",
                    2: "DFS",
                    3: "GREEDY",
                    4: "ASTAR",
                    5: "DIJKSTRA"
                }
                problema.encontrar_melhor_rota_completa(algoritmos[opcao])

            elif opcao == 6:
                problema.visualize_graph()

            elif opcao == 7:
                problema.maze.update_weather()
                print(f"\nNovas condicoes meteorologicas: {problema.maze.weather.current_weather}")
                print(f"Zonas afetadas: {problema.maze.weather.affected_zones}")

            elif opcao == 8:
                print("\nStatus dos Veiculos:")
                for vehicle in problema.vehicles:
                    print(f"\n{vehicle.name.upper()}:")
                    print(f"Combustivel: {vehicle.current_fuel}/{vehicle.fuel_capacity}")
                    print(f"Capacidade de carga: {vehicle.capacity}")
                    print(f"Velocidade: {vehicle.speed}")
                    print(f"Peso: {vehicle.weight}")

            elif opcao == 9:
                print("\nStatus das Zonas:")
                for pos, zone in problema.maze.zones.items():
                    print(f"\nZona {problema.maze.goal_names[pos]}:")
                    print(f"Prioridade: {zone.priority}/5")
                    print(f"Populacao: {zone.population}")
                    print(f"Tempo restante: {zone.time_remaining():.1f} horas")
                    if pos in problema.maze.weather.affected_zones:
                        print("Zona Afetada por condicoes meteorologicas adversas!")

            elif opcao == 10:
                print("Saindo...")
                break

            else:
                print("Opcao invalida. Por favor escolha um numero entre 1 e 10.")

        except ValueError:
            print("Por favor insira um numero valido.")

if __name__ == "__main__":
    """
    Main program entry point.
    Creates a scenario and starts the interactive menu.
    """
    problem = create_specific_scenario()
    menu(problem)
