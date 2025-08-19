<script setup>
import { ref, onMounted, computed } from 'vue';
import HeaderSidebar from '../common/HeaderSidebar.vue';
import api from '../../services/api';
import { 
  loadMultipleData, 
  loadDirectorData, 
  loadData,
  loadCourses as loadCoursesUtil
} from '../../utils/common';

// Estado da página
const courses = ref([]);
const loading = ref(true);
const error = ref(null);

// Estado para componentes relacionados
const shifts = ref([]);
const allocations = ref([]);
const teachers = ref([]);

// Filtros
const selectedSemester = ref(2);
const selectedYear = ref(3);
const searchTerm = ref('');

// Modal de edição de turno
const showShiftModal = ref(false);
const currentShift = ref(null);
const currentCourse = ref(null);

// Seleção de detalhes do curso
const selectedCourse = ref(null);

// Anos e semestres
const yearOptions = [
  { value: 1, label: '1º Ano' },
  { value: 2, label: '2º Ano' },
  { value: 3, label: '3º Ano' }
];

const semesterOptions = [
  { value: 1, label: '1º Semestre' },
  { value: 2, label: '2º Semestre' }
];

// Todas as páginas do diretor devem ter estas linhas
const directorData = ref(null);
const directorId = ref(parseInt(localStorage.getItem('userId') || '1'));

// Função atualizada para carregar cursos com contagem de inscritos
async function loadCourses() {
  try {
    // Definir estado de carregamento
    loading.value = true;
    error.value = null;
    
    // Definir os dados que precisamos carregar e suas referências
    const dataLoads = [
      { endpoint: '/courses', ref: courses },
      { endpoint: '/allocations', ref: allocations },
      { endpoint: '/shifts', ref: shifts },
      { endpoint: '/teachers', ref: teachers }
    ];
    
    // Usar loadMultipleData para carregar tudo em paralelo
    const success = await loadMultipleData(dataLoads, loading, error);
    
    if (success) {
      // Processar os dados após o carregamento
      processCoursesData();
    }
    
  } catch (err) {
    console.error('Erro ao carregar cursos:', err);
    error.value = 'Erro ao carregar cursos. Tente novamente.';
    loading.value = false;
  }
}

// Função para processar os dados dos cursos
function processCoursesData() {
  // Contar alunos únicos inscritos por curso
  const enrolledCount = {};

  // Primeiro, agrupar as alocações por estudante
  const studentAllocations = {};
  allocations.value.forEach(allocation => {
    if (!studentAllocations[allocation.studentId]) {
      studentAllocations[allocation.studentId] = [];
    }
    studentAllocations[allocation.studentId].push(allocation.shiftId);
  });

  // Depois, contar alunos únicos por curso
  Object.keys(studentAllocations).forEach(studentId => {
    const studentShifts = studentAllocations[studentId];
    
    // Criar um conjunto para armazenar os cursos em que o aluno está inscrito
    const studentCourses = new Set();
    
    // Adicionar os IDs dos cursos ao conjunto
    studentShifts.forEach(shiftId => {
      const shift = shifts.value.find(s => s.id === shiftId);
      if (shift && shift.courseId) {
        studentCourses.add(shift.courseId);
      }
    });
    
    // Incrementar a contagem para cada curso
    studentCourses.forEach(courseId => {
      enrolledCount[courseId] = (enrolledCount[courseId] || 0) + 1;
    });
  });

  // Contar turnos por curso
  const shiftsCount = shifts.value.reduce((acc, shift) => {
    acc[shift.courseId] = (acc[shift.courseId] || 0) + 1;
    return acc;
  }, {});
  
  // Mapeando professores para os seus IDs para fácil acesso
  const teachersMap = teachers.value.reduce((acc, teacher) => {
    acc[teacher.id] = teacher.name;
    return acc;
  }, {});

  // Filtrar e mapear os cursos
  courses.value = courses.value
    .filter(course => 
      course.year === selectedYear.value && 
      course.semester === selectedSemester.value
    )
    .map(course => ({
      ...course,
      enrolledStudents: enrolledCount[course.id] || 0,
      shifts: shiftsCount[course.id] || 0,
      teacherName: course.teacherId ? teachersMap[course.teacherId] : null
    }));
}

