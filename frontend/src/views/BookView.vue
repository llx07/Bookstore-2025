<template>
    <n-grid x-gap="12" y-gap="12" :cols="2">

        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 1">
            <NCard title="Show">
                <div>Show all books or query books that satisfy some conditions.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showShowModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
    </n-grid>


    <!-- <NModal v-model:show="showXXXModal">
        <n-card closable @close="showXXXModal = false" style="width: 600px" title="XXX" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="xxxFormRef" :model="xxxForm" :rules="xxxRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="xxxForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Username" path="username">
                    <NInput v-model:value="xxxForm.username" maxlength="30" show-count
                        placeholder="Please enter username" />
                </NFormItem>
                <NFormItem label="Password" path="password">
                    <NInput v-model:value="xxxForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showXXXModal = false">取消</NButton>
                    <NButton type="primary" @click="handleXXXSubmit" :disabled="!xxxButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal> -->
</template>
<script setup lang="ts">
import { useLoginStackStore } from '@/stores/loginStack';
import { passwordValidator, usernameValidator, userIDValidator } from '@/stores/validators';
import {
    NCard,
    NGrid,
    NGi,
    NButton,
    NFlex,
    NModal,
    NInput,
    NFormItem,
    NForm,
    type FormInst,
    useMessage,
    NSelect,
    type FormItemRule
} from 'naive-ui';
import { computed, ref } from 'vue';
import axios, { type AxiosResponse, AxiosError } from 'axios';
import { API_BASE_URL } from '@/constants'

const loginStackStore = useLoginStackStore();
const message = useMessage()

const showShowModal = ref(false);
const showButtonAvailable = ref(true)
const showForm = ref({
    userid: '',
    password: '',
    username: ''
});
const showFormRef = ref<FormInst | null>(null);
const showRules = {
    userid: {
        required: true, trigger: 'blur',
        validator: userIDValidator
    },
    username: {
        required: true, trigger: 'blur',
        validator: usernameValidator
    },
    password: {
        required: true, trigger: 'blur',
        validator: passwordValidator
    },
};
const handleShowSubmit = () => {
    console.log("handle show")
    .value?.validate(async (errors) => {
        showButtonAvailable.value = false;
         console.log("handle 1")
        if (!errors) {
            try {
                const headers: { [key: string]: string } = {
                    'Content-Type': 'application/json',
                };
                const token = loginStackStore.getCurrentToken();
                if (token) {
                    headers["Authorization"] = `Bearer ${token}`;
                }
                const userid = showForm.value.userid;
                const password = showForm.value.password;
                const username = showForm.value.username;
                const data: { [key: string]: string } = {
                    userid: userid,
                    password: password,
                    username: username
                };

                await axios.post(
                    API_BASE_URL + '/api/v1/users/?????????',
                    data,
                    { headers: headers });

                message.success("Show success.");
                showShowModal.value = false;
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
        showButtonAvailable.value = true;
    })
}
// const showXXXModal = ref(false);
// const xxxButtonAvailable = ref(true)
// const xxxForm = ref({
//     userid: '',
//     password: '',
//     username: ''
// });
// const xxxFormRef = ref<FormInst | null>(null);
// const xxxRules = {
//     userid: {
//         required: true, trigger: 'blur',
//         validator: userIDValidator
//     },
//     username: {
//         required: true, trigger: 'blur',
//         validator: usernameValidator
//     },
//     password: {
//         required: true, trigger: 'blur',
//         validator: passwordValidator
//     },
// };
// const handleXXXSubmit = () => {
//     console.log("handle xxx")
//     .value?.validate(async (errors) => {
//         xxxButtonAvailable.value = false;
//          console.log("handle 1")
//         if (!errors) {
//             try {
//                 const headers: { [key: string]: string } = {
//                     'Content-Type': 'application/json',
//                 };
//                 const token = loginStackStore.getCurrentToken();
//                 if (token) {
//                     headers["Authorization"] = `Bearer ${token}`;
//                 }
//                 const userid = xxxForm.value.userid;
//                 const password = xxxForm.value.password;
//                 const username = xxxForm.value.username;
//                 const data: { [key: string]: string } = {
//                     userid: userid,
//                     password: password,
//                     username: username
//                 };

//                 await axios.post(
//                     API_BASE_URL + '/api/v1/users/?????????',
//                     data,
//                     { headers: headers });

//                 message.success("XXX success.");
//                 showXXXModal.value = false;
//             } catch (error) {
//                 if (axios.isAxiosError(error)) {
//                     console.log(error)
//                     if (error.status === 401) {
//                         message.error("Token unavailable. Logging out current account.");
//                         loginStackStore.logout();
//                     }
//                     if (error.status === 400) {
//                         message.error('Failed   ' + error.response?.data.message);
//                     }
//                 }
//                 else {
//                     message.error('Unknown error: ' + error);
//                 }
//             }
//         }
//         else {
//             message.error("Invalid Input.")
//         }
//         xxxButtonAvailable.value = true;
//     })
// }
</script>

<style scoped></style>