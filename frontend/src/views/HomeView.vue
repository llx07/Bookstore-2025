<template>
    <div class="hero-section">
      <n-gradient-text type="primary" :size="64" weight="800">
        {{ formatTime(currentTime) }}
      </n-gradient-text>
      <div class="date-text">
        {{ formatDate(currentTime) }}
      </div>

      <n-card class="user-info-card" :bordered="false">
        <n-flex vertical style='align-items:"center"'>
          <n-text depth="1" style="font-size: 20px; font-weight: 600">
            Welcome Back, {{ loginStackStore.getCurrentUser().username }}
          </n-text>
          <n-text depth="3">
            User ID: <n-tag size="small" round type="info">{{ loginStackStore.getCurrentUser().userid }}</n-tag>
          </n-text>
        </n-flex>
      </n-card>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { NGradientText, NFlex, NText, NCard, NTag } from 'naive-ui'
import { useLoginStackStore } from '../stores/loginStack';
const currentTime = ref(new Date())
let timer = null
const loginStackStore = useLoginStackStore();

setInterval(() => {
  currentTime.value = new Date();
}, 1000);

const formatTime = (date: Date) => {
  return date.toLocaleTimeString('en-US', { hour12: false })
}

const formatDate = (date: Date) => {
  return date.toLocaleDateString('en-US', { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' })
}
</script>

<style lang="css" scoped>
 .hero-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 60px 0;
  text-align: center;
  background: white;
  border-radius: 16px;
  box-shadow: 0 4px 12px rgba(0,0,0,0.05);
}

.date-text {
  font-size: 18px;
  color: #666;
  margin-top: -10px;
  margin-bottom: 30px;
}
</style>