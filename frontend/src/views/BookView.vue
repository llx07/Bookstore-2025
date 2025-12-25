<template>
    <NText>Selected Book ID:
        <NTag round type="info">
            {{ loginStackStore.getCurrentUser().selected_id }}
        </NTag>
    </NText>
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
    <NModal v-model:show="showShowModal">
        <n-card closable @close="showShowModal = false" style="width: 600px" title="Show" :bordered="false" size="huge"
            role="dialog" aria-modal="true">

            <NFormItem label="Type:" path="type">
                <NSelect v-model:value="showSelectValue" :options="showSelecetOptions"></NSelect>
            </NFormItem>
            <NForm ref="showFormRef" :model="showForm" :rules="showRules">

                <NFormItem label="ISBN:" path="isbn" v-if="showSelectValue === 'isbn'">
                    <NInput v-model:value="showForm.isbn" maxlength="20" show-count placeholder="Please enter ISBN" />
                </NFormItem>
                <NFormItem label="Book name:" path="name" v-if="showSelectValue === 'name'">
                    <NInput v-model:value="showForm.name" maxlength="30" show-count
                        placeholder="Please enter book name" />
                </NFormItem>
                <NFormItem label="Author:" path="author" v-if="showSelectValue === 'author'">
                    <NInput v-model:value="showForm.author" maxlength="30" show-count
                        placeholder="Please enter author name" />
                </NFormItem>
                <NFormItem label="Keyword:" path="keyword" v-if="showSelectValue === 'keyword'">
                    <NInput v-model:value="showForm.keyword" maxlength="30" show-count
                        placeholder="Please enter keyword (only one is allowed)" />
                </NFormItem>
                <NDataTable :columns="showColumns" :data="showResults" >

                </NDataTable>
                <NFlex justify="end">
                    <NButton type="primary" @click="handleQuery">Query</NButton>
                </NFlex>
            </NForm>
        </n-card>
    </NModal>
    <!-- <NModal v-model:show="showShowModal">
        <n-card closable @close="showShowModal = false" style="width: 600px" title="Show" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="showFormRef" :model="showForm" :rules="showRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="showForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Username" path="username">
                    <NInput v-model:value="showForm.username" maxlength="30" show-count
                        placeholder="Please enter username" />
                </NFormItem>
                <NFormItem label="Password" path="password">
                    <NInput v-model:value="showForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showShowModal = false">取消</NButton>
                    <NButton type="primary" @click="handleShowSubmit" :disabled="!showButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal> -->

    <!-- <NModal v-model:show="showShowModal">
        <n-card closable @close="showShowModal = false" style="width: 600px" title="Show" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="showFormRef" :model="showForm" :rules="showRules">

                <NFormItem label="User ID:" path="userid">
                    <NInput v-model:value="showForm.userid" maxlength="30" show-count
                        placeholder="Please enter user ID" />
                </NFormItem>
                <NFormItem label="Username" path="username">
                    <NInput v-model:value="showForm.username" maxlength="30" show-count
                        placeholder="Please enter username" />
                </NFormItem>
                <NFormItem label="Password" path="password">
                    <NInput v-model:value="showForm.password" maxlength="30" show-count type="password"
                        placeholder="Please enter password" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showShowModal = false">取消</NButton>
                    <NButton type="primary" @click="handleShowSubmit" :disabled="!showButtonAvailable">确定
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal> -->
</template>
<script setup lang="ts">
import { useLoginStackStore } from '@/stores/loginStack';
import {
    passwordValidator,
    usernameValidator,
    userIDValidator,
    ISBNValidator,
    bookNameValidator,
    authorValidator,
    keywordValidator
} from '@/stores/validators';
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
    NText,
    NTag,
    type FormInst,
    useMessage,
    NSelect,
    type FormItemRule,
    NDataTable
} from 'naive-ui';
import { computed, reactive, ref } from 'vue';
import axios, { type AxiosResponse, AxiosError } from 'axios';
import { API_BASE_URL } from '@/constants'

const loginStackStore = useLoginStackStore();
const message = useMessage()

const showColumns = [
    {
      title: 'ISBN',
      key: 'isbn'
    },
    {
      title: 'Book Name',
      key: 'name'
    },
    {
      title: 'Author',
      key: 'author'
    },
    {
      title: 'Keywords',
      key: 'keyword'
    },
    {
      title: 'Price',
      key: 'price'
    },
    {
      title: 'Quantity',
      key: 'quantity'
    }
]
const showSelectValue = ref("None");
const showSelecetOptions = [
    {
        label: 'None',
        value: 'None',
    },
    {
        label: 'ISBN',
        value: 'isbn',
    },
    {
        label: 'Book name',
        value: 'name',
    },
    {
        label: 'Author',
        value: 'author',
    },
    {
        label: 'Keyword',
        value: 'keyword',
    },
]
const showResults = ref([]);
const showShowModal = ref(false);
const showButtonAvailable = ref(true)
const showForm = ref({
    isbn: '',
    name: '',
    author: '',
    keyword: '',
});
const showFormRef = ref<FormInst | null>(null);
const showRules = {
    isbn: {
        required: true, trigger: 'blur',
        validator: ISBNValidator
    },
    name: {
        required: true, trigger: 'blur',
        validator: bookNameValidator
    },
    author: {
        required: true, trigger: 'blur',
        validator: authorValidator
    },
    keyword: {
        required: true, trigger: 'blur',
        validator: keywordValidator
    },
};
const handleQuery = () => {
    console.log("handle show")
    showFormRef.value?.validate(async (errors) => {
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

                const params: {[key: string] : string} = {};
                console.log(showSelectValue.value)
                if(showSelectValue.value=="isbn"){
                    params["isbn"] = showForm.value.isbn;
                }
                if(showSelectValue.value=="name"){
                    params["name"] = showForm.value.name;
                }
                if(showSelectValue.value=="author"){
                    params["author"] = showForm.value.author;
                }
                if(showSelectValue.value=="keyword"){
                    params["keyword"] = showForm.value.keyword;
                }
                console.log(params);
                const response = await axios.get(
                    API_BASE_URL + '/api/v1/books',
                    { params:params, headers: headers });

                message.success("Show success.");

                showResults.value = response.data;
                console.log("data", response.data);
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
// const showShowModal = ref(false);
// const showButtonAvailable = ref(true)
// const showForm = ref({
//     userid: '',
//     password: '',
//     username: ''
// });
// const showFormRef = ref<FormInst | null>(null);
// const showRules = {
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
// const handleShowSubmit = () => {
//     console.log("handle show")
//     .value?.validate(async (errors) => {
//         showButtonAvailable.value = false;
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
//                 const userid = showForm.value.userid;
//                 const password = showForm.value.password;
//                 const username = showForm.value.username;
//                 const data: { [key: string]: string } = {
//                     userid: userid,
//                     password: password,
//                     username: username
//                 };

//                 await axios.post(
//                     API_BASE_URL + '/api/v1/users/?????????',
//                     data,
//                     { headers: headers });

//                 message.success("Show success.");
//                 showShowModal.value = false;
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
//         showButtonAvailable.value = true;
//     })
// }
</script>

<style scoped></style>