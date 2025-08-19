// src/utils/common.js
import { ref } from 'vue';
import api from '../services/api';

/**
 * Mapeamento dos dias da semana em inglês para português
 */
export const dayMap = {
  'Monday': 'segunda-feira',
  'Tuesday': 'terça-feira',
  'Wednesday': 'quarta-feira',
  'Thursday': 'quinta-feira',
  'Friday': 'sexta-feira'
};

/**
 * Dias da semana em português
 */
export const weekDays = Object.values(dayMap);

/**
 * Lista de horários padrão
 */
export const timeSlots = [
  '8:00', '9:00', '10:00', '11:00', '12:00', '13:00', 
  '14:00', '15:00', '16:00', '17:00', '18:00', '19:00'
];

/**
 * Função para gerar um objeto com cores para disciplinas
 * @returns {Object} Mapa de IDs para cores
 */
const generateCourseColors = () => {
  const colors = [
    '#CCFFCC', // verde claro
    '#ADD8E6', // azul claro
    '#FFCCCC', // vermelho claro
    '#CCCCFF', // roxo claro
    '#FFFFCC', // amarelo claro
    '#FFCC99', // laranja claro
    '#E6B8B7', // rosa claro
    '#B7DEE8', // azul turquesa
    '#FFF2CC', // amarelo pálido
    '#E2EFDA', // verde pálido
    '#CCC0DA', // roxo pálido
    '#F8CBAD', // laranja pálido
    '#DDEBF7', // azul pálido
    '#FADBD8', // rosa pálido
    '#D0E0E3'  // azul esverdeado
  ];
  
  const courseColors = {};
  for (let i = 1; i <= 30; i++) {
    courseColors[i] = colors[Math.floor((i - 1) / 2) % colors.length];
  }
  
  return courseColors;
};

/**
 * Cores para as disciplinas
 */
export const courseColors = generateCourseColors();

/**
 * Função genérica para carregar dados da API
 * @param {string} endpoint - Endpoint da API (ex: '/courses', '/students/1')
 * @param {Ref} dataRef - Referência onde armazenar os dados carregados
 * @param {Ref} loadingRef - Referência ao estado de carregamento
 * @param {Ref} errorRef - Referência para mensagens de erro
 * @param {Function} processor - Função opcional para processar os dados após carregamento
 * @returns {Promise<boolean>} - Sucesso ou falha na operação
 */
export const loadData = async (endpoint, dataRef, loadingRef, errorRef, processor = null) => {
  try {
    if (loadingRef) loadingRef.value = true;
    if (errorRef) errorRef.value = null;
    
    const response = await api.get(endpoint);
    const responseData = response.data;
    
    // Se um processador foi fornecido, use-o para transformar os dados
    if (processor && typeof processor === 'function') {
      dataRef.value = processor(responseData);
    } else {
      dataRef.value = responseData;
    }
    
    if (loadingRef) loadingRef.value = false;
    return true;
  } catch (err) {
    console.error(`Erro ao carregar dados de ${endpoint}:`, err);
    if (errorRef) errorRef.value = `Erro ao carregar dados. Por favor, tente novamente.`;
    if (loadingRef) loadingRef.value = false;
    return false;
  }
};

/**
 * Função para carregar múltiplos dados em paralelo
 * @param {Array} dataLoads - Array de objetos {endpoint, ref, processor}
 * @param {Ref} loadingRef - Referência ao estado de carregamento
 * @param {Ref} errorRef - Referência para mensagens de erro
 * @returns {Promise<boolean>} - Sucesso ou falha na operação
 */
export const loadMultipleData = async (dataLoads, loadingRef, errorRef) => {
  try {
    if (loadingRef) loadingRef.value = true;
    if (errorRef) errorRef.value = null;
    
    const loadPromises = dataLoads.map(({ endpoint, ref, processor }) => {
      return api.get(endpoint)
        .then(response => {
          const data = response.data;
          ref.value = processor ? processor(data) : data;
          return true;
        })
        .catch(err => {
          console.error(`Erro ao carregar dados de ${endpoint}:`, err);
          throw new Error(`Falha ao carregar dados de ${endpoint}`);
        });
    });
    
    await Promise.all(loadPromises);
    
    if (loadingRef) loadingRef.value = false;
    return true;
  } catch (err) {
    console.error('Erro ao carregar múltiplos dados:', err);
    if (errorRef) errorRef.value = `Erro ao carregar dados. Por favor, tente novamente.`;
    if (loadingRef) loadingRef.value = false;
    return false;
  }
};

