<script setup>
import { ref, computed, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import Notification from '../common/Notification.vue';
import { 
  formatDate,
  loadStudentData,
  loadData,
  loadMultipleData
} from '../../utils/common';
import api from '../../services/api';

// Dados do aluno e mensagens
const studentData = ref(null);
const messages = ref([]);
const loading = ref(true);
const error = ref(null);
const sending = ref(false);

// ID e email do aluno
const studentId = ref(parseInt(localStorage.getItem('userId') || '1'));
const studentEmail = computed(() => {
  return studentData.value?.email || localStorage.getItem('userEmail') || '';
});

// Notificação
const notification = ref({
  show: false,
  message: '',
  type: 'info'
});

// Paginação
const currentPage = ref(1);
const totalPages = ref(1);
const itemsPerPage = 5;

// Estados da UI
const showDetailsModal = ref(false);
const showNewMessageModal = ref(false);
const selectedMessage = ref(null);
const activeTab = ref('received');
const newMessage = ref({
  to: '',
  subject: '',
  body: ''
});

// Função para processar mensagens
function processMessages(data) {
  const studentEmail = studentData.value?.email || localStorage.getItem('userEmail') || '';
  
  return data
    .filter(msg => 
      // Mensagens onde o aluno é o destinatário (to) ou remetente (from)
      msg.to.toLowerCase() === studentEmail.toLowerCase() || 
      msg.from.toLowerCase() === studentEmail.toLowerCase() ||
      // Ou mensagens especificamente enviadas para este aluno (com studentId)
      (msg.studentId && msg.studentId === studentId.value)
    )
    .map(msg => ({
      ...msg,
      isReceived: 
        msg.to.toLowerCase() === studentEmail.toLowerCase() ||
        (msg.senderType !== 'student' && msg.studentId === studentId.value)
    }))
    .sort((a, b) => new Date(b.date) - new Date(a.date));
}

// Computed properties
const filteredMessages = computed(() => {
  return messages.value.filter(msg => 
    activeTab.value === 'received' ? msg.isReceived : !msg.isReceived
  );
});

const paginatedMessages = computed(() => {
  const start = (currentPage.value - 1) * itemsPerPage;
  return filteredMessages.value.slice(start, start + itemsPerPage);
});

// Carregar todos os dados necessários
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar dados do aluno e mensagens em paralelo
    await loadMultipleData([
      { 
        endpoint: `/students/${studentId.value}`, 
        ref: studentData 
      },
      { 
        endpoint: `/messages?studentId=${studentId.value}`, 
        ref: messages,
        processor: processMessages
      }
    ], loading, error);
    
    // Atualizar paginação após carregar os dados
    updatePagination();
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente.';
  } finally {
    loading.value = false;
  }
}

// Atualizar paginação
function updatePagination() {
  totalPages.value = Math.ceil(filteredMessages.value.length / itemsPerPage) || 1;
  if (currentPage.value > totalPages.value) {
    currentPage.value = totalPages.value;
  }
}

// Navegação de páginas
function goToPage(page) {
  currentPage.value = page;
}

// Mostrar detalhes da mensagem
async function showDetails(message) {
  selectedMessage.value = message;
  if (message.isReceived && !message.read) {
    await markAsRead(message.id);
  }
  showDetailsModal.value = true;
}

// Marcar como lida
async function markAsRead(messageId) {
  try {
    await api.patch(`/messages/${messageId}`, { read: true });
    const msgIndex = messages.value.findIndex(m => m.id === messageId);
    if (msgIndex !== -1) messages.value[msgIndex].read = true;
  } catch (err) {
    console.error('Erro ao marcar mensagem como lida:', err);
  }
}

// Enviar mensagem
async function sendMessage() {
  if (!newMessage.value.to || !newMessage.value.subject) {
    showNotification('Por favor, preencha todos os campos obrigatórios.', 'error');
    return;
  }
  
  try {
    sending.value = true;
    
    // Referências para usuários a serem verificados
    const directors = ref([]);
    const teachers = ref([]);
    const students = ref([]);
    const errorCheck = ref(null);
    
    // Verificar destinatário
    await loadMultipleData([
      { endpoint: '/directors', ref: directors },
      { endpoint: '/teachers', ref: teachers },
      { endpoint: '/students', ref: students }
    ], null, errorCheck);
    
    if (errorCheck.value) {
      showNotification('Erro ao verificar destinatário. Tente novamente.', 'error');
      sending.value = false;
      return;
    }
    
    const recipientExists = [
      ...directors.value,
      ...teachers.value,
      ...students.value
    ].some(user => user.email?.toLowerCase() === newMessage.value.to.toLowerCase());
    
    if (!recipientExists) {
      showNotification('Destinatário não encontrado. Verifique o email informado.', 'error');
      sending.value = false;
      return;
    }
    
    // Enviar mensagem
    const messageData = {
      studentId: studentId.value,
      from: studentEmail.value,
      to: newMessage.value.to,
      subject: newMessage.value.subject,
      body: newMessage.value.body || '(Sem conteúdo)',
      date: new Date().toISOString(),
      read: false,
      senderType: 'student'
    };
    
    await api.post('/messages', messageData);
    
    // Adicionar a mensagem ao array local
    messages.value.unshift({
      ...messageData,
      id: Date.now(), // ID temporário até recarregar os dados
      isReceived: false
    });
    
    // Atualizar paginação
    updatePagination();
    
    showNotification('Mensagem enviada com sucesso!', 'success');
    showNewMessageModal.value = false;
    newMessage.value = { to: '', subject: '', body: '' };
    
  } catch (err) {
    console.error('Erro ao enviar mensagem:', err);
    showNotification(`Erro ao enviar mensagem: ${err.message}`, 'error');
  } finally {
    sending.value = false;
  }
}

