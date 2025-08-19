<script setup>
import { ref, onMounted } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import api from '../../services/api';
import { loadDirectorData, loadData, loadMultipleData } from '../../utils/common';

// Dados do diretor
const directorData = ref(null);
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Estado de carregamento e erro
const loading = ref(true);
const error = ref(null);

// Estado da aba ativa
const activeTab = ref('general');

// Estado das configurações
const settings = ref({
  academicYear: '2024/2025',
  currentSemester: '2',
  practicalShiftLimit: '30',
  theoreticalShiftLimit: '120',
  allowExceedLimit: false
});

// Backup das configurações originais para cancelamento
const originalSettings = ref({});

// Definição de valores padrão
const defaultSettings = {
  general: {
    academicYear: '2024/2025',
    currentSemester: '2'
  },
  allocation: {
    practicalShiftLimit: '30',
    theoreticalShiftLimit: '120',
    allowExceedLimit: false
  }
};

// Função para carregar todos os dados necessários
async function loadAllData() {
  try {
    loading.value = true;
    error.value = null;
    
    // Primeiro, carregamos os dados do diretor
    await loadDirectorData(directorId, directorData, ref(false), ref(null));
    
    // Depois, tentamos carregar as configurações
    try {
      const response = await api.get('/settings');
      const settingsData = processSettingsData(response.data);
      settings.value = settingsData;
      // Fazer backup das configurações originais
      originalSettings.value = { ...settingsData };
    } catch (settingsErr) {
      console.warn('Erro ao carregar configurações, usando valores padrão:', settingsErr);
      
      // Definir configurações padrão se não conseguirmos carregar do servidor
      settings.value = {
        academicYear: defaultSettings.general.academicYear,
        currentSemester: defaultSettings.general.currentSemester,
        practicalShiftLimit: defaultSettings.allocation.practicalShiftLimit,
        theoreticalShiftLimit: defaultSettings.allocation.theoreticalShiftLimit,
        allowExceedLimit: defaultSettings.allocation.allowExceedLimit
      };
      
      // Fazer backup das configurações padrão
      originalSettings.value = { ...settings.value };
    }
    
  } catch (err) {
    console.error('Erro ao carregar dados:', err);
    error.value = 'Erro ao carregar dados. Por favor, tente novamente.';
  } finally {
    loading.value = false;
  }
}

// Processador para os dados de configuração
function processSettingsData(data) {
  if (!data) {
    return {
      academicYear: defaultSettings.general.academicYear,
      currentSemester: defaultSettings.general.currentSemester,
      practicalShiftLimit: defaultSettings.allocation.practicalShiftLimit,
      theoreticalShiftLimit: defaultSettings.allocation.theoreticalShiftLimit,
      allowExceedLimit: defaultSettings.allocation.allowExceedLimit
    };
  }
  
  return {
    academicYear: data.academicYear || defaultSettings.general.academicYear,
    currentSemester: data.currentSemester || defaultSettings.general.currentSemester,
    practicalShiftLimit: data.practicalShiftLimit || defaultSettings.allocation.practicalShiftLimit,
    theoreticalShiftLimit: data.theoreticalShiftLimit || defaultSettings.allocation.theoreticalShiftLimit,
    allowExceedLimit: data.allowExceedLimit !== undefined ? data.allowExceedLimit : defaultSettings.allocation.allowExceedLimit
  };
}

// Salvar configurações - com melhor tratamento de erros
async function saveSettings() {
  try {
    loading.value = true;
    error.value = null;
    
    // Converter valores numéricos
    const settingsToSave = {
      ...settings.value,
      currentSemester: Number(settings.value.currentSemester),
      practicalShiftLimit: Number(settings.value.practicalShiftLimit),
      theoreticalShiftLimit: Number(settings.value.theoreticalShiftLimit)
    };
    
    await api.patch('/settings', settingsToSave);
    
    // Atualizar backup após salvar com sucesso
    originalSettings.value = { ...settings.value };
    
    alert('Configurações salvas com sucesso!');
  } catch (err) {
    console.error('Erro ao salvar configurações:', err);
    
    if (err.response && err.response.status === 404) {
      // Se for erro 404, o endpoint não existe ou ainda não foi criado
      // Tentamos criar as configurações em vez de atualizá-las
      try {
        const settingsToCreate = {
          ...settings.value,
          currentSemester: Number(settings.value.currentSemester),
          practicalShiftLimit: Number(settings.value.practicalShiftLimit),
          theoreticalShiftLimit: Number(settings.value.theoreticalShiftLimit)
        };
        
        await api.post('/settings', settingsToCreate);
        originalSettings.value = { ...settings.value };
        alert('Configurações criadas com sucesso!');
      } catch (postErr) {
        console.error('Erro ao criar configurações:', postErr);
        error.value = 'Não foi possível criar as configurações. Verifique sua conexão e tente novamente.';
      }
    } else {
      error.value = 'Não foi possível salvar as configurações. Verifique sua conexão e tente novamente.';
    }
  } finally {
    loading.value = false;
  }
}