// Definições de funções de carregamento específicas
const endpointLoaders = {
  student: (id) => `/students/${id.value}`,
  director: (id) => `/directors/${id.value}`,
  courses: () => '/courses',
  shifts: () => '/shifts',
  students: () => '/students',
  shiftRequests: () => '/shiftRequests',
  studentAllocations: (id) => `/allocations?studentId=${id.value}`,
  studentConflicts: (id) => `/conflicts?studentId=${id.value}`
};

// Processadores para diferentes tipos de dados
const dataProcessors = {
  shiftRequests: (data, pendingOnly = false) => {
    const sortedData = data.sort((a, b) => new Date(b.date) - new Date(a.date));
    return pendingOnly ? sortedData.filter(request => request.response === null) : sortedData;
  }
};

// Função para carregar dados com base no tipo
export const loadEntityData = (type, idRef = null, dataRef, loadingRef, errorRef, options = {}) => {
  const endpoint = endpointLoaders[type](idRef);
  const processor = options.processor || (options.pendingOnly && type === 'shiftRequests' 
    ? data => dataProcessors.shiftRequests(data, true)
    : null);
  
  return loadData(endpoint, dataRef, loadingRef, errorRef, processor);
};

// Funções exportadas para compatibilidade com código existente
export const loadStudentData = (studentId, studentData, loading, error) => 
  loadEntityData('student', studentId, studentData, loading, error);

export const loadDirectorData = (directorId, directorData, loading, error) => 
  loadEntityData('director', directorId, directorData, loading, error);

export const loadCourses = (courses, loading, error) => 
  loadEntityData('courses', null, courses, loading, error);

export const loadShifts = (shifts, loading, error) => 
  loadEntityData('shifts', null, shifts, loading, error);

export const loadStudents = (students, loading, error) => 
  loadEntityData('students', null, students, loading, error);

export const loadShiftRequests = (requests, loading, error, pendingOnly = false) => 
  loadEntityData('shiftRequests', null, requests, loading, error, { pendingOnly });

export const loadStudentAllocations = (studentId, allocations, loading, error) => 
  loadEntityData('studentAllocations', studentId, allocations, loading, error);

export const loadStudentConflicts = (studentId, conflicts, loading, error) => 
  loadEntityData('studentConflicts', studentId, conflicts, loading, error);

/**
 * Função otimizada para carregar todos os dados necessários para o dashboard do diretor
 * usando carregamento paralelo
 */
export const loadDirectorDashboardData = async (
  directorId, 
  directorData, 
  courses, 
  shifts, 
  students, 
  pendingRequests, 
  loading, 
  error
) => {
  try {
    console.log('Iniciando carregamento de dados do dashboard do diretor...');
    console.log('Director ID:', directorId.value);
    
    // Inicializar arrays para evitar erros de undefined
    directorData.value = null;
    courses.value = [];
    shifts.value = [];
    students.value = [];
    pendingRequests.value = [];
    
    const dataLoads = [
      { 
        endpoint: `/directors/${directorId.value}`, 
        ref: directorData 
      },
      { 
        endpoint: '/courses', 
        ref: courses,
        processor: data => Array.isArray(data) ? data : []
      },
      { 
        endpoint: '/shifts', 
        ref: shifts,
        processor: data => Array.isArray(data) ? data : []
      },
      { 
        endpoint: '/students', 
        ref: students,
        processor: data => Array.isArray(data) ? data : []
      },
      { 
        endpoint: '/shiftRequests', 
        ref: pendingRequests,
        processor: data => {
          const requestsData = Array.isArray(data) ? data : [];
          return requestsData
            .filter(request => request && request.response === null)
            .sort((a, b) => new Date(b.date) - new Date(a.date));
        }
      }
    ];
    
    const result = await loadMultipleData(dataLoads, loading, error);
    
    if (result) {
      console.log('Carregamento de dados concluído com sucesso!');
      console.log(`Dados do diretor:`, directorData.value);
      console.log(`${courses.value.length} cursos carregados.`);
      console.log(`${shifts.value.length} turnos carregados.`);
      console.log(`${students.value.length} estudantes carregados.`);
      console.log(`${pendingRequests.value.length} pedidos pendentes carregados.`);
    }
    
    return result;
  } catch (err) {
    console.error('Erro geral ao carregar dados do dashboard:', err);
    error.value = 'Erro ao carregar dados. Por favor, verifique se o servidor está rodando e tente novamente.';
    loading.value = false;
    return false;
  }
};

