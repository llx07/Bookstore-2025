<template>
    <n-grid x-gap="12" y-gap="12" :cols="2">

        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 0">
            <NCard title="Register">
                <div>Register a new user.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showRegisterModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>

        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 1">
            <NCard title="Change Password">

                <div>Change the password of a user.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showChangePasswordModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>

        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 3">
            <NCard title="Add New User">

                <div>Add a new user with custome privilege.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showAddUserModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 7">
            <NCard title="Delete User">

                <div>Delete a user.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showDeleteModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
    </n-grid>

    <NModal v-model:show="showRegisterModal">
        <n-card closable @close="showRegisterModal = false" style="width: 600px" title="Register" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="registerFormRef" :model="registerForm" :rules="registerRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="registerForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Username:" path="username">
                    <NInput v-model:value="registerForm.username" maxlength="30" show-count
                        placeholder="Please enter username" />
                </NFormItem>
                <NFormItem label="Password:" path="password">
                    <NInput v-model:value="registerForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showRegisterModal = false">取消</NButton>
                    <NButton type="primary" @click="handleRegisterSubmit" :disabled="!registerButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

    <NModal v-model:show="showChangePasswordModal">
        <n-card closable @close="showChangePasswordModal = false" style="width: 600px" title="ChangePassword"
            :bordered="false" size="huge" role="dialog" aria-modal="true">
            <NForm ref="changePasswordFormRef" :model="changePasswordForm" :rules="changePasswordRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="changePasswordForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Current Password:" path="cuurent_password">
                    <NInput v-model:value="changePasswordForm.current_password" maxlength="30" show-count
                        type="password" placeholder="Please enter current password" />
                </NFormItem>
                <NFormItem label="New Password:" path="new_password">
                    <NInput v-model:value="changePasswordForm.new_password" maxlength="30" show-count type="password"
                        placeholder="Please enter new password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showChangePasswordModal = false">取消</NButton>
                    <NButton type="primary" @click="handleChangePasswordSubmit"
                        :disabled="!changePasswordButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

    <NModal v-model:show="showAddUserModal">
        <n-card closable @close="showAddUserModal = false" style="width: 600px" title="AddUser" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="addUserFormRef" :model="addUserForm" :rules="addUserRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="addUserForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Username:" path="username">
                    <NInput v-model:value="addUserForm.username" maxlength="30" show-count
                        placeholder="Please enter username" />
                </NFormItem>
                <NFormItem label="Privilege:" path="privilege">
                    <NSelect v-model:value="addUserForm.privilege" :options="PRIVILEGE_OPTIONS" />
                </NFormItem>
                <NFormItem label="Password:" path="password">
                    <NInput v-model:value="addUserForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showAddUserModal = false">取消</NButton>
                    <NButton type="primary" @click="handleAddUserSubmit" :disabled="!addUserButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>
    <NModal v-model:show="showDeleteModal">
        <n-card closable @close="showDeleteModal = false" style="width: 600px" title="Delete" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="deleteFormRef" :model="deleteForm" :rules="deleteRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="deleteForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showDeleteModal = false">取消</NButton>
                    <NButton type="primary" @click="handleDeleteSubmit" :disabled="!deleteButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

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

