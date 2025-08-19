<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import ScheduleTable from '../common/ScheduleTable.vue';
import api from '../../services/api';
import { 
  loadMultipleData, 
  dayMap, 
  weekDays, 
  timeSlots, 
  getCourseName,
  getCourseAbbr,
  getCourseColor
} from '../../utils/common';

// Dados do aluno e horário
const studentData = ref(null);
const courses = ref([]);
const shifts = ref([]);
const studentAllocations = ref([]);
const conflicts = ref([]);
const loading = ref(true);
const error = ref(null);
const canViewSchedule = ref(false);
const settings = ref(null);

// ID do aluno 
const studentId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Calcular o semestre e ano letivo atual
const currentAcademicInfo = computed(() => {
  // Fallback padrão se settings não estiver disponível
  const defaultYear = '2024/2025';
  const defaultSemester = '2';

  // Verificação segura
  if (!settings.value) {
    return `${defaultSemester}º Semestre - ${defaultYear}`;
  }

  // Acessos seguros com fallback
  const academicYear = settings.value.academicYear || defaultYear;
  const semester = settings.value.currentSemester || defaultSemester;

  return `${semester}º Semestre - ${academicYear}`;
});

// Carregar dados básicos do estudante independente dos horários estarem publicados
async function loadStudentData() {
  try {
    const response = await api.get(`/students/${studentId.value}`);
    studentData.value = response.data;
  } catch (err) {
    console.error('Erro ao carregar dados do estudante:', err);
  }
}

// Carregar todos os dados necessários
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar dados básicos do estudante sempre
    await loadStudentData();
    
    // Verificar se os horários estão publicados
    const settingsResponse = await api.get('/settings');
    settings.value = settingsResponse.data;
    canViewSchedule.value = settingsResponse.data.publishedSchedules;
    
    if (!canViewSchedule.value) {
      error.value = 'Os horários ainda não foram publicados pelo diretor.';
      loading.value = false;
      return;
    }
    
    // Carregar os demais dados do horário em paralelo
    await loadMultipleData([
      { 
        endpoint: '/courses', 
        ref: courses 
      },
      { 
        endpoint: '/shifts', 
        ref: shifts 
      },
      { 
        endpoint: `/allocations?studentId=${studentId.value}`, 
        ref: studentAllocations 
      }
    ], loading, error);

    // Detectar conflitos no horário
    await detectScheduleConflicts();
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente mais tarde.';
  } finally {
    loading.value = false;
  }
}

// Detectar conflitos de horário na carga do aluno
async function detectScheduleConflicts() {
  try {
    // Limpar conflitos existentes
    conflicts.value = [];
    
    // Se não tiver alocações, não tem conflito
    if (!studentAllocations.value.length) return;
    
    // Verificar se já existem conflitos na tabela
    const conflictsResponse = await api.get(`/conflicts?studentId=${studentId.value}`);
    
    if (conflictsResponse.data && conflictsResponse.data.length > 0) {
      // Conflitos já existem na tabela, apenas carregar
      conflicts.value = conflictsResponse.data.map(conflict => {
        // Adicionar informações extras para exibição
        const shift1 = shifts.value.find(s => s.id === conflict.shift1Id);
        const shift2 = shifts.value.find(s => s.id === conflict.shift2Id);
        
        return {
          ...conflict,
          shift1Name: shift1 ? shift1.name : 'Desconhecido',
          shift2Name: shift2 ? shift2.name : 'Desconhecido',
          course1Abbr: getCourseAbbr(courses.value, shift1 ? shift1.courseId : null),
          course2Abbr: getCourseAbbr(courses.value, shift2 ? shift2.courseId : null)
        };
      });
    } else {
      // Se não existirem conflitos na tabela, fazer verificação local
      
      // Obter IDs dos turnos alocados
      const shiftIds = studentAllocations.value.map(allocation => allocation.shiftId);
      
      // Obter turnos completos
      const studentShiftDetails = shifts.value.filter(shift => shiftIds.includes(shift.id));
      
      // Verificar conflitos para cada turno
      for (let i = 0; i < studentShiftDetails.length; i++) {
        const shift1 = studentShiftDetails[i];
        
        for (let j = i + 1; j < studentShiftDetails.length; j++) {
          const shift2 = studentShiftDetails[j];
          
          // Verificar se os dois turnos são no mesmo dia
          if (shift1.day === shift2.day) {
            // Verificar se há sobreposição de horários
            const shift1Start = shift1.from;
            const shift1End = shift1.to;
            const shift2Start = shift2.from;
            const shift2End = shift2.to;
            
            // Detectar sobreposição
            if ((shift1Start < shift2End) && (shift1End > shift2Start)) {
              // Criar objeto de conflito
              const newConflict = {
                id: conflicts.value.length + 1,
                shift1Id: shift1.id,
                shift2Id: shift2.id,
                studentId: studentId.value,
                day: shift1.day,
                timeRange: `${Math.max(shift1Start, shift2Start)}:00 - ${Math.min(shift1End, shift2End)}:00`,
                // Informações adicionais para exibição
                shift1Name: shift1.name,
                shift2Name: shift2.name,
                course1Abbr: getCourseAbbr(courses.value, shift1.courseId),
                course2Abbr: getCourseAbbr(courses.value, shift2.courseId)
              };
              
              // Adicionar ao array local
              conflicts.value.push(newConflict);
              
              // Salvar na tabela de conflitos
              try {
                await api.post('/conflicts', {
                  shift1Id: shift1.id,
                  shift2Id: shift2.id,
                  studentId: studentId.value,
                  day: shift1.day,
                  timeRange: newConflict.timeRange
                });
              } catch (err) {
                console.error('Erro ao salvar conflito:', err);
              }
            }
          }
        }
      }
    }
  } catch (err) {
    console.error('Erro ao detectar conflitos:', err);
  }
}

