<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import Notification from '../common/Notification.vue';
import api from '../../services/api';
import { 
  loadDirectorDashboardData,
  getCourseName as getCourseNameUtil,
  getStudentName as getStudentNameUtil
} from '../../utils/common';
import { useRouter } from 'vue-router'

// Dados do diretor e estatísticas
const directorData = ref(null);
const courses = ref([]);
const shifts = ref([]);
const students = ref([]);
const pendingRequests = ref([]);
const loading = ref(true);
const error = ref(null);

const router = useRouter();

// ID do diretor (obtido do localStorage)
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Notifications
const notification = ref({
  show: false,
  message: '',
  type: 'info'
});

// Carregar todos os dados necessários usando a função otimizada do common.js
async function loadDashboardData() {
  try {
    // Usar a função otimizada loadDirectorDashboardData que carrega tudo de uma vez
    const success = await loadDirectorDashboardData(
      directorId,
      directorData,
      courses,
      shifts,
      students,
      pendingRequests,
      loading,
      error
    );
    
    if (!success) {
      console.error('Falha ao carregar dashboard do diretor');
    }
  } catch (err) {
    console.error('Erro ao carregar dados do dashboard:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente mais tarde.';
    loading.value = false;
  }
}

// Computar estatísticas para exibição no dashboard
const activeCoursesCount = computed(() => {
  return courses.value.length;
});

const totalStudentsCount = computed(() => {
  return students.value.length;
});

const pendingRequestsCount = computed(() => {
  return pendingRequests.value.length;
});

const studentsWithoutShiftCount = computed(() => {
  return 0;
});

// Obter o nome da UC 
function getCourseName(courseId) {
  return getCourseNameUtil(courses.value, courseId) || '';
}

// Obter o nome do aluno 
function getStudentName(studentId) {
  return getStudentNameUtil(students.value, studentId) || '';
}

// Obter dados de UC para o gráfico
const coursesForGraph = computed(() => {
  if (!courses.value.length) return [];
  
  return courses.value.map(course => ({
    id: course.id,
    abbreviation: course.abbreviation
  })).slice(0, 25); // Limitar a 25 UCs para não ficar muito largo
});

// Ver detalhes do pedido
function viewRequestDetails() {
  // Redirecionar para a página de detalhes do pedido
  router.push(`/pedidos-mudanca`);
}

// Fechar notificação
function closeNotification() {
  notification.value.show = false;
}

// Ver horários do curso
function viewCourseSchedule() {
  router.push('/diretor/horarios');
}

// Carregar dados ao montar o componente
onMounted(() => {
  loadDashboardData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name" 
    userType="diretor"
    activeMenuItem="dashboard"
  >
    <div class="dashboard-container">
      <h2 class="page-title">Dashboard</h2>
      <p class="page-subtitle">Visão geral do semestre atual</p>
      
      <!-- Conteúdo (visível apenas quando não está carregando) -->
      <div v-if="!loading && !error">
        <!-- Cards de estatísticas -->
        <div class="stats-cards">
          <div class="stat-card">
            <h3>UCs Ativas</h3>
            <p class="stat-value stat-blue">{{ activeCoursesCount }}</p>
          </div>
          
          <div class="stat-card">
            <h3>Total de Alunos</h3>
            <p class="stat-value stat-blue">{{ totalStudentsCount }}</p>
          </div>
          
          <div class="stat-card">
            <h3>Pedidos Pendentes</h3>
            <p class="stat-value stat-red">{{ pendingRequestsCount }}</p>
          </div>
          
          <div class="stat-card">
            <h3>Alunos sem Turno</h3>
            <p class="stat-value stat-red">{{ studentsWithoutShiftCount }}</p>
          </div>
        </div>
        
        <!-- Gráfico de ocupação de turnos -->
        <div class="chart-section">
          <h3 class="section-title">Ocupação das UC</h3>
          
          <div class="chart-container">
            <div class="chart">
              <div class="bars-container">
                <div 
                  v-for="course in coursesForGraph" 
                  :key="course.id" 
                  class="bar-column"
                >
                  <div class="bar-wrapper">
                    <div class="bar-empty"></div>
                    <div class="bar-filled"></div>
                  </div>
                  <div class="bar-label">{{ course.abbreviation }}</div>
                </div>
              </div>
            </div>
          </div>
          
          <!-- Botão adicionado para ver horários do curso -->
          <div class="chart-actions">
            <table class="action-table">
              <tbody> <!-- Add this tbody wrapper -->
                <tr>
                  <td>
                    <button @click="viewCourseSchedule" class="view-schedule-button">
                      Ver Horários do Curso
                    </button>
                  </td>
                </tr>
              </tbody>
            </table>
          </div>
        </div>
        
        <!-- Tabela de pedidos recentes -->
        <div class="requests-section">
          <h3 class="section-title">Pedidos Recentes</h3>
          
          <div v-if="pendingRequests.length > 0">
            <table class="requests-table">
              <thead>
                <tr>
                  <th>Aluno</th>
                  <th>UC</th>
                  <th>Pedido</th>
                  <th>Data</th>
                  <th>Status</th>
                  <th>Ação</th>
                </tr>
              </thead>
              <tbody>
                <tr v-for="request in pendingRequests.slice(0, 1)" :key="request.id">
                  <td>{{ getStudentName(request.studentId) || 'Mafalda' }}</td>
                  <td>{{ getCourseName(shifts.find(s => s.id === request.shiftId)?.courseId) || 'FP' }}</td>
                  <td>Mudança de T1 para T1</td>
                  <td>Hoje, 15:25</td>
                  <td>
                    <span class="status-badge">Pendente</span>
                  </td>
                  <td>
                    <button @click="viewRequestDetails(request.id)" class="view-button">
                      Ver
                    </button>
                  </td>
                </tr>
              </tbody>
            </table>
          </div>
          <div v-else class="no-requests">
            Não há pedidos pendentes no momento.
          </div>
        </div>
      </div>
      
      <!-- Loading state -->
      <div v-else-if="loading" class="loading-state">
        Carregando dados...
      </div>
      
      <!-- Error state -->
      <div v-else-if="error" class="error-state">
        {{ error }}
        <button @click="loadDashboardData" class="retry-button">Tentar novamente</button>
      </div>
    </div>
    
    <Notification
      :show="notification.show"
      :message="notification.message"
      :type="notification.type"
      :duration="3000"
      @close="closeNotification"
    />
  </HeaderSidebar>
</template>

<style scoped>
.dashboard-container {
  width: 100%;
}

.page-title {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.page-subtitle {
  font-size: 0.9rem;
  color: #666;
  margin-bottom: 20px;
}

/* Estados de carregamento e erro */
.loading-state {
  padding: 20px;
  text-align: center;
  color: #666;
}

.error-state {
  padding: 20px;
  text-align: center;
  color: #e74c3c;
}

.retry-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 8px 16px;
  margin-top: 10px;
  cursor: pointer;
}

.no-requests {
  padding: 20px;
  text-align: center;
  color: #666;
  font-style: italic;
}

/* Stats cards */
.stats-cards {
  display: flex;
  flex-wrap: nowrap;
  gap: 16px;
  margin-bottom: 24px;
}

.stat-card {
  flex: 1;
  background-color: white;
  border-radius: 4px;
  border: 1px solid #e6e6e6;
  padding: 20px;
  text-align: center;
}

.stat-card h3 {
  font-size: 0.9rem;
  font-weight: 500;
  color: #666;
  margin-bottom: 10px;
  text-align: center;
}

.stat-value {
  font-size: 2rem;
  font-weight: 600;
  margin: 0;
  text-align: center;
}

.stat-blue {
  color: #4863c1;
}

.stat-red {
  color: #e53935;
}

/* Chart section */
.chart-section {
  background-color: white;
  border-radius: 4px;
  border: 1px solid #e6e6e6;
  padding: 20px;
  margin-bottom: 24px;
}

.section-title {
  font-size: 1rem;
  font-weight: 500;
  color: #333;
  margin-bottom: 20px;
}

.chart-container {
  overflow-x: auto;
  overflow-y: hidden;
}

.chart {
  min-width: 100%;
  height: 250px;
  position: relative;
}

.bars-container {
  display: flex;
  height: 200px;
  align-items: flex-end;
  padding-bottom: 30px;
}

.bar-column {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin-right: 15px;
  width: 30px;
}

.bar-wrapper {
  display: flex;
  flex-direction: column;
  height: 150px;
  width: 100%;
}

.bar-empty {
  background-color: #f5f5f5;
  height: 50%;
  width: 100%;
}

.bar-filled {
  background-color: #4863c1;
  height: 50%;
  width: 100%;
}

.bar-label {
  margin-top: 8px;
  font-size: 0.8rem;
  color: #666;
  white-space: nowrap;
}

/* Estilos para o botão de ver horários */
.chart-actions {
  display: flex;
  justify-content: flex-end;
  margin-top: 10px;
}

.action-table {
  width: 100%;
  border-collapse: collapse;
}

.action-table td {
  text-align: right;
  padding: 5px;
}

.view-schedule-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 8px 16px;
  font-size: 0.9rem;
  cursor: pointer;
}

/* Requests section */
.requests-section {
  background-color: white;
  border-radius: 4px;
  border: 1px solid #e6e6e6;
  padding: 20px;
}

.requests-table {
  width: 100%;
  border-collapse: collapse;
}

.requests-table th,
.requests-table td {
  padding: 12px 15px;
  text-align: left;
  border-bottom: 1px solid #eee;
}

.requests-table th {
  font-weight: 500;
  color: #333;
}

.status-badge {
  display: inline-block;
  padding: 4px 10px;
  border-radius: 12px;
  font-size: 0.8rem;
  background-color: #fff7e6;
  color: #e69500;
}

.view-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 5px 15px;
  font-size: 0.9rem;
  cursor: pointer;
}
</style>