// Cancelar alterações
function cancelChanges() {
  // Restaurar para o backup
  settings.value = { ...originalSettings.value };
}

// Restaurar configurações padrão
function restoreDefaults() {
  if (confirm('Tem certeza que deseja restaurar as configurações padrão?')) {
    if (activeTab.value === 'general') {
      settings.value.academicYear = defaultSettings.general.academicYear;
      settings.value.currentSemester = defaultSettings.general.currentSemester;
    } else {
      settings.value.practicalShiftLimit = defaultSettings.allocation.practicalShiftLimit;
      settings.value.theoreticalShiftLimit = defaultSettings.allocation.theoreticalShiftLimit;
      settings.value.allowExceedLimit = defaultSettings.allocation.allowExceedLimit;
    }
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
    activeMenuItem="configuracoes"
  >
    <div class="configurations-container">
      <div class="header-section">
        <h1>Configurações do Sistema</h1>
        <p>Defina parâmetros do sistema de gestão de turnos</p>
      </div>
      
      <div v-if="loading" class="loading-state">
        Carregando configurações...
      </div>
      
      <div v-else-if="error" class="error-state">
        {{ error }}
        <button @click="loadAllData" class="primary-button">Tentar Novamente</button>
      </div>
      
      <div v-else class="content-wrapper">
        <!-- Sidebar de navegação das abas -->
        <div class="tabs-sidebar">
          <div class="tabs-header">
            <h3>Configurações</h3>
          </div>
          <div class="tabs-list">
            <div 
              class="tab-item" 
              :class="{ 'active': activeTab === 'general' }"
              @click="activeTab = 'general'"
            >
              Geral
            </div>
            <div 
              class="tab-item" 
              :class="{ 'active': activeTab === 'allocation' }"
              @click="activeTab = 'allocation'"
            >
              Regras de Alocação
            </div>
          </div>
        </div>
        
        <!-- Conteúdo da aba selecionada -->
        <div class="tab-content">
          <!-- Configurações Gerais -->
          <div v-if="activeTab === 'general'" class="general-settings">
            <h2>Configurações Gerais</h2>
            
            <div class="settings-section">
              <h3>Período Académico:</h3>
              
              <div class="form-group">
                <label>Ano Letivo:</label>
                <input 
                  type="text" 
                  v-model="settings.academicYear" 
                  placeholder="Ex: 2024/2025"
                />
              </div>
              
              <div class="form-group">
                <label>Semestre Atual:</label>
                <input 
                  type="number" 
                  v-model="settings.currentSemester" 
                  min="1" 
                  max="2" 
                  placeholder="1 ou 2"
                />
              </div>
            </div>
          </div>
          
          <!-- Regras de Alocação -->
          <div v-if="activeTab === 'allocation'" class="allocation-settings">
            <h2>Regras de Alocação</h2>
            
            <div class="settings-section">
              <h3>Distribuição pelos Turnos:</h3>
              
              <div class="form-group">
                <label>Limite max. de alunos nos turnos práticos:</label>
                <input 
                  type="number"
                  v-model="settings.practicalShiftLimit" 
                  min="1" 
                  placeholder="Ex: 30"
                />
              </div>
              
              <div class="form-group">
                <label>Limite max. de alunos nos turnos teóricos:</label>
                <input 
                  type="number" 
                  v-model="settings.theoreticalShiftLimit" 
                  min="1" 
                  placeholder="Ex: 120"
                />
              </div>
              
              <div class="form-group toggle-group">
                <label>Permitir exceder o limite dos turnos:</label>
                <div class="toggle-switch">
                  <input 
                    type="checkbox" 
                    id="exceedLimitToggle" 
                    v-model="settings.allowExceedLimit"
                  />
                  <label for="exceedLimitToggle"></label>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <!-- Botões de ação -->
      <div v-if="!loading && !error" class="action-buttons">
        <button @click="saveSettings" class="primary-button">
          Salvar Novas {{ activeTab === 'general' ? 'Definições' : 'Regras' }}
        </button>
        <button @click="cancelChanges" class="secondary-button">
          Cancelar
        </button>
        <button @click="restoreDefaults" class="warning-button">
          Restaurar {{ activeTab === 'general' ? 'Definições' : 'Regras' }} Padrão
        </button>
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
.configurations-container {
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

/* Estados de carregamento e erro */
.loading-state,
.error-state {
  text-align: center;
  padding: 20px;
  margin: 20px 0;
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.error-state {
  color: #d9534f;
  margin-bottom: 20px;
}

.error-state button {
  margin-top: 10px;
}

/* Estrutura de layout */
.content-wrapper {
  display: flex;
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  margin-bottom: 20px;
  overflow: hidden;
}

/* Sidebar de navegação */
.tabs-sidebar {
  width: 220px;
  border-right: 1px solid #eee;
  background-color: white;
}

.tabs-header {
  padding: 20px;
  background-color: #4863c1;
  color: white;
}

.tabs-header h3 {
  margin: 0;
  font-size: 1.1rem;
  font-weight: 500;
}

.tabs-list {
  padding: 10px 0;
}

.tab-item {
  padding: 15px 20px;
  cursor: pointer;
  color: #666;
  transition: all 0.2s;
}

.tab-item.active {
  background-color: #e8eefa;
  color: #4863c1;
  font-weight: 500;
  border-left: 3px solid #4863c1;
}

/* Conteúdo das abas */
.tab-content {
  flex: 1;
  padding: 25px;
  min-height: 400px;
}

.tab-content h2 {
  color: #333;
  font-size: 1.3rem;
  margin-top: 0;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 1px solid #eee;
}

.settings-section {
  margin-bottom: 30px;
}

.settings-section h3 {
  color: #555;
  font-size: 1rem;
  margin-bottom: 20px;
  font-weight: 500;
}

/* Formulários */
.form-group {
  display: flex;
  align-items: center;
  margin-bottom: 15px;
}

.form-group label {
  width: 250px;
  color: #555;
  font-size: 0.95rem;
}

.form-group input[type="text"],
.form-group input[type="number"] {
  padding: 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  width: 200px;
  font-size: 0.95rem;
}

.form-group input:focus {
  border-color: #4863c1;
  outline: none;
  box-shadow: 0 0 0 2px rgba(72, 99, 193, 0.2);
}

/* Toggle switch */
.toggle-group {
  align-items: center;
}

.toggle-switch {
  position: relative;
  display: inline-block;
  width: 50px;
  height: 24px;
}

.toggle-switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.toggle-switch label {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  transition: .4s;
  border-radius: 24px;
  width: 50px;
}

.toggle-switch label:before {
  position: absolute;
  content: "";
  height: 16px;
  width: 16px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: .4s;
  border-radius: 50%;
}

.toggle-switch input:checked + label {
  background-color: #4863c1;
}

.toggle-switch input:checked + label:before {
  transform: translateX(26px);
}

/* Botões de ação */
.action-buttons {
  display: flex;
  justify-content: flex-end;
  gap: 10px;
  margin-top: 20px;
}

.primary-button {
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 10px 20px;
  cursor: pointer;
  font-size: 0.9rem;
}

.secondary-button {
  background-color: #6e6e6e;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 10px 20px;
  cursor: pointer;
  font-size: 0.9rem;
}

.warning-button {
  background-color: #d1a000;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 10px 20px;
  cursor: pointer;
  font-size: 0.9rem;
}

.primary-button:hover {
  background-color: #3a4e9c;
}

.secondary-button:hover {
  background-color: #5a5a5a;
}

.warning-button:hover {
  background-color: #b38900;
}

/* Responsividade */
@media (max-width: 768px) {
  .content-wrapper {
    flex-direction: column;
  }
  
  .tabs-sidebar {
    width: 100%;
    border-right: none;
    border-bottom: 1px solid #eee;
  }
  
  .tabs-list {
    display: flex;
    overflow-x: auto;
  }
  
  .tab-item {
    padding: 10px 15px;
    white-space: nowrap;
  }
  
  .tab-item.active {
    border-left: none;
    border-bottom: 3px solid #4863c1;
  }
  
  .form-group {
    flex-direction: column;
    align-items: flex-start;
  }
  
  .form-group label {
    width: 100%;
    margin-bottom: 5px;
  }
  
  .form-group input[type="text"],
  .form-group input[type="number"] {
    width: 100%;
  }
  
  .action-buttons {
    flex-direction: column;
  }
  
  .primary-button,
  .secondary-button,
  .warning-button {
    width: 100%;
  }
}
</style>