// Obter os turnos alocados ao aluno
const studentShifts = computed(() => {
  if (!studentAllocations.value.length || !shifts.value.length) return [];
  
  const shiftIds = studentAllocations.value.map(allocation => allocation.shiftId);
  return shifts.value.filter(shift => shiftIds.includes(shift.id));
});

// Verificar se tem conflito
const hasScheduleConflict = computed(() => {
  return conflicts.value && conflicts.value.length > 0;
});

onMounted(loadAllData);
</script>

<template>
  <HeaderSidebar 
    :userName="studentData?.name" 
    userType="aluno"
    activeMenuItem="meu-horario"
  >
    <div class="student-schedule-container">
      <h2 class="page-title">Meu Horário</h2>
      <div class="semester-info">{{ currentAcademicInfo }}</div>
      
      <!-- Alerta de conflito -->
      <div v-if="hasScheduleConflict" class="conflict-alert">
        <div class="conflict-alert-header">
          <span class="warning-icon">⚠️</span> Conflitos Detectados no Seu Horário
        </div>
        <div class="conflict-alert-content">
          <p>Foram detectados conflitos no seu horário. Considere solicitar mudança de turno para resolver estes problemas:</p>
          <ul class="conflicts-list">
            <li v-for="conflict in conflicts" :key="conflict.id">
              <strong>{{ dayMap[conflict.day] }}</strong>: 
              {{ conflict.course1Abbr }}
              ({{ conflict.shift1Name }}) e 
              {{ conflict.course2Abbr }}
              ({{ conflict.shift2Name }})
              {{ conflict.timeRange }}
            </li>
          </ul>
        </div>
      </div>
      
      <!-- Mensagem de carregamento -->
      <div v-if="loading" class="loading-state">
        <div class="spinner"></div>
        <p>Carregando horário...</p>
      </div>
      
      <!-- Mensagem de erro -->
      <div v-else-if="error" class="error-state">
        <p>{{ error }}</p>
        <button v-if="error !== 'Os horários ainda não foram publicados pelo diretor.'" 
                @click="loadAllData" 
                class="retry-button">
          Tentar novamente
        </button>
      </div>
      
      <!-- Tabela de horário -->
       <template v-else-if="canViewSchedule">
          <div class="schedule-table-container">
            <ScheduleTable
              :shifts="studentShifts"
              :courses="courses"
              :dayMap="dayMap"
              :showAbbreviation="true"
              @retry="loadAllData"
            />
          </div>
      </template>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
.student-schedule-container {
  padding: 20px;
  width: 100%;
}

.page-title {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.semester-info {
  font-size: 0.9rem;
  color: #666;
  margin-bottom: 20px;
}

/* Loading State */
.loading-state {
  padding: 30px;
  text-align: center;
  background-color: #f9f9f9;
  border-radius: 8px;
  border: 1px solid #e6e6e6;
  margin-bottom: 20px;
}

.spinner {
  border: 4px solid rgba(0, 0, 0, 0.1);
  width: 36px;
  height: 36px;
  border-radius: 50%;
  border-left-color: #4863c1;
  animation: spin 1s linear infinite;
  margin: 0 auto 15px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Error State */
.error-state {
  padding: 20px;
  text-align: center;
  border-radius: 8px;
  border: 1px solid #000000;
  margin-bottom: 20px;
}

.retry-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 8px 16px;
  margin-top: 15px;
  cursor: pointer;
  font-size: 0.9rem;
}

.schedule-table-container {
  overflow-x: auto;
  margin-bottom: 20px;
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

/* Alerta de conflito */
.conflict-alert {
  background-color: #fff3f3;
  border: 1px solid #f8d7da;
  border-left: 4px solid #e74c3c;
  border-radius: 8px;
  margin-bottom: 20px;
  overflow: hidden;
}

.conflict-alert-header {
  background-color: #ffecec;
  padding: 10px 15px;
  color: #e74c3c;
  font-weight: 600;
  display: flex;
  align-items: center;
}

.conflict-alert-content {
  padding: 15px;
}

.conflicts-list {
  margin-top: 10px;
  padding-left: 20px;
}

.conflicts-list li {
  margin-bottom: 5px;
}
</style>