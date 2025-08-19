<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import Notification from '../common/Notification.vue';
import { 
  loadMultipleData, 
  formatDate,
  getStatusClass,
  getRequestStatus,
  getCourseAbbr
} from '../../utils/common';
import api from '../../services/api';

// Dados do aluno e dos pedidos
const studentData = ref(null);
const courses = ref([]);
const shifts = ref([]);
const studentRequests = ref([]);
const loading = ref(true);
const error = ref(null);

// ID do aluno (obtido do localStorage)
const studentId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Notificação
const notification = ref({
  show: false,
  message: '',
  type: 'info'
});

const settings = ref({
  publishedSchedules: false,
  lastPublishedDate: null
});

// Paginação
const currentPage = ref(1);
const totalPages = ref(1);
const itemsPerPage = 5; // Número de itens por página

// Modais
const showDetailsModal = ref(false);
const showNewRequestModal = ref(false);
const selectedRequest = ref(null);
const newRequest = ref({
  courseId: '',
  currentShift: '',
  targetShift: '',
  reason: ''
});

// Estado de envio de formulário
const submitting = ref(false);
const cancelingRequest = ref(false);

// Função processadora para pedidos
function processRequests(data) {
  // Ordenar pedidos: pendentes primeiro, depois por data (mais recentes primeiro)
  return data.sort((a, b) => {
    // Priorizar pedidos pendentes
    if (a.response === null && b.response !== null) return -1;
    if (a.response !== null && b.response === null) return 1;
    
    // Depois, ordenar por data (mais recentes primeiro)
    return new Date(b.date) - new Date(a.date);
  });
}

// Carregar todos os dados
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
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
        ref: shifts 
      },
      { 
        endpoint: `/shiftRequests?studentId=${studentId.value}`, 
        ref: studentRequests,
        processor: processRequests
      },
      { 
        endpoint: '/settings', 
        ref: settings 
      }
    ], loading, error);
    
    updatePagination(); // Atualizar a paginação após carregar os dados
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente mais tarde.';
  } finally {
    loading.value = false;
  }
}

// Atualizar paginação
function updatePagination() {
  totalPages.value = Math.ceil(studentRequests.value.length / itemsPerPage) || 1;
  
  // Garantir que a página atual não ultrapasse o total de páginas
  if (currentPage.value > totalPages.value) {
    currentPage.value = totalPages.value;
  }
}

// Obter informações do turno com horário e dia
function getShiftInfo(shiftId) {
  const shift = shifts.value.find(s => s.id === shiftId);
  if (!shift) return 'Turno desconhecido';
  
  return `${shift.name} (${shift.type}) - ${shift.day}, ${shift.from}:00 - ${shift.to}:00`;
}

// Obter horário do turno em formato curto (para a tabela principal)
function getShiftInfoShort(shiftId) {
  const shift = shifts.value.find(s => s.id === shiftId);
  return shift ? `${shift.name} (${shift.type})` : 'Turno desconhecido';
}

// Obter informações da UC baseado no ID do turno
function getUcInfoFromShift(shiftId) {
  const shift = shifts.value.find(s => s.id === shiftId);
  if (!shift) return 'N/A';
  
  const course = courses.value.find(c => c.id === shift.courseId);
  return course ? getCourseAbbr(courses.value, course.id) : 'N/A';
}

// Paginação
function goToNextPage() {
  if (currentPage.value < totalPages.value) {
    currentPage.value++;
  }
}

function goToPreviousPage() {
  if (currentPage.value > 1) {
    currentPage.value--;
  }
}

// Pedidos na página atual
const paginatedRequests = computed(() => {
  const start = (currentPage.value - 1) * itemsPerPage;
  const end = start + itemsPerPage;
  return studentRequests.value.slice(start, end);
});

// Interações do usuário
function showDetails(request) {
  selectedRequest.value = request;
  showDetailsModal.value = true;
}

function closeDetailsModal() {
  showDetailsModal.value = false;
  selectedRequest.value = null;
}

function showNewRequest() {
  newRequest.value = {
    courseId: '',
    currentShift: '',
    targetShift: '',
    reason: ''
  };
  
  showNewRequestModal.value = true;
}

function closeNewRequestModal() {
  showNewRequestModal.value = false;
}

function handleCurrentShiftChange() {
  // Resetar turno pretendido quando o turno atual muda
  newRequest.value.targetShift = '';
}

