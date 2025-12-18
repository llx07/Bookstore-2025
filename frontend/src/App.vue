<template>
  <n-message-provider>
    <header class="app-header">
      <!-- 导航链接 -->
      <div class="app-title">
        Bookstore Management System
      </div>

      <UserIndicator />
    </header>
    <NLayout has-sider position="absolute" class="main-layout" >
      <NLayoutSider collapse-mode="width" :collapsed-width="55" :width="200"
        show-trigger="arrow-circle" content-style="background-color:#f3f4f6" class="layout-sider">
        <n-menu :options="menuOptions" />
      </NLayoutSider>
      <NLayoutContent>
        <main>
          <router-view />
        </main>
      </NLayoutContent>
    </NLayout>
  </n-message-provider>
</template>

<script setup lang="ts">
import {
  useMessage,
  NMessageProvider,
  NMenu,
  NIcon,
  NLayout,
  NLayoutSider,
  NLayoutContent
} from 'naive-ui'
import UserIndicator from './components/UserIndicator.vue';
import { h } from 'vue'
import type { Component } from 'vue'
import type { MenuOption } from 'naive-ui'
import { UserCog, Home } from "@vicons/fa"

import { RouterLink } from 'vue-router'
function renderIcon(icon: Component) {
  return () => h(NIcon, null, { default: () => h(icon) })
}
const menuOptions: MenuOption[] = [
  {
    label: () => h(RouterLink, { to: { name: 'home' } }, { default: () => 'Home' }),
    key: 'home',
    icon: renderIcon(Home)
  },
  {
    label: () => h(RouterLink, { to: { name: 'user' } }, { default: () => 'User Managemnt' }),
    key: 'user',
    icon: renderIcon(UserCog)
  }
]
</script>

<style scoped>
.app-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0 16px;
  height: 60px;
  background-color: #1f1f1f;
  color: #ffffff;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
}

.app-title {
  font-size: 18px;
  font-weight: bold;
}

.main-layout{
  margin-top: 60px;
  width: 100%;
  height: 100%;
}
</style>
