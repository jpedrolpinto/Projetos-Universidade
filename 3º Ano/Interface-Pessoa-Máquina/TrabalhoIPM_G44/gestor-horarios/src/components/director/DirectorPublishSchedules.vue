<script setup>
import { ref, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import api from '../../services/api';
import { 
  loadDirectorData, 
  formatDate as formatDateUtil, 
  loadMultipleData,
  loadData
} from '../../utils/common';

// Estados de carregamento e erro
const loading = ref(true);
const error = ref(null);

// Dados da publicação de horários
const settings = ref({
  publishedSchedules: false,
  lastPublishedDate: null,
  notificationSent: false
});

// Estatísticas
const totalStudents = ref(0);
const allocatedStudents = ref(0);
const unallocatedStudents = ref(0);

// Opções de publicação
const sendEmailToStudents = ref(true);
const notifyTeachers = ref(true);
const publishing = ref(false);
const showConfirmModal = ref(false);

// Dados do diretor
const directorData = ref(null);
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

const shifts = ref([]);

// Processador para os dados de configurações
function processSettingsData(data) {
  return {
    publishedSchedules: data.publishedSchedules || false,
    lastPublishedDate: data.lastPublishedDate || null,
    notificationSent: data.notificationSent || false
  };
}

// Processador para os dados de alocações
function processAllocationData(data, allStudents) {
  // Mapa para acompanhar as disciplinas alocadas por aluno
  const studentAllocations = new Map();
  
  // Inicializar o mapa com todos os alunos
  for (const student of allStudents) {
    studentAllocations.set(student.id, {
      student: student,
      enrolledCourses: new Set(student.enrolled || []),  // Disciplinas em que está inscrito
      allocatedCourses: new Set()                        // Disciplinas para as quais tem turno
    });
  }
  
  // Processar alocações para preencher as disciplinas alocadas
  for (const allocation of data) {
    const studentId = allocation.studentId;
    const studentData = studentAllocations.get(studentId);
    
    if (studentData) {
      // Encontrar o turno associado a esta alocação
      const shift = shifts.value.find(s => s.id === allocation.shiftId);
      
      if (shift) {
        // Adicionar o curso deste turno às disciplinas alocadas do aluno
        studentData.allocatedCourses.add(shift.courseId);
      }
    }
  }
  
  // Agora contamos alunos totalmente alocados vs parcialmente alocados
  let fullyAllocated = 0;
  let partiallyAllocated = 0;
  let notAllocated = 0;
  
  for (const studentData of studentAllocations.values()) {
    const enrolledCount = studentData.enrolledCourses.size;
    const allocatedCount = studentData.allocatedCourses.size;
    
    if (enrolledCount === 0) {
      // Aluno não está inscrito em nenhuma disciplina, considerar como alocado
      fullyAllocated++;
    } else if (allocatedCount === 0) {
      // Aluno está inscrito mas não tem nenhum turno atribuído
      notAllocated++;
    } else if (allocatedCount >= enrolledCount) {
      // Aluno tem alocação para todas as disciplinas em que está inscrito
      fullyAllocated++;
    } else {
      // Aluno tem alocação parcial (algumas disciplinas sem turno)
      partiallyAllocated++;
    }
  }
  
  // Atualizar as refs
  totalStudents.value = allStudents.length;
  allocatedStudents.value = fullyAllocated;
  unallocatedStudents.value = notAllocated + partiallyAllocated;
  
  return data;
}


async function loadInitialData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar o diretor
    await loadDirectorData(directorId, directorData, ref(false), ref(null));
    
    // Carregar dados em paralelo
    const settingsData = ref(null);
    const studentsData = ref([]);
    const allocationsData = ref([]);
    const shiftsData = ref([]);
    
    await loadMultipleData([
      { endpoint: '/settings', ref: settingsData, processor: processSettingsData },
      { endpoint: '/students', ref: studentsData },
      { endpoint: '/shifts', ref: shiftsData },
      { endpoint: '/allocations', ref: allocationsData }
    ], loading, error);
    
    if (error.value) return;
    
    // Atualizar configurações
    settings.value = settingsData.value;
    shifts.value = shiftsData.value;
    
    // Processar os dados de alocação para calcular estatísticas
    processAllocationData(allocationsData.value, studentsData.value);
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Tente novamente.';
  } finally {
    loading.value = false;
  }
}