// Notificação
function showNotification(message, type = 'info') {
  notification.value = { show: true, message, type };
}

function closeNotification() {
  notification.value.show = false;
}

// Carregar dados ao montar o componente
onMounted(() => {
  loadAllData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="studentData?.name || 'Carregando...'" 
    userType="aluno"
    activeMenuItem="mensagens"
  >
    <div class="messages-container">
      <div class="header-section">
        <h2>Minhas Mensagens</h2>
        <p>Gerencie suas mensagens recebidas e enviadas</p>
      </div>
      
      <div class="tabs-container">
        <div class="tabs">
          <button 
            @click="activeTab = 'received'; loadAllData()" 
            :class="{ 'active': activeTab === 'received' }"
          >
            Recebidas
          </button>
          <button 
            @click="activeTab = 'sent'; loadAllData()" 
            :class="{ 'active': activeTab === 'sent' }"
          >
            Enviadas
          </button>
        </div>
      </div>
      
      <div class="actions-bar">
        <button @click="showNewMessageModal = true" class="primary-button">
          Nova Mensagem
        </button>
      </div>
      
      <div class="table-container">
        <table>
          <thead>
            <tr>
              <th width="25%">{{ activeTab === 'received' ? 'De' : 'Para' }}</th>
              <th width="45%">Assunto</th>
              <th width="20%">Data</th>
              <th width="10%">Ação</th>
            </tr>
          </thead>
          <tbody>
            <tr v-if="loading">
              <td colspan="4" class="loading-cell">
                <div class="spinner"></div>
                <div>Carregando...</div>
              </td>
            </tr>
            
            <tr v-else-if="error">
              <td colspan="4" class="error-cell">
                {{ error }}
                <button @click="loadAllData" class="retry-button">Tentar novamente</button>
              </td>
            </tr>
            
            <tr v-else-if="filteredMessages.length === 0">
              <td colspan="4" class="empty-cell">
                Nenhuma mensagem {{ activeTab === 'received' ? 'recebida' : 'enviada' }}.
              </td>
            </tr>
            
            <tr 
              v-else
              v-for="message in paginatedMessages" 
              :key="message.id"
              :class="{ 'unread': message.isReceived && !message.read }"
            >
              <td>{{ activeTab === 'received' ? message.from : message.to }}</td>
              <td>{{ message.subject }}</td>
              <td>{{ formatDate(message.date) }}</td>
              <td>
                <button @click="showDetails(message)" class="action-button">
                  Ver
                </button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>
      
      <div v-if="totalPages > 1" class="pagination">
        <button 
          v-for="page in totalPages" 
          :key="page"
          @click="goToPage(page)"
          :class="{ 'active': page === currentPage }"
        >
          {{ page }}
        </button>
      </div>
    </div>
    
    <!-- Modal de detalhes -->
    <div v-if="showDetailsModal" class="modal-overlay" @click.self="showDetailsModal = false">
      <div class="modal">
        <div class="modal-header">
          <h3>Detalhes da Mensagem</h3>
          <button @click="showDetailsModal = false" class="modal-close">×</button>
        </div>
        <div class="modal-content">
          <div class="detail-row">
            <span>{{ selectedMessage?.isReceived ? 'De:' : 'Para:' }}</span>
            <strong>{{ selectedMessage?.isReceived ? selectedMessage?.from : selectedMessage?.to }}</strong>
          </div>
          <div class="detail-row">
            <span>Assunto:</span>
            <strong>{{ selectedMessage?.subject }}</strong>
          </div>
          <div class="detail-row">
            <span>Data:</span>
            <strong>{{ formatDate(selectedMessage?.date) }}</strong>
          </div>
          <div class="message-body">
            {{ selectedMessage?.body }}
          </div>
        </div>
        <div class="modal-footer">
          <button @click="showDetailsModal = false" class="secondary-button">Fechar</button>
        </div>
      </div>
    </div>
    
    <!-- Modal de nova mensagem -->
    <div v-if="showNewMessageModal" class="modal-overlay" @click.self="showNewMessageModal = false">
      <div class="modal">
        <div class="modal-header">
          <h3>Nova Mensagem</h3>
          <button @click="showNewMessageModal = false" class="modal-close">×</button>
        </div>
        <form @submit.prevent="sendMessage" class="modal-content">
          <div class="form-group">
            <label>Para:</label>
            <input 
              v-model="newMessage.to" 
              type="email" 
              placeholder="email@example.com"
              required
            />
          </div>
          
          <div class="form-group">
            <label>Assunto:</label>
            <input 
              v-model="newMessage.subject" 
              type="text" 
              placeholder="Assunto da mensagem"
              required
            />
          </div>
          
          <div class="form-group">
            <label>Mensagem:</label>
            <textarea 
              v-model="newMessage.body" 
              rows="5"
              placeholder="Escreva sua mensagem aqui..."
            ></textarea>
          </div>
          
          <div class="modal-footer">
            <button type="button" @click="showNewMessageModal = false" class="secondary-button">
              Cancelar
            </button>
            <button type="submit" class="primary-button" :disabled="sending">
              {{ sending ? 'Enviando...' : 'Enviar' }}
            </button>
          </div>
        </form>
      </div>
    </div>
    
    <Notification
      :show="notification.show"
      :message="notification.message"
      :type="notification.type"
      @close="closeNotification"
    />
  </HeaderSidebar>
</template>

<style scoped>
/* Estilos mantidos iguais ao original */
.messages-container {
  width: 100%;
  padding: 1rem;
}

.header-section {
  margin-bottom: 1.5rem;
}

.header-section h2 {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.header-section p {
  font-size: 0.9rem;
  color: #666;
}

.tabs-container {
  margin-bottom: 1rem;
}

.tabs {
  display: flex;
  border-bottom: 1px solid #ddd;
}

.tabs button {
  padding: 0.5rem 1rem;
  background: none;
  border: none;
  border-bottom: 2px solid transparent;
  cursor: pointer;
  font-size: 0.9rem;
  color: #666;
}

.tabs button:hover {
  color: #333;
}

.tabs button.active {
  color: #4863c1;
  border-bottom-color: #4863c1;
  font-weight: 500;
}

.actions-bar {
  display: flex;
  justify-content: flex-end;
  margin-bottom: 1rem;
}

.primary-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.5rem 1rem;
  cursor: pointer;
}

.primary-button:hover:not(:disabled) {
  background-color: #3a4e9c;
}

.primary-button:disabled {
  background-color: #a0afd7;
  cursor: not-allowed;
}

.secondary-button {
  background-color: #f0f0f0;
  color: #333;
  border: 1px solid #ddd;
  border-radius: 4px;
  padding: 0.5rem 1rem;
  cursor: pointer;
}

.secondary-button:hover {
  background-color: #e0e0e0;
}

.table-container {
  background-color: white;
  border-radius: 4px;
  border: 1px solid #ddd;
  overflow: hidden;
  margin-bottom: 1rem;
}

table {
  width: 100%;
  border-collapse: collapse;
}

th, td {
  padding: 0.75rem 1rem;
  text-align: left;
  border-bottom: 1px solid #eee;
}

th {
  background-color: #f8f8f8;
  font-weight: 500;
}

tr:last-child td {
  border-bottom: none;
}

tr.unread {
  background-color: #f0f7ff;
  font-weight: 500;
}

.loading-cell,
.error-cell,
.empty-cell {
  text-align: center;
  padding: 2rem;
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

.retry-button {
  margin-top: 0.75rem;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.4rem 0.75rem;
}

.action-button {
  padding: 0.25rem 0.75rem;
  background-color: #e8f0ff;
  color: #4863c1;
  border: none;
  border-radius: 3px;
  cursor: pointer;
}

.pagination {
  display: flex;
  justify-content: center;
  gap: 0.5rem;
  margin-top: 1rem;
}

.pagination button {
  padding: 0.25rem 0.5rem;
  border: 1px solid #ddd;
  background: white;
  cursor: pointer;
}

.pagination button.active {
  background-color: #4863c1;
  color: white;
  border-color: #4863c1;
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
}

.modal {
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 5px 15px rgba(0, 0, 0, 0.15);
  width: 500px;
  max-width: 90%;
  max-height: 90vh;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
}

.modal-header {
  padding: 1rem;
  border-bottom: 1px solid #eee;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.modal-header h3 {
  margin: 0;
  font-size: 1.1rem;
}

.modal-close {
  background: none;
  border: none;
  font-size: 1.5rem;
  cursor: pointer;
  color: #999;
}

.modal-content {
  padding: 1rem;
  flex-grow: 1;
}

.modal-footer {
  padding: 1rem;
  border-top: 1px solid #eee;
  display: flex;
  justify-content: flex-end;
  gap: 0.5rem;
}

.detail-row {
  margin-bottom: 0.75rem;
  display: flex;
  gap: 0.5rem;
}

.message-body {
  margin-top: 1rem;
  padding: 1rem;
  background-color: #f9f9f9;
  border-radius: 4px;
  white-space: pre-line;
}

.form-group {
  margin-bottom: 1rem;
}

.form-group label {
  display: block;
  margin-bottom: 0.5rem;
  font-weight: 500;
}

.form-group input,
.form-group textarea {
  width: 100%;
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.form-group textarea {
  min-height: 100px;
  resize: vertical;
}

@media (max-width: 768px) {
  th, td {
    padding: 0.5rem;
    font-size: 0.9rem;
  }
  
  .modal {
    width: 95%;
  }
}
</style>