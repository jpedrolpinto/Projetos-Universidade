<script setup>
import { ref, computed, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import api from '../../services/api';
import { 
  loadMultipleData,
  dayMap
} from '../../utils/common';

// Data
const allCourses = ref([]);
const selectedCourse = ref('');
const students = ref([]);
const availableShifts = ref([]);
const loading = ref(false);
const saving = ref(false);
const showConfirmationModal = ref(false);
const error = ref(null);

// Dados do diretor
const directorData = ref(null);
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Função para obter o nome do dia em português
function getDayName(day) {
  return dayMap[day] || day;
}

// Função unificada para carregar dados
async function loadData(courseId = null) {
  try {
    loading.value = true;
    error.value = null;
    
    // Definir endpoints para carregar
    const endpoints = [
      { endpoint: `/directors/${directorId.value}`, ref: directorData },
      { endpoint: '/courses', ref: allCourses }
    ];
    
    // Se um curso for selecionado, adicionar endpoints relacionados
    if (courseId) {
      selectedCourse.value = courseId;
      
      // Referências temporárias para os dados específicos do curso
      const courseStudents = ref([]);
      const courseShifts = ref([]);
      const courseAllocations = ref([]);
      
      // Adicionar endpoints específicos do curso
      endpoints.push(
        { endpoint: `/courses/${courseId}/students`, ref: courseStudents },
        { endpoint: `/courses/${courseId}/shifts`, ref: courseShifts },
        { endpoint: `/courses/${courseId}/allocations`, ref: courseAllocations }
      );
      
      // Carregar todos os dados em paralelo
      await loadMultipleData(endpoints, loading, error);
      
      // Processar dados do curso
      availableShifts.value = processShifts(courseShifts.value);
      students.value = processStudents(courseStudents.value, courseShifts.value, courseAllocations.value);
    } else {
      // Carregar apenas os dados básicos
      await loadMultipleData(endpoints, loading, error);
      
      // Limpar dados específicos do curso
      students.value = [];
      availableShifts.value = [];
    }
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Tente novamente.';
  } finally {
    loading.value = false;
  }
}

// Processar estudantes para exibição
function processStudents(studentsData, shiftsData, allocationsData) {
  return studentsData
    .filter(student => student.enrolled.includes(parseInt(selectedCourse.value)))
    .map(student => {
      // Find all allocations for this student in the selected course
      const studentAllocations = allocationsData.filter(
        alloc => alloc.studentId === student.id
      );

      // Find theoretical (T) and practical (PL) shifts
      const theoreticalShift = availableShifts.value.find(
        shift => shift.id === studentAllocations.find(
          alloc => shiftsData.find(s => s.id === alloc.shiftId)?.type === 'T'
        )?.shiftId
      );

      const practicalShift = availableShifts.value.find(
        shift => shift.id === studentAllocations.find(
          alloc => shiftsData.find(s => s.id === alloc.shiftId)?.type === 'PL'
        )?.shiftId
      );

      return {
        ...student,
        theoreticalShift,
        practicalShift,
        // O valor inicial agora pode ser "", indicando "sem turno"
        newTheoreticalShiftId: theoreticalShift?.id || '',
        newPracticalShiftId: practicalShift?.id || '',
        studentNumber: student.email?.split('@')[0]?.toUpperCase() || 'N/A'
      };
    });
}

// Processar turnos para exibição
function processShifts(shiftsData) {
  return shiftsData.map(shift => ({
    ...shift,
    enrolled: shift.enrolled || 0,
    capacity: shift.capacity || 30
  }));
}

// Funções para obter turnos disponíveis (excluindo o turno atual do aluno)
function getAvailableTheoreticalShifts(student) {
  return theoreticalShifts.value.filter(shift => 
    shift.id !== student.theoreticalShift?.id
  );
}

function getAvailablePracticalShifts(student) {
  return practicalShifts.value.filter(shift => 
    shift.id !== student.practicalShift?.id
  );
}

// Handle shift changes
function handleShiftChange(student) {
  // Esta função é chamada quando o aluno altera um turno
  // Pode ser usada para verificar conflitos ou atualizar outros dados
}

// Check if shift is full
function isShiftFull(shift) {
  return shift.enrolled >= shift.capacity;
}

// Show confirmation before saving
function saveChanges() {
  showConfirmationModal.value = true;
}

// Confirm and save changes
async function confirmSave() {
  try {
    saving.value = true;
    showConfirmationModal.value = false;
    
    // Array para armazenar todas as operações a serem realizadas
    const operations = [];
    
    // Processar estudantes um por um
    for (const student of students.value) {
      // Alterações no turno teórico
      if (student.newTheoreticalShiftId !== (student.theoreticalShift?.id || '')) {
        // Se o aluno já tem um turno teórico atribuído, remover essa alocação
        if (student.theoreticalShift?.id) {
          operations.push(async () => {
            // Encontre o ID da alocação para excluir
            const allocationsResponse = await api.get(`/allocations?studentId=${student.id}&shiftId=${student.theoreticalShift.id}`);
            
            if (allocationsResponse.data && allocationsResponse.data.length > 0) {
              for (const allocation of allocationsResponse.data) {
                await api.delete(`/allocations/${allocation.id}`);
              }
            }
          });
        }
        
        // Adicionar nova alocação apenas se um novo turno foi selecionado
        if (student.newTheoreticalShiftId) {
          operations.push(async () => {
            await api.post('/allocations', {
              shiftId: parseInt(student.newTheoreticalShiftId),
              studentId: student.id
            });
          });
        }
      }
      
      // Alterações no turno prático
      if (student.newPracticalShiftId !== (student.practicalShift?.id || '')) {
        // Se o aluno já tem um turno prático atribuído, remover essa alocação
        if (student.practicalShift?.id) {
          operations.push(async () => {
            // Encontre o ID da alocação para excluir
            const allocationsResponse = await api.get(`/allocations?studentId=${student.id}&shiftId=${student.practicalShift.id}`);
            
            if (allocationsResponse.data && allocationsResponse.data.length > 0) {
              for (const allocation of allocationsResponse.data) {
                await api.delete(`/allocations/${allocation.id}`);
              }
            }
          });
        }
        
        // Adicionar nova alocação apenas se um novo turno foi selecionado
        if (student.newPracticalShiftId) {
          operations.push(async () => {
            await api.post('/allocations', {
              shiftId: parseInt(student.newPracticalShiftId),
              studentId: student.id
            });
          });
        }
      }
    }
    
    // Executar todas as operações sequencialmente
    for (const operation of operations) {
      await operation();
    }
    
    // Atualizar a visualização
    await loadData(selectedCourse.value);
    alert('Alterações guardadas com sucesso!');
  } catch (err) {
    console.error('Erro ao guardar:', err);
    error.value = 'Erro ao guardar alterações: ' + (err.message || 'Tente novamente');
    alert('Erro ao guardar alterações: ' + (err.message || 'Tente novamente'));
  } finally {
    saving.value = false;
  }
}

// Computed properties
const hasChanges = computed(() => {
  return students.value.some(s => 
    s.newTheoreticalShiftId !== (s.theoreticalShift?.id || '') ||
    s.newPracticalShiftId !== (s.practicalShift?.id || '')
  );
});

const changesCount = computed(() => {
  return students.value.filter(s => 
    s.newTheoreticalShiftId !== (s.theoreticalShift?.id || '') ||
    s.newPracticalShiftId !== (s.practicalShift?.id || '')
  ).length;
});

const theoreticalShifts = computed(() => {
  return availableShifts.value.filter(shift => shift.type === 'T');
});

const practicalShifts = computed(() => {
  return availableShifts.value.filter(shift => shift.type === 'PL');
});

// Initialize
onMounted(async () => {
  await loadData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name " 
    userType="diretor"
    activeMenuItem="distribuicao-manual"
  >
    <div class="manual-distribution-container">
      <div class="header-section">
        <h1>Redistribuição Manual</h1>
        <p>Altere manualmente os turnos dos alunos</p>
      </div>

      <!-- Loading and Error State -->
      <div v-if="loading" class="loading-state">
        <div class="spinner"></div>
        <p>Carregando dados...</p>
      </div>

      <div v-else-if="error" class="error-state">
        <p>{{ error }}</p>
        <button @click="loadData()" class="retry-button">Tentar novamente</button>
      </div>

      <div v-else>
        <!-- Course Selection -->
        <div class="course-selection">
          <div class="form-group">
            <label>Disciplina:</label>
            <select v-model="selectedCourse" @change="loadData(selectedCourse)">
              <option value="">Selecione uma disciplina</option>
              <option 
                v-for="course in allCourses" 
                :key="course.id" 
                :value="course.id"
              >
                {{ course.abbreviation }} - {{ course.name }}
              </option>
            </select>
          </div>
        </div>

        <!-- Students Table -->
        <div class="students-table-container" v-if="selectedCourse">
          <div class="table-actions">
            <button 
              @click="saveChanges" 
              class="save-button"
              :disabled="!hasChanges || saving"
            >
              <span v-if="saving">Guardando...</span>
              <span v-else>Guardar Alterações</span>
            </button>
            <span v-if="hasChanges" class="unsaved-changes">
              Existem alterações não guardadas
            </span>
          </div>

          <table class="students-table">
            <thead>
              <tr>
                <th>Nº</th>
                <th>Aluno</th>
                <th>Turno Teórico Atual</th>
                <th>Novo Turno Teórico</th>
                <th>Turno Prático Atual</th>
                <th>Novo Turno Prático</th>
                <th>Estado</th>
              </tr>
            </thead>
            <tbody>
              <tr v-if="students.length === 0">
                <td colspan="7" class="empty-cell">
                  Não existem alunos inscritos nesta disciplina.
                </td>
              </tr>
              <tr v-for="student in students" :key="student.id">
                <td>{{ student.studentNumber }}</td>
                <td>{{ student.name }}</td>
                
                <!-- Turno Teórico Atual -->
                <td>
                  <span v-if="student.theoreticalShift">
                    {{ student.theoreticalShift.name }} (T) - 
                    {{ getDayName(student.theoreticalShift.day) }}, 
                    {{ student.theoreticalShift.from }}:00 - {{ student.theoreticalShift.to }}:00
                  </span>
                  <span v-else class="no-shift">Sem turno</span>
                </td>
                
                <!-- Novo Turno Teórico -->
                <td>
                  <select 
                    v-model="student.newTheoreticalShiftId" 
                    @change="handleShiftChange(student)"
                    :disabled="saving"
                  >
                    <option value="">Sem turno</option>
                    <option 
                      v-for="shift in getAvailableTheoreticalShifts(student)" 
                      :key="shift.id" 
                      :value="shift.id"
                      :disabled="isShiftFull(shift)"
                    >
                      {{ shift.name }} (T) - 
                      {{ getDayName(shift.day) }}, {{ shift.from }}:00 - {{ shift.to }}:00
                      ({{ shift.enrolled }}/{{ shift.capacity }})
                      {{ isShiftFull(shift) ? '(Lotado)' : '' }}
                    </option>
                  </select>
                </td>
                
                <!-- Turno Prático Atual -->
                <td>
                  <span v-if="student.practicalShift">
                    {{ student.practicalShift.name }} (PL) - 
                    {{ getDayName(student.practicalShift.day) }}, 
                    {{ student.practicalShift.from }}:00 - {{ student.practicalShift.to }}:00
                  </span>
                  <span v-else class="no-shift">Sem turno</span>
                </td>
                
                <!-- Novo Turno Prático -->
                <td>
                  <select 
                    v-model="student.newPracticalShiftId" 
                    @change="handleShiftChange(student)"
                    :disabled="saving"
                  >
                    <option value="">Sem turno</option>
                    <option 
                      v-for="shift in getAvailablePracticalShifts(student)" 
                      :key="shift.id" 
                      :value="shift.id"
                      :disabled="isShiftFull(shift)"
                    >
                      {{ shift.name }} (PL) - 
                      {{ getDayName(shift.day) }}, {{ shift.from }}:00 - {{ shift.to }}:00
                      ({{ shift.enrolled }}/{{ shift.capacity }})
                      {{ isShiftFull(shift) ? '(Lotado)' : '' }}
                    </option>
                  </select>
                </td>
                
                <td>
                  <span v-if="(student.newTheoreticalShiftId && student.newTheoreticalShiftId !== (student.theoreticalShift?.id || '')) || 
                            (student.newPracticalShiftId && student.newPracticalShiftId !== (student.practicalShift?.id || ''))" 
                        class="status-change">
                    Alteração pendente
                  </span>
                  <span v-else class="status-ok">
                    Sem alteração
                  </span>
                </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>

      <!-- Confirmation Modal -->
      <div v-if="showConfirmationModal" class="modal-overlay">
        <div class="modal-container">
          <div class="modal-header">
            <h2>Confirmar Alterações</h2>
            <button @click="showConfirmationModal = false" class="modal-close">×</button>
          </div>
          <div class="modal-content">
            <p>Tem a certeza que quer guardar estas alterações?</p>
            <div class="changes-summary">
              <p>Alunos a modificar: {{ changesCount }}</p>
            </div>
          </div>
          <div class="modal-footer">
            <button @click="showConfirmationModal = false" class="cancel-button">
              Cancelar
            </button>
            <button @click="confirmSave" class="confirm-button">
              Confirmar
            </button>
          </div>
        </div>
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
/* Estilos permanecem iguais */
.manual-distribution-container {
  padding: 20px;
  background-color: #f5f5f5;
  border-radius: 8px;
}

.header-section {
  margin-bottom: 20px;
}

.header-section h1 {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.header-section p {
  color: #666;
  font-size: 0.9rem;
  margin-bottom: 0;
}

/* Loading & Error States */
.loading-state,
.error-state {
  background-color: white;
  border-radius: 8px;
  padding: 30px;
  text-align: center;
  margin: 20px 0;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.error-state {
  color: #dc3545;
}

.retry-button {
  margin-top: 15px;
  padding: 8px 16px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.spinner {
  border: 3px solid rgba(0,0,0,0.1);
  border-radius: 50%;
  border-top: 3px solid #4863c1;
  width: 24px;
  height: 24px;
  animation: spin 1s linear infinite;
  margin: 0 auto 10px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Course Selection */
.course-selection {
  background-color: white;
  padding: 15px;
  border-radius: 8px;
  margin-bottom: 20px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.form-group {
  display: flex;
  align-items: center;
  gap: 15px;
}

.form-group label {
  font-weight: 500;
  color: #555;
  font-size: 0.9rem;
}

.form-group select {
  flex: 1;
  padding: 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
  max-width: 400px;
}

/* Table Actions */
.table-actions {
  display: flex;
  align-items: center;
  gap: 15px;
  margin-bottom: 15px;
}

.save-button {
  padding: 8px 20px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
}

.save-button:disabled {
  background-color: #cccccc;
  cursor: not-allowed;
}

.save-button:hover:not(:disabled) {
  background-color: #3a4e9c;
}

.unsaved-changes {
  color: #e69500;
  font-size: 0.9rem;
}

/* Students Table */
.students-table-container {
  background-color: white;
  border-radius: 8px;
  overflow-x: auto;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  padding: 15px;
}

.students-table {
  width: 100%;
  border-collapse: collapse;
}

.students-table th,
.students-table td {
  padding: 12px 15px;
  text-align: left;
  border-bottom: 1px solid #eee;
}

.students-table th {
  background-color: #f9f9f9;
  font-weight: 600;
  color: #333;
}

.students-table tr:hover {
  background-color: #f5f5f5;
}

.students-table select {
  padding: 6px 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
  min-width: 260px; /* Aumentado para acomodar mais texto */
}

/* Status indicators */
.no-shift {
  color: #dc3545;
  font-style: italic;
}

.status-change {
  color: #e69500;
  font-weight: 500;
}

.status-ok {
  color: #2e8b57;
}

/* Empty cell */
.empty-cell {
  text-align: center;
  padding: 30px !important;
  font-style: italic;
  color: #666;
}

/* Modal */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(0,0,0,0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.modal-container {
  background-color: white;
  border-radius: 8px;
  width: 450px;
  max-width: 95%;
  box-shadow: 0 5px 15px rgba(0,0,0,0.2);
}

.modal-header {
  padding: 15px 20px;
  border-bottom: 1px solid #eee;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.modal-header h2 {
  margin: 0;
  font-size: 1.2rem;
  color: #333;
}

.modal-close {
  background: none;
  border: none;
  font-size: 1.5rem;
  cursor: pointer;
  color: #999;
}

.modal-close:hover {
  color: #666;
}

.modal-content {
  padding: 20px;
}

.modal-content p {
  margin-bottom: 15px;
}

.changes-summary {
  background-color: #f5f5f5;
  padding: 10px;
  border-radius: 4px;
  margin-top: 15px;
}

.modal-footer {
  padding: 15px 20px;
  border-top: 1px solid #eee;
  display: flex;
  justify-content: flex-end;
  gap: 10px;
}

.cancel-button {
  padding: 8px 15px;
  background-color: #f0f0f0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.confirm-button {
  padding: 8px 15px;
  background-color: #28a745;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.cancel-button:hover {
  background-color: #e0e0e0;
}

.confirm-button:hover {
  background-color: #218838;
}

/* Responsive adjustments */
@media (max-width: 768px) {
  .form-group {
    flex-direction: column;
    align-items: flex-start;
  }
  
  .form-group select {
    width: 100%;
    max-width: none;
  }
  
  .students-table th,
  .students-table td {
    padding: 8px 10px;
    font-size: 0.85rem;
  }
  
  .students-table select {
    min-width: 200px;
  }
  
  .table-actions {
    flex-direction: column;
    align-items: flex-start;
  }
}
</style>