// Abrir detalhes do curso - versão atualizada
async function openCourseDetails(course) {
  try {
    // Usar loadData para carregar os turnos
    const shiftsForCourse = ref([]);
    const allocationsData = ref([]);
    const teachersData = ref([]);
    const loadingDetails = ref(false);
    const errorDetails = ref(null);
    
    // Carregar dados em paralelo
    await loadMultipleData([
      { 
        endpoint: `/shifts?courseId=${course.id}`, 
        ref: shiftsForCourse 
      },
      { 
        endpoint: '/teachers', 
        ref: teachersData 
      },
      { 
        endpoint: '/allocations', 
        ref: allocationsData 
      }
    ], loadingDetails, errorDetails);
    
    if (errorDetails.value) {
      error.value = errorDetails.value;
      return;
    }
    
    // Contar inscritos por turno
    const shiftsWithStudents = shiftsForCourse.value.map(shift => {
      const studentsInShift = allocationsData.value.filter(
        allocation => allocation.shiftId === shift.id
      ).length;
      
      return {
        ...shift,
        totalStudentsRegistered: studentsInShift
      };
    });
    
    // Atualizar o curso selecionado
    selectedCourse.value = {
      ...course,
      shifts: shiftsWithStudents,
      teachers: teachersData.value
    };
  } catch (err) {
    console.error('Erro ao carregar detalhes do curso:', err);
    error.value = 'Erro ao carregar detalhes do curso.';
  }
}

// Abrir modal de edição de turno
function openShiftModal(shift, course) {
  currentShift.value = { ...shift };
  currentCourse.value = course;
  showShiftModal.value = true;
}

// Salvar alterações do curso
async function saveCourseChanges() {
  try {
    // Criar um objeto com todos os campos existentes do curso
    const courseUpdate = {
      name: selectedCourse.value.name,
      abbreviation: selectedCourse.value.abbreviation,
      teacherId: selectedCourse.value.teacherId,
      // Manter os campos existentes que não são editáveis na interface
      year: selectedCourse.value.year,
      semester: selectedCourse.value.semester,
      degreeId: selectedCourse.value.degreeId
    };
    
    // Atualizar informações do curso
    await api.patch(`/courses/${selectedCourse.value.id}`, courseUpdate);
    
    // Usar loadData para buscar o curso atualizado
    const updatedCourse = ref(null);
    await loadData(`/courses/${selectedCourse.value.id}`, updatedCourse, null, error);
    
    // Obter o nome do professor
    if (updatedCourse.value.teacherId) {
      const teacher = teachers.value.find(t => t.id === updatedCourse.value.teacherId);
      updatedCourse.value.teacherName = teacher ? teacher.name : null;
    }

    // Atualizar a lista de cursos em memória também
    const courseIndex = courses.value.findIndex(c => c.id === updatedCourse.value.id);
    if (courseIndex !== -1) {
      // Manter outros dados como enrolledStudents e shifts que foram calculados
      const existingCourse = courses.value[courseIndex];
      courses.value[courseIndex] = {
        ...updatedCourse.value,
        enrolledStudents: existingCourse.enrolledStudents,
        shifts: existingCourse.shifts
      };
    }
    
    // Recarregar detalhes do curso usando o curso atualizado do servidor
    await openCourseDetails(updatedCourse.value);
    
    // Mostrar notificação de sucesso
    alert('Alterações salvas com sucesso!');
  } catch (err) {
    console.error('Erro ao salvar alterações do curso:', err);
    alert('Erro ao salvar alterações. Tente novamente.');
  }
}

// Salvar alterações do turno
async function saveShift() {
  try {
    await api.patch(`/shifts/${currentShift.value.id}`, currentShift.value);
    showShiftModal.value = false;
    
    // Recarregar detalhes do curso
    await openCourseDetails(currentCourse.value);
    
    // Mostrar notificação de sucesso
    alert('Turno atualizado com sucesso!');
  } catch (err) {
    console.error('Erro ao salvar turno:', err);
    alert('Erro ao salvar turno. Tente novamente.');
  }
}

// Adicionar novo turno
async function addNewShift() {
  try {
    // Verificar se existe um curso selecionado
    if (!selectedCourse.value) {
      alert('Selecione primeiro um curso.');
      return;
    }
    
    // Criar novo turno
    const newShift = {
      courseId: selectedCourse.value.id,
      name: 'Novo Turno',
      type: 'T', // Padrão como teórico
      day: 'Segunda',
      from: 10,
      to: 12,
      classroomId: 1, // ID padrão ou obtenha uma sala disponível
      teacherId: null
    };
    
    const response = await api.post('/shifts', newShift);
    
    // Atualizar o número de turnos no curso atual
    const courseIndex = courses.value.findIndex(c => c.id === selectedCourse.value.id);
    if (courseIndex !== -1) {
      courses.value[courseIndex].shifts++;
    }
    
    // Recarregar detalhes do curso para mostrar o novo turno
    await openCourseDetails(selectedCourse.value);
    
    // Abrir modal de edição para o novo turno
    openShiftModal(response.data, selectedCourse.value);
  } catch (err) {
    console.error('Erro ao adicionar novo turno:', err);
    alert('Erro ao adicionar novo turno. Tente novamente.');
  }
}