// Mostrar modal de confirmação antes de publicar
function publishSchedules() {
  showConfirmModal.value = true;
}

// Publicar horários após confirmação
async function confirmPublishSchedules() {
  try {
    publishing.value = true;
    showConfirmModal.value = false;
    
    // Atualizar estado de publicação
    const publishData = {
      publishedSchedules: true,
      lastPublishedDate: new Date().toISOString(),
      notificationSent: false
    };
    
    await api.patch('/settings', publishData);
    settings.value = publishData;
    
    // Enviar notificações se selecionado
    if (sendEmailToStudents.value || notifyTeachers.value) {
      await sendNotifications();
    }
    
    alert('Horários publicados com sucesso!');
    
  } catch (err) {
    console.error('Erro ao publicar horários:', err);
    alert('Erro ao publicar horários. Tente novamente.');
  } finally {
    publishing.value = false;
  }
}

// Enviar notificações
async function sendNotifications() {
  try {
    // Obter todos os alunos
    const studentsResponse = await api.get('/students');
    const studentsList = studentsResponse.data;
    
    // Obter email do diretor para usar como remetente
    const directorEmail = directorData.value?.email || 'sistema@universidade.edu';
    
    // Preparar mensagens em massa
    const messagePromises = [];
    
    // Criar uma mensagem para cada aluno se a opção estiver selecionada
    if (sendEmailToStudents.value) {
      for (const student of studentsList) {
        const messageData = {
          from: directorEmail,
          to: student.email,
          subject: "Horários Publicados",
          body: `Prezado(a) ${student.name},\n\nOs horários para o semestre foram publicados e já estão disponíveis para consulta no sistema.\n\nAtenciosamente,\nDireção Acadêmica`,
          date: new Date().toISOString(),
          read: false,
          directorId: directorId.value
        };
        
        // Adicionar à lista de promessas
        messagePromises.push(api.post('/messages', messageData));
      }
    }
    
    // Notificar professores se a opção estiver selecionada
    if (notifyTeachers.value) {
      const teachersResponse = await api.get('/teachers');
      const teachersList = teachersResponse.data;
      
      for (const teacher of teachersList) {
        const messageData = {
          from: directorEmail,
          to: teacher.email,
          subject: "Horários Publicados para Docentes",
          body: `Prezado(a) Professor(a) ${teacher.name},\n\nOs horários para o semestre foram publicados e já estão disponíveis para consulta no sistema.\n\nAtenciosamente,\nDireção Acadêmica`,
          date: new Date().toISOString(),
          read: false,
          directorId: directorId.value
        };
        
        // Adicionar à lista de promessas
        messagePromises.push(api.post('/messages', messageData));
      }
    }
    
    // Executar todas as requisições em paralelo
    await Promise.all(messagePromises);
    
    // Marcar como notificações enviadas
    await api.patch('/settings', { notificationSent: true });
    settings.value.notificationSent = true;
    
  } catch (err) {
    console.error('Erro ao enviar notificações:', err);
    // Não rejeitar a publicação principal por falha nas notificações
  }
}

// Formatar data usando a função do common.js
function formatDate(dateString) {
  if (!dateString) return '';
  return formatDateUtil(dateString);
}

