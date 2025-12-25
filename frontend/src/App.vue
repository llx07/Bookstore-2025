<template>
  <n-message-provider>
    <NLayout has-sider position="absolute" class="main-layout">
      <n-layout-header :inverted="true" bordered style="height: 64px" position="absolute">
        <n-flex justify="space-between" style="height: 100%; align-items: center; padding: 0 1em;">
            <div class="app-title">
              Bookstore Management System
            </div>
            <UserIndicator />
        </n-flex>

        <!-- </header> -->
      </n-layout-header>
      <NLayoutSider collapse-mode="width" :collapsed-width="55" :width="240" bordered show-trigger="arrow-circle"
        content-style="background-color:#f3f4f6" class="layout-sider" style="margin-top: 64px;">
        <n-menu :options="menuOptions" />
      </NLayoutSider>
      <NLayoutContent style="margin-top: 64px">
        <main class="main">
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
  NLayoutHeader,
  NLayoutContent,
  NFlex
} from 'naive-ui'
import UserIndicator from './components/UserIndicator.vue';
import { h } from 'vue'
import type { Component } from 'vue'
import type { MenuOption } from 'naive-ui'
import { UserCog, Home, Book } from "@vicons/fa"

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
    label: () => h(RouterLink, { to: { name: 'user' } }, { default: () => 'Users Managemnt' }),
    key: 'user',
    icon: renderIcon(UserCog)
  },
  {
    label: () => h(RouterLink, { to: { name: 'book' } }, { default: () => 'Books Managemnt' }),
    key: 'book',
    icon: renderIcon(Book)
  },
]
</script>

<style scoped>
/* .app-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0 16px;
  height: 60px;
  background-color: #1f1f1f;
  color: #ffffff;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
} */

.app-title {
  font-size: 18px;
  font-weight: bold;
}

.main-layout {
  width: 100%;
  height: 100%;
}

.main {
  padding: 1em;
}
</style>