// Método para deletar um turno
async function deleteShift(shift) {
  try {
    // Verificar se há alunos inscritos no turno
    if (shift.totalStudentsRegistered > 0) {
      alert('Não é possível apagar um turno com alunos inscritos!');
      return;
    }
    
    if (!confirm(`Tem certeza que deseja apagar o turno ${shift.name}?`)) {
      return;
    }
    
    await api.delete(`/shifts/${shift.id}`);
    
    // Recarregar detalhes do curso
    await openCourseDetails(selectedCourse.value);
    
    alert('Turno apagado com sucesso!');
  } catch (err) {
    console.error('Erro ao apagar turno:', err);
    alert('Erro ao apagar turno. Tente novamente.');
  }
}

// Filtrar cursos pelo termo de pesquisa
const filteredCourses = computed(() => {
  if (!searchTerm.value) return courses.value;
  
  const searchLower = searchTerm.value.toLowerCase();
  return courses.value.filter(course => 
    course.name.toLowerCase().includes(searchLower) ||
    course.abbreviation.toLowerCase().includes(searchLower)
  );
});

// Carregar dados ao montar o componente
onMounted(async () => {
  // Usar loadDirectorData para carregar os dados do diretor
  await loadDirectorData(directorId, directorData, loading, error);
  
  // Carregar os cursos e dados relacionados
  await loadCourses();
});
</script>