/**
 * Função genérica para encontrar um item em uma coleção por ID
 * @param {Array} collection - Coleção de itens
 * @param {Number} id - ID a procurar
 * @param {String} defaultMsg - Mensagem padrão se não encontrar
 * @param {String} propertyToReturn - Propriedade a retornar (se null, retorna o objeto inteiro)
 * @returns {Any} Item encontrado, propriedade especificada ou mensagem padrão
 */
const findById = (collection, id, defaultMsg = 'Não encontrado', propertyToReturn = null) => {
  if (!collection || !Array.isArray(collection) || !id) return defaultMsg;
  
  const item = collection.find(i => i && i.id === id);
  if (!item) return defaultMsg;
  
  return propertyToReturn ? item[propertyToReturn] : item;
};

/**
 * Função para obter o nome da disciplina
 */
export const getCourseName = (courses, courseId) => 
  findById(courses, courseId, 'Disciplina desconhecida', 'name');

/**
 * Função para obter a abreviação da disciplina
 */
export const getCourseAbbr = (courses, courseId) => 
  findById(courses, courseId, 'N/A', 'abbreviation');

/**
 * Função para obter a cor da disciplina
 */
export const getCourseColor = (courseId) => 
  courseColors[courseId] || '#EEEEEE'; // Cor padrão cinza claro

/**
 * Função para formatar data para exibição amigável
 */
export const formatDate = (dateString) => {
  if (!dateString) return '-';
  
  const date = new Date(dateString);
  const now = new Date();
  const yesterday = new Date();
  yesterday.setDate(yesterday.getDate() - 1);
  
  const formatTime = date => `${date.getHours().toString().padStart(2, '0')}:${date.getMinutes().toString().padStart(2, '0')}`;
  
  if (date.toDateString() === now.toDateString()) {
    return `Hoje, ${formatTime(date)}`;
  }
  
  if (date.toDateString() === yesterday.toDateString()) {
    return `Ontem, ${formatTime(date)}`;
  }
  
  return `${date.getDate().toString().padStart(2, '0')}/${(date.getMonth() + 1).toString().padStart(2, '0')}/${date.getFullYear()}`;
};

/**
 * Mapeamento de status para texto
 */
const requestStatusMap = {
  null: 'Pendente',
  'ok': 'Aprovado',
  'rejected': 'Recusado'
};

/**
 * Função para obter o status do pedido em texto
 */
export const getRequestStatus = (status) => 
  requestStatusMap[status] || 'Desconhecido';

/**
 * Mapeamento de status para classes CSS
 */
const statusClassMap = {
  null: 'status-pending',
  'ok': 'status-approved',
  'rejected': 'status-rejected'
};

/**
 * Função para obter a classe CSS do status do pedido
 */
export const getStatusClass = (status) => 
  statusClassMap[status] || 'status-unknown';

/**
 * Função para obter o nome do aluno
 */
export const getStudentName = (students, studentId) => 
  findById(students, studentId, 'Aluno não encontrado', 'name');

/**
 * Função para obter o nome do turno
 */
export const getShiftName = (shifts, shiftId) => 
  findById(shifts, shiftId, 'Turno não encontrado', 'name');