// Mostrar notificação
function showNotification(message, type = 'info') {
  notification.value = {
    show: true,
    message,
    type
  };
}

// Fechar notificação
function closeNotification() {
  notification.value.show = false;
}

// Enviar novo pedido
async function submitNewRequest() {
  // Validação de formulário
  if (!newRequest.value.courseId || !newRequest.value.currentShift || !newRequest.value.targetShift) {
    showNotification('Por favor, preencha todos os campos obrigatórios.', 'error');
    return;
  }
  
  try {
    submitting.value = true;
    
    // Criar objeto do pedido com a estrutura adequada
    const requestData = {
      studentId: studentId.value,
      shiftId: parseInt(newRequest.value.currentShift),
      alternativeShiftId: parseInt(newRequest.value.targetShift),
      reason: newRequest.value.reason || 'Nenhum motivo fornecido.',
      date: new Date().toISOString(),
      response: null,
      responseSeenByStudent: false,
      directorComment: null,
      responseDate: null
    };
    
    // Enviar pedido para o servidor
    const response = await api.post('/shiftRequests', requestData);
    
    // Adicionar o novo pedido à lista
    if (response.data && response.data.id) {
      // Adicionar o novo pedido com as informações necessárias
      studentRequests.value.unshift({
        ...response.data,
      });
      
      // Reorganizar os pedidos usando o processador
      studentRequests.value = processRequests([...studentRequests.value]);
      
      // Atualizar paginação
      updatePagination();
    } else {
      // Se não recebemos um ID, recarregar todos os dados
      await loadAllData();
    }
    
    // Fechar formulário e mostrar notificação de sucesso
    closeNewRequestModal();
    showNotification('Pedido enviado com sucesso!', 'success');
  } catch (err) {
    console.error('Erro ao enviar pedido:', err);
    showNotification('Erro ao enviar pedido. Tente novamente mais tarde.', 'error');
  } finally {
    submitting.value = false;
  }
}

// Cancelar pedido
async function cancelRequest(requestId) {
  try {
    cancelingRequest.value = true;
    
    // Excluir o pedido do servidor
    await api.delete(`/shiftRequests/${requestId}`);
    
    // Remover o pedido da lista local
    studentRequests.value = studentRequests.value.filter(req => req.id !== requestId);
    
    // Atualizar paginação
    updatePagination();
    
    // Mostrar notificação de sucesso
    showNotification('Pedido cancelado com sucesso!', 'success');
  } catch (err) {
    console.error('Erro ao cancelar pedido:', err);
    showNotification('Erro ao cancelar pedido. Tente novamente mais tarde.', 'error');
    
    // Recarregar dados em caso de erro
    await loadAllData();
  } finally {
    cancelingRequest.value = false;
  }
}

// Computed properties para selects do formulário
// Obter disciplinas do aluno
const studentCourses = computed(() => {
  if (!studentData.value || !studentData.value.enrolled || !courses.value.length) 
    return [];
  
  return courses.value.filter(course => {
    return studentData.value.enrolled.includes(course.id);
  });
});

// Obter turnos disponíveis para a disciplina selecionada
const availableShifts = computed(() => {
  if (!newRequest.value.courseId) return [];
  
  return shifts.value.filter(shift => {
    return shift.courseId === parseInt(newRequest.value.courseId);
  });
});

// Obter turnos compatíveis com o turno atual (mesmo tipo)
const compatibleShifts = computed(() => {
  if (!newRequest.value.currentShift) return [];
  
  const currentShift = shifts.value.find(s => s.id === parseInt(newRequest.value.currentShift));
  if (!currentShift) return [];
  
  // Filtrar turnos do mesmo tipo e disciplina, excluindo o turno atual
  return shifts.value.filter(shift => {
    return shift.courseId === parseInt(newRequest.value.courseId) && 
           shift.type === currentShift.type && 
           shift.id !== parseInt(newRequest.value.currentShift);
  });
});

// Obter tipo do turno atual
const currentShiftType = computed(() => {
  if (!newRequest.value.currentShift) return '';
  
  const currentShift = shifts.value.find(s => s.id === parseInt(newRequest.value.currentShift));
  return currentShift ? currentShift.type : '';
});

