<script setup>
import { computed } from 'vue';
import { 
  weekDays, 
  timeSlots, 
  getCourseName, 
  getCourseAbbr, 
  getCourseColor 
} from '../../utils/common';

const props = defineProps({
  shifts: {
    type: Array,
    required: true
  },
  courses: {
    type: Array,
    required: true
  },
  dayMap: {
    type: Object,
    required: true
  },
  showAbbreviation: {
    type: Boolean,
    default: false
  },
  loading: {
    type: Boolean,
    default: false
  },
  error: {
    type: [String, null],
    default: null
  }
});

const emit = defineEmits(['retry']);

/**
 * Função para obter as aulas em determinado horário e dia
 * Modificada para retornar todas as aulas em vez de apenas uma
 */
const getClassesForTimeAndDay = (time, day) => {
  if (!props.shifts.length) return [];
  
  const timeHour = parseInt(time.split(':')[0]);
  
  return props.shifts
    .filter(s => {
      return props.dayMap[s.day] === day && 
             timeHour >= s.from && 
             timeHour < s.to;
    })
    .map(shift => {
      return {
        id: shift.id,
        courseId: shift.courseId,
        time: `${shift.from}:00`,
        endTime: `${shift.to}:00`,
        name: getCourseName(props.courses, shift.courseId),
        abbr: getCourseAbbr(props.courses, shift.courseId),
        room: shift.name,
        type: shift.type,
        color: getCourseColor(shift.courseId),
        duration: shift.to - shift.from,
        startHour: shift.from,
        endHour: shift.to
      };
    });
};

/**
 * Função para verificar se é a primeira hora da aula
 */
const isFirstHour = (classItem, time) => {
  const currentHour = parseInt(time.split(':')[0]);
  return classItem.startHour === currentHour;
};

/**
 * Função para obter o percentual de largura para cada classe em um conflito
 */
const getClassWidth = (classes) => {
  return classes.length > 1 ? `${100 / classes.length}%` : '100%';
};

// Semestre atual
const currentSemester = computed(() => 'Semestre 2 - 2024/2025');

// Função para tentar novamente quando houver erro
const handleRetry = () => {
  emit('retry');
};
</script>

<template>
  <div>
    <!-- Mensagem de carregamento -->
    <div v-if="loading" class="loading-message">
      <div class="spinner"></div>
      <p>Carregando dados do horário...</p>
      <p class="loading-detail">Isso pode levar alguns instantes, por favor aguarde.</p>
    </div>
    
    <!-- Mensagem de erro -->
    <div v-else-if="error" class="error-message">
      <p>{{ error }}</p>
      <button @click="handleRetry" class="retry-button">Tentar novamente</button>
    </div>
    
    <!-- Tabela de horário -->
    <div v-else class="schedule-table-container">
      <table class="schedule-table">
        <thead>
          <tr>
            <th class="time-column"></th>
            <th v-for="day in weekDays" :key="day">{{ day }}</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="time in timeSlots" :key="time">
            <td class="time-cell">{{ time }}</td>
            <td 
              v-for="day in weekDays" 
              :key="`${day}-${time}`"
              class="day-cell"
            >
              <div class="classes-container">
                <template v-for="classItem in getClassesForTimeAndDay(time, day)" :key="classItem.id">
                  <div 
                    v-if="isFirstHour(classItem, time)"
                    class="class-card"
                    :style="{
                      backgroundColor: classItem.color,
                      height: `${classItem.duration * 40}px`,
                      width: getClassWidth(getClassesForTimeAndDay(time, day)),
                      position: 'absolute',
                      top: '0',
                      left: getClassesForTimeAndDay(time, day).length > 1 ? 
                             `${getClassesForTimeAndDay(time, day).findIndex(c => c.id === classItem.id) * 
                             (100 / getClassesForTimeAndDay(time, day).length)}%` : '0',
                      right: getClassesForTimeAndDay(time, day).length > 1 ? 'auto' : '0',
                    }"
                  >
                    <div class="class-title">
                      {{ showAbbreviation ? classItem.abbr : classItem.name }}
                    </div>
                    <div class="class-details">{{ classItem.room }} - {{ classItem.type }}</div>
                  </div>
                </template>
              </div>
            </td>
          </tr>
        </tbody>
      </table>
    </div>
    
    <!-- Legenda -->
    <div class="legend-container">
      <h3>Legenda:</h3>
      <ul class="legend-list">
        <li>
          <span class="legend-icon">➡️</span> TP - aulas teórico-práticas;
        </li>
        <li>
          <span class="legend-icon">➡️</span> PL - aulas de prática-laboratorial;
        </li>
        <li>
          <span class="legend-icon">➡️</span> T - aulas teóricas;
        </li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.semester-info {
  font-size: 0.9rem;
  color: #666;
  margin-bottom: 20px;
}

.loading-message {
  padding: 30px;
  text-align: center;
  background-color: #f9f9f9;
  border-radius: 5px;
  margin-bottom: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

.loading-detail {
  font-size: 0.8rem;
  color: #777;
  margin-top: 5px;
}

.spinner {
  border: 4px solid rgba(0, 0, 0, 0.1);
  width: 36px;
  height: 36px;
  border-radius: 50%;
  border-left-color: #4863c1;
  animation: spin 1s linear infinite;
  margin-bottom: 15px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.error-message {
  padding: 20px;
  text-align: center;
  background-color: #fdeded;
  color: #e74c3c;
  border-radius: 5px;
  margin-bottom: 20px;
  border-left: 4px solid #e74c3c;
}

.retry-button {
  margin-top: 15px;
  padding: 8px 15px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
}

.retry-button:hover {
  background-color: #3a4e9c;
}

.schedule-table-container {
  overflow-x: auto;
  margin-bottom: 20px;
}

.schedule-table {
  width: 100%;
  border-collapse: collapse;
  border: 1px solid #ddd;
  table-layout: fixed;
}

.schedule-table th, 
.schedule-table td {
  border: 1px solid #ddd;
  padding: 0;
  text-align: center;
  vertical-align: top;
  position: relative;
}

.schedule-table th {
  background-color: #f5f5f5;
  font-weight: 500;
  padding: 8px;
}

.time-column {
  width: 60px;
}

.time-cell {
  font-size: 0.8rem;
  color: #666;
  width: 60px;
  padding: 8px !important;
}

.day-cell {
  height: 40px;
  position: relative;
  overflow: visible;
}

.classes-container {
  width: 100%;
  height: 100%;
  position: relative;
}

.class-card {
  padding: 3px;
  border-radius: 3px;
  font-size: 0.75rem;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  overflow: hidden;
  box-sizing: border-box;
  border: 1px solid rgba(0,0,0,0.1);
  z-index: 10;
}

.class-title {
  font-weight: 600;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  margin-bottom: 2px;
}

.class-details {
  font-size: 0.65rem;
  color: #555;
}

.legend-container {
  background-color: #f9f9f9;
  border: 1px solid #ddd;
  border-radius: 5px;
  padding: 15px;
}

.legend-container h3 {
  margin-top: 0;
  margin-bottom: 10px;
  font-size: 1rem;
}

.legend-list {
  list-style-type: none;
  padding: 0;
  margin: 0;
}

.legend-list li {
  margin-bottom: 5px;
  display: flex;
  align-items: center;
}

.legend-icon {
  margin-right: 8px;
}
</style>