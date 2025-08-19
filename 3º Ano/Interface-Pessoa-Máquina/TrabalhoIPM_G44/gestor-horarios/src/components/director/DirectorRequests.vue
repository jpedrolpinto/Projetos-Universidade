<script setup>
import { ref, computed, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import api from '../../services/api';
import { 
  formatDate, 
  loadDirectorData, 
  loadMultipleData,
  getStudentName,
  getCourseName,
  getCourseAbbr,
  getRequestStatus,
  getStatusClass,
  dayMap
} from '../../utils/common';

// Dados
const requests = ref([]);
const courses = ref([]);
const students = ref([]);
const shifts = ref([]);
const loading = ref(true);
const error = ref(null);

// Filtros
const filters = ref({
  course: '',
  status: ''
});

// Paginação
const currentPage = ref(1);
const itemsPerPage = 10;

// Modal
const showDetailsModal = ref(false);
const selectedRequest = ref({});
const directorComment = ref('');
const selectedRequestConflicts = ref([]);

// Dados do diretor
const directorData = ref(null);
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Função para obter o ID do curso de um turno
function getShiftCourseId(shiftsArray, shiftId) {
  const shift = shiftsArray.find(s => s.id === shiftId);
  return shift ? shift.courseId : null;
}

// Função para obter o nome do turno
function getShiftName(shiftsArray, shiftId) {
  const shift = shiftsArray.find(s => s.id === shiftId);
  return shift ? `${shift.name} (${shift.type})` : 'Turno desconhecido';
}

// Função para carregar todos os dados necessários
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar diretor, pedidos, cursos, estudantes e turnos em paralelo
    await loadMultipleData([
      { endpoint: `/directors/${directorId.value}`, ref: directorData },
      { endpoint: '/shiftRequests', ref: requests },
      { endpoint: '/courses', ref: courses },
      { endpoint: '/students', ref: students },
      { endpoint: '/shifts', ref: shifts }
    ], loading, error);
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Tente novamente.';
    loading.value = false;
  }
}

// Função para verificar conflitos e armazená-los na tabela de conflitos
async function checkForConflicts(studentId, newShiftId, currentShiftId) {
  try {
    // 1. Obter as alocações do aluno
    const allocationsResponse = await api.get(`/allocations?studentId=${studentId}`);
    let studentAllocations = allocationsResponse.data;
    
    // 2. Simular a troca de turno para verificação
    // Remover a alocação atual do turno que será substituído
    studentAllocations = studentAllocations.filter(allocation => 
      allocation.shiftId !== currentShiftId
    );
    
    // Adicionar temporariamente a nova alocação
    studentAllocations.push({
      studentId: studentId,
      shiftId: newShiftId
    });
    
    // 3. Obter os IDs dos turnos alocados
    const shiftIds = studentAllocations.map(allocation => allocation.shiftId);
    
    // 4. Obter detalhes completos dos turnos
    const studentShiftDetails = [];
    for (const shiftId of shiftIds) {
      const shiftResponse = await api.get(`/shifts/${shiftId}`);
      studentShiftDetails.push(shiftResponse.data);
    }
    
    // 5. Verificar conflitos para cada turno
    let conflicts = [];
    
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
            // Obter detalhes dos cursos
            const course1Response = await api.get(`/courses/${shift1.courseId}`);
            const course2Response = await api.get(`/courses/${shift2.courseId}`);
            
            // Criar objeto de conflito para a tabela conflicts
            conflicts.push({
              shift1Id: shift1.id,
              shift2Id: shift2.id,
              studentId: studentId,
              day: shift1.day,
              timeRange: `${Math.max(shift1Start, shift2Start)}:00 - ${Math.min(shift1End, shift2End)}:00`,
              // Informações adicionais apenas para exibição (não serão salvas na tabela)
              course1: course1Response.data.abbreviation,
              course2: course2Response.data.abbreviation,
              shift1Name: shift1.name,
              shift2Name: shift2.name
            });
          }
        }
      }
    }
    
    return conflicts;
  } catch (err) {
    console.error('Erro ao verificar conflitos:', err);
    return [];
  }
}

// Aplicar filtros
function applyFilters() {
  currentPage.value = 1;
}

// Resetar filtros
function resetFilters() {
  filters.value = { course: '', status: '' };
  applyFilters();
}

