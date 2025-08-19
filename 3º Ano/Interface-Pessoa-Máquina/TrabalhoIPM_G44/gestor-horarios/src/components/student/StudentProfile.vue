<script setup>
import { ref, computed, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import Notification from '../common/Notification.vue';
import { 
  getCourseAbbr,
  loadMultipleData
} from '../../utils/common';
import api from '../../services/api';

// Student data
const studentData = ref({
  name: '',
  email: '',
  degree: '',
  year: null,
  regime: '',
  specialStatus: '',
  enrolled: []
});

// Course data
const coursesData = ref([]);

// State
const enrolledCourses = ref([]);
const loading = ref(true);
const error = ref(null);
const studentId = ref(parseInt(localStorage.getItem('userId') || '0'));

// UI states
const savingPreferences = ref(false);
const changingPassword = ref(false);
const sendingRecovery = ref(false);

// Preferences
const preferences = ref({
  emailNotifications: true,
  systemNotifications: true,
  language: 'pt'
});

// Password form
const password = ref({
  current: '',
  new: '',
  confirm: ''
});

// Password recovery
const showRecovery = ref(false);
const recoveryEmail = ref('');
const recoveryMessage = ref('');

// Notification system
const notification = ref({
  show: false,
  message: '',
  type: 'info' // info, success, error
});

// Carregar todos os dados necessários para o perfil
async function loadProfileData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Carregar dados do aluno e cursos em paralelo
    await loadMultipleData([
      { 
        endpoint: `/students/${studentId.value}`, 
        ref: studentData 
      },
      { 
        endpoint: '/courses', 
        ref: coursesData 
      }
    ], loading, error);
    
    // Processar as UCs inscritas
    if (studentData.value.enrolled && studentData.value.enrolled.length > 0) {
      enrolledCourses.value = studentData.value.enrolled.map(courseId => {
        const course = coursesData.value.find(c => c.id === courseId);
        return {
          id: courseId,
          abbreviation: course ? getCourseAbbr(coursesData.value, course.id) : 'N/A'
        };
      });
    } else {
      enrolledCourses.value = [];
    }

    // Carregar preferências do usuário (se implementado no backend)
    try {
      const prefsResponse = await api.get(`/students/${studentId.value}/preferences`);
      if (prefsResponse.data) {
        preferences.value = { ...preferences.value, ...prefsResponse.data };
      }
    } catch (prefErr) {
      console.log('Preferences not available or not implemented yet');
    }
  } catch (err) {
    error.value = 'Erro ao carregar dados do perfil';
    console.error(err);
  } finally {
    loading.value = false;
  }
}

// Computed properties
const studentNumber = computed(() => {
  if (!studentData.value.email) return 'N/A';
  return studentData.value.email.split('@')[0].toUpperCase();
});

// Methods
function showNotification(message, type = 'info') {
  notification.value = {
    show: true,
    message,
    type
  };
  
  // Auto-hide notification after 3 seconds
  setTimeout(() => {
    notification.value.show = false;
  }, 3000);
}

async function changePassword() {
  // Validate passwords
  if (!password.value.current) {
    showNotification('Por favor, informe a senha atual.', 'error');
    return;
  }
  
  if (!password.value.new || password.value.new.length < 3) {
    showNotification('A nova senha deve ter pelo menos 3 caracteres.', 'error');
    return;
  }
  
  if (password.value.new !== password.value.confirm) {
    showNotification('A confirmação da senha não corresponde à nova senha.', 'error');
    return;
  }

  try {
    changingPassword.value = true;
    
    // First validate the current password
    const studentResponse = await api.get(`/students/${studentId.value}`);
    
    if (studentResponse.data && studentResponse.data.password !== password.value.current) {
      showNotification('Senha atual incorreta.', 'error');
      return;
    }

    // Update the password
    await api.patch(`/students/${studentId.value}`, {
      password: password.value.new
    });

    showNotification('Senha alterada com sucesso!', 'success');
    
    // Clear the password fields
    password.value = {
      current: '',
      new: '',
      confirm: ''
    };
  } catch (err) {
    console.error('Erro ao alterar senha:', err);
    showNotification('Ocorreu um erro ao alterar a senha. Tente novamente.', 'error');
  } finally {
    changingPassword.value = false;
  }
}

async function savePreferences() {
  try {
    savingPreferences.value = true;
    
    await api.patch(`/students/${studentId.value}/preferences`, preferences.value);
    showNotification('Preferências salvas com sucesso!', 'success');
  } catch (err) {
    console.error('Erro ao salvar preferências:', err);
    showNotification('Ocorreu um erro ao salvar as preferências.', 'error');
  } finally {
    savingPreferences.value = false;
  }
}

async function sendRecoveryEmail() {
  if (!recoveryEmail.value) {
    recoveryEmail.value = studentData.value.email;
  }
  
  try {
    sendingRecovery.value = true;
    
    // Simulação de envio com mensagem de sucesso
    recoveryMessage.value = 'Email de recuperação enviado para ' + recoveryEmail.value;
    setTimeout(() => {
      showRecovery.value = false;
      recoveryMessage.value = '';
    }, 5000);
  } catch (err) {
    recoveryMessage.value = 'Erro ao enviar email de recuperação.';
  } finally {
    sendingRecovery.value = false;
  }
}

