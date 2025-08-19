<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import ScheduleTable from '../common/ScheduleTable.vue';
import { 
  dayMap, 
  loadMultipleData
} from '../../utils/common';
import { useRouter } from 'vue-router'

// Dados do diretor
const directorData = ref(null);
const courses = ref([]);
const shifts = ref([]);
const loading = ref(true);
const error = ref(null);

// ID do diretor (obtido do localStorage)
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Filtros de visualização
const selectedYear = ref(1);
const selectedSemester = ref(1); // Alterado para 1 como padrão
const yearOptions = [
  { value: 1, label: '1º Ano' },
  { value: 2, label: '2º Ano' },
  { value: 3, label: '3º Ano' },
];

// Carregar dados necessários usando loadMultipleData para melhor eficiência
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Usar loadMultipleData para carregar todos os dados em paralelo
    await loadMultipleData([
      { endpoint: `/directors/${directorId.value}`, ref: directorData },
      { endpoint: '/courses', ref: courses },
      { endpoint: '/shifts', ref: shifts }
    ], loading, error);
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente mais tarde.';
  } finally {
    loading.value = false;
  }
}

// Filtrar turnos com base no ano e semestre selecionados
const filteredShifts = computed(() => {
  if (!shifts.value.length || !courses.value.length) return [];
  
  const filteredCourses = courses.value.filter(course => 
    course.year === selectedYear.value && 
    course.semester === parseInt(selectedSemester.value)
  );
  
  const courseIds = filteredCourses.map(course => course.id);
  
  return shifts.value.filter(shift => courseIds.includes(shift.courseId));
});

// Obter semestre atual do ano letivo
const currentAcademicYear = computed(() => {
  const now = new Date();
  const year = now.getFullYear();
  const month = now.getMonth() + 1; // getMonth() retorna 0-11
  
  // Se estamos em agosto ou depois, é o ano letivo que começa agora
  // Se não, é o ano letivo que começou no ano passado
  if (month >= 8) {
    return `${year}/${year + 1}`;
  } else {
    return `${year - 1}/${year}`;
  }
});

const router = useRouter();

// Voltar para o dashboard
function goBack() {
  router.push('/diretor/dashboard');
}

// Carregar dados ao montar o componente
onMounted(() => {
  loadAllData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name" 
    userType="diretor"
    activeMenuItem="dashboard"
  >
    <div class="schedule-container">
      <div class="header-row">
        <h2 class="page-title">Horário do Curso</h2>
        <button @click="goBack" class="back-button">Voltar</button>
      </div>
      
      <div class="semester-info">{{ selectedSemester }}º Semestre - {{ currentAcademicYear }}</div>
      
      <!-- Filtros de visualização -->
      <div class="filters-section">
        <div class="filter-container">
          <h3 class="filter-label">Tipo:</h3>
          
          <div class="year-filters">
            <div v-for="yearOption in yearOptions" :key="yearOption.value" class="year-option">
              <label class="radio-container">
                <input 
                  type="radio" 
                  :value="yearOption.value" 
                  v-model="selectedYear"
                  name="yearFilter"
                >
                <span class="radio-label">{{ yearOption.label }}</span>
              </label>
              
              <select class="semester-select" v-model="selectedSemester" v-if="selectedYear === yearOption.value">
                <option value="1">1º Semestre</option>
                <option value="2">2º Semestre</option>
              </select>
            </div>
          </div>
        </div>
      </div>
      
      <!-- Loading state -->
      <div v-if="loading" class="loading-state">
        <div class="spinner"></div>
        <p>Carregando horários...</p>
      </div>
      
      <!-- Error state -->
      <div v-else-if="error" class="error-state">
        {{ error }}
        <button @click="loadAllData" class="retry-button">Tentar novamente</button>
      </div>
      
      <!-- Tabela de horário -->
      <div v-else class="schedule-table-wrapper">
        <ScheduleTable
          :shifts="filteredShifts"
          :courses="courses"
          :dayMap="dayMap"
          :showAbbreviation="true"
          @retry="loadAllData"
        />
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
.schedule-container {
  width: 100%;
  max-width: 100%;
  padding: 1rem 0;
}

.header-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
}

.page-title {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.back-button {
  background-color: #f0f0f0;
  color: #333;
  border: 1px solid #ddd;
  border-radius: 4px;
  padding: 6px 12px;
  font-size: 0.9rem;
  cursor: pointer;
  transition: background-color 0.2s;
}

.back-button:hover {
  background-color: #e0e0e0;
}

.semester-info {
  font-size: 0.9rem;
  color: #666;
  margin-bottom: 1.5rem;
}

/* Filtros */
.filters-section {
  background-color: white;
  border-radius: 8px;
  border: 1px solid #e6e6e6;
  padding: 1.5rem;
  margin-bottom: 1.5rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.filter-container {
  width: 100%;
}

.filter-label {
  font-size: 1rem;
  font-weight: 500;
  margin-bottom: 1rem;
  color: #333;
}

.year-filters {
  display: flex;
  flex-direction: column;
  gap: 1.5rem;
}

.year-option {
  display: flex;
  align-items: center;
  gap: 1rem;
}

.radio-container {
  display: flex;
  align-items: center;
  cursor: pointer;
  width: 100px;
}

.radio-label {
  margin-left: 0.5rem;
  font-size: 0.9rem;
  color: #333;
}

.semester-select {
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
  background-color: white;
  width: 180px;
}

/* Loading and Error states */
.loading-state {
  padding: 3rem 1.5rem;
  text-align: center;
  background-color: white;
  border-radius: 8px;
  border: 1px solid #e6e6e6;
  margin-bottom: 1.5rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.error-state {
  padding: 1.5rem;
  text-align: center;
  background-color: #fef2f2;
  border-radius: 8px;
  border: 1px solid #fee2e2;
  color: #dc2626;
  margin-bottom: 1.5rem;
}

.retry-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.5rem 1rem;
  margin-top: 1rem;
  font-size: 0.9rem;
  cursor: pointer;
}

.spinner {
  border: 4px solid rgba(0, 0, 0, 0.1);
  width: 36px;
  height: 36px;
  border-radius: 50%;
  border-left-color: #4863c1;
  animation: spin 1s linear infinite;
  margin: 0 auto 1rem;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Schedule table wrapper */
.schedule-table-wrapper {
  margin-bottom: 1.5rem;
}

/* Legenda */
.legend-container {
  background-color: white;
  border-radius: 8px;
  border: 1px solid #e6e6e6;
  padding: 1.5rem;
  margin-bottom: 1.5rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.legend-container h3 {
  font-size: 1rem;
  font-weight: 500;
  margin-top: 0;
  margin-bottom: 1rem;
  color: #333;
}

.legend-list {
  list-style-type: none;
  padding: 0;
  margin: 0;
}

.legend-list li {
  margin-bottom: 0.5rem;
  font-size: 0.9rem;
  color: #555;
  display: flex;
  align-items: center;
}

.legend-icon {
  margin-right: 0.5rem;
}
</style>