// Pedidos filtrados
const filteredRequests = computed(() => {
  return requests.value.filter(request => {
    // Filtrar por curso
    if (filters.value.course && filters.value.course !== '') {
      const courseId = getShiftCourseId(shifts.value, request.shiftId);
      if (courseId !== parseInt(filters.value.course)) {
        return false;
      }
    }
    
    // Filtrar por status
    if (filters.value.status === 'pending' && request.response !== null) return false;
    if (filters.value.status === 'approved' && request.response !== 'ok') return false;
    if (filters.value.status === 'rejected' && request.response !== 'rejected') return false;
    
    return true;
  });
});

// Total de páginas
const totalPages = computed(() => {
  return Math.ceil(filteredRequests.value.length / itemsPerPage);
});

// Pedidos paginados
const paginatedRequests = computed(() => {
  const start = (currentPage.value - 1) * itemsPerPage;
  const end = start + itemsPerPage;
  return filteredRequests.value.slice(start, end);
});

// Navegação de páginas
function nextPage() {
  if (currentPage.value < totalPages.value) currentPage.value++;
}

function previousPage() {
  if (currentPage.value > 1) currentPage.value--;
}

// Modal functions
async function openDetailsModal(request) {
  selectedRequest.value = request;
  directorComment.value = request.directorComment || '';
  
  // Se o pedido ainda estiver pendente, verificar por conflitos
  if (request.response === null) {
    // Mostrar carregamento
    loading.value = true;
    
    // Verificar conflitos
    const conflicts = await checkForConflicts(
      request.studentId,
      request.alternativeShiftId,
      request.shiftId
    );
    
    // Armazenar conflitos e mostrar aviso se houver
    selectedRequestConflicts.value = conflicts;
    
    loading.value = false;
  }
  
  showDetailsModal.value = true;
}

function closeDetailsModal() {
  showDetailsModal.value = false;
  selectedRequest.value = {};
  directorComment.value = '';
  selectedRequestConflicts.value = [];
}

// Aprovar pedido
async function approveRequest() {
  try {
    // Se houver conflitos, perguntar ao diretor se deseja continuar
    if (selectedRequestConflicts.value.length > 0) {
      const confirmAction = confirm(
        `ATENÇÃO: Este pedido causará ${selectedRequestConflicts.value.length} conflito(s) no horário do aluno. Deseja realmente aprovar?`
      );
      
      if (!confirmAction) {
        return; // Cancelar aprovação
      }
      
      // Se diretor escolher prosseguir, armazenar os conflitos na tabela
      for (const conflict of selectedRequestConflicts.value) {
        // Preparar objeto para armazenamento (remover propriedades extras)
        const conflictToSave = {
          shift1Id: conflict.shift1Id,
          shift2Id: conflict.shift2Id,
          studentId: selectedRequest.value.studentId,
          day: conflict.day,
          timeRange: conflict.timeRange
        };
        
        // Salvar na tabela conflicts
        await api.post('/conflicts', conflictToSave);
      }
    }
    
    loading.value = true;
    
    // Atualizar status do pedido
    await api.patch(`/shiftRequests/${selectedRequest.value.id}`, {
      response: 'ok',
      directorComment: directorComment.value,
      responseDate: new Date().toISOString()
    });
    
    // Encontrar a alocação atual
    const allocationsResponse = await api.get(`/allocations?studentId=${selectedRequest.value.studentId}&shiftId=${selectedRequest.value.shiftId}`);
    const currentAllocation = allocationsResponse.data[0];
    
    if (currentAllocation) {
      // Atualizar a alocação para o novo turno
      await api.patch(`/allocations/${currentAllocation.id}`, {
        shiftId: selectedRequest.value.alternativeShiftId
      });
    } else {
      // Se não houver alocação, criar uma nova
      await api.post('/allocations', {
        studentId: selectedRequest.value.studentId,
        shiftId: selectedRequest.value.alternativeShiftId
      });
    }
    
    // Atualizar lista de pedidos
    await loadAllData();
    closeDetailsModal();
  } catch (err) {
    console.error('Erro ao aprovar pedido:', err);
    alert('Erro ao aprovar pedido. Tente novamente.');
    loading.value = false;
  }
}

// Rejeitar pedido
async function rejectRequest() {
  try {
    loading.value = true;
    
    await api.patch(`/shiftRequests/${selectedRequest.value.id}`, {
      response: 'rejected',
      directorComment: directorComment.value,
      responseDate: new Date().toISOString()
    });
    
    // Atualizar lista de pedidos
    await loadAllData();
    closeDetailsModal();
  } catch (err) {
    console.error('Erro ao rejeitar pedido:', err);
    alert('Erro ao rejeitar pedido. Tente novamente.');
    loading.value = false;
  }
}

