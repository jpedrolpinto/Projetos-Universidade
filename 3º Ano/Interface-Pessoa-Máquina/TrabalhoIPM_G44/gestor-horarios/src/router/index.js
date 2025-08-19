import { createRouter, createWebHistory } from 'vue-router'
import WelcomePage from '../components/common/WelcomePage.vue'
import LoginPage from '../components/common/LoginPage.vue'
import RecoverPassPage from '../components/common/RecoverPassPage.vue'
import StudentDashboard from '../components/student/StudentDashboard.vue'
import StudentCourseSchedule from '../components/student/StudentCourseSchedule.vue'
import StudentRequests from '../components/student/StudentRequests.vue'
import StudentMessages from '../components/student/StudentMessages.vue'
import StudentProfile from '../components/student/StudentProfile.vue'
import DirectorDashboard from '../components/director/DirectorDashboard.vue'
import DirectorSchedule from '../components/director/DirectorSchedule.vue'
import CourseManagement from '../components/director/CourseManagement.vue'
import DirectorRequests from '../components/director/DirectorRequests.vue'
import DirectorManualDistribution from '../components/director/DirectorManualDistribution.vue'
import DirectorPublishSchedules from '../components/director/DirectorPublishSchedules.vue'
import DirectorMessages from '../components/director/DirectorMessages.vue'
import DirectorConfigurations from '../components/director/DirectorConfigurations.vue'

const routes = [
  {
    path: '/',
    name: 'Welcome',
    component: WelcomePage
  },
  {
    path: '/login',
    name: 'Login',  
    component: LoginPage
  },
  {
    path: '/recuperar-senha',
    name: 'RecoverPassword',  
    component: RecoverPassPage
  },
  
  // Rotas do estudante
  {
    path: '/dashboard',
    name: 'Dashboard',  
    component: StudentDashboard,
    meta: { requiresAuth: true, userType: 'aluno' },
  },
  {
    path: '/horario',
    name: 'CourseSchedule',  
    component: StudentCourseSchedule,
    meta: { requiresAuth: true, userType: 'aluno' }
  },
  {
    path: '/pedidos',
    name: 'Requests',  
    component: StudentRequests,
    meta: { requiresAuth: true, userType: 'aluno' }
  },
  {
    path: '/mensagens',
    name: 'Messages',  
    component: StudentMessages,
    meta: { requiresAuth: true }
  },
  {
    path: '/perfil',
    name: 'Profile',  
    component: StudentProfile,
    meta: { requiresAuth: true, userType: 'aluno' }
  },
  
  // Rotas do diretor
  {
    path: '/diretor/dashboard',
    name: 'DirectorDashboard',
    component: DirectorDashboard,
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/diretor/horarios',
    name: 'DirectorSchedule',
    component: DirectorSchedule,
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/gestao-ucs',
    name: 'CourseManagement',
    component: CourseManagement, 
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/pedidos-mudanca',
    name: 'DirectorRequests',
    component: DirectorRequests, 
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/distribuicao-manual',
    name: 'ManualDistribution',
    component: DirectorManualDistribution, 
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/publicar-horarios',
    name: 'PublishSchedules',
    component: DirectorPublishSchedules, 
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/mensagensDiretor',
    name: 'DirectorMessages',
    component: DirectorMessages,
    meta: { requiresAuth: true, userType: 'diretor' }
  },
  {
    path: '/configuracoes',
    name: 'Settings',
    component: DirectorConfigurations,
    meta: { requiresAuth: true, userType: 'diretor' }
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

// Navegação guard para verificar autenticação e permissões de usuário
router.beforeEach((to, from, next) => {
  const isAuthenticated = localStorage.getItem('userId') !== null
  const userType = localStorage.getItem('userType')
  
  // Se a rota requer autenticação e usuário não está autenticado
  if (to.meta.requiresAuth && !isAuthenticated) {
    // Redireciona para login se não autenticado
    next('/login')
  } 
  // Se a rota requer um tipo específico de utilizador
  else if (to.meta.userType && userType !== to.meta.userType) {
    // Redirecionar para o dashboard apropriado
    if (userType === 'diretor') {
      next('/diretor/dashboard')
    } else if (userType === 'aluno') {
      next('/dashboard')
    } else {
      next('/login')
    }
  } else {
    // Permite a navegação
    next()
  }
})

export default router