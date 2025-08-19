<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import ScheduleTable from '../common/ScheduleTable.vue';
import { 
  dayMap,
  loadMultipleData
} from '../../utils/common';

// Dados do aluno e horário
const studentData = ref(null);
const courses = ref([]);
const allShifts = ref([]);
const loading = ref(true);
const error = ref(null);

// ID do aluno (obtido do localStorage)
const studentId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Semestre atual (pode ser obtido dinamicamente)
const currentSemester = ref(2); // 1 ou 2
const currentYear = ref(1); // Inicia com 1, será atualizado

// Obter ano acadêmico atual
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

// Filtra os turnos do semestre atual (mostra todos, não apenas os matriculados)
const filteredShifts = computed(() => {
  if (!allShifts.value.length || !courses.value.length) return [];
  
  // 1. Filtrar cursos do ano e semestre atual
  const currentCourses = courses.value.filter(course => 
    course.year === currentYear.value &&
    course.semester === currentSemester.value
  );
  
  const currentCourseIds = currentCourses.map(c => c.id);
  
  // 2. Filtrar turnos desses cursos
  return allShifts.value.filter(shift => 
    currentCourseIds.includes(shift.courseId)
  );
});

// Carrega os dados do aluno e horário usando loadMultipleData para maior eficiência
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar todos os dados em paralelo
    await loadMultipleData([
      { 
        endpoint: `/students/${studentId.value}`, 
        ref: studentData 
      },
      { 
        endpoint: '/courses', 
        ref: courses 
      },
      { 
        endpoint: '/shifts', 
        ref: allShifts 
      }
    ], loading, error);
    
    // Determinar ano do aluno após carregamento dos dados
    if (studentData.value) {
      currentYear.value = determineStudentYear(studentData.value);
    }
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente mais tarde.';
  } finally {
    loading.value = false;
  }
}

// Função para determinar o ano do aluno (implemente sua lógica aqui)
function determineStudentYear() {
  return 3; // Temporariamente fixo como 3º ano
}

onMounted(() => {
  loadAllData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="studentData?.name || 'Carregando...'" 
    userType="aluno"
    activeMenuItem="horario"
  >
    <div class="schedule-container">
      <h2 class="page-title">Horário do Curso</h2>
      <div class="semester-info">
        {{ currentYear }}º Ano - {{ currentSemester }}º Semestre - {{ currentAcademicYear }}
      </div>
      
      <!-- Loading state -->
      <div v-if="loading" class="loading-state">
        <div class="spinner"></div>
        <p>Carregando horário...</p>
      </div>
      
      <!-- Error state -->
      <div v-else-if="error" class="error-state">
        {{ error }}
        <button @click="loadAllData" class="retry-button">Tentar novamente</button>
      </div>
      
      <!-- Schedule table -->
      <div v-else>
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
  padding: 1rem 0;
}

.page-title {
  font-size: 1.5rem;
  margin-bottom: 5px;
  color: #333;
}

.semester-info {
  font-size: 1rem;
  margin-bottom: 15px;
  color: #666;
}

.info-note {
  font-size: 0.9rem;
  color: #888;
  font-style: italic;
  margin-left: 8px;
}

/* Loading state */
.loading-state {
  padding: 3rem 1.5rem;
  text-align: center;
  background-color: white;
  border-radius: 8px;
  border: 1px solid #e6e6e6;
  margin-bottom: 1.5rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
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

/* Error state */
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
</style>