// Carregar dados ao montar o componente
onMounted(async () => {
  await loadAllData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name"
    userType="diretor"
    activeMenuItem="pedidos-mudanca"
  >
    <div class="requests-management-container">
      <div class="header-section">
        <h1>Gestão de Pedidos de Mudança</h1>
        <p>Gerencie os pedidos de alteração de turno dos alunos</p>
      </div>

      <!-- Filtros -->
      <div class="filters-container">
        <div class="filter-group">
          <label>Disciplina:</label>
          <select v-model="filters.course">
            <option value="">Todas</option>
            <option v-for="course in courses" :key="course.id" :value="course.id">
              {{ course.abbreviation }}
            </option>
          </select>
        </div>

        <div class="filter-group">
          <label>Status:</label>
          <select v-model="filters.status">
            <option value="">Todos</option>
            <option value="pending">Pendentes</option>
            <option value="approved">Aprovados</option>
            <option value="rejected">Rejeitados</option>
          </select>
        </div>

        <div class="filter-actions">
          <button @click="resetFilters" class="filter-button secondary">Limpar</button>
        </div>
      </div>

      <!-- Tabela de pedidos -->
      <div class="requests-table-container">
        <table class="requests-table">
          <thead>
            <tr>
              <th>Aluno</th>
              <th>UC</th>
              <th>Pedido</th>
              <th>Data</th>
              <th>Status</th>
              <th>Ações</th>
            </tr>
          </thead>
          <tbody v-if="loading">
            <tr>
              <td colspan="6" class="loading-cell">
                <div class="spinner"></div>
                Carregando pedidos...
              </td>
            </tr>
          </tbody>
          <tbody v-else-if="error">
            <tr>
              <td colspan="6" class="error-cell">
                {{ error }}
                <button @click="loadAllData" class="retry-button">Tentar novamente</button>
              </td>
            </tr>
          </tbody>
          <tbody v-else-if="filteredRequests.length === 0">
            <tr>
              <td colspan="6" class="empty-cell">
                Nenhum pedido encontrado com os filtros atuais
              </td>
            </tr>
          </tbody>
          <tbody v-else>
            <tr v-for="request in paginatedRequests" :key="request.id">
              <td>{{ getStudentName(students, request.studentId) }}</td>
              <td>{{ getCourseAbbr(courses, getShiftCourseId(shifts, request.shiftId)) }}</td>
              <td>De {{ getShiftName(shifts, request.shiftId) }} para {{ getShiftName(shifts, request.alternativeShiftId) }}</td>
              <td>{{ formatDate(request.date) }}</td>
              <td>
                <span :class="['status-badge', getStatusClass(request.response)]">
                  {{ getRequestStatus(request.response) }}
                </span>
              </td>
              <td class="actions-cell">
                <button 
                  @click="openDetailsModal(request)" 
                  class="details-button"
                >
                  Analisar
                </button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <!-- Paginação -->
      <div v-if="totalPages > 1" class="pagination">
        <button 
          @click="previousPage" 
          :disabled="currentPage === 1"
          class="pagination-button"
        >
          Anterior
        </button>
        <span class="page-info">
          Página {{ currentPage }} de {{ totalPages }}
        </span>
        <button 
          @click="nextPage" 
          :disabled="currentPage === totalPages"
          class="pagination-button"
        >
          Próxima
        </button>
      </div>

      <!-- Modal de detalhes do pedido -->
      <div v-if="showDetailsModal" class="modal-overlay" @click.self="closeDetailsModal">
        <div class="modal-container">
          <div class="modal-header">
            <h2>Detalhes do Pedido</h2>
            <button @click="closeDetailsModal" class="modal-close">×</button>
          </div>
          
          <div class="modal-content">
            <div class="detail-row">
              <span class="detail-label">Aluno:</span>
              <span class="detail-value">{{ getStudentName(students, selectedRequest.studentId) }}</span>
            </div>
            
            <div class="detail-row">
              <span class="detail-label">UC:</span>
              <span class="detail-value">{{ getCourseAbbr(courses, getShiftCourseId(shifts, selectedRequest.shiftId)) }}</span>
            </div>
            
            <div class="detail-row">
              <span class="detail-label">Pedido:</span>
              <span class="detail-value">
                Mudança de {{ getShiftName(shifts, selectedRequest.shiftId) }} para {{ getShiftName(shifts, selectedRequest.alternativeShiftId) }}
              </span>
            </div>
            
            <div class="detail-row">
              <span class="detail-label">Data:</span>
              <span class="detail-value">{{ formatDate(selectedRequest.date) }}</span>
            </div>
            
            <div class="detail-row">
              <span class="detail-label">Status:</span>
              <span :class="['status-badge', getStatusClass(selectedRequest.response)]">
                {{ getRequestStatus(selectedRequest.response) }}
              </span>
            </div>
            
            <div class="detail-row">
              <span class="detail-label">Motivo do aluno:</span>
              <div class="detail-value-box">
                {{ selectedRequest.reason || 'Nenhum motivo fornecido' }}
              </div>
            </div>
            
            <!-- Aviso de conflitos de horário -->
            <div v-if="selectedRequest.response === null && selectedRequestConflicts.length > 0" 
                class="conflict-alert">
              <div class="conflict-alert-header">
                <span class="warning-icon">⚠️</span> Atenção: Conflitos Detectados no Horário
              </div>
              <div class="conflict-alert-content">
                <p>Aprovar este pedido irá gerar os seguintes conflitos no horário do aluno:</p>
                <ul class="conflicts-list">
                  <li v-for="conflict in selectedRequestConflicts" :key="`${conflict.shift1Id}-${conflict.shift2Id}`">
                    <strong>{{ dayMap[conflict.day] }}</strong>: 
                    {{ conflict.course1 }} ({{ conflict.shift1Name }}) e 
                    {{ conflict.course2 }} ({{ conflict.shift2Name }})
                    {{ conflict.timeRange }}
                  </li>
                </ul>
              </div>
            </div>
            
            <!-- Apenas para pedidos pendentes -->
            <div v-if="selectedRequest.response === null" class="decision-section">
              <div class="form-group">
                <label for="directorComment">Comentário (opcional):</label>
                <textarea
                  id="directorComment"
                  v-model="directorComment"
                  placeholder="Adicione um comentário para o aluno"
                  rows="3"
                ></textarea>
              </div>
              
              <div class="decision-buttons">
                <button 
                  @click="rejectRequest" 
                  class="reject-button"
                >
                  Rejeitar
                </button>
                <button 
                  @click="approveRequest" 
                  class="approve-button"
                >
                  Aprovar
                </button>
              </div>
            </div>
            
            <!-- Para pedidos já respondidos -->
            <div v-else class="response-details">
              <div class="detail-row">
                <span class="detail-label">Resposta em:</span>
                <span class="detail-value">{{ formatDate(selectedRequest.responseDate) }}</span>
              </div>
              
              <div class="detail-row">
                <span class="detail-label">Comentário:</span>
                <div class="detail-value-box">
                  {{ selectedRequest.directorComment || 'Nenhum comentário adicionado' }}
                </div>
              </div>
            </div>
          </div>
          
          <div class="modal-footer">
            <button @click="closeDetailsModal" class="close-button">Fechar</button>
          </div>
        </div>
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
.requests-management-container {
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

/* Filters */
.filters-container {
  display: flex;
  gap: 15px;
  margin-bottom: 20px;
  background-color: white;
  padding: 15px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.filter-group {
  flex: 1;
  min-width: 150px;
}

.filter-group label {
  display: block;
  margin-bottom: 5px;
  font-size: 0.9rem;
  color: #555;
  font-weight: 500;
}

.filter-group select {
  width: 100%;
  padding: 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
  background-color: white;
}

.filter-actions {
  display: flex;
  align-items: flex-end;
  gap: 10px;
}

.filter-button {
  padding: 8px 15px;
  border: none;
  border-radius: 4px;
  background-color: #4863c1;
  color: white;
  cursor: pointer;
  font-size: 0.9rem;
  white-space: nowrap;
}

.filter-button.secondary {
  background-color: #f0f0f0;
  color: #333;
}

/* Table Container */
.requests-table-container {
  background-color: white;
  border-radius: 8px;
  overflow-x: auto;
  -webkit-overflow-scrolling: touch;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

/* Table Styles */
.requests-table {
  width: 100%;
  border-collapse: collapse;
}

.requests-table th,
.requests-table td {
  padding: 12px 15px;
  text-align: left;
  border-bottom: 1px solid #eee;
  font-size: 0.9rem;
}

.requests-table th {
  background-color: #f9f9f9;
  font-weight: 600;
  color: #333;
}

.requests-table tr:hover {
  background-color: #f5f5f5;
}

/* Action buttons */
.details-button {
  padding: 6px 12px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.85rem;
  white-space: nowrap;
}

.details-button:hover {
  background-color: #3a4e9c;
}

/* Status badges */
.status-badge {
  display: inline-block;
  padding: 4px 10px;
  border-radius: 12px;
  font-size: 0.8rem;
  font-weight: 500;
}

.status-pending {
  background-color: #fff7e6;
  color: #e69500;
}

.status-approved {
  background-color: #e6f7e6;
  color: #2e8b57;
}

.status-rejected {
  background-color: #ffeaea;
  color: #e53935;
}

/* Pagination */
.pagination {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 15px;
  margin-top: 20px;
}

.pagination-button {
  padding: 8px 15px;
  border: 1px solid #ddd;
  background-color: white;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
}

.pagination-button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.page-info {
  font-size: 0.9rem;
  color: #666;
}

/* Modal styles */
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
  width: 600px;
  max-width: 95%;
  max-height: 90vh;
  overflow-y: auto;
  box-shadow: 0 5px 15px rgba(0,0,0,0.2);
}

.modal-header {
  padding: 20px;
  border-bottom: 1px solid #eee;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.modal-header h2 {
  margin: 0;
  font-size: 1.3rem;
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

.detail-row {
  margin-bottom: 15px;
  display: flex;
}

.detail-label {
  font-weight: 500;
  color: #555;
  min-width: 120px;
}

.detail-value {
  flex: 1;
}

.detail-value-box {
  flex: 1;
  padding: 10px;
  background-color: #f9f9f9;
  border-radius: 4px;
  border: 1px solid #eee;
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

.warning-icon {
  margin-right: 8px;
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

/* Decision section */
.decision-section {
  margin-top: 25px;
  padding-top: 20px;
  border-top: 1px solid #eee;
}

.form-group {
  margin-bottom: 15px;
}

.form-group label {
  display: block;
  margin-bottom: 5px;
  font-weight: 500;
  color: #555;
}

.form-group textarea {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  min-height: 80px;
}

.decision-buttons {
  display: flex;
  justify-content: flex-end;
  gap: 10px;
  margin-top: 15px;
}

.approve-button {
  padding: 10px 20px;
  background-color: #28a745;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.reject-button {
  padding: 10px 20px;
  background-color: #dc3545;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.approve-button:hover {
  background-color: #218838;
}

.reject-button:hover {
  background-color: #c82333;
}

.modal-footer {
  padding: 15px 20px;
  border-top: 1px solid #eee;
  display: flex;
  justify-content: flex-end;
}

.close-button {
  padding: 8px 15px;
  background-color: #f0f0f0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.close-button:hover {
  background-color: #e0e0e0;
}

/* Loading and error states */
.loading-cell,
.error-cell,
.empty-cell {
  text-align: center;
  padding: 30px !important;
  color: #666;
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

.retry-button {
  margin-top: 10px;
  padding: 6px 12px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

/* Responsive adjustments */
@media (max-width: 768px) {
  .filters-container {
    flex-direction: column;
  }
  
  .filter-group {
    width: 100%;
  }
  
  .modal-container {
    width: 95%;
  }
  
  .detail-row {
    flex-direction: column;
  }
  
  .detail-label {
    margin-bottom: 5px;
  }
  
  .decision-buttons {
    flex-direction: column;
  }
  
  .approve-button,
  .reject-button {
    width: 100%;
  }
}

@media (max-width: 480px) {
  .requests-table th,
  .requests-table td {
    padding: 8px 10px;
    font-size: 0.8rem;
    white-space: nowrap;
    min-width: 100px;
    max-width: 200px;
    overflow: hidden;
    text-overflow: ellipsis;
  }
  
  .details-button {
    padding: 4px 8px;
    font-size: 0.8rem;
    white-space: nowrap;
    min-width: 100px;
    max-width: 200px;
    overflow: hidden;
    text-overflow: ellipsis;
  }
  
  .details-button {
    padding: 4px 8px;
    font-size: 0.8rem;
  }
  
  .pagination {
    gap: 8px;
  }
  
  .pagination-button {
    padding: 6px 10px;
  }
}
</style>