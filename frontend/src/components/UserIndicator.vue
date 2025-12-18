<template>
    <NSpace :size="0">
        <div class="user-indicator">
            <NDropdown :options="dropdownOptions" trigger="hover">
                <div>
                    <NIcon class="user-indicator-icon">
                        <User />
                    </NIcon>
                    <span class="current-username">{{ currentUser?.username }}</span>
                    <span class="current-privilege" :class="getPermissionColor(currentUser!.privilege)">{{
                        currentUser?.privilege }}</span>
                    <NIcon class="user-indicator-icon user-indicator-down-icon">
                        <AngleDown />
                    </NIcon>
                </div>
            </NDropdown>
        </div>

        <n-tooltip trigger="hover">
            <template #trigger>
                <NButton circle strong secondary class="log-button" @click="handleLoginBtn">
                    <template #icon>
                        <n-icon>
                            <LogInOutline />
                        </n-icon>
                    </template>
                </NButton>
            </template>
            Login
        </n-tooltip>

        <n-tooltip trigger="hover">
            <template #trigger>
                <NButton circle strong secondary class="log-button" @click="handleLogoutBtn" :disabled="isGuestUser">
                    <template #icon>
                        <n-icon>
                            <LogOutOutline />
                        </n-icon>
                    </template>
                </NButton>
            </template>
            Logout
        </n-tooltip>
    </NSpace>

    <NModal v-model:show="showLoginModal">
        <n-card closable @close="showLoginModal = false" style="width: 600px" title="Login" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="loginFormRef" :model="loginForm" :rules="rules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="loginForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>

                <NFormItem label="Password" path="password">
                    <NInput v-model:value="loginForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #footer>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showLoginModal = false">取消</NButton>
                    <NButton type="primary" @click="handleLoginSubmit" :disabled="!loginBtnAvailable">确定</NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>
</template>

<script setup lang="ts">
import type { Component } from 'vue'
import { User, AngleDown } from '@vicons/fa'
import { LogInOutline, LogOut, LogOutOutline } from '@vicons/ionicons5'
import { ref, computed } from 'vue';
import {
    NDropdown,
    NButton,
    NIcon,
    NSpace,
    NTooltip,
    NModal,
    NCard,
    NForm,
    NFormItem,
    NInput,
    NFlex,
    useMessage,
    type FormInst
} from 'naive-ui';
import { useLoginStackStore } from '../stores/loginStack';
import { userIDValidator, passwordValidator } from '../stores/validators';
import axios, { type AxiosResponse, AxiosError } from 'axios';
import { API_BASE_URL } from '@/constants'

const loginBtnAvailable = ref(true)

const loginStackStore = useLoginStackStore();
const loginForm = ref({
    userid: '',
    password: '',
});

const message = useMessage()
const loginFormRef = ref<FormInst | null>(null);
const rules = {
    userid: {
        required: true, trigger: 'blur',
        validator: userIDValidator
    },
    password: {
        required: false, trigger: 'blur', validator: passwordValidator
    },
};
async function handleLoginSubmit() {
    loginFormRef.value?.validate(async (errors) => {
        loginBtnAvailable.value = false;
        if (!errors) {
            try {
                const headers: { [key: string]: string } = {
                    'Content-Type': 'application/json',
                };
                const token = loginStackStore.getCurrentToken();
                if (token) {
                    headers["Authorization"] = `Bearer ${token}`;
                }


                const userid = loginForm.value.userid;
                const password = loginForm.value.password;
                const data: { [key: string]: string } = {
                    userid: userid
                };
                if (password) data['password'] = password;

                const response: AxiosResponse = await axios.post(
                    API_BASE_URL + '/api/v1/auth/login',
                    data,
                    { headers: headers });

                const result = response.data;

                console.log('Success! Data received:', result);
                message.success("Login success.");

                loginStackStore.login({
                    userid: userid,
                    username: result.username,
                    privilege: result.privilege,
                    selected_id: 0,
                    token: result.access_token
                });

                showLoginModal.value = false;
            } catch (error) {
                if (axios.isAxiosError(error)) {
                    console.log(error)
                    if (error.status === 401) {
                        message.error("Token unavailable. Logging out current account.");
                        loginStackStore.logout();
                    }
                    if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                }
                else {
                    message.error('Unknown error: ' + error);
                }
            }
        }
        else {
            message.error("Invalid Input.")
        }
        loginBtnAvailable.value = true;
    })
}

const currentUser = computed(() => loginStackStore.getCurrentUser());
const isGuestUser = computed(() => currentUser.value?.userid === "<GUEST>")

const dropdownOptions = computed(() => {
    const users = [
        { label: 'Login stack:', key: 'text-current-stack', disabled: true },
        ...loginStackStore.stack
            .filter((user) => user.userid !== "<GUEST>")
            .map(
                (user) => {
                    return {
                        label: `${user.username}{${user.privilege}}`,
                        key: user.userid,
                        disabled: true
                    };
                }
            )
    ];
    return users;
});

const showLoginModal = ref(false);

const getPermissionColor = (permission: number) => {
    switch (permission) {
        case 7:
            return "permission-orange";
        case 3:
            return "permission-blue";
        case 1:
            return "permission-green";
        case 0:
            return "permission-gray";
    }
};
const handleLoginBtn = () => {
    // loginStackStore.login({ userid: "123", username: "456" + Date.now().toLocaleString(), privilege: 7, token: "123", selected_id: 0 });
    showLoginModal.value = true;
}
const handleLogoutBtn = async () => {
    // TODO(llx) add logout request here.
    loginStackStore.logout();
    console.log(currentUser.value?.userid, isGuestUser.value);
    const headers: { [key: string]: string } = {
        'Content-Type': 'application/json',
    };
    const token = loginStackStore.getCurrentToken();
    if (token) {
        headers["Authorization"] = `Bearer ${token}`;
    }
    try {
        await axios.post(
            API_BASE_URL + '/api/v1/auth/logout',
            {},
            { headers: headers });
    } catch (error) {

    }
}
</script>


<style scoped>
.user-indicator *:not(.current-privilege) {
    color: #ffffff;
}

.user-indicator:hover *:not(.current-privilege) {
    color: #ddd
}

.user-indicator-icon {
    transition: transform 0.5s ease;
    /* 平滑旋转动画 */
    font-size: 20px;
    vertical-align: bottom;
    padding-bottom: 0.1em;
}

.user-indicator:hover .user-indicator-down-icon {
    transform: rotate(180deg);
    /* 悬停时旋转 180° */
}

.user-indicator:hover {
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    /* 悬停时添加一点阴影 */
    cursor: pointer;
}

.user-indicator {
    font-size: 18px;
}

.log-button {
    color: #ffffff
}

.log-button:hover,
.log-button:focus {
    color: #ddd
}

.current-username {
    color: #ffffff;
    margin: 0 0.5em;
}


.user-indicator .current-privilege {
    border-radius: 3px;
    font-size: 16px;
    padding: 0.2em 0.5em;
    margin-right: 0.5em;
    font-weight: bold;
}

.permission-orange {
    background-color: #ffa500;
    /* 橙色 */
    color: #5a3000;
    /* 深棕色 */
}

.permission-blue {
    background-color: #007bff;
    /* 蓝色 */
    color: #ffffff;
    /* 白色 */
}

.permission-green {
    background-color: #28a745;
    /* 绿色 */
    color: #ffffff;
    /* 白色 */
}

.permission-gray {
    background-color: #6c757d;
    /* 灰色 */
    color: #ffffff;
    /* 白色 */
}
</style>