const showRegisterModal = ref(false);
const registerButtonAvailable = ref(true)
const registerForm = ref({
    userid: '',
    password: '',
    username: ''
});
const registerFormRef = ref<FormInst | null>(null);
const registerRules = {
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
const handleRegisterSubmit = () => {
    console.log("handle register")
    registerFormRef.value?.validate(async (errors) => {
        registerButtonAvailable.value = false;
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
                const userid = registerForm.value.userid;
                const password = registerForm.value.password;
                const username = registerForm.value.username;
                const data: { [key: string]: string } = {
                    userid: userid,
                    password: password,
                    username: username
                };

                await axios.post(
                    API_BASE_URL + '/api/v1/users',
                    data,
                    { headers: headers });

                message.success("Register success.");
                showRegisterModal.value = false;
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
        registerButtonAvailable.value = true;
    })
}

const PRIVILEGE_OPTIONS = computed(() => {
    const data = [

        {
            label: 'Guest (0)',
            value: 0
        },
        {
            label: 'Customer (1)',
            value: 1
        },
        {
            label: 'Worker (3)',
            value: 3,
            disabled: loginStackStore.getCurrentUser().privilege <= 3,
        },
        {
            label: 'Manager (7)',
            value: 7,
            disabled: loginStackStore.getCurrentUser().privilege <= 7,
        }];
    return data;
}
);


const showChangePasswordModal = ref(false);
const changePasswordButtonAvailable = ref(true)
const changePasswordForm = ref({
    userid: '',
    current_password: '',
    new_password: '',
});
const changePasswordFormRef = ref<FormInst | null>(null);
const changePasswordRules = {
    userid: {
        required: true, trigger: 'blur',
        validator: userIDValidator
    },
    current_password: {
        required: false, trigger: 'blur',
        validator: passwordValidator
    },
    new_password: {
        required: true, trigger: 'blur',
        validator: passwordValidator
    },
};
const handleChangePasswordSubmit = () => {
    console.log("handle changePassword")
    changePasswordFormRef.value?.validate(async (errors) => {
        changePasswordButtonAvailable.value = false;
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
                const userid = changePasswordForm.value.userid;
                const current_password = changePasswordForm.value.current_password;
                const new_password = changePasswordForm.value.new_password;
                const data: { [key: string]: string } = {
                    new_password: new_password
                };
                if (current_password) {
                    data["current_password"] = current_password;
                }

                await axios.patch(
                    API_BASE_URL + `/api/v1/users/${userid}/password`,
                    data,
                    { headers: headers });

                message.success("ChangePassword success.");
                showChangePasswordModal.value = false;
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
        changePasswordButtonAvailable.value = true;
    })
}


const showAddUserModal = ref(false);
const addUserButtonAvailable = ref(true)
const addUserForm = ref({
    userid: '',
    password: '',
    username: '',
    privilege: 0,
});
const addUserFormRef = ref<FormInst | null>(null);
const addUserRules = {
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
    privilege:{
        required: true, trigger: 'blur',
        validator: (rule: FormItemRule, value: any): boolean | Error =>{
            if(!value){
                return Error("Field cannot be empty");
            }
            if(value >= loginStackStore.getCurrentUser().privilege){
                return Error("Cannot add user with same or higher privilege");
            }
            return true;
        }
    }
};
const handleAddUserSubmit = () => {
    console.log("handle addUser")
    addUserFormRef.value?.validate(async (errors) => {
        addUserButtonAvailable.value = false;
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
                const userid = addUserForm.value.userid;
                const password = addUserForm.value.password;
                const username = addUserForm.value.username;
                const privilege = addUserForm.value.privilege;
                const data: { [key: string]: string | number } = {
                    userid: userid,
                    password: password,
                    username: username,
                    privilege: privilege,
                };

                await axios.post(
                    API_BASE_URL + '/api/v1/users',
                    data,
                    { headers: headers });

                message.success("AddUser success.");
                showAddUserModal.value = false;
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
        addUserButtonAvailable.value = true;
    })
}

const showDeleteModal = ref(false);
const deleteButtonAvailable = ref(true)
const deleteForm = ref({
    userid: '',
});
const deleteFormRef = ref<FormInst | null>(null);
const deleteRules = {
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
const handleDeleteSubmit = () => {
    console.log("handle delete")
    deleteFormRef.value?.validate(async (errors) => {
        deleteButtonAvailable.value = false;
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
                const userid = deleteForm.value.userid;

                await axios.delete(
                    API_BASE_URL + `/api/v1/users/${userid}`,
                    { headers: headers });

                message.success("Delete success.");
                showDeleteModal.value = false;
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
        deleteButtonAvailable.value = true;
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