// Carregar dados ao montar o componente
onMounted(async () => {
  await loadInitialData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name" 
    userType="diretor"
    activeMenuItem="publicar-horarios"
  >
    <div class="publish-container">
      <h1>Publicar Horários</h1>
      
      <div v-if="loading" class="loading-state">
        <div class="spinner"></div>
        <p>Carregando dados...</p>
      </div>
      
      <div v-else-if="error" class="error-state">
        <p>{{ error }}</p>
        <button @click="loadInitialData" class="retry-button">Tentar novamente</button>
      </div>
      
      <div v-else>
        <div class="status-card">
          <h2>Estado Atual</h2>
          <p v-if="settings.publishedSchedules" class="published-status">
            <i class="fas fa-check-circle"></i> Horários publicados em {{ formatDate(settings.lastPublishedDate) }}
          </p>
          <p v-else class="unpublished-status">
            <i class="fas fa-times-circle"></i> Horários não publicados
          </p>
        </div>

        <div class="verifications">
          <h2>Verificações</h2>
          <div class="stats-grid">
            <div class="stat-card">
              <h3>Total Alunos</h3>
              <p>{{ totalStudents }}</p>
            </div>
            <div class="stat-card">
              <h3>Alunos Não Alocados</h3>
              <p>{{ unallocatedStudents }}</p>
            </div>
            <div class="stat-card">
              <h3>Alunos Alocados</h3>
              <p>{{ allocatedStudents }}</p>
            </div>
          </div>
        </div>

        <div class="publish-options">
          <h2>Opções de Publicação</h2>
          <div class="options-grid">
            <label class="option-item">
              <input type="checkbox" v-model="sendEmailToStudents">
              Enviar email aos alunos a informar da publicação dos horários
            </label>
            <label class="option-item">
              <input type="checkbox" v-model="notifyTeachers">
              Notificar os docentes da publicação dos horários
            </label>
          </div>

          <div class="note">
            <p><strong>Nota:</strong> Os alunos que não foram alocados devem fazer sua alocação manualmente</p>
          </div>

          <button 
            @click="publishSchedules"
            :disabled="publishing || settings.publishedSchedules"
            class="publish-button"
          >
            <span v-if="publishing">Publicando...</span>
            <span v-else>Publicar Horários</span>
          </button>
        </div>
      </div>
      
      <!-- Modal de confirmação -->
      <div v-if="showConfirmModal" class="modal-overlay">
        <div class="modal-content">
          <div class="modal-header">
            <h2>Confirmar Publicação</h2>
          </div>
          <div class="modal-body">
            <p><strong>Atenção:</strong> A publicação dos horários é uma ação irreversível.</p>
            <p>Todos os alunos poderão visualizar seus horários e serão notificados caso essa opção esteja selecionada.</p>
            
            <div class="stats-grid">
              <div class="stat-card">
                <h3>Total Alunos</h3>
                <p>{{ totalStudents }}</p>
              </div>
              <div class="stat-card">
                <h3>Alunos Não Alocados</h3>
                <p>{{ unallocatedStudents }}</p>
                <small>Com pelo menos uma UC sem turno</small>
              </div>
              <div class="stat-card">
                <h3>Alunos Alocados</h3>
                <p>{{ allocatedStudents }}</p>
                <small>Com todas as UCs alocadas</small>
              </div>
            </div>
            
            <div class="warning-message" v-if="unallocatedStudents > 0">
              <p><i class="fas fa-exclamation-triangle"></i> Existem {{ unallocatedStudents }} alunos sem turnos atribuídos.</p>
            </div>
          </div>
          <div class="modal-footer">
            <button @click="showConfirmModal = false" class="cancel-button">
              Cancelar
            </button>
            <button @click="confirmPublishSchedules" class="confirm-button" :disabled="publishing">
              <span v-if="publishing">Publicando...</span>
              <span v-else>Confirmar Publicação</span>
            </button>
          </div>
        </div>
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
.publish-container {
  padding: 20px;
  max-width: 900px;
  margin: 0 auto;
}

h1 {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

/* Estado de carregamento e erro */
.loading-state,
.error-state {
  text-align: center;
  padding: 30px;
  background-color: white;
  border-radius: 8px;
  margin: 20px 0;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.error-state {
  color: #dc3545;
}

.spinner {
  border: 3px solid rgba(0,0,0,0.1);
  border-radius: 50%;
  border-top: 3px solid #4863c1;
  width: 24px;
  height: 24px;
  animation: spin 1s linear infinite;
  margin: 0 auto 15px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.retry-button {
  margin-top: 15px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 8px 16px;
  cursor: pointer;
}

.status-card {
  background: #f8f9fa;
  padding: 20px;
  border-radius: 8px;
  margin-bottom: 30px;
  border-left: 4px solid #4863c1;
}

.published-status {
  color: #28a745;
  font-weight: 500;
}

.unpublished-status {
  color: #dc3545;
  font-weight: 500;
}

.verifications {
  margin-bottom: 30px;
}

.stats-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 20px;
  margin-top: 15px;
}

.stat-card {
  background: white;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  text-align: center;
}

.stat-card h3 {
  color: #6c757d;
  font-size: 1rem;
  margin-bottom: 10px;
}

.stat-card p {
  font-size: 1.5rem;
  font-weight: 600;
  color: #2c3e50;
  margin: 0;
}

.publish-options {
  background: #f8f9fa;
  padding: 25px;
  border-radius: 8px;
}

.options-grid {
  display: grid;
  gap: 15px;
  margin: 20px 0;
}

.option-item {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px;
  background: white;
  border-radius: 6px;
  cursor: pointer;
}

.option-item input {
  cursor: pointer;
}

.note {
  margin: 20px 0;
  padding: 10px;
  background: #fff3cd;
  border-left: 4px solid #ffc107;
  font-size: 0.9rem;
}

.publish-button {
  background-color: #28a745;
  color: white;
  border: none;
  padding: 12px 25px;
  border-radius: 6px;
  font-size: 1rem;
  cursor: pointer;
  transition: background-color 0.3s;
}

.publish-button:hover:not(:disabled) {
  background-color: #218838;
}

.publish-button:disabled {
  background-color: #6c757d;
  cursor: not-allowed;
}

/* Ícones */
.fas {
  margin-right: 8px;
}

/* Modal de confirmação */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.modal-content {
  background-color: white;
  border-radius: 8px;
  max-width: 600px;
  width: 90%;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
}

.modal-header {
  padding: 15px 20px;
  border-bottom: 1px solid #eee;
}

.modal-header h2 {
  margin: 0;
  font-size: 1.3rem;
  color: #333;
}

.modal-body {
  padding: 20px;
}

.confirmation-stats {
  background-color: #f8f9fa;
  padding: 15px;
  border-radius: 8px;
  margin: 15px 0;
}

.stat-item {
  display: flex;
  justify-content: space-between;
  margin-bottom: 10px;
}

.stat-label {
  font-weight: 500;
}

.warning-message {
  background-color: #fff3cd;
  color: #856404;
  padding: 10px 15px;
  border-radius: 5px;
  margin-top: 15px;
  border-left: 4px solid #ffc107;
}

.modal-footer {
  padding: 15px 20px;
  display: flex;
  justify-content: flex-end;
  gap: 10px;
  border-top: 1px solid #eee;
}

.cancel-button {
  background-color: #f8f9fa;
  border: 1px solid #ddd;
  color: #333;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
}

.confirm-button {
  background-color: #28a745;
  color: white;
  border: none;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
}

.confirm-button:disabled {
  background-color: #6c757d;
  cursor: not-allowed;
}

.cancel-button:hover {
  background-color: #e2e6ea;
}

.confirm-button:hover:not(:disabled) {
  background-color: #218838;
}

@media (max-width: 768px) {
  .stats-grid {
    grid-template-columns: 1fr;
  }
  
  .modal-content {
    width: 95%;
  }
}
</style>