<template>
  <HeaderSidebar 
    :userName="directorData?.name" 
    userType="diretor"
    activeMenuItem="gestao-ucs"
  >
    <div class="course-management-container">
      <!-- Título e Filtros -->
      <div class="page-header">
        <h1>Gestão de UCs</h1>
        <p>Visão geral das Unidades Curriculares</p>
      </div>
      
      <div class="filters-container">
        <div class="filter-group">
          <label>Semestre:</label>
          <select v-model="selectedSemester" @change="loadCourses">
            <option v-for="semester in semesterOptions" 
                    :key="semester.value" 
                    :value="semester.value">
              {{ semester.label }}
            </option>
          </select>
        </div>
        
        <div class="filter-group">
          <label>Ano:</label>
          <select v-model="selectedYear" @change="loadCourses">
            <option v-for="year in yearOptions" 
                    :key="year.value" 
                    :value="year.value">
              {{ year.label }}
            </option>
          </select>
        </div>
        
        <div class="filter-group search-group">
          <input 
            type="text" 
            v-model="searchTerm" 
            placeholder="Pesquisar UC..."
          />
        </div>
      </div>
      
      <!-- Lista de Cursos -->
      <div v-if="loading" class="loading-state">
        Carregando cursos...
      </div>
      
      <div v-else-if="error" class="error-state">
        {{ error }}
        <button @click="loadCourses">Tentar Novamente</button>
      </div>
      
      <div v-else class="courses-table">
        <table>
          <thead>
            <tr>
              <th>Nome</th>
              <th>Código</th>
              <th>Ano</th>
              <th>Turnos</th>
              <th>Inscritos</th>
              <th>Docente</th>
              <th>Ação</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="course in filteredCourses" :key="course.id">
              <td>{{ course.name }}</td>
              <td>{{ course.abbreviation }}</td>
              <td>{{ course.year }}º</td>
              <td>{{ course.shifts }}</td>
              <td>{{ course.enrolledStudents }}</td>
              <td>{{ course.teacherName || 'Não definido' }}</td>
              <td>
                <button @click="openCourseDetails(course)">
                  Detalhes
                </button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>
      
      <!-- Modal de Detalhes do Curso -->
      <div v-if="selectedCourse" class="modal-overlay">
        <div class="modal-content">
          <div class="modal-header">
            <h2>Detalhes da UC: {{ selectedCourse.name }}</h2>
            <button class="modal-close" @click="selectedCourse = null">×</button>
          </div>
          
          <div class="course-info">
            <div class="info-section">
              <div class="form-group">
                <label>Nome:</label>
                <input 
                  v-model="selectedCourse.name" 
                  placeholder="Nome da Unidade Curricular"
                />
              </div>
              
              <div class="form-group">
                <label>Código:</label>
                <input 
                  v-model="selectedCourse.abbreviation" 
                  placeholder="Código da UC"
                />
              </div>
              
              <div class="form-group">
                <label>Docente:</label>
                <select v-model="selectedCourse.teacherId">
                  <option value="">Selecione um docente</option>
                  <option 
                    v-for="teacher in selectedCourse.teachers" 
                    :key="teacher.id" 
                    :value="teacher.id"
                  >
                    {{ teacher.name }}
                  </option>
                </select>
              </div>
            </div>
            
            <div class="shifts-section">
              <div class="shifts-header">
                <h3>Turnos da UC</h3>
                <button 
                  class="add-shift-btn" 
                  @click="addNewShift"
                >
                  + Adicionar Turno
                </button>
              </div>
              
              <table class="shifts-table">
                <thead>
                  <tr>
                    <th>Turno</th>
                    <th>Tipo</th>
                    <th>Dia</th>
                    <th>Horas</th>
                    <th>Inscritos</th>
                    <th>Ações</th>
                  </tr>
                </thead>
                <tbody>
                  <tr v-for="shift in selectedCourse.shifts" :key="shift.id">
                    <td>{{ shift.name }}</td>
                    <td>{{ shift.type }}</td>
                    <td>{{ shift.day }}</td>
                    <td>{{ shift.from }}:00 - {{ shift.to }}:00</td>
                    <td>{{ shift.totalStudentsRegistered }}</td>
                    <td>
                      <div class="action-buttons">
                        <button 
                          class="edit-btn"
                          @click="openShiftModal(shift, selectedCourse)"
                        >
                          Editar
                        </button>
                        <button 
                          class="delete-btn"
                          @click="deleteShift(shift)"
                        >
                          Apagar
                        </button>
                      </div>
                    </td>
                  </tr>
                </tbody>
              </table>
            </div>
          </div>
          
          <div class="modal-footer">
            <button 
              class="cancel-btn" 
              @click="selectedCourse = null"
            >
              Cancelar
            </button>
            <button 
              class="save-btn" 
              @click="saveCourseChanges"
            >
              Salvar Alterações
            </button>
          </div>
        </div>
      </div>
      
      <!-- Modal de Edição de Turno -->
      <div v-if="showShiftModal" class="modal-overlay">
        <div class="modal-content shift-modal">
          <div class="modal-header">
            <h2>Adicionar Turno: {{ currentShift.name }}</h2>
            <button class="modal-close" @click="showShiftModal = false">×</button>
          </div>
          
          <div class="form-grid">
            <div class="form-group">
              <label>Nome do Turno:</label>
              <input v-model="currentShift.name" placeholder="Nome do Turno" />
            </div>
            
            <div class="form-group">
              <label>Tipo:</label>
              <select v-model="currentShift.type">
                <option value="T">Teórico</option>
                <option value="PL">Prático</option>
                <option value="TP">Teórico-Prático</option>
              </select>
            </div>
            
            <div class="form-group">
              <label>Dia:</label>
              <select v-model="currentShift.day">
                <option>Segunda</option>
                <option>Terça</option>
                <option>Quarta</option>
                <option>Quinta</option>
                <option>Sexta</option>
              </select>
            </div>
            
            <div class="form-group">
              <label>Hora de Início:</label>
              <input 
                type="number" 
                v-model="currentShift.from" 
                min="8" 
                max="20" 
                placeholder="Hora de início"
              />
            </div>
            
            <div class="form-group">
              <label>Hora de Fim:</label>
              <input 
                type="number" 
                v-model="currentShift.to" 
                min="8" 
                max="20" 
                placeholder="Hora de fim"
              />
            </div>
            
            <div class="form-group">
              <label>Sala:</label>
              <input 
                v-model="currentShift.classroom" 
                placeholder="Sala"
              />
            </div>
            <div class="form-group">
              <label>Docente:</label>
              <select v-model="currentShift.teacherId">
                <option value="">Selecione um docente</option>
                <option 
                  v-for="teacher in currentCourse.teachers" 
                  :key="teacher.id" 
                  :value="teacher.id"
                >
                  {{ teacher.name }}
                </option>
              </select>
            </div>
          </div>
          
          <div class="modal-footer">
            <button 
              class="cancel-btn" 
              @click="showShiftModal = false"
            >
              Cancelar
            </button>
            <button 
              class="save-btn" 
              @click="saveShift"
            >
              Salvar Turno
            </button>
          </div>
        </div>
      </div>
    </div>
  </HeaderSidebar>
