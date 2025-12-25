import {createRouter, createWebHistory} from 'vue-router'
import HomeView from '../views/HomeView.vue'
import UserView from '@/views/UserView.vue'
import BookView from '@/views/BookView.vue'
import LogView from '@/views/LogView.vue'

const router = createRouter({
    history: createWebHistory(import.meta.env.BASE_URL),
    routes: [
        {
            path: '/',
            name: 'home',
            component: HomeView,
        },
        {
            path: '/user',
            name: 'user',
            component: UserView
        },
        {
            path: '/book',
            name: 'book',
            component: BookView
        },
        {
            path: '/log',
            name: 'log',
            component: LogView
        }
    ],
})

export default router
