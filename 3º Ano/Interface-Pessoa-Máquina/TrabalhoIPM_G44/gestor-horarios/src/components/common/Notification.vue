<script setup>
import { onMounted, onUnmounted, watch, ref } from 'vue';

const props = defineProps({
  show: {
    type: Boolean,
    default: false
  },
  message: {
    type: String,
    default: ''
  },
  type: {
    type: String,
    default: 'info', // 'info', 'success', 'error', 'warning'
    validator: (value) => ['info', 'success', 'error', 'warning'].includes(value)
  },
  autoClose: {
    type: Boolean,
    default: true
  },
  duration: {
    type: Number,
    default: 3000
  }
});

const emit = defineEmits(['close']);

const localShow = ref(props.show);
let timer = null;

const startAutoCloseTimer = () => {
  if (props.autoClose && !timer) {
    timer = setTimeout(() => {
      localShow.value = false;
      emit('close');
    }, props.duration);
  }
};

const clearAutoCloseTimer = () => {
  if (timer) {
    clearTimeout(timer);
    timer = null;
  }
};

const getTypeClass = () => {
  return {
    'info': 'notification-info',
    'success': 'notification-success',
    'error': 'notification-error',
    'warning': 'notification-warning'
  }[props.type] || 'notification-info';
};

const handleClose = () => {
  localShow.value = false;
  emit('close');
};

watch(() => props.show, (newValue) => {
  localShow.value = newValue;
  if (newValue) {
    startAutoCloseTimer();
  } else {
    clearAutoCloseTimer();
  }
});

onMounted(() => {
  if (props.show) {
    startAutoCloseTimer();
  }
});

onUnmounted(() => {
  clearAutoCloseTimer();
});
</script>

<template>
  <transition name="notification-fade">
    <div v-if="localShow" :class="['notification', getTypeClass()]">
      <div class="notification-content">
        <p>{{ message }}</p>
        <button v-if="!autoClose" class="notification-close" @click="handleClose">×</button>
      </div>
    </div>
  </transition>
</template>

<style scoped>
.notification {
  position: fixed;
  top: 20px;
  left: 50%;
  transform: translateX(-50%);
  padding: 15px 25px;
  border-radius: 25px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
  z-index: 1000;
  display: flex;
  align-items: center;
  justify-content: space-between;
  animation: fadeIn 0.3s ease-in;
}

.notification-content {
  text-align: center;
  display: flex;
  align-items: center;
}

.notification-close {
  background: none;
  border: none;
  font-size: 18px;
  cursor: pointer;
  margin-left: 10px;
  padding: 0 5px;
  line-height: 1;
}

.notification-info {
  background-color: rgba(95, 188, 208, 0.9);
  color: #14606e;
}

.notification-success {
  background-color: rgba(95, 208, 104, 0.9);
  color: #145e1a;
}

.notification-error {
  background-color: rgba(208, 95, 95, 0.9);
  color: #6e1414;
}

.notification-warning {
  background-color: rgba(208, 188, 95, 0.9);
  color: #7b6214;
}

.notification-fade-enter-active,
.notification-fade-leave-active {
  transition: opacity 0.3s, transform 0.3s;
}

.notification-fade-enter-from {
  opacity: 0;
  transform: translate(-50%, -20px);
}

.notification-fade-leave-to {
  opacity: 0;
  transform: translate(-50%, -20px);
}

@keyframes fadeIn {
  from { opacity: 0; transform: translate(-50%, -20px); }
  to { opacity: 1; transform: translate(-50%, 0); }
}
</style>