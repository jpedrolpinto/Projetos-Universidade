from collections import defaultdict
import psutil
import time

class AlgorithmMetrics:
    """
    Classe para coletar e armazenar metricas de desempenho dos algoritmos.
    """

    def __init__(self):
        self.execution_times = defaultdict(list)
        self.memory_usage = defaultdict(list)
        self.nodes_explored = defaultdict(list)
        self.path_lengths = defaultdict(list)
        # Add accumulators for the current route search
        self.current_route_metrics = defaultdict(lambda: {'nodes': 0, 'path_length': 0})
        self.current_algorithm = None

    def start_new_route_search(self, algorithm_name):
        """Reset accumulators when starting a new complete route search"""
        self.current_algorithm = algorithm_name
        self.current_route_metrics[algorithm_name] = {
            'nodes': 0,
            'path_length': 0,
            'total_time': 0,
            'max_memory': 0
        }

    def add_metrics(self, algorithm_name, execution_time, memory_used, nodes_explored, path_length):
        """
        Accumulate metrics for the current route search and store individual search metrics
        """
        # Update current route accumulators
        if algorithm_name == self.current_algorithm:
            self.current_route_metrics[algorithm_name]['nodes'] += nodes_explored
            self.current_route_metrics[algorithm_name]['path_length'] += path_length
            self.current_route_metrics[algorithm_name]['total_time'] += execution_time
            self.current_route_metrics[algorithm_name]['max_memory'] = max(
                self.current_route_metrics[algorithm_name].get('max_memory', 0),
                memory_used
            )
        else:
            # If algorithm changed, store the previous route's accumulated metrics
            self._store_accumulated_metrics()
            # Start new accumulation
            self.start_new_route_search(algorithm_name)
            self.current_route_metrics[algorithm_name]['nodes'] = nodes_explored
            self.current_route_metrics[algorithm_name]['path_length'] = path_length
            self.current_route_metrics[algorithm_name]['total_time'] = execution_time
            self.current_route_metrics[algorithm_name]['max_memory'] = memory_used

    def _store_accumulated_metrics(self):
        """Store the accumulated metrics for the current route"""
        if self.current_algorithm and self.current_algorithm in self.current_route_metrics:
            metrics = self.current_route_metrics[self.current_algorithm]
            self.execution_times[self.current_algorithm].append(metrics['total_time'])
            self.memory_usage[self.current_algorithm].append(metrics['max_memory'])
            self.nodes_explored[self.current_algorithm].append(metrics['nodes'])
            self.path_lengths[self.current_algorithm].append(metrics['path_length'])

    def get_average_metrics(self, algorithm_name):
        """Calculate average metrics for an algorithm, including current route if not stored"""
        # Make sure current route metrics are stored
        if algorithm_name == self.current_algorithm:
            self._store_accumulated_metrics()

        if not self.execution_times[algorithm_name]:
            return {
                'avg_time': 0,
                'avg_memory': 0,
                'avg_nodes': 0,
                'avg_path_length': 0
            }

        return {
            'avg_time': sum(self.execution_times[algorithm_name]) / len(self.execution_times[algorithm_name]),
            'avg_memory': sum(self.memory_usage[algorithm_name]) / len(self.memory_usage[algorithm_name]),
            'avg_nodes': sum(self.nodes_explored[algorithm_name]) / len(self.nodes_explored[algorithm_name]),
            'avg_path_length': sum(self.path_lengths[algorithm_name]) / len(self.path_lengths[algorithm_name])
        }

    def print_comparison(self):
        """Print detailed comparison between algorithms, ensuring current route is included"""
        # Store any pending accumulated metrics
        if self.current_algorithm:
            self._store_accumulated_metrics()

        print("\n=== Comparacao de Desempenho dos Algoritmos ===")
        print("\nMetricas medias por algoritmo:")
        print("-" * 80)
        print(f"{'Algoritmo':<15} {'Tempo (s)':<12} {'Memoria (MB)':<15} {'Nos Explorados':<15} {'Tamanho Rota':<12}")
        print("-" * 80)

        for algo in self.execution_times.keys():
            metrics = self.get_average_metrics(algo)
            print(f"{algo:<15} {metrics['avg_time']:<12.4f} {metrics['avg_memory']:<15.2f} "
                  f"{metrics['avg_nodes']:<15.0f} {metrics['avg_path_length']:<12.0f}")

def measure_algorithm_performance(func):
    """
    Decorator to measure algorithm performance, now handling accumulated metrics
    """
    def wrapper(*args, **kwargs):
        start_time = time.time()
        start_memory = psutil.Process().memory_info().rss / 1024 / 1024

        result = func(*args, **kwargs)

        end_time = time.time()
        end_memory = psutil.Process().memory_info().rss / 1024 / 1024

        execution_time = end_time - start_time
        memory_used = end_memory - start_memory
        nodes_explored = len(result.path_traversed) if result else 0
        path_length = len(result.path) if result else 0

        if hasattr(args[0], 'metrics'):
            algo_name = func.__name__.replace('procura_', '').upper()
            args[0].metrics.add_metrics(
                algo_name,
                execution_time,
                memory_used,
                nodes_explored,
                path_length
            )

        return result

    return wrapper
