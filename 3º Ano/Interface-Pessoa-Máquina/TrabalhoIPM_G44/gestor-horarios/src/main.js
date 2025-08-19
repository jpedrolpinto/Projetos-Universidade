import { createApp } from 'vue'
import App from './App.vue'
import router from './router'

// Criar a aplicação Vue
const app = createApp(App)

// Registrar o router
app.use(router)

// Montar a aplicação
app.mount('#app')