// Carregar dados ao montar o componente
onMounted(() => {
  loadProfileData();
});
</script>

<template>
  <HeaderSidebar 
    :userName="studentData?.name || 'Carregando...'" 
    userType="aluno"
    activeMenuItem="perfil"
  >
    <div v-if="loading" class="loading-state">
      <div class="spinner"></div>
      <p>Carregando perfil...</p>
    </div>
    
    <div v-else-if="error" class="error-state">
      <p>{{ error }}</p>
      <button @click="loadProfileData" class="retry-button">Tentar novamente</button>
    </div>
    
    <div v-else class="profile-container">
      <h1>Meu Perfil</h1>
      <p class="subtitle">Visualizar e editar as minhas informações pessoais</p>

      <!-- Two-column layout -->
      <div class="two-column-layout">
        <!-- Left Column -->
        <div class="left-column">
          <!-- Personal Info Card -->
          <div class="profile-card">
            <div class="avatar-section">
              <div class="user-avatar">
                <span class="avatar-icon">👤</span>
              </div>
              <div class="user-info">
                <h2>{{ studentData.name }}</h2>
                <p class="user-degree">{{ studentData.degree }}</p>
              </div>
            </div>

            <div class="info-grid">
              <div class="info-item">
                <span class="info-label">Nº Mecanográfico:</span>
                <span class="info-value">{{ studentNumber }}</span>
              </div>
              <div class="info-item">
                <span class="info-label">Email:</span>
                <span class="info-value">{{ studentData.email }}</span>
              </div>
              <div class="info-item">
                <span class="info-label">Ano:</span>
                <span class="info-value">{{ studentData.year }}º Ano</span>
              </div>
              <div class="info-item">
                <span class="info-label">Regime:</span>
                <span class="info-value">{{ studentData.regime }}</span>
              </div>
              <div class="info-item">
                <span class="info-label">Estatuto:</span>
                <span class="info-value">{{ studentData.specialStatus || 'Nenhum' }}</span>
              </div>
            </div>
          </div>

          <!-- Enrolled Courses Card -->
          <div class="profile-card">
            <h3>UCs Inscritas</h3>
            <div class="courses-grid">
              <span v-for="course in enrolledCourses" :key="course.id" class="course-badge">
                {{ course.abbreviation }}
              </span>
              <span v-if="enrolledCourses.length === 0" class="no-courses">
                Nenhuma UC inscrita
              </span>
            </div>
          </div>
        </div>

        <!-- Right Column -->
        <div class="right-column">
          <!-- Preferences Card -->
          <div class="profile-card">
            <h3>Preferências</h3>
            <div class="preferences-list">
              <div class="preference-item">
                <input type="checkbox" id="email-notifications" v-model="preferences.emailNotifications">
                <label for="email-notifications">Notificações por Email</label>
              </div>
              <div class="preference-item">
                <input type="checkbox" id="system-notifications" v-model="preferences.systemNotifications">
                <label for="system-notifications">Notificações no Sistema</label>
              </div>
              <div class="preference-item">
                <label for="system-language">Idioma do Sistema:</label>
                <select id="system-language" v-model="preferences.language">
                  <option value="pt">Português</option>
                  <option value="en">English</option>
                </select>
              </div>
            </div>

            <div class="form-actions">
              <button class="normal-button" @click="savePreferences" :disabled="savingPreferences">
                {{ savingPreferences ? 'Salvando...' : 'Salvar Preferências' }}
              </button>
            </div>
          </div>

          <!-- Password Change Card -->
          <div class="profile-card">
            <h3>Alteração de Senha</h3>
            <div class="password-form">
              <div class="form-group">
                <label for="current-password">Senha Atual:</label>
                <input type="password" id="current-password" v-model="password.current" placeholder="••••••••">
              </div>
              <div class="form-group">
                <label for="new-password">Nova Senha:</label>
                <input type="password" id="new-password" v-model="password.new" placeholder="••••••••">
              </div>
              <div class="form-group">
                <label for="confirm-password">Confirmar Senha:</label>
                <input type="password" id="confirm-password" v-model="password.confirm" placeholder="••••••••">
              </div>
              <div class="form-actions">
                <button class="normal-button" @click="changePassword" :disabled="changingPassword">
                  {{ changingPassword ? 'Alterando...' : 'Alterar Senha' }}
                </button>
                <a href="#" class="forgot-password" @click.prevent="showRecovery = true">
                  Esqueceu a sua password?
                </a>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- Password Recovery Section -->
      <div class="recovery-section" v-if="showRecovery">
        <div class="profile-card">
          <h3>Recuperação de Password</h3>
          <div class="recovery-form">
            <div class="form-group">
              <label for="recovery-email">Email de Recuperação:</label>
              <input type="email" id="recovery-email" v-model="recoveryEmail" :placeholder="studentData.email">
            </div>
            <div class="form-actions">
              <button class="send-button" @click="sendRecoveryEmail" :disabled="sendingRecovery">
                {{ sendingRecovery ? 'Enviando...' : 'Enviar Email' }}
              </button>
              <button class="cancel-button" @click="showRecovery = false">
                Cancelar
              </button>
            </div>
            <p v-if="recoveryMessage" class="recovery-message">
              {{ recoveryMessage }}
            </p>
          </div>
        </div>
      </div>
    </div>

    <!-- Notificação -->
    <Notification
      :show="notification.show"
      :message="notification.message"
      :type="notification.type"
      @close="notification.show = false"
    />
  </HeaderSidebar>