// Carregar dados ao montar o componente
onMounted(() => {
  loadAllData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="studentData?.name || 'Carregando...'" 
    userType="aluno"
    activeMenuItem="pedidos"
  >
    <div class="pedidos-container">
      <div class="header-section">
        <h2 class="page-title">Meus Pedidos</h2>
        <p class="page-subtitle">Acompanhe os seus pedidos de alteração de turno</p>
      </div>
      
      <div class="actions-bar">
        <button 
          @click="showNewRequest" 
          class="new-request-button"
          :disabled="!settings.publishedSchedules"
          :title="!settings.publishedSchedules ? 'Os horários ainda não foram publicados' : 'Criar novo pedido'"
        >
          Novo Pedido
        </button>

        <!-- Mensagem de aviso quando os horários não estão publicados -->
        <div v-if="!settings.publishedSchedules" class="schedule-info-warning">
          <i class="war"></i> Os horários ainda não foram publicados pelo diretor
        </div>
      </div>
      
      <!-- Tabela de pedidos -->
      <div class="pedidos-table-container">
        <table class="pedidos-table">
          <thead>
            <tr>
              <th class="col-uc">UC</th>
              <th class="col-pedido">Pedido</th>
              <th class="col-data">Data</th>
              <th class="col-status">Status</th>
              <th class="col-action">Ação</th>
            </tr>
          </thead>
          <tbody v-if="loading">
            <tr>
              <td colspan="5" class="loading-cell">
                <div class="spinner"></div>
                <div>Carregando...</div>
              </td>
            </tr>
          </tbody>
          <tbody v-else-if="error">
            <tr>
              <td colspan="5" class="error-cell">
                {{ error }}
                <div class="retry-action">
                  <button @click="loadAllData" class="retry-button">Tentar novamente</button>
                </div>
              </td>
            </tr>
          </tbody>
          <tbody v-else-if="studentRequests.length === 0">
            <tr>
              <td colspan="5" class="empty-cell">
                <div class="empty-state">
                  <div class="empty-message">Nenhum pedido de alteração de turno encontrado.</div>                 
                </div>
              </td>
            </tr>
          </tbody>
          <tbody v-else>
            <tr v-for="request in paginatedRequests" :key="request.id">
              <td>{{ getUcInfoFromShift(request.shiftId) }}</td>
              <td>Mudança de {{ getShiftInfoShort(request.shiftId) }} para {{ getShiftInfoShort(request.alternativeShiftId) }}</td>
              <td>{{ formatDate(request.date) }}</td>
              <td>
                <span :class="['status-badge', getStatusClass(request.response)]">
                  {{ getRequestStatus(request.response) }}
                </span>
              </td>
              <td class="actions-cell">
                <button 
                  v-if="request.response === null" 
                  @click="cancelRequest(request.id)" 
                  class="cancel-button"
                  title="Cancelar pedido"
                  :disabled="cancelingRequest"
                >
                  {{ cancelingRequest ? 'Cancelando...' : 'Cancelar' }}
                </button>
                <button 
                  v-else 
                  @click="showDetails(request)" 
                  class="details-button"
                  title="Ver detalhes do pedido"
                >
                  Detalhes
                </button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>
      
      <!-- Paginação (mostrada apenas se tiver mais de uma página) -->
      <div v-if="totalPages > 1" class="pagination">
        <button @click="goToPreviousPage" class="pagination-button" :disabled="currentPage === 1">
          <span class="pagination-icon">◀</span>
        </button>
        <span class="pagination-info">Página {{ currentPage }} de {{ totalPages }}</span>
        <button @click="goToNextPage" class="pagination-button" :disabled="currentPage === totalPages">
          <span class="pagination-icon">▶</span>
        </button>
      </div>
    </div>
    
    <!-- Modal de detalhes do pedido -->
    <div v-if="showDetailsModal" class="modal-overlay" @click.self="closeDetailsModal">
      <div class="modal-container">
        <div class="modal-header">
          <h3>Detalhes do Pedido</h3>
          <button @click="closeDetailsModal" class="modal-close-button" aria-label="Fechar">×</button>
        </div>
        <div class="modal-content">
          <div class="detail-item">
            <span class="detail-label">UC:</span>
            <span class="detail-value">{{ getUcInfoFromShift(selectedRequest?.shiftId) }}</span>
          </div>
          <div class="detail-item">
            <span class="detail-label">Turno Atual:</span>
            <span class="detail-value">{{ getShiftInfo(selectedRequest?.shiftId) }}</span>
          </div>
          <div class="detail-item">
            <span class="detail-label">Turno Pretendido:</span>
            <span class="detail-value">{{ getShiftInfo(selectedRequest?.alternativeShiftId) }}</span>
          </div>
          <div class="detail-item">
            <span class="detail-label">Data:</span>
            <span class="detail-value">{{ formatDate(selectedRequest?.date) }}</span>
          </div>
          <div class="detail-item">
            <span class="detail-label">Status:</span>
            <span :class="['status-badge', getStatusClass(selectedRequest?.response)]">
              {{ getRequestStatus(selectedRequest?.response) }}
            </span>
          </div>
          <div class="detail-item">
            <span class="detail-label">Motivo:</span>
            <p class="detail-reason">{{ selectedRequest?.reason || 'Nenhum motivo fornecido.' }}</p>
          </div>
          
          <!-- Comentário do diretor (apenas se houver resposta) -->
          <div v-if="selectedRequest?.response !== null" class="detail-item">
            <span class="detail-label">Resposta:</span>
            <div class="detail-response">
              <p class="response-date">{{ formatDate(selectedRequest?.responseDate) }}</p>
              <p class="response-comment">{{ selectedRequest?.directorComment || 'Sem comentários adicionais.' }}</p>
            </div>
          </div>
        </div>
        <div class="modal-footer">
          <button @click="closeDetailsModal" class="modal-button">Fechar</button>
        </div>
      </div>
    </div>
    
    <!-- Modal de novo pedido -->
    <div v-if="showNewRequestModal" class="modal-overlay" @click.self="closeNewRequestModal">
      <div class="modal-container">
        <div class="modal-header">
          <h3>Novo Pedido de Alteração de Turno</h3>
          <button @click="closeNewRequestModal" class="modal-close-button" aria-label="Fechar">×</button>
        </div>
        <form @submit.prevent="submitNewRequest" class="modal-content">
          <div class="form-group">
            <label for="courseId">Unidade Curricular:</label>
            <select 
              id="courseId" 
              v-model="newRequest.courseId" 
              class="form-control"
              required
            >
              <option value="" disabled selected>Selecione a UC</option>
              <option v-for="course in studentCourses" :key="course.id" :value="course.id">
                {{ course.abbreviation }} - {{ course.name }}
              </option>
            </select>
          </div>
          
          <div class="form-group">
            <label for="currentShift">Turno Atual:</label>
            <select 
              id="currentShift" 
              v-model="newRequest.currentShift" 
              class="form-control" 
              :disabled="!newRequest.courseId"
              @change="handleCurrentShiftChange"
              required
            >
              <option value="" disabled selected>Selecione o turno atual</option>
              <option v-for="shift in availableShifts" :key="shift.id" :value="shift.id">
                {{ shift.name }} ({{ shift.type }}) - {{ shift.day }}, {{ shift.from }}:00 - {{ shift.to }}:00
              </option>
            </select>
            <small v-if="currentShiftType" class="form-text">
              Tipo de turno selecionado: {{ currentShiftType }}
            </small>
          </div>
          
          <div class="form-group">
            <label for="targetShift">Turno Pretendido:</label>
            <select 
              id="targetShift" 
              v-model="newRequest.targetShift" 
              class="form-control" 
              :disabled="!newRequest.currentShift"
              required
            >
              <option value="" disabled selected>Selecione o turno pretendido</option>
              <option v-for="shift in compatibleShifts" :key="shift.id" :value="shift.id">
                {{ shift.name }} ({{ shift.type }}) - {{ shift.day }}, {{ shift.from }}:00 - {{ shift.to }}:00
              </option>
            </select>
            <small v-if="compatibleShifts.length === 0 && newRequest.currentShift" class="form-text text-warning">
              Não existem turnos compatíveis disponíveis.
            </small>
            <small v-else-if="currentShiftType" class="form-text">
              Apenas turnos do tipo {{ currentShiftType }} são mostrados.
            </small>
          </div>
          
          <div class="form-group">
            <label for="reason">Motivo (opcional):</label>
            <textarea 
              id="reason" 
              v-model="newRequest.reason" 
              class="form-control"
              rows="3"
              placeholder="Explique o motivo do seu pedido de alteração"
            ></textarea>
            <small class="form-text">
              Fornecer um motivo pode aumentar as chances de aprovação do seu pedido.
            </small>
          </div>
          
          <div class="modal-footer">
            <button type="button" @click="closeNewRequestModal" class="modal-button-secondary">
              Cancelar
            </button>
            <button 
              type="submit" 
              class="modal-button-primary" 
              :disabled="submitting || !newRequest.courseId || !newRequest.currentShift || !newRequest.targetShift || compatibleShifts.length === 0"
            >
              <span v-if="submitting">Enviando...</span>
              <span v-else>Enviar Pedido</span>
            </button>
          </div>
        </form>
      </div>
    </div>
    
    <!-- Notification para mensagens -->
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
.pedidos-container {
  width: 100%;
  max-width: 100%;
  padding: 1rem 0;
}

.header-section {
  margin-bottom: 1.5rem;
}

.page-title {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.page-subtitle {
  font-size: 0.9rem;
  color: #666;
}

.actions-bar {
  display: flex;
  align-items: center;
  margin-bottom: 1rem;
}

.new-request-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.5rem 1rem;
  cursor: pointer;
  font-size: 0.9rem;
  font-weight: 500;
  transition: background-color 0.2s;
}

.new-request-button:hover {
  background-color: #3a4e9c;
}

.new-request-button:disabled {
  background-color: #a5b1e0;
  opacity: 0.7;
}

.schedule-info-warning {
  margin-left: 1rem;
  color: #e69500;
  font-size: 0.9rem;
  display: flex;
  align-items: center;
}

/* Tabela de pedidos */
.pedidos-table-container {
  background-color: white;
  border-radius: 4px;
  border: 1px solid #ddd;
  overflow: hidden;
  margin-bottom: 1rem;
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
}

.pedidos-table {
  width: 100%;
  border-collapse: collapse;
}

.pedidos-table th,
.pedidos-table td {
  padding: 0.75rem 1rem;
  text-align: left;
  border-bottom: 1px solid #eee;
}

.pedidos-table th {
  background-color: #f8f8f8;
  font-weight: 500;
  color: #333;
  white-space: nowrap;
}

.pedidos-table tr:last-child td {
  border-bottom: none;
}

.col-uc {
  width: 10%;
}

.col-pedido {
  width: 45%;
}

.col-data, 
.col-status {
  width: 15%;
}

.col-action {
  width: 15%;
  text-align: center;
}

.loading-cell,
.error-cell,
.empty-cell {
  text-align: center;
  padding: 2.5rem 1.5rem !important;
  color: #666;
}

.spinner {
  border: 3px solid rgba(0, 0, 0, 0.1);
  border-radius: 50%;
  border-top: 3px solid #4863c1;
  width: 24px;
  height: 24px;
  animation: spin 1s linear infinite;
  margin: 0 auto 0.75rem;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.retry-action {
  margin-top: 0.75rem;
}

.retry-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.4rem 0.75rem;
  font-size: 0.85rem;
  cursor: pointer;
}

/* Empty state */
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 1.5rem;
}

.empty-message {
  margin-bottom: 1rem;
  color: #666;
}

/* Status badges */
.status-badge {
  display: inline-block;
  padding: 0.25rem 0.75rem;
  border-radius: 20px;
  font-size: 0.8rem;
  font-weight: 500;
  white-space: nowrap;
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

/* Action buttons */
.actions-cell {
  text-align: center;
  white-space: nowrap;
}

.cancel-button,
.details-button {
  padding: 0.25rem 0.75rem;
  border-radius: 3px;
  font-size: 0.8rem;
  cursor: pointer;
  border: none;
  transition: background-color 0.2s;
}

.cancel-button {
  background-color: #f0f0f0;
  color: #333;
}

.cancel-button:disabled {
  background-color: #f0f0f0;
  color: #999;
  cursor: not-allowed;
}

.details-button {
  background-color: #e8f0ff;
  color: #4863c1;
}

.cancel-button:hover:not(:disabled) {
  background-color: #e0e0e0;
}

.details-button:hover {
  background-color: #d6e4ff;
}

/* Paginação */
.pagination {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-top: 1rem;
}

.pagination-button {
  background-color: #fff;
  border: 1px solid #ddd;
  border-radius: 50%;
  width: 30px;
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  margin: 0 0.5rem;
  transition: background-color 0.2s;
}

.pagination-button:hover:not(:disabled) {
  background-color: #f0f0f0;
}

.pagination-button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.pagination-info {
  font-size: 0.9rem;
  color: #666;
}

.pagination-icon {
  font-size: 0.7rem;
}

/* Modais */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  animation: fadeIn 0.2s ease-out;
}

@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

.modal-container {
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 5px 15px rgba(0, 0, 0, 0.15);
  width: 500px;
  max-width: 90%;
  max-height: 90vh;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  animation: slideIn 0.2s ease-out;
}

@keyframes slideIn {
  from { transform: translateY(-20px); opacity: 0; }
  to { transform: translateY(0); opacity: 1; }
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1rem;
  border-bottom: 1px solid #eee;
}

.modal-header h3 {
  font-size: 1.1rem;
  font-weight: 600;
  margin: 0;
  color: #333;
}

.modal-close-button {
  background: none;
  border: none;
  font-size: 1.5rem;
  line-height: 1;
  cursor: pointer;
  color: #999;
  padding: 0;
}

.modal-close-button:hover {
  color: #666;
}

.modal-content {
  padding: 1rem;
  flex-grow: 1;
}

.modal-footer {
  padding: 1rem;
  display: flex;
  justify-content: flex-end;
  gap: 0.5rem;
  border-top: 1px solid #eee;
}

/* Botões modais */
.modal-button,
.modal-button-primary,
.modal-button-secondary {
  padding: 0.5rem 1rem;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background-color 0.2s;
}

.modal-button {
  background-color: #f0f0f0;
  color: #333;
  border: 1px solid #ddd;
}

.modal-button:hover {
  background-color: #e0e0e0;
}

.modal-button-primary {
  background-color: #4863c1;
  color: white;
  border: none;
}

.modal-button-primary:hover:not(:disabled) {
  background-color: #3a4e9c;
}

.modal-button-primary:disabled {
  background-color: #a5b1e0;
  cursor: not-allowed;
}

.modal-button-secondary {
  background-color: #f0f0f0;
  color: #333;
  border: 1px solid #ddd;
}

.modal-button-secondary:hover {
  background-color: #e0e0e0;
}

/* Detalhes do pedido */
.detail-item {
  margin-bottom: 0.75rem;
  display: flex;
  align-items: flex-start;
}

.detail-label {
  font-weight: 500;
  min-width: 80px;
  margin-right: 0.5rem;
  color: #444;
}

.detail-reason {
  margin-top: 0.5rem;
  padding: 0.75rem;
  background-color: #f9f9f9;
  border-radius: 4px;
  border: 1px solid #eee;
  font-size: 0.9rem;
  color: #333;
}

.detail-response {
  margin-top: 0.5rem;
  padding: 0.75rem;
  background-color: #e8f0ff;
  border-radius: 4px;
  border: 1px solid #d6e4ff;
  font-size: 0.9rem;
}

.response-date {
  margin-bottom: 0.5rem;
  font-size: 0.8rem;
  color: #666;
}

.response-comment {
  color: #333;
}

/* Formulário */
.form-group {
  margin-bottom: 1rem;
}

.form-group label {
  display: block;
  font-size: 0.9rem;
  font-weight: 500;
  margin-bottom: 0.5rem;
  color: #444;
}

.form-control {
  width: 100%;
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
  transition: border-color 0.2s;
}

.form-control:focus {
  border-color: #4863c1;
  outline: none;
  box-shadow: 0 0 0 2px rgba(72, 99, 193, 0.1);
}

.form-control:disabled {
  background-color: #f5f5f5;
  cursor: not-allowed;
}

select.form-control {
  height: 36px;
  appearance: none;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='10' height='5' viewBox='0 0 10 5'%3E%3Cpath fill='%23333' d='M0 0l5 5 5-5z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: right 0.7rem center;
  padding-right: 1.5rem;
}

textarea.form-control {
  resize: vertical;
  min-height: 80px;
}

.form-text {
  font-size: 0.8rem;
  color: #666;
  margin-top: 0.25rem;
  display: block;
}

.text-warning {
  color: #e69500;
}

/* Responsividade */
@media (max-width: 768px) {
  .col-pedido {
    max-width: 150px;
  }
  
  .col-data {
    max-width: 100px;
  }
  
  .modal-container {
    width: 95%;
  }
  
  .detail-item {
    flex-direction: column;
  }
  
  .detail-label {
    margin-bottom: 0.25rem;
  }
}
</style>