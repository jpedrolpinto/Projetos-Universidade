<script setup>
import { computed } from 'vue';
import { useRouter } from 'vue-router';

const props = defineProps({
  userName: {
    type: String,
    required: true
  },
  userType: {
    type: String
  },
  activeMenuItem: {
    type: String
  }
});

// Formatar o tipo do usuário com a primeira letra maiúscula
const formattedUserType = computed(() => {
  if (!props.userType) return '';
  return props.userType.charAt(0).toUpperCase() + props.userType.slice(1);
});


const router = useRouter();

// Função de navegação usando Vue Router
const navigateTo = (page) => {
  if (props.userType === 'aluno') {
    // Navegação para alunos
    switch(page) {
      case 'meu-horario':
        router.push('/dashboard');
        break;
      case 'horario':
        router.push('/horario');
        break;
      case 'pedidos':
        router.push('/pedidos');
        break;
      case 'mensagens':
        router.push('/mensagens');
        break;
      case 'perfil':
        router.push('/perfil');
        break;
      default:
        router.push('/dashboard');
    }
  } else if (props.userType === 'diretor') {
    // Navegação para diretor
    switch(page) {
      case 'dashboard':
        router.push('/diretor/dashboard');
        break;
      case 'gestao-ucs':
        router.push('/gestao-ucs');
        break;
      case 'pedidos-mudanca':
        router.push('/pedidos-mudanca');
        break;
      case 'distribuicao-manual':
        router.push('/distribuicao-manual');
        break;
      case 'publicar-horarios':
        router.push('/publicar-horarios');
        break;
      case 'mensagensDiretor':
        router.push('/mensagensDiretor');
        break;
      case 'configuracoes':
        router.push('/configuracoes');
        break;
      default:
        router.push('/diretor/dashboard');
    }
  }
};

// Sair do sistema
const logout = () => {
  // Limpar dados do usuário do localStorage
  localStorage.removeItem('userId');
  localStorage.removeItem('userType');
  localStorage.removeItem('userName');
  localStorage.removeItem('userEmail');
  
  // Redirecionar para a página inicial usando Vue Router
  router.push('/');
};

</script>

<template>
  <div class="dashboard-container">
    <!-- Barra superior -->
    <header class="top-header">
      <div class="system-title">SISTEMA DE GESTÃO DE HORÁRIOS</div>
      <div class="user-info">
        <span class="user-name">{{ userName }} ({{ formattedUserType }})</span>
        <button @click="logout" class="logout-button" title="Sair">
          <i class="logout-icon">➡️</i>
        </button>
      </div>
    </header>
    
    <!-- Conteúdo principal -->
    <div class="main-content">
      <!-- Menu lateral -->
      <aside class="sidebar">
        <nav class="nav-menu">
          <!-- Menu para alunos -->
          <template v-if="userType === 'aluno'">
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'meu-horario' }"
              @click="navigateTo('meu-horario')"
            >
              Meu Horário
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'horario' }"
              @click="navigateTo('horario')"
            >
              Horário
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'pedidos' }"
              @click="navigateTo('pedidos')"
            >
              Pedidos
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'mensagens' }"
              @click="navigateTo('mensagens')"
            >
              Mensagens
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'perfil' }"
              @click="navigateTo('perfil')"
            >
              Perfil
            </div>
          </template>
          
          <!-- Menu para diretor -->
          <template v-else-if="userType === 'diretor'">
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'dashboard' }"
              @click="navigateTo('dashboard')"
            >
              Dashboard
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'gestao-ucs' }"
              @click="navigateTo('gestao-ucs')"
            >
              Gestão de UCs
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'pedidos-mudanca' }"
              @click="navigateTo('pedidos-mudanca')"
            >
              Pedidos de Mudança
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'distribuicao-manual' }"
              @click="navigateTo('distribuicao-manual')"
            >
              Distribuição Manual
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'publicar-horarios' }"
              @click="navigateTo('publicar-horarios')"
            >
              Publicar Horários
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'mensagensDiretor' }"
              @click="navigateTo('mensagensDiretor')"
            >
              Mensagens
            </div>
            <div 
              class="menu-item" 
              :class="{ active: activeMenuItem === 'configuracoes' }"
              @click="navigateTo('configuracoes')"
            >
              Configurações
            </div>
          </template>
        </nav>
      </aside>
      
      <!-- Slot para conteúdo da página -->
      <main class="page-content">
        <slot></slot>
      </main>
    </div>
  </div>
</template>

<style scoped>
.dashboard-container {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
  margin: 0;
  padding: 0;
  background-color: #f5f5f5;
}

.top-header {
  background-color: #4863c1;
  color: white;
  padding: 15px 20px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin: 0;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.system-title {
  font-size: 1.2rem;
  font-weight: 600;
}

.user-info {
  display: flex;
  align-items: center;
  gap: 15px;
}

.user-name {
  font-size: 0.9rem;
}

.logout-button {
  background: none;
  border: none;
  color: white;
  cursor: pointer;
  font-size: 1.2rem;
}

.main-content {
  display: flex;
  flex: 1;
  margin: 0;
  padding: 0;
}

.sidebar {
  width: 200px;
  min-width: 200px; /* Garatir a mesma largura em todas as telas */
  background-color: #f0f0f0;
  border-right: 1px solid #ddd;
  margin-top: 0;
}

.nav-menu {
  padding: 20px 0;
}

.menu-item {
  padding: 12px 20px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.menu-item:hover {
  background-color: #e0e0e0;
}

.menu-item.active {
  background-color: #e8f0ff;
  border-left: 4px solid #4863c1;
  font-weight: 500;
}

.page-content {
  flex: 1;
  padding: 20px;
  overflow-x: auto; /* Adiciona scroll horizontal se o conteúdo for muito largo */
}
</style>