</template>

<style scoped>
.profile-container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

h1 {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.subtitle {
  color: #7f8c8d;
  margin-bottom: 30px;
}

/* Loading and Error states */
.loading-state, .error-state {
  max-width: 1200px;
  margin: 40px auto;
  padding: 30px;
  text-align: center;
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.spinner {
  border: 4px solid rgba(0, 0, 0, 0.1);
  width: 40px;
  height: 40px;
  border-radius: 50%;
  border-left-color: #4863c1;
  animation: spin 1s linear infinite;
  margin: 0 auto 20px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.error-state {
  color: #dc3545;
}

.retry-button {
  margin-top: 15px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 8px 16px;
  cursor: pointer;
  font-size: 0.9rem;
}

/* Two-column layout */
.two-column-layout {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 25px;
}

.left-column, .right-column {
  display: flex;
  flex-direction: column;
  gap: 25px;
}

/* Profile cards */
.profile-card {
  background: #ffffff;
  border-radius: 10px;
  padding: 25px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

/* Avatar section */
.avatar-section {
  display: flex;
  align-items: center;
  gap: 20px;
  margin-bottom: 20px;
}

.user-avatar {
  width: 80px;
  height: 80px;
  border-radius: 50%;
  background: #f0f2f5;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
}

.avatar-icon {
  font-size: 2.5rem;
}

.user-info h2 {
  margin: 0;
  font-size: 1.4rem;
  color: #2c3e50;
}

.user-degree {
  margin: 5px 0 0 0;
  color: #7f8c8d;
  font-size: 0.95rem;
}

/* Info grid */
.info-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 15px;
}

.info-item {
  display: flex;
  flex-direction: column;
}

.info-label {
  font-weight: 500;
  color: #555;
  font-size: 0.9rem;
}

.info-value {
  color: #333;
  font-size: 0.95rem;
}

/* Courses grid */
.courses-grid {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.course-badge {
  background: #f0f2f5;
  padding: 6px 12px;
  border-radius: 15px;
  font-size: 0.85rem;
  color: #2c3e50;
}

.no-courses {
  color: #7f8c8d;
  font-style: italic;
  font-size: 0.9rem;
}

/* Preferences */
.preferences-list {
  display: flex;
  flex-direction: column;
  gap: 15px;
}

.preference-item {
  display: flex;
  align-items: center;
}

.preference-item label {
  margin-left: 8px;
}

.preference-item select {
  margin-left: 8px;
  padding: 6px 10px;
  border-radius: 4px;
  border: 1px solid #ddd;
  background: white;
}

/* Forms */
.form-group {
  margin-bottom: 20px;
}

.form-group label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: #555;
}

.form-group input {
  width: 100%;
  padding: 10px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.95rem;
}

/* Botões */
.normal-button, .save-button {
  background: #4863c1;
  color: white;
  border: none;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background 0.2s;
}

.normal-button:hover:not(:disabled), .save-button:hover:not(:disabled) {
  background: #3a4e9c;
}

.normal-button:disabled, .save-button:disabled {
  background: #a0afd7;
  cursor: not-allowed;
}

.forgot-password {
  color: #4863c1;
  text-decoration: none;
  font-size: 0.9rem;
  margin-left: 15px;
}

.send-button {
  background: #28a745;
  color: white;
  border: none;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background 0.2s;
}

.send-button:hover:not(:disabled) {
  background: #218838;
}

.send-button:disabled {
  background: #86d79a;
  cursor: not-allowed;
}

.cancel-button {
  background: #f8f9fa;
  color: #333;
  border: 1px solid #ddd;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9rem;
  margin-left: 10px;
  transition: background 0.2s;
}

.cancel-button:hover {
  background: #e9ecef;
}

.form-actions {
  display: flex;
  align-items: center;
}

/* Recovery section */
.recovery-section {
  margin-top: 30px;
}

.recovery-message {
  color: #28a745;
  margin-top: 15px;
  font-size: 0.9rem;
}

/* Responsive design */
@media (max-width: 768px) {
  .two-column-layout {
    grid-template-columns: 1fr;
  }
  
  .info-grid {
    grid-template-columns: 1fr;
  }
  
  .form-actions {
    flex-direction: column;
    align-items: flex-start;
    gap: 10px;
  }
  
  .forgot-password {
    margin-left: 0;
    margin-top: 10px;
  }
}
</style>