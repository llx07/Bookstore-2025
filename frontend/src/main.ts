import {createApp} from 'vue'
import App from './App.vue'
import router from './router'
import { createPinia } from 'pinia'
import { useLoginStackStore } from './stores/loginStack';
const app = createApp(App)

app.use(router)
const pinia = createPinia();
app.use(pinia)

const loginStackStore = useLoginStackStore();
loginStackStore.loadFromLocalStorage();
loginStackStore.initGuest();

app.mount('#app')