</template>

<style scoped>
/* Estilo global */
.course-management-container {
  background-color: #f5f5f5;
  padding: 20px;
  border-radius: 8px;
}

.page-header {
  margin-bottom: 20px;
}

.page-header h1 {
  color: #333;
  font-size: 1.5rem;
  margin-bottom: 5px;
}

.page-header p {
  color: #666;
  font-size: 0.9rem;
}

/* Filtros */
.filters-container {
  display: flex;
  gap: 15px;
  margin-bottom: 20px;
  background-color: white;
  padding: 15px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.filter-group {
  display: flex;
  flex-direction: column;
}

.filter-group label {
  margin-bottom: 5px;
  font-size: 0.8rem;
  color: #555;
}

.filter-group select,
.filter-group input {
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
}

.search-group {
  flex-grow: 1;
}

/* Tabela de cursos */
.courses-table table {
  width: 100%;
  background-color: white;
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  border-collapse: separate;
  border-spacing: 0;
}

.courses-table th,
.courses-table td {
  padding: 12px;
  text-align: left;
  border-bottom: 1px solid #eee;
}

.courses-table th {
  background-color: #f9f9f9;
  font-weight: 600;
  color: #333;
}

.courses-table button {
  background-color: #4863c1;
  color: white;
  border: none;
  padding: 5px 10px;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.courses-table button:hover {
  background-color: #3a4e9c;
}

/* Modal overlay */
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
  padding: 20px;
}

.modal-content {
  background-color: white;
  border-radius: 12px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
  width: 100%;
  max-width: 900px;
  max-height: 90vh;
  overflow-y: auto;
  padding: 25px;
  position: relative;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  border-bottom: 1px solid #eee;
  padding-bottom: 15px;
}

.modal-header h2 {
  margin: 0;
  font-size: 1.2rem;
  color: #333;
}

.modal-close {
  background: none;
  border: none;
  font-size: 1.5rem;
  color: #666;
  cursor: pointer;
}

.course-info {
  display: flex;
  gap: 25px;
}

.info-section,
.shifts-section {
  flex: 1;
}

.form-group {
  margin-bottom: 15px;
}

.form-group label {
  display: block;
  margin-bottom: 5px;
  font-size: 0.9rem;
  color: #555;
}

.form-group input,
.form-group select {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 0.9rem;
}

.shifts-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 15px;
}

.shifts-table {
  width: 100%;
  border-collapse: collapse;
}

.shifts-table th,
.shifts-table td {
  border: 1px solid #eee;
  padding: 10px;
  text-align: left;
}

.shifts-table th {
  background-color: #f9f9f9;
  font-weight: 600;
}

.modal-footer {
  display: flex;
  justify-content: flex-end;
  gap: 15px;
  margin-top: 20px;
  border-top: 1px solid #eee;
  padding-top: 15px;
}

.cancel-btn,
.save-btn {
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  font-size: 0.9rem;
  cursor: pointer;
  transition: background-color 0.2s;
}

.cancel-btn {
  background-color: #f0f0f0;
  color: #333;
}

.save-btn {
  background-color: #4863c1;
  color: white;
}

.save-btn:hover {
  background-color: #3a4e9c;
}

.cancel-btn:hover {
  background-color: #e0e0e0;
}

.add-shift-btn {
  background-color: #28a745;
  color: white;
  border: none;
  padding: 8px 15px;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.add-shift-btn:hover {
  background-color: #218838;
}

.form-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 15px;
}

.action-buttons {
  white-space: nowrap;
  display: flex;
  gap: 5px;
}

.edit-btn {
  background-color: #ffc107;
  color: #333;
  border: none;
  padding: 5px 10px;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
  display: inline-block;
}

.edit-btn:hover {
  background-color: #e0a800;
}

.delete-btn {
  background-color: #dc3545;
  color: white;
  border: none;
  padding: 5px 10px;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
  display: inline-block;
}

.delete-btn:hover {
  background-color: #c82333;
}

/* Estados de carregamento e erro */
.loading-state,
.error-state {
  text-align: center;
  padding: 20px;
  background-color: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.error-state {
  color: #d9534f;
}

@media (max-width: 768px) {
  .filters-container,
  .course-info {
    flex-direction: column;
  }

  .modal-content {
    width: 95%;
    max-width: 95%;
  }

  .form-grid {
    grid-template-columns: 1fr;
  }
}
</style>