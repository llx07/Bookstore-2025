<template>
    <NText>Selected Book ID:
        <NTag round type="info">
            {{
                loginStackStore.getCurrentUser().selected_id == 0 ?
                    "None" :
                    loginStackStore.getCurrentUser().selected_id
            }}
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
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 1">
            <NCard title="Buy">
                <div>Buy a book.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showBuyModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 3">
            <NCard title="Select">
                <div>Select a book.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showSelectModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 3">
            <NCard title="Modify">
                <div>Modify the data of the selected book.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showModifyModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 3">
            <NCard title="Import">
                <div>Import the selected book.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showImportModal = true">Execute</NButton>
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
                <NDataTable :columns="showColumns" :data="showResults">

                </NDataTable>
                <NFlex justify="end">
                    <NButton type="primary" @click="handleQuery">Query</NButton>
                </NFlex>
            </NForm>
        </n-card>
    </NModal>


    <NModal v-model:show="showSelectModal">
        <n-card closable @close="showSelectModal = false" style="width: 600px" title="Select" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="selectFormRef" :model="selectForm" :rules="selectRules">
                <NFormItem label="ISBN:" path="isbn">
                    <NInput v-model:value="selectForm.isbn" maxlength="20" show-count placeholder="Please enter ISBN" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showSelectModal = false">Cancel</NButton>
                    <NButton type="primary" @click="handleSelectSubmit" :disabled="!selectButtonAvailable">Submit
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>


    <NModal v-model:show="showBuyModal">
        <n-card closable @close="showBuyModal = false" style="width: 600px" title="Buy" :bordered="false" size="huge"
            role="dialog" aria-modal="true">
            <NForm ref="buyFormRef" :model="buyForm" :rules="buyRules">

                <NFormItem label="ISBN:" path="isbn">
                    <NInput v-model:value="buyForm.isbn" maxlength="20" show-count placeholder="Please enter ISBN" />
                </NFormItem>
                <NFormItem label="Quantity:" path="quantity">
                    <NInputNumber v-model:value="buyForm.quantity" :min="1" :max="2147483647"
                        placeholder="Please enter quantity" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showBuyModal = false">Cancel</NButton>
                    <NButton type="primary" @click="handleBuySubmit" :disabled="!buyButtonAvailable">Submit
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>


    <NModal v-model:show="showModifyModal">
        <n-card closable @close="showModifyModal = false" style="width: 600px" title="Modify" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="modifyFormRef" :model="modifyForm" :rules="modifyRules">


                <NFormItem label="ISBN:" path="isbn">
                    <NInput v-model:value="modifyForm.isbn" maxlength="20" show-count placeholder="Please enter ISBN" />
                </NFormItem>
                <NFormItem label="Book name:" path="name">
                    <NInput v-model:value="modifyForm.name" maxlength="30" show-count
                        placeholder="Please enter book name" />
                </NFormItem>
                <NFormItem label="Author:" path="author">
                    <NInput v-model:value="modifyForm.author" maxlength="30" show-count
                        placeholder="Please enter author name" />
                </NFormItem>
                <NFormItem label="Keyword:" path="keyword">
                    <NInput v-model:value="modifyForm.keyword" maxlength="30" show-count
                        placeholder="Please enter keywords (separated with '|')" />
                </NFormItem>
                <NFormItem label="Price:" path="price">
                    <NInputNumber v-model:value="modifyForm.price" :precision="2" placeholder="Please enter price" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showModifyModal = false">Cancel</NButton>
                    <NButton type="primary" @click="handleModifySubmit" :disabled="!modifyButtonAvailable">Submit
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

    
    <NModal v-model:show="showImportModal">
        <n-card closable @close="showImportModal = false" style="width: 600px" title="Import" :bordered="false"
            size="huge" role="dialog" aria-modal="true">
            <NForm ref="importFormRef" :model="importForm" :rules="importRules">

                <NFormItem label="Quantity:" path="quantity">
                    <NInputNumber v-model:value="importForm.quantity" :min="1" :max="2147483647"
                        placeholder="Please enter quantity" />
                </NFormItem>
                <NFormItem label="Total cost:" path="price">
                    <NInputNumber v-model:value="importForm.price" :precision="2" placeholder="Please enter total cost" />
                </NFormItem>
            </NForm>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton @click="showImportModal = false">Cancel</NButton>
                    <NButton type="primary" @click="handleImportSubmit" :disabled="!importButtonAvailable">Submit
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>
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
    NDataTable,
    NInputNumber
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

                const params: { [key: string]: string } = {};
                console.log(showSelectValue.value)
                if (showSelectValue.value == "isbn") {
                    params["isbn"] = showForm.value.isbn;
                }
                if (showSelectValue.value == "name") {
                    params["name"] = showForm.value.name;
                }
                if (showSelectValue.value == "author") {
                    params["author"] = showForm.value.author;
                }
                if (showSelectValue.value == "keyword") {
                    params["keyword"] = showForm.value.keyword;
                }
                console.log(params);
                const response = await axios.get(
                    API_BASE_URL + '/api/v1/books',
                    { params: params, headers: headers });

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
                    else if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                    else message.error("network error.")
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


