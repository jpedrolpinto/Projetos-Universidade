<script setup>
import { ref } from 'vue';
import Notification from '../common/Notification.vue';
import api from '../../services/api';
import { useRouter } from 'vue-router';


const username = ref('');
const password = ref('');
const errorMessage = ref('');
const showError = ref(false);

const router = useRouter();

async function login() {
  // Verificar se todos os campos estão preenchidos
  if (!username.value || !password.value) {
    errorMessage.value = 'Para conseguir fazer login tem que preencher todos os campos!';
    showError.value = true;
    return;
  }
  
  try {
    // Primeiro tenta encontrar o utilizador entre os diretores
    const directorsResponse = await api.get(`/directors?email=${username.value}`);
    
    if (directorsResponse.data.length > 0) {
      const user = directorsResponse.data[0];
      
      // Verificar senha
      if (user.password === password.value) {
        handleSuccessfulLogin(user, 'diretor');
        return;
      } else {
        errorMessage.value = 'Palavra passe incorreta';
        showError.value = true;
        return;
      }
    }
    
    // Se não encontrou entre diretores, tenta entre os alunos
    const studentsResponse = await api.get(`/students?email=${username.value}`);
    
    if (studentsResponse.data.length > 0) {
      const user = studentsResponse.data[0];
      
      // Verificar senha
      if (user.password === password.value) {
        handleSuccessfulLogin(user, 'aluno');
        return;
      } else {
        errorMessage.value = 'Palavra passe incorreta';
        showError.value = true;
        return;
      }
    }
    
    // Se não encontrou em nenhum dos dois, usuário não existe
    errorMessage.value = 'Utilizador não encontrado';
    showError.value = true;
    
  } catch (error) {
    console.error('Erro de login:', error);
    errorMessage.value = 'Erro ao tentar fazer login. Tente novamente mais tarde.';
    showError.value = true;
  }
}

function handleSuccessfulLogin(user, userType) {
  console.log('Login bem-sucedido!');
  
  // Armazenar dados do usuário no localStorage para uso posterior
  localStorage.setItem('userId', user.id);
  localStorage.setItem('userType', userType);
  localStorage.setItem('userName', user.name);
  localStorage.setItem('userEmail', user.email);
  
  // Redirecionar para o dashboard apropriado com base no tipo de usuário
  if (userType === 'diretor') {
    console.log('Redirecionando para dashboard do diretor...');
    router.push('/diretor/dashboard');
  } else {
    console.log('Redirecionando para dashboard do aluno...');
    router.push('/dashboard');
  }
}

function forgotPassword() {
  router.push('/recuperar-senha');
}

function handleCloseError() {
  showError.value = false;
}
</script>

<template>
  <div class="login-container">
    <div class="login-card">
      <div class="login-header">
        <h1>GESTÃO DE TURNOS</h1>
      </div>
      
      <div class="login-content">
        <h2>Entrar no Sistema</h2>
        
        <div class="input-group">
          <input 
            type="text" 
            v-model="username" 
            placeholder="Email do utilizador"
            class="login-input"
          />
        </div>
        
        <div class="input-group">
          <input 
            type="password" 
            v-model="password" 
            placeholder="Palavra-passe"
            class="login-input"
          />
        </div>
        
        <button @click="login" class="login-button">ENTRAR</button>
        
        <div class="forgot-password">
          <a href="#" @click.prevent="forgotPassword">Esqueceu a palavra-passe?</a>
        </div>
      </div>
    </div>
    
    <!-- Notification para erro -->
    <Notification
      :show="showError"
      :message="errorMessage"
      type="error"
      :duration="3000"
      @close="handleCloseError"
    />
  </div>
</template>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f5f5;
  position: relative;
}

.login-card {
  width: 400px;
  background-color: white;
  border-radius: 10px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  overflow: hidden;
}

.login-header {
  background-color: #4863c1;
  color: white;
  padding: 20px;
  text-align: center;
}

.login-header h1 {
  margin: 0;
  font-size: 1.5rem;
  font-weight: 600;
}

.login-content {
  padding: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.login-content h2 {
  margin-top: 0;
  margin-bottom: 20px;
  font-size: 1.2rem;
  font-weight: 500;
  color: #333;
}

.input-group {
  width: 100%;
  margin-bottom: 15px;
}

.login-input {
  width: 100%;
  padding: 12px 15px;
  border: 1px solid #ddd;
  border-radius: 25px;
  font-size: 1rem;
  outline: none;
}

.login-input:focus {
  border-color: #4863c1;
}

.login-button {
  width: 60%;
  padding: 12px;
  background-color: #4863c1;
  color: white;
  border: none;
  border-radius: 25px;
  font-size: 1rem;
  font-weight: 600;
  cursor: pointer;
  transition: background-color 0.3s;
  margin-top: 15px;
}

.login-button:hover {
  background-color: #3a4e9c;
}

.forgot-password {
  margin-top: 15px;
}

.forgot-password a {
  color: #4863c1;
  text-decoration: none;
  font-size: 0.9rem;
}

.forgot-password a:hover {
  text-decoration: underline;
}
</style>