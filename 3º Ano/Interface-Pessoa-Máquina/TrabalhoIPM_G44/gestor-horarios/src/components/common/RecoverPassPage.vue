<script setup>
import { ref } from 'vue';
import Notification from '../common/Notification.vue';
import { useRouter } from 'vue-router'

const email = ref('');
const showSuccess = ref(false);
const errorMessage = ref('');
const showError = ref(false);

const router = useRouter();

function recoverPassword() {
  // Verificar se o email foi preenchido
  if (!email.value) {
    errorMessage.value = 'Por favor, insira um email para recuperação.';
    showError.value = true;
    return;
  }
  
  // Mostrar a mensagem de sucesso
  showSuccess.value = true;
  
  // Após um tempo, redirecionar para a página de login
  setTimeout(() => {
    router.push('/login');
  }, 3000);
}

function handleCloseError() {
  showError.value = false;
}

function handleCloseSuccess() {
  showSuccess.value = false;
}
</script>

<template>
  <div class="recover-container">
    <div class="recover-card">
      <div class="recover-header">
        <h1>GESTÃO DE TURNOS</h1>
      </div>
      
      <div class="recover-content">
        <h2>Recuperar Senha</h2>
        
        <div class="input-group">
          <label for="email">Email de Recuperação:</label>
          <input 
            type="email" 
            id="email"
            v-model="email" 
            placeholder="Insira o seu email"
            class="recover-input"
          />
        </div>
        
        <button @click="recoverPassword" class="recover-button">
          Enviar Email de Recuperação
        </button>
      </div>
    </div>
    
    <!-- Utilizando o componente Notification para mensagens de erro e sucesso -->
    <Notification
      :show="showError"
      :message="errorMessage"
      type="error"
      :duration="3000"
      @close="handleCloseError"
    />
    
    <Notification
      :show="showSuccess"
      message="Email para recuperação de password enviado com sucesso, prossiga no seu email."
      type="success"
      :duration="3000"
      @close="handleCloseSuccess"
    />
  </div>
</template>

<style scoped>
.recover-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f5f5;
  position: relative;
}

.recover-card {
  width: 400px;
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  overflow: hidden;
}

.recover-header {
  background-color: #4863c1;
  color: white;
  padding: 20px;
  text-align: center;
}

.recover-header h1 {
  margin: 0;
  font-size: 1.5rem;
  font-weight: 600;
}

.recover-content {
  padding: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.recover-content h2 {
  margin-top: 0;
  margin-bottom: 20px;
  font-size: 1.2rem;
  font-weight: 500;
  color: #333;
}

.input-group {
  width: 100%;
  margin-bottom: 20px;
}

.input-group label {
  display: block;
  margin-bottom: 5px;
  font-size: 0.9rem;
  color: #555;
}

.recover-input {
  width: 100%;
  padding: 12px 15px;
  border: 1px solid #ddd;
  border-radius: 25px;
  font-size: 1rem;
  outline: none;
}

.recover-input:focus {
  border-color: #4863c1;
}

.recover-button {
  width: 80%;
  padding: 12px;
  margin-top: 10px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 25px;
  font-size: 1rem;
  font-weight: 600;
  cursor: pointer;
  transition: background-color 0.3s;
}

.recover-button:hover {
  background-color: #3a4e9c;
}
</style>