<script setup>
import { useRouter } from 'vue-router'

const router = useRouter();

function navigateToLogin() {
  // Navegar para a página de login 
  router.push('/login');
}
</script>

<template>
  <div class="welcome-container">
    <header class="app-header">
      <h1>GESTÃO DE HORÁRIOS</h1>
    </header>
    
    <main class="welcome-content">
      <h2>Bem-vindo ao Sistema de Gestão de Horários</h2>
      <p>Clique em iniciar para prosseguir para login</p>
      
      <div class="graphics-container">
        <!-- Ícones e ilustrações -->
        <div class="left-graphics">
          <div class="calendar-icon"></div>
          <div class="user-icon"></div>
          <button class="start-button" @click="navigateToLogin">
            Iniciar
          </button>
        </div>
        
        <div class="right-graphics">
          <div class="alarm-icon"></div>
          <div class="calendar-large"></div>
          <div class="timeline"></div>
        </div>
      </div>
    </main>
  </div>
</template>

<style scoped>
.welcome-container {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
}

.app-header {
  background-color: #3f51b5;
  color: white;
  padding: 1rem;
}

.app-header h1 {
  margin: 0;
  font-size: 1.5rem;
}

.welcome-content {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  padding: 2rem;
}

.welcome-content h2 {
  font-size: 1.8rem;
  margin-bottom: 0.5rem;
  color: #333;
}

.welcome-content p {
  color: #666;
  margin-bottom: 2rem;
}

.graphics-container {
  display: flex;
  justify-content: space-between;
  width: 100%;
  max-width: 900px;
}

.left-graphics, .right-graphics {
  display: flex;
  flex-direction: column;
  align-items: center;
}

/* Estilizar os ícones com pseudo-elementos usando cores azuis */
.calendar-icon {
  width: 200px;
  height: 180px;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%233f51b5'%3E%3Cpath d='M19 4h-1V2h-2v2H8V2H6v2H5c-1.11 0-1.99.9-1.99 2L3 20c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2V6c0-1.1-.9-2-2-2zm0 16H5V9h14v11z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: center;
  margin-bottom: 20px;
}

.user-icon {
  width: 120px;
  height: 120px;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%233f51b5'%3E%3Cpath d='M12 12c2.21 0 4-1.79 4-4s-1.79-4-4-4-4 1.79-4 4 1.79 4 4 4zm0 2c-2.67 0-8 1.34-8 4v2h16v-2c0-2.66-5.33-4-8-4z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: center;
  margin-bottom: 20px;
}

.alarm-icon {
  width: 150px;
  height: 150px;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%233f51b5'%3E%3Cpath d='M22 5.72l-4.6-3.86-1.29 1.53 4.6 3.86L22 5.72zM7.88 3.39L6.6 1.86 2 5.71l1.29 1.53 4.59-3.85zM12.5 8H11v6l4.75 2.85.75-1.23-4-2.37V8zM12 4c-4.97 0-9 4.03-9 9s4.02 9 9 9c4.97 0 9-4.03 9-9s-4.03-9-9-9zm0 16c-3.87 0-7-3.13-7-7s3.13-7 7-7 7 3.13 7 7-3.13 7-7 7z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: center;
  margin-bottom: 20px;
}

.calendar-large {
  width: 200px;
  height: 200px;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%233f51b5'%3E%3Cpath d='M19 4h-1V2h-2v2H8V2H6v2H5c-1.11 0-1.99.9-1.99 2L3 20c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2V6c0-1.1-.9-2-2-2zm0 16H5V9h14v11zM7 11h5v5H7v-5z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: center;
  margin-bottom: 20px;
}

.timeline {
  width: 200px;
  height: 120px;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%233f51b5'%3E%3Cpath d='M23 8c0 1.1-.9 2-2 2-.18 0-.35-.02-.51-.07l-3.56 3.55c.05.16.07.34.07.52 0 1.1-.9 2-2 2s-2-.9-2-2c0-.18.02-.36.07-.52l-2.55-2.55c-.16.05-.34.07-.52.07s-.36-.02-.52-.07l-4.55 4.56c.05.16.07.33.07.51 0 1.1-.9 2-2 2s-2-.9-2-2 .9-2 2-2c.18 0 .35.02.51.07l4.56-4.55C8.02 9.36 8 9.18 8 9c0-1.1.9-2 2-2s2 .9 2 2c0 .18-.02.36-.07.52l2.55 2.55c.16-.05.34-.07.52-.07s.36.02.52.07l3.55-3.56C19.02 8.35 19 8.18 19 8c0-1.1.9-2 2-2s2 .9 2 2z'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: center;
}

.start-button {
  background-color: #3f51b5;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 0.75rem 2rem;
  font-size: 1rem;
  cursor: pointer;
  transition: background-color 0.3s;
}

.start-button:hover {
  background-color: #303f9f;
}
</style>