const showSelectModal = ref(false);
const selectButtonAvailable = ref(true)
const selectForm = ref({
    isbn: '',
});
const selectFormRef = ref<FormInst | null>(null);
const selectRules = {
    isbn: {
        required: true, trigger: 'blur',
        validator: ISBNValidator
    },
};
const handleSelectSubmit = () => {
    console.log("handle select")
    selectFormRef.value?.validate(async (errors) => {
        selectButtonAvailable.value = false;
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
                const isbn = selectForm.value.isbn;

                const response = await axios.post(
                    API_BASE_URL + `/api/v1/users/select/${isbn}`,
                    {},
                    { headers: headers });
                loginStackStore.setCurrentToken(response.data['access_token']);
                loginStackStore.setCurrentId(response.data['selected_id'])
                message.success("Select success.");
                showSelectModal.value = false;
            } catch (error) {
                if (axios.isAxiosError(error)) {
                    console.log(error)
                    if (error.status === 401) {
                        message.error("Token unavailable. Logging out current account.");
                        loginStackStore.logout();
                    }
                    else if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                    else message.error("network error.")
                }
                else {
                    message.error('Unknown error: ' + error);
                }
            }
        }
        else {
            message.error("Invalid Input.")
        }
        selectButtonAvailable.value = true;
    })
}



const showBuyModal = ref(false);
const buyButtonAvailable = ref(true)
const buyForm = ref({
    isbn: '',
    quantity: 1,
});
const buyFormRef = ref<FormInst | null>(null);
const buyRules = {
    isbn: {
        required: true, trigger: 'blur',
        validator: ISBNValidator
    },
    quantity: {
        required: true, trigger: 'blur', validator: (rule: FormItemRule, value: any): boolean | Error => {
            return !!value;
        }
    },
};
const handleBuySubmit = () => {
    console.log("handle buy")
    buyFormRef.value?.validate(async (errors) => {
        buyButtonAvailable.value = false;
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
                const isbn = buyForm.value.isbn;
                const quantity = buyForm.value.quantity;
                const data: { [key: string]: string } = {
                    isbn: isbn,
                    quantity: quantity.toString(),
                };

                const response = await axios.post(
                    API_BASE_URL + '/api/v1/books/buy',
                    data,
                    { headers: headers });

                message.success(`Buy success. cost = ${response.data["message"]}`);
                showBuyModal.value = false;
            } catch (error) {
                if (axios.isAxiosError(error)) {
                    console.log(error)
                    if (error.status === 401) {
                        message.error("Token unavailable. Logging out current account.");
                        loginStackStore.logout();
                    }
                    else if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                    else {
                        message.error('Network error.')
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
        buyButtonAvailable.value = true;
    })
}




const showModifyModal = ref(false);
const modifyButtonAvailable = ref(true)
const modifyForm = ref({
    isbn: '',
    name: '',
    author: '',
    keyword: '',
    price: null
});
const modifyFormRef = ref<FormInst | null>(null);
const modifyRules = {
    isbn: {
        required: false, trigger: 'blur',
        validator: ISBNValidator
    },
    name: {
        required: false, trigger: 'blur',
        validator: bookNameValidator
    },
    author: {
        required: false, trigger: 'blur',
        validator: authorValidator
    },
    keyword: {
        required: false, trigger: 'blur',
        validator: keywordValidator
    },
};
const handleModifySubmit = () => {
    console.log("handle modify")
    modifyFormRef.value?.validate(async (errors) => {
        modifyButtonAvailable.value = false;
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
                const data: { [key: string]: string } = {
                };
                if (modifyForm.value.isbn) data['isbn'] = modifyForm.value.isbn;
                if (modifyForm.value.name) data['name'] = modifyForm.value.name;
                if (modifyForm.value.author) data['author'] = modifyForm.value.author;
                if (modifyForm.value.keyword) data['keyword'] = modifyForm.value.keyword;
                if (modifyForm.value.price !== null) data['price'] = modifyForm.value.price.toFixed(2);



                await axios.patch(
                    API_BASE_URL + '/api/v1/books/modify',
                    data,
                    { headers: headers });

                message.success("Modify success.");
                showModifyModal.value = false;
            } catch (error) {
                if (axios.isAxiosError(error)) {
                    console.log(error)
                    if (error.status === 401) {
                        message.error("Token unavailable. Logging out current account.");
                        loginStackStore.logout();
                    }
                    else if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                    else {
                        message.error('Network error.')
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
        modifyButtonAvailable.value = true;
    })
}


const showImportModal = ref(false);
const importButtonAvailable = ref(true)
const importForm = ref({
    quantity: 1,
    price: null
});
const importFormRef = ref<FormInst | null>(null);
const importRules = {
    quantity: {
        required: true, trigger: 'blur', validator: (rule: FormItemRule, value: any): boolean | Error => {
            return !!value;
        }
    },
    price: {
        required: true, trigger: 'blur', validator: (rule: FormItemRule, value: any): boolean | Error => {
            return !!value;
        }
    },
};
const handleImportSubmit = () => {
    console.log("handle import")
    importFormRef.value?.validate(async (errors) => {
        importButtonAvailable.value = false;
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
                const price = importForm.value.price;
                const quantity = importForm.value.quantity;
                const data: { [key: string]: string } = {
                    total_cost: price!.toFixed(2),
                    quantity: quantity.toString(),
                };

                await axios.post(
                    API_BASE_URL + '/api/v1/books/import',
                    data,
                    { headers: headers });

                message.success("Import success.");
                showImportModal.value = false;
            } catch (error) {
                if (axios.isAxiosError(error)) {
                    console.log(error)
                    if (error.status === 401) {
                        message.error("Token unavailable. Logging out current account.");
                        loginStackStore.logout();
                    }
                    else if (error.status === 400) {
                        message.error('Failed   ' + error.response?.data.message);
                    }
                     else {
                         message.error('Network error.')
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
        importButtonAvailable.value = true;
    })
}
